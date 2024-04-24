#ifndef __SPI_D_HPP__
#define __SPI_D_HPP__

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "stdio.h"

#define CLK_PIN 18
#define MISO_PIN 19
#define MOSI_PIN 23
#define CS 5

typedef struct {
  spi_device_handle_t spi;
  gpio_num_t cs_pin;
  spi_host_device_t host;
} SPIDriver;

esp_err_t spi_init(spi_device_handle_t *spi_handle);
uint8_t read_byte(spi_device_handle_t spi, uint8_t read_command);
esp_err_t read_multiple_byte();
esp_err_t write_byte(spi_device_handle_t spi, uint8_t command);
esp_err_t write_multiple_byte();

#endif // __SPI_D_HPP__
