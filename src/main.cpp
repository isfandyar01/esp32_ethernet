
#include "enc28j60.hpp"
#include "esp_efuse.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "spi_d.hpp"
#include <stdio.h>


extern "C"
{
    void app_main(void);
}

spi_device_handle_t spi;
#define MAC_ADDR_SIZE 6


void app_main()
{


    printf("Hello PlatformIO!\n");

    esp_err_t ret;

    ret = spi_init(&spi, 5, 3);
    if (ret != ESP_OK)
    {
        printf("failed to init spi\n");
    }

    ENC28J60 ether_obj(spi);
    ether_obj.enc28j60_reset();

    vTaskDelay(10 / portTICK_PERIOD_MS);

    ether_obj.init_enc28j60();

    uint8_t data = ether_obj.Read_control_register(ECON1);
    printf("red data %X\n", data);
}