
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

uint8_t arp_request[42] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Destination MAC address (broadcast)
    0x00, 0x17, 0x22, 0xED, 0xA5, 0x01, // Source MAC address (assuming this is your MAC address)
    0x08, 0x06,                         // EtherType (ARP: 0x0806)
    0x00, 0x01,                         // Hardware type (Ethernet: 0x0001)
    0x08, 0x00,                         // Protocol type (IPv4: 0x0800)
    0x06, 0x04, // Hardware size (Ethernet MAC address size: 6 bytes), Protocol size (IPv4 address size: 4 bytes)
    0x00, 0x01, // ARP opcode (Request: 0x0001)
    0x00, 0x17, 0x22, 0xED, 0xA5, 0x01, // Sender MAC address
    0xc0, 0xa8, 0x12, 0x98,             // Sender IP address (192.168.18.152)
    0x00, 0x00, 0x00, 0x00,             // Target MAC address (unknown, typically left as zeros in ARP requests)
    0xc0, 0xa8, 0x12, 0x01              // Target IP address (192.168.18.1, your router's IP)
};

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

    while (true)
    {
        ether_obj.enc_packet_send(arp_request, 42);
        vTaskDelay(1000 / portTICK_PERIOD_MS); /* code */
    }

    // ether_obj.enc_packet_send(arp_request, 42);
    // vTaskDelay(10 / portTICK_PERIOD_MS); /* code */
}