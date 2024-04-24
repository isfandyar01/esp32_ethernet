
#include "enc28j60.hpp"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "spi_d.hpp"
#include <stdio.h>

extern "C" {
void app_main(void);
}

spi_device_handle_t spi;

void app_main() {
  printf("Hello PlatformIO!\n");

  esp_err_t ret;

  ret = spi_init(&spi);
  if (ret != ESP_OK) {
    printf("failed to init spi\n");
  }

  ENC28J60 ether_obj(spi);
  ether_obj.enc28j60_reset();
  vTaskDelay(100 / portTICK_PERIOD_MS);
  uint8_t reg = ether_obj.Read_control_register(ERXRDPTL);
  printf("reg_data %X\n", reg);
}