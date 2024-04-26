
#include "enc28j60.hpp"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "spi_d.hpp"
#include <stdio.h>

uint8_t data[5] = {0xBE, 0xEF, 0xFF, 0xCB, 0xAB};

extern "C" {
void app_main(void);
}

spi_device_handle_t spi;

void app_main() {
  printf("Hello PlatformIO!\n");

  esp_err_t ret;

  ret = spi_init(&spi, 5, 3);
  if (ret != ESP_OK) {
    printf("failed to init spi\n");
  }

  ENC28J60 ether_obj(spi);
  ether_obj.enc28j60_reset();
  vTaskDelay(100 / portTICK_PERIOD_MS);
  // ether_obj.write_control_reg(ECON1, 0x02);

  uint8_t reg2 = ether_obj.Read_control_register(ECON2);
  // uint8_t address = ether_obj.get_reg_address(ECON1);
  // write_multiple_byte(spi, data, sizeof(data), WRITE_CONTROL_REG, address);

  printf("ECON1_data %X\n", reg2);
}