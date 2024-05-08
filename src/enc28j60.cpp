#include "enc28j60.hpp"
#include "spi_d.hpp"

// /**
//   void init_enc28j60();
//   void Bit_field_set(uint8_t reg, uint8_t data);
//   void Bit_field_clear(uint8_t reg, uint8_t data);
//   void write_control_reg(uint8_t reg, uint8_t data);
//   void write_buffer_memory(uint8_t *data, uint16_t size);
//   void Read_buffer_memory(uint8_t *data, uint16_t size);
//   void switch_bank(ENC28J60_RegBank bank);
//   void enc28j60_reset();

//   ENC28J60_RegBank get_register_bank(uint8_t reg);
//   uint8_t Read_control_register(uint8_t reg);
//   uint8_t get_reg_address(uint8_t reg);
//  *
//  *
//  */
uint8_t ENC28J60::current_bank = BANK_0;

ENC28J60::ENC28J60(spi_device_handle_t spi_handle)
{
    spi = spi_handle;
}


void ENC28J60::write_control_reg_pair(uint8_t reg_address, uint16_t data)
{
    uint8_t high_byte = data >> 8;
    ENC28J60_RegBank reg_bank = get_register_bank(reg_address);
    // printf("highbyte %X\n", high_byte);
    switch_bank(reg_bank);
    transfer_and_read_byte(spi, nullptr, (uint8_t)data, WRITE_CONTROL_REG, reg_address);
    transfer_and_read_byte(spi, nullptr, high_byte, WRITE_CONTROL_REG, reg_address + 1);
}

void ENC28J60::write_control_reg(uint8_t reg, uint8_t data)
{

    ENC28J60_RegBank reg_bank = get_register_bank(reg);
    switch_bank(reg_bank);
    transfer_and_read_byte(spi, nullptr, data, WRITE_CONTROL_REG, reg);
}


void ENC28J60::write_phy_reg(uint8_t reg_address, uint16_t data)
{
    write_control_reg(MIREGADR, reg_address);
    write_control_reg_pair(MIWRL, data);
    while ((Read_control_register(MISTAT) & MISTAT_BUSY_BIT) != 0)
        ;
}

uint16_t ENC28J60::Read_phy_reg(uint8_t reg)
{
    write_control_reg(MIREGADR, reg);
    write_control_reg(MICMD, MICMD_MIIRD_BIT);
    while ((Read_control_register(MISTAT) & MISTAT_BUSY_BIT) != 0)
        ;
    write_control_reg(MICMD, 0x00);
    uint16_t data = Read_control_register(MIRDL);
    data |= Read_control_register(MIRDH) << 8;
    return data;
}

void ENC28J60::Bit_field_set(uint8_t reg, uint8_t data)
{
    ENC28J60_RegBank reg_bank = get_register_bank(reg);
    switch_bank(reg_bank);
    esp_err_t ret;

    ret = transfer_and_read_byte(spi, nullptr, data, BIT_FIELD_SET, reg);
    if (ret != ESP_OK)
    {
        return;
    }
}


void ENC28J60::Bit_field_clear(uint8_t reg, uint8_t data)
{
    ENC28J60_RegBank reg_bank = get_register_bank(reg);
    switch_bank(reg_bank);
    esp_err_t ret;

    ret = transfer_and_read_byte(spi, nullptr, data, BIT_FIELD_CLEAR, reg);
    if (ret != ESP_OK)
    {
        return;
    }
}
void ENC28J60::init_enc28j60()
{
    // writing recieve buffer start address and end address  00:17:22:ed:a5:01
    uint8_t macAddr[6] = {0x36, 0x30, 0x2d, 0x69, 0x69, 0x74};

    printf("writing buffer mem addreses \n");
    write_control_reg_pair(ERXSTL, ENC28J60_RX_BUF_START); // start address of receive buffer
    write_control_reg_pair(ERXNDL, ENC28J60_RX_BUF_END);   // end address  of receive buffer

    write_control_reg_pair(ETXSTL, ENC28J60_TX_BUF_START);
    write_control_reg_pair(ETXNDL, ENC28J60_TX_BUF_END);

    write_control_reg_pair(ERXRDPTL, ENC28J60_RX_BUF_START); // for tracking of receive pointer
    write_control_reg_pair(ERXWRPTL, ENC28J60_RX_BUF_START);
    printf("writing buffer mem addreses done \n");

    write_control_reg(ERXFCON, ERXFCON_UCEN | ERXFCON_ANDOR | ERXFCON_CRCEN);


    write_control_reg(MACON1, MACON1_MARXEN_BIT | MACON1_RXPAUS_BIT | MACON1_TXPAUS_BIT | MACON1_PASSALL_BIT);
    write_control_reg(MACON3, MACON3_PADCFG0_BIT | MACON3_TXCRCEN_BIT | MACON3_FRMLNEN_BIT);
    write_control_reg_pair(MAIPGL, 0x0c12); // value came from datasheet
    write_control_reg(MABBIP, 0x12);        // value came from datasheet
    write_control_reg_pair(MAMXFLL, 1500);


    printf("writing mac \n"); // my mac address 0x74,0x69,0x69,0x2D,0x30,0x36
    write_control_reg(MAADR1, macAddr[0]);
    write_control_reg(MAADR2, macAddr[1]);
    write_control_reg(MAADR3, macAddr[2]);
    write_control_reg(MAADR4, macAddr[3]);
    write_control_reg(MAADR5, macAddr[4]);
    write_control_reg(MAADR6, macAddr[5]);
    printf("writing mac done \n");

    write_phy_reg(PHLCON, 0x0122);
    write_phy_reg(PHCON2, PHCON2_HDLDIS_BIT);

    // write_control_reg(ECON1, ECON1_RXEN_BIT);
    Bit_field_set(ECON1, ECON1_RXEN_BIT);
}

