#ifndef ENC28J60_HPP
#define ENC28J60_HPP

#include "driver/spi_master.h"
#include <stdint.h>

#define OP_CODE_OFFSET 5

#define reg_address_mask 0x1f

#define reg_type_offset 7
#define ethernet_reg_type_bit ((ENC28J60_RegType)ETH_REG << reg_type_offset)
#define mac_reg_type_bit ((ENC28J60_RegType)MAC_MII_REG << reg_type_offset)

#define bank_mask 0x60
#define bank_offset 5

#define ENC28J60_TX_BUF_START 0x0c00
#define ENC28J60_TX_BUF_END 0x11ff
#define ENC28J60_RX_BUF_START 0x0000
#define ENC28J60_RX_BUF_END 0x0bff

#define EIE_RXERIE (1 << 0)
#define EIE_TXERIE (1 << 1)
#define EIE_TXIE (1 << 3)
#define EIE_LINKIE (1 << 4)
#define EIE_DMAIE (1 << 5)
#define EIE_PKTIE (1 << 6)
#define EIE_INTIE (1 << 7)

#define ERXFCON_UCEN 0x80
#define ERXFCON_ANDOR 0x40
#define ERXFCON_CRCEN 0x20
#define ERXFCON_PMEN 0x10
#define ERXFCON_MPEN 0x08
#define ERXFCON_HTEN 0x04
#define ERXFCON_MCEN 0x02
#define ERXFCON_BCEN 0x01

#define EIR_PKTIF_BIT (1 << 6)
#define EIR_DMAIF_BIT (1 << 5)
#define EIR_LINKIF_BIT (1 << 4)
#define EIR_TXIF_BIT (1 << 3)
#define EIR_TXERIF_BIT (1 << 1)
#define EIR_RXERIF_BIT (1 << 0)

#define ECON2_AUTOINC_BIT (1 << 7)
#define ECON2_PKTDEC_BIT (1 << 6)
#define ECON2_PWRSV_BIT (1 << 5)
#define ECON2_VRPS_BIT (1 << 3)

#define ECON1_TXRST_BIT (1 << 7)
#define ECON1_RXRST_BIT (1 << 6)
#define ECON1_DMAST_BIT (1 << 5)
#define ECON1_CSUMEN_BIT (1 << 4)
#define ECON1_TXRTS_BIT (1 << 3)
#define ECON1_RXEN_BIT (1 << 2)
#define ECON1_BSEL1_BIT (1 << 1)
#define ECON1_BSEL0_BIT (1 << 0)

#define MACON1_TXPAUS_BIT (1 << 3)
#define MACON1_RXPAUS_BIT (1 << 2)
#define MACON1_PASSALL_BIT (1 << 1)
#define MACON1_MARXEN_BIT (1 << 0)

#define MACON3_PADCFG2_BIT (1 << 7)
#define MACON3_PADCFG1_BIT (1 << 6)
#define MACON3_PADCFG0_BIT (1 << 5)
#define MACON3_TXCRCEN_BIT (1 << 4)
#define MACON3_PHDRLEN_BIT (1 << 3)
#define MACON3_HFRMEN_BIT (1 << 2)
#define MACON3_FRMLNEN_BIT (1 << 1)
#define MACON3_FULDPX_BIT (1 << 0)

#define MICMD_MIIRD_BIT (1 << 0)

#define MISTAT_BUSY_BIT (1 << 0)

#define PHCON2_FRCLNK_BIT (1 << 14)
#define PHCON2_TXIS_BIT (1 << 13)
#define PHCON2_JABBER_BIT (1 << 10)
#define PHCON2_HDLDIS_BIT (1 << 8)

#define PHLCON_LACFG3_BIT (1 << 11)
#define PHLCON_LACFG2_BIT (1 << 10)
#define PHLCON_LACFG1_BIT (1 << 9)
#define PHLCON_LACFG0_BIT (1 << 8)
#define PHLCON_LBCFG3_BIT (1 << 7)
#define PHLCON_LBCFG2_BIT (1 << 6)
#define PHLCON_LBCFG1_BIT (1 << 5)
#define PHLCON_LBCFG0_BIT (1 << 4)
#define PHLCON_LFRQ1_BIT (1 << 3)
#define PHLCON_LFRQ0_BIT (1 << 2)
#define PHLCON_STRCH_BIT (1 << 1)

#define bank_0_bits ((ENC28J60_RegBank)BANK_0 << bank_offset)
#define bank_1_bits ((ENC28J60_RegBank)BANK_1 << bank_offset)
#define bank_2_bits ((ENC28J60_RegBank)BANK_2 << bank_offset)
#define bank_3_bits ((ENC28J60_RegBank)BANK_3 << bank_offset)

