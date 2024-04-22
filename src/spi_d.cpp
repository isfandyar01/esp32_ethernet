#include "spi_d.hpp"
#include "string.h"
#include <iostream>
#include <stdio.h>

esp_err_t spi_init(spi_device_handle_t *spi_handle) {
  esp_err_t ret;

  spi_bus_config_t bus_config = {
      .mosi_io_num = MOSI_PIN,
      .miso_io_num = MISO_PIN,
      .sclk_io_num = CLK_PIN,
      .quadwp_io_num = -1,  // Not used
      .quadhd_io_num = -1,  // Not used
      .max_transfer_sz = 0, // Default maximum transfer size
  };

  esp_err_t ret = spi_bus_initialize(VSPI_HOST, &bus_config, SPI_DMA_DISABLED);
  if (ret != ESP_OK) {
    printf("Failed to initialize SPI bus: %d\n", ret);
    return ret;
  }

  spi_device_interface_config_t dev_config = {
      .command_bits = 0,
      .address_bits = 0,
      .dummy_bits = 0,
      .mode = 0,
      .duty_cycle_pos = 128,
      .cs_ena_pretrans = 0,
      .cs_ena_posttrans = 0,
      .clock_speed_hz = 1000000, // 1 MHz
      .input_delay_ns = 0,
      .spics_io_num = CS,
      .flags = 0,
      .queue_size = 7,
      .pre_cb = NULL,
      .post_cb = NULL,
  };

  ret = spi_bus_add_device(VSPI_HOST, &dev_config, spi_handle);
  if (ret != ESP_OK) {
    printf("Failed to add SPI device: %d\n", ret);
    spi_bus_free(VSPI_HOST); // Free SPI bus in case of failure
    return ret;
  }

  return ESP_OK;
}

uint8_t read_byte(spi_device_handle_t spi, uint8_t reg_addr) {
  uint8_t data;
  spi_transaction_t t;
  memset(&t, 0, sizeof(t)); // Zero out the transaction object
  t.length = 8;             // 8 bits (1 byte)
  t.rxlength = 8;           // We'll be reading 8 bits
  t.flags = 0;              // No additional flags
  t.cmd = reg_addr;         // Command byte indicating the register address
  t.rx_buffer = &data;      // Buffer to receive the data
  esp_err_t ret = spi_device_transmit(spi, &t); // Transmit and receive
  if (ret != ESP_OK) {
    printf("SPI transaction failed: %d\n", ret);
    return 0; // Or handle error as appropriate
  }
  return data;
}
esp_err_t read_multiple_byte();

esp_err_t write_byte(spi_device_handle_t spi, uint8_t command) {

  spi_transaction_t t;
  memset(&t, 0, sizeof(t)); // Zero out the transaction object
  t.length = 8;             // 8 bits (1 byte)
  t.flags = 0;              // No additional flags
  t.tx_buffer = &command;   // Buffer containing the command byte
                            // Buffer to receive the data
  esp_err_t ret = spi_device_transmit(spi, &t); // Transmit and receive
  if (ret != ESP_OK) {
    printf("SPI transaction failed: %d\n", ret);
    return ret; // Or handle error as appropriate
  }
  return ESP_OK;
}

esp_err_t write_multiple_byte();
