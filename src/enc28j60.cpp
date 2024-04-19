#include "enc28j60.hpp"

uint8_t ENC28J60::current_bank = BANK_0;

void ENC28J60::init_enc28j60() {}

uint8_t ENC28J60::get_red_address(uint8_t reg) {
  return (reg & (reg_address_mask));
}

ENC28J60_RegBank ENC28J60 ::get_register_bank(uint8_t register_address) {

  return ((ENC28J60_RegBank)((register_address >> bank_offset) & bank_mask));
}