uint8_t ENC28J60::get_reg_address(uint8_t reg)
{
    return (reg & (reg_address_mask));
}

ENC28J60_RegBank ENC28J60 ::get_register_bank(uint8_t register_address)
{

    return (ENC28J60_RegBank)((register_address & bank_mask) >> bank_offset);
}

void ENC28J60::switch_bank(ENC28J60_RegBank bank)
{
    esp_err_t ret;
    uint8_t econ1_address = get_reg_address(ECON1);
    if (current_bank != bank)
    {

        // clearing bank select bits in econ 1
        ret = transfer_and_read_byte(spi, nullptr, ECON1_BSEL1_BIT | ECON1_BSEL0_BIT, BIT_FIELD_CLEAR, econ1_address);
        if (ret != ESP_OK)
        {
            return;
        }

        // setting bank select bits in econ 1
        ret = transfer_and_read_byte(spi, nullptr, bank, BIT_FIELD_SET, econ1_address);
        if (ret != ESP_OK)
        {
            return;
        }
    }

    current_bank = bank;
    // printf("bank %X\n", current_bank);
}

uint8_t ENC28J60 ::Read_control_register(uint8_t reg)
{

    ENC28J60_RegBank reg_bank = get_register_bank(reg);

    uint8_t reg_addrs = get_reg_address(reg);
    // printf("reg_addrs %X\n", reg_addrs);

    switch_bank(reg_bank);

    uint8_t data = 0;
    transfer_and_read_byte(spi, &data, 0xff, READ_CONTROL_REG, reg_addrs);
    return data;
}

uint8_t ENC28J60::read_mac_register(uint8_t reg)
{
    ENC28J60_RegBank reg_bank = get_register_bank(reg);

    uint8_t reg_addrs = get_reg_address(reg);
    // printf("reg_addrs %X\n", reg_addrs);

    switch_bank(reg_bank);

    uint8_t data = 0;
    transfer_and_read_byte(spi, &data, 0xff, READ_CONTROL_REG, reg_addrs);
    return data;
}
void ENC28J60::enc28j60_reset()
{
    transfer_and_read_byte(spi, nullptr, 0, System_Reset, 0x1f);
}

void ENC28J60::write_buffer_memory(uint8_t *data, uint16_t size)
{
    transfer_and_read_MultiplesBytes(spi, 0x1A, data, nullptr, size, WRITE_BUFFER_MEM);
}

void ENC28J60::Read_buffer_memory(uint8_t *data, uint16_t size)
{
    transfer_and_read_MultiplesBytes(spi, 0x1A, nullptr, data, size, READ_BUFFER_MEM);
}

void ENC28J60::enc_packet_send(uint8_t *data, uint16_t length)
{
    while (1)
    {
        Bit_field_set(ECON1, ECON1_TXRST_BIT);
        Bit_field_clear(ECON1, ECON1_TXRST_BIT);
        Bit_field_clear(EIR, EIR_TXERIF_BIT | EIR_TXERIF_BIT);

        write_control_reg_pair(EWRPTL, ENC28J60_TX_BUF_START);
        write_control_reg_pair(ETXNDL, ENC28J60_TX_BUF_START + length);
        // transfer_and_read_byte(spi, nullptr, 0x00, WRITE_BUFFER_MEM, 0x1a);
        write_buffer_memory(data, length);

        Bit_field_set(ECON1, ECON1_TXRTS_BIT);

        uint16_t count = 0;
        while ((Read_control_register(EIR) & (EIR_TXIF_BIT | EIR_TXERIF_BIT)) == 0 && ++count < 1000U)
            ;
        if (!(Read_control_register(EIR) & EIR_TXERIF_BIT) && count < 1000U)
        {
            printf("transmission done \n");
            break;
        }
        printf("cancel prev transmission \n");
        Bit_field_clear(ECON1, ECON1_TXRTS_BIT);
        break;
    }
}