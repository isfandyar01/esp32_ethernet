#include "enc28j60.hpp"
#include "esp_log.h"
#include "esp_timer.h"
#include "spi_d.hpp"

uint8_t ENC28J60::current_bank = BANK_0;
uint16_t ENC28J60 ::nxt_pakt_pointer = ENC28J60_RX_BUF_START;

ENC28J60::ENC28J60(spi_device_handle_t spi_handle)
{
    if (spi == nullptr)
    {
        ESP_LOGE("ERROR", "SPI handle is not initialized in ENC28J60 constructor.");
    }
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
    uint8_t macAddr[6] = {0x74, 0x69, 0x69, 0x2D, 0x30, 0x36};

    printf("writing buffer mem addreses \n");
    write_control_reg_pair(ERXSTL, ENC28J60_RX_BUF_START); // start address of receive buffer
    write_control_reg_pair(ERXNDL, ENC28J60_RX_BUF_END);   // end address  of receive buffer

    write_control_reg_pair(ETXSTL, ENC28J60_TX_BUF_START);
    write_control_reg_pair(ETXNDL, ENC28J60_TX_BUF_END);

    write_control_reg_pair(ERXRDPTL, ENC28J60_RX_BUF_START); // for tracking of receive pointer
    write_control_reg_pair(ERXWRPTL, ENC28J60_RX_BUF_START);
    printf("writing buffer mem addreses done \n");


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


    write_control_reg(ERXFCON, ERXFCON_UCEN | ERXFCON_PMEN | ERXFCON_CRCEN);


    // For broadcast packets we allow only ARP packtets
    // All other packets should be unicast only for our mac (MAADR)
    //
    // The pattern to match on is therefore
    // Type     ETH.DST
    // ARP      BROADCAST
    // 06 08 -- ff ff ff ff ff ff -> ip checksum for theses bytes=f7f9
    // in binary these poitions are:11 0000 0011 1111
    // This is hex 303F->EPMM0=0x3f,EPMM1=0x30
    write_control_reg(EPMM0, 0x3f);
    write_control_reg(EPMM1, 0x30);
    write_control_reg(EPMCSL, 0xf9);
    write_control_reg(EPMCSH, 0xf7);
    // write_control_reg(ECON1, ECON1_RXEN_BIT);
    Bit_field_set(EIE, EIE_INTIE | EIE_PKTIE);
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

uint16_t ENC28J60::Read_buffer_memory()
{
    uint8_t bytes[2] = {0};
    uint16_t dataSize = 0;
    uint8_t packet_count = Read_control_register(EPKTCNT);

    // Check if there are packets in the buffer
    if (packet_count != 0)
    {
        // Set the read pointer to the start of the current packet
        write_control_reg(ERDPTL, (nxt_pakt_pointer & 0xff));
        write_control_reg(ERDPTH, (nxt_pakt_pointer) >> 8);

        // Read the next packet pointer
        transfer_and_read_MultiplesBytes(spi, 0x1A, nullptr, bytes, sizeof(bytes), READ_BUFFER_MEM);
        nxt_pakt_pointer = bytes[0] | (bytes[1] << 8);

        // Read packet length
        transfer_and_read_MultiplesBytes(spi, 0x1A, nullptr, bytes, sizeof(bytes), READ_BUFFER_MEM);
        dataSize = (bytes[0] | (bytes[1] << 8)) - ENC28J60_CRC_SIZE;

        // Read packet status to validate
        transfer_and_read_MultiplesBytes(spi, 0x1A, nullptr, bytes, sizeof(bytes), READ_BUFFER_MEM);
        uint16_t packet_status = bytes[0] | (bytes[1] << 8);

        // Ensure data size does not exceed the buffer max
        if (dataSize > ENC28J60_FRAME_DATA_MAX - 1)
        {
            dataSize = ENC28J60_FRAME_DATA_MAX - 1;
        }

        if ((packet_status & 0x80) == 0) // Invalid packet check
        {
            dataSize = 0;
            // printf("Invalid packet received\n");
            // write_control_reg_pair(ERXRDPTL, nxt_pakt_pointer - 1);
            // Bit_field_set(ECON2, ECON2_PKTDEC_BIT);

            // return dataSize;
        }
        else
        {
            // Read the packet data into ENC_data array
            transfer_and_read_MultiplesBytes(spi, 0x1A, nullptr, (uint8_t *)&(ENC_data[0]), dataSize, READ_BUFFER_MEM);

            // Optionally read CRC if needed
            // transfer_and_read_MultiplesBytes(spi, 0x1A, nullptr, (uint8_t *)&checkSum, ENC28J60_CRC_SIZE,
            //                                  READ_BUFFER_MEM);
        }

        write_control_reg(ERXRDPTL, (nxt_pakt_pointer & 0xff));
        write_control_reg(ERXRDPTH, (nxt_pakt_pointer) >> 8);

        // Adjust RX read pointer to start of next packet
        uint16_t nextPtr = nxt_pakt_pointer - 1;
        if (nextPtr < ENC28J60_RX_BUF_START || nextPtr > ENC28J60_RX_BUF_END)
        {
            write_control_reg(ERXRDPTL, (ENC28J60_RX_BUF_END) & 0xff);
            write_control_reg(ERXRDPTH, (ENC28J60_RX_BUF_END) >> 8);
        }
        else
        {
            write_control_reg(ERXRDPTL, (nextPtr) & 0xff);
            write_control_reg(ERXRDPTH, (nextPtr) >> 8);
        }

        Bit_field_set(ECON2, ECON2_PKTDEC_BIT);
    }
    return dataSize;
}
void ENC28J60::enc_packet_send(uint8_t *data, uint16_t length)
{


    // Set the TX buffer start pointer
    write_control_reg_pair(EWRPTL, ENC28J60_TX_BUF_START);

    // Set the TX end pointer to mark the end of the data
    write_control_reg_pair(ETXNDL, ENC28J60_TX_BUF_START + length);

    // Reset the transmit logic, as per errata
    Bit_field_set(ECON1, ECON1_TXRST_BIT);
    Bit_field_clear(ECON1, ECON1_TXRST_BIT);

    // Clear any existing TX flags
    Bit_field_clear(EIR, EIR_TXERIF_BIT | EIR_TXIF_BIT);


    uint8_t control_byte = 0x00;

    // Write control byte to start of the TX buffer
    write_buffer_memory(&control_byte, 1);
    // Write data to the buffer
    write_buffer_memory(data, length);
    // Start transmission
    Bit_field_set(ECON1, ECON1_TXRTS_BIT);


    // Wait for transmission to complete or timeout
    unsigned long timer = esp_timer_get_time() / 1000;
    uint8_t eir;
    while (((eir = Read_control_register(EIR)) & (EIR_TXIF_BIT | EIR_TXERIF_BIT)) == 0)
    {
        // Check for timeout after 1000 ms
        if ((esp_timer_get_time() / 1000) - timer > 1000)
        {
            printf("Transmission timeout\n");
            Bit_field_clear(ECON1, ECON1_TXRTS_BIT); // Stop transmission if stuck
            return;                                  // Exit on failure
        }
    }

    // Clear the TXRTS bit to finalize
    Bit_field_clear(ECON1, ECON1_TXRTS_BIT);

    // Log success or failure
    if (eir & EIR_TXERIF_BIT)
    {
        printf("Transmission failed\n");
    }
    else
    {
        printf("Transmission successful\n");
    }
}