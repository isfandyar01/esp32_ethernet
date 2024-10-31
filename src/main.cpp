
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
#include "lwip/inet.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "lwip/tcpip.h"
#include "lwipopts.h"
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
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Destination MAC address
    0x74, 0x69, 0x69, 0x2D, 0x30, 0x36, // Source MAC address
    0x08, 0x06,                         // EtherType (ARP: 0x0806)
    0x00, 0x01,                         // Hardware type (Ethernet: 0x0001)
    0x08, 0x00,                         // Protocol type (IPv4: 0x0800)
    0x06, 0x04, // Hardware size (Ethernet MAC address size: 6 bytes), Protocol size (IPv4 address size: 4 bytes)
    0x00, 0x01, // ARP opcode (Request: 0x0001)
    0x74, 0x69, 0x69, 0x2D, 0x30, 0x36, // Sender MAC address
    0xc0, 0xa8, 0x12, 0x34,             // Sender IP address (192.168.18.52)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Target MAC address (unknown, set to zeros)
    0xc0, 0xa8, 0x12, 0x01              // Target IP address (192.168.18.1)
};

err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    struct pbuf *q;
    for (q = p; q != NULL; q = q->next)
    {
        ether_obj.enc_packet_send((uint8_t *)q->payload, q->len);
    }
    return ERR_OK;
}

void app_main()
{
    tcpip_init(NULL, NULL);


    printf("Hello PlatformIO!\n");

    esp_err_t ret;

    ret = spi_init(&spi, 5, 3);
    if (ret != ESP_OK)
    {
        printf("failed to init spi\n");
    }

    ENC28J60 ether_obj(spi);
    ether_obj.enc28j60_reset();

    vTaskDelay(100 / portTICK_PERIOD_MS);

    ether_obj.init_enc28j60();


    while (true)
    {

        vTaskDelay(1000 / portTICK_PERIOD_MS);
        ethernet_process(&ether_obj);
    }
}