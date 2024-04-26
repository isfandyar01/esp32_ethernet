
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
  uint8_t data = ether_obj.Read_control_register(ERXFCON);
  printf("red data %X\n", data);
}