// Common regs in all banks
#define EIE (0x1b | bank_0_bits | ethernet_reg_type_bit)
#define EIR (0x1c | bank_0_bits | ethernet_reg_type_bit)
#define ESTAT (0x1d | bank_0_bits | ethernet_reg_type_bit)
#define ECON2 (0x1e | bank_0_bits | ethernet_reg_type_bit)
#define ECON1 (0x1f | bank_0_bits | ethernet_reg_type_bit)

// Bank 0 regs
#define ERDPTL (0x00 | bank_0_bits | ethernet_reg_type_bit)
#define ERDPTH (0x01 | bank_0_bits | ethernet_reg_type_bit)
#define EWRPTL (0x02 | bank_0_bits | ethernet_reg_type_bit)
#define EWRPTH (0x03 | bank_0_bits | ethernet_reg_type_bit)
#define ETXSTL (0x04 | bank_0_bits | ethernet_reg_type_bit)
#define ETXSTH (0x05 | bank_0_bits | ethernet_reg_type_bit)
#define ETXNDL (0x06 | bank_0_bits | ethernet_reg_type_bit)
#define ETXNDH (0x07 | bank_0_bits | ethernet_reg_type_bit)
#define ERXSTL (0x08 | bank_0_bits | ethernet_reg_type_bit)
#define ERXSTH (0x09 | bank_0_bits | ethernet_reg_type_bit)
#define ERXNDL (0x0A | bank_0_bits | ethernet_reg_type_bit)
#define ERXNDH (0x0B | bank_0_bits | ethernet_reg_type_bit)
#define ERXRDPTL (0x0C | bank_0_bits | ethernet_reg_type_bit)
#define ERXRDPTH (0x0D | bank_0_bits | ethernet_reg_type_bit)
#define ERXWRPTL (0x0E | bank_0_bits | ethernet_reg_type_bit)
#define ERXWRPTH (0x0F | bank_0_bits | ethernet_reg_type_bit)
#define EDMASTL (0x10 | bank_0_bits | ethernet_reg_type_bit)
#define EDMASTH (0x11 | bank_0_bits | ethernet_reg_type_bit)
#define EDMANDL (0x12 | bank_0_bits | ethernet_reg_type_bit)
#define EDMANDH (0x13 | bank_0_bits | ethernet_reg_type_bit)
#define EDMADSTL (0x14 | bank_0_bits | ethernet_reg_type_bit)
#define EDMADSTH (0x15 | bank_0_bits | ethernet_reg_type_bit)
#define EDMACSL (0x16 | bank_0_bits | ethernet_reg_type_bit)
#define EDMACSH (0x17 | bank_0_bits | ethernet_reg_type_bit)

/// Bank 1
#define EHT0 (0x00 | bank_1_bits | ethernet_reg_type_bit)
#define EHT1 (0x01 | bank_1_bits | ethernet_reg_type_bit)
#define EHT2 (0x02 | bank_1_bits | ethernet_reg_type_bit)
#define EHT3 (0x03 | bank_1_bits | ethernet_reg_type_bit)
#define EHT4 (0x04 | bank_1_bits | ethernet_reg_type_bit)
#define EHT5 (0x05 | bank_1_bits | ethernet_reg_type_bit)
#define EHT6 (0x06 | bank_1_bits | ethernet_reg_type_bit)
#define EHT7 (0x07 | bank_1_bits | ethernet_reg_type_bit)
#define EPMM0 (0x08 | bank_1_bits | ethernet_reg_type_bit)
#define EPMM1 (0x09 | bank_1_bits | ethernet_reg_type_bit)
#define EPMM2 (0x0A | bank_1_bits | ethernet_reg_type_bit)
#define EPMM3 (0x0B | bank_1_bits | ethernet_reg_type_bit)
#define EPMM4 (0x0C | bank_1_bits | ethernet_reg_type_bit)
#define EPMM5 (0x0D | bank_1_bits | ethernet_reg_type_bit)
#define EPMM6 (0x0E | bank_1_bits | ethernet_reg_type_bit)
#define EPMM7 (0x0F | bank_1_bits | ethernet_reg_type_bit)
#define EPMCSL (0x10 | bank_1_bits | ethernet_reg_type_bit)
#define EPMCSH (0x11 | bank_1_bits | ethernet_reg_type_bit)
#define EPMOL (0x14 | bank_1_bits | ethernet_reg_type_bit)
#define EPMOH (0x15 | bank_1_bits | ethernet_reg_type_bit)
#define ERXFCON (0x18 | bank_1_bits | ethernet_reg_type_bit)
#define EPKTCNT (0x19 | bank_1_bits | ethernet_reg_type_bit)

