
#include "enc28j60.hpp"
#include "esp_efuse.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "ethernet.hpp"
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

uint8_t arp_request[] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Destination MAC address (broadcast)
    0x74, 0x69, 0x69, 0x2D, 0x30, 0x36, // Source MAC address (assuming this is your MAC address)
    0x08, 0x06,                         // EtherType (ARP: 0x0806)
    0x00, 0x01,                         // Hardware type (Ethernet: 0x0001)
    0x08, 0x00,                         // Protocol type (IPv4: 0x0800)
    0x06, 0x04, // Hardware size (Ethernet MAC address size: 6 bytes), Protocol size (IPv4 address size: 4 bytes)
    0x00, 0x01, // ARP opcode (Request: 0x0001)
    0x74, 0x69, 0x69, 0x2D, 0x30, 0x36, // Sender MAC address
    0xc0, 0xa8, 0x12, 0x34,             // Sender IP address (192.168.18.52)
    0x00, 0x00, 0x00, 0x00,             // Target MAC address (unknown, set to zeros)
    0xc0, 0xa8, 0x12, 0x01              // Target IP address (192.168.18.1, your router's IP)
};


uint8_t arp_request_2[] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Destination MAC address
    0x74, 0x69, 0x69, 0x2D, 0x30, 0x36,       // Source MAC address
    0x08, 0x06,                               // EtherType (ARP: 0x0806)
    0x00, 0x01,                               // Hardware type (Ethernet: 0x0001)
    0x08, 0x00,                               // Protocol type (IPv4: 0x0800)
    0x06, 0x04, // Hardware size (Ethernet MAC address size: 6 bytes), Protocol size (IPv4 address size: 4 bytes)
    0x00, 0x01, // ARP opcode (Request: 0x0001)
    0x74, 0x69, 0x69, 0x2D, 0x30, 0x36, // Sender MAC address
    0xc0, 0xa8, 0x12, 0x90,             // Sender IP address (192.168.18.152)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Target MAC address (unknown, set to zeros)
    0xc0, 0xa8, 0x12, 0x01              // Target IP address (192.168.18.1)
};


void app_main()
{
    arp_packet_struct request_packet = {
        {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}, // Destination MAC address (broadcast)
        {0x74, 0x69, 0x69, 0x2D, 0x30, 0x36},       // Source MAC address (assuming this is your MAC address)
        0x0806,                                     // EtherType (ARP: 0x0806)
        0x0001,                                     // Hardware type (Ethernet: 0x0001)
        0x0800,                                     // Protocol type (IPv4: 0x0800)
        6,                                          // Hardware size (Ethernet MAC address size: 6 bytes)
        4,                                          // Protocol size (IPv4 address size: 4 bytes)
        0x0001,                                     // ARP opcode (Request: 0x0001)
        {0x74, 0x69, 0x69, 0x2D, 0x30, 0x36},       // Sender MAC address
        0xc0a81234,                                 // Sender IP address (192.168.18.52)
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},       // Target MAC address (unknown, set to zeros)
        0xc0a81201};


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

    // uint8_t mac = ether_obj.read_mac_register(MAADR5);
    // printf("mac address %x\n", mac);
    uint8_t *arp_data = (uint8_t *)&request_packet;

    while (true)
    {
        ether_obj.enc_packet_send(arp_request_2, sizeof(arp_request_2));
        vTaskDelay(1000 / portTICK_PERIOD_MS); /* code */
        // ether_obj.Read_buffer_memory(read_adata, 32);
    }

    // ether_obj.enc_packet_send(arp_request, 42);
    // vTaskDelay(10 / portTICK_PERIOD_MS); /* code */
}