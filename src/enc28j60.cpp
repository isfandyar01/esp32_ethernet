#include "enc28j60.hpp"
#include "spi_d.hpp"

/**
  void init_enc28j60();
  void Bit_field_set(uint8_t reg, uint8_t data);
  void Bit_field_clear(uint8_t reg, uint8_t data);
  void write_control_reg(uint8_t reg, uint8_t data);
  void write_buffer_memory(uint8_t *data, uint16_t size);
  void Read_buffer_memory(uint8_t *data, uint16_t size);
  void switch_bank(ENC28J60_RegBank bank);
  void enc28j60_reset();

  ENC28J60_RegBank get_register_bank(uint8_t reg);
  uint8_t Read_control_register(uint8_t reg);
  uint8_t get_reg_address(uint8_t reg);
 *
 *
 */
uint8_t ENC28J60::current_bank = BANK_0;

void ENC28J60::init_enc28j60() {}

uint8_t ENC28J60::get_reg_address(uint8_t reg) {
  return (reg & (reg_address_mask));
}

ENC28J60_RegBank ENC28J60 ::get_register_bank(uint8_t register_address) {

  return ((ENC28J60_RegBank)((register_address >> bank_offset) & bank_mask));
}

void ENC28J60::switch_bank(ENC28J60_RegBank bank) {
  esp_err_t ret;
  uint8_t econ1_address = get_reg_address(ECON1);
  if (current_bank != bank) {

    uint8_t command = (BIT_FIELD_CLEAR << OP_CODE_OFFSET | econ1_address);
    // cs low
    ret = write_byte(&spi, command);
    if (ret != ESP_OK) {
      return;
    }
    write_byte(&spi, ECON1_BSEL1_BIT | ECON1_BSEL0_BIT);
  }
  // cs high

  // cs low
  uint8_t command = (BIT_FIELD_SET << OP_CODE_OFFSET | econ1_address);
  ret = write_byte(&spi, command);
  if (ret != ESP_OK) {
    return;
  }
  write_byte(&spi, bank);
}
current_bank = bank;
}

uint8_t ENC28J60 ::Read_control_register(uint8_t reg) {

  ENC28J60_RegBank reg_bank = get_register_bank(reg);
  uint8_t reg_addrs = get_reg_address(reg);
  switch_bank(reg_bank);
}