/// Bank 2
#define MACON1 (0x00 | bank_2_bits | mac_reg_type_bit)
#define MACON3 (0x02 | bank_2_bits | mac_reg_type_bit)
#define MACON4 (0x03 | bank_2_bits | mac_reg_type_bit)
#define MABBIP (0x04 | bank_2_bits | mac_reg_type_bit)
#define MAIPGL (0x06 | bank_2_bits | mac_reg_type_bit)
#define MAIPGH (0x07 | bank_2_bits | mac_reg_type_bit)
#define MACLCON1 (0x08 | bank_2_bits | mac_reg_type_bit)
#define MACLCON2 (0x09 | bank_2_bits | mac_reg_type_bit)
#define MAMXFLL (0x0A | bank_2_bits | mac_reg_type_bit)
#define MAMXFLH (0x0B | bank_2_bits | mac_reg_type_bit)
#define MICMD (0x12 | bank_2_bits | mac_reg_type_bit)
#define MIREGADR (0x14 | bank_2_bits | mac_reg_type_bit)
#define MIWRL (0x16 | bank_2_bits | mac_reg_type_bit)
#define MIWRH (0x17 | bank_2_bits | mac_reg_type_bit)
#define MIRDL (0x18 | bank_2_bits | mac_reg_type_bit)
#define MIRDH (0x19 | bank_2_bits | mac_reg_type_bit)
/// Bank 3 regs
#define MAADR5 (0x00 | bank_3_bits | mac_reg_type_bit)
#define MAADR6 (0x01 | bank_3_bits | mac_reg_type_bit)
#define MAADR3 (0x02 | bank_3_bits | mac_reg_type_bit)
#define MAADR4 (0x03 | bank_3_bits | mac_reg_type_bit)
#define MAADR1 (0x04 | bank_3_bits | mac_reg_type_bit)
#define MAADR2 (0x05 | bank_3_bits | mac_reg_type_bit)
#define EBSTSD (0x06 | bank_3_bits | ethernet_reg_type_bit)
#define EBSTCON (0x07 | bank_3_bits | ethernet_reg_type_bit)
#define EBSTCSL (0x08 | bank_3_bits | ethernet_reg_type_bit)
#define EBSTCSH (0x09 | bank_3_bits | ethernet_reg_type_bit)
#define MISTAT (0x0A | bank_3_bits | mac_reg_type_bit)
#define EREVID (0x12 | bank_3_bits | ethernet_reg_type_bit)
#define ECOCON (0x15 | bank_3_bits | ethernet_reg_type_bit)
#define EFLOCON (0x17 | bank_3_bits | ethernet_reg_type_bit)
#define EPAUSL (0x18 | bank_3_bits | ethernet_reg_type_bit)
#define EPAUSH (0x19 | bank_3_bits | ethernet_reg_type_bit)

// PHY registers
#define PHCON1 (0x00)
#define PHSTAT1 (0x01)
#define PHID1 (0x02)
#define PHID2 (0x03)
#define PHCON2 (0x10)
#define PHSTAT2 (0x11)
#define PHIE (0x12)
#define PHIR (0x13)
#define PHLCON (0x14)

typedef enum
{
    READ_CONTROL_REG,
    READ_BUFFER_MEM,
    WRITE_CONTROL_REG,
    WRITE_BUFFER_MEM,
    BIT_FIELD_SET,
    BIT_FIELD_CLEAR,
    System_Reset = 7
} ENC28J60_Command;

typedef enum
{
    BANK_0,
    BANK_1,
    BANK_2,
    BANK_3,
} ENC28J60_RegBank;

typedef enum
{
    CS_LOW = 0,
    CS_HIGH = 1,
} ENC28J60_CS_State;

typedef enum
{
    ETH_REG,
    MAC_MII_REG,
} ENC28J60_RegType;

class ENC28J60
{

  private:
    spi_device_handle_t spi;

  public:
    static uint8_t current_bank;
    ENC28J60(spi_device_handle_t spi_handle);
    void init_enc28j60();
    void Bit_field_set(uint8_t reg, uint8_t data);
    void Bit_field_clear(uint8_t reg, uint8_t data);
    void write_control_reg(uint8_t reg, uint8_t data);
    void write_phy_reg(uint8_t reg_address, uint16_t data);
    void write_buffer_memory(uint8_t *data, uint16_t size);
    void enc_packet_send(uint8_t *data, uint16_t length);
    void Read_buffer_memory(uint8_t *data, uint16_t size);
    void switch_bank(ENC28J60_RegBank bank);
    void enc28j60_reset();
    void write_control_reg_pair(uint8_t reg_address, uint16_t data);

    ENC28J60_RegBank get_register_bank(uint8_t reg);
    uint8_t Read_control_register(uint8_t reg);
    uint8_t get_reg_address(uint8_t reg);
    uint16_t Read_phy_reg(uint8_t reg);
    uint8_t read_mac_register(uint8_t reg);
};

#endif // ENC28J60_HPP
