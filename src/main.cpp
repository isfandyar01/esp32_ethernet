
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
#include "lwip/etharp.h"
#include "lwip/inet.h"
#include "lwip/init.h"
#include "lwip/ip4_addr.h"
#include "lwip/netif.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "lwip/tcpip.h"
#include "lwipopts.h"
#include "nvs_flash.h"
#include "spi_d.hpp"
#include <stdio.h>

#define TAG "ENC28J60-NETIF"

extern "C"
{
    void app_main(void);
}


#define MAC_ADDR_SIZE 6
// TCP Server Implementation
#define TCP_PORT 7 // Echo server port

spi_device_handle_t spi;

ENC28J60 ether_obj(spi);

static struct netif enc_netif;
static ENC28J60 *enc_driver = nullptr;

static err_t enc28j60_netif_init(struct netif *netif);
static err_t enc28j60_netif_output(struct netif *netif, struct pbuf *p);
static void enc28j60_input_task(void *pvParameters);

// Declare the network init function from previous file
bool enc28j60_network_init(ENC28J60 *driver, const ip4_addr_t *ipaddr, const ip4_addr_t *netmask, const ip4_addr_t *gw);

// uint8_t arp_request[] = {
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Destination MAC address
//     0x74, 0x69, 0x69, 0x2D, 0x30, 0x36, // Source MAC address
//     0x08, 0x06,                         // EtherType (ARP: 0x0806)
//     0x00, 0x01,                         // Hardware type (Ethernet: 0x0001)
//     0x08, 0x00,                         // Protocol type (IPv4: 0x0800)
//     0x06, 0x04, // Hardware size (Ethernet MAC address size: 6 bytes), Protocol size (IPv4 address size: 4 bytes)
//     0x00, 0x01, // ARP opcode (Request: 0x0001)
//     0x74, 0x69, 0x69, 0x2D, 0x30, 0x36, // Sender MAC address
//     0xc0, 0xa8, 0x12, 0x34,             // Sender IP address (192.168.18.52)
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Target MAC address (unknown, set to zeros)
//     0xc0, 0xa8, 0x12, 0x01              // Target IP address (192.168.18.1)
// };


static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    if (p == NULL)
    {
        // Connection closed
        tcp_close(tpcb);
        return ERR_OK;
    }

    // Echo received data back
    tcp_write(tpcb, p->payload, p->len, 1);
    tcp_output(tpcb);

    pbuf_free(p);
    return ERR_OK;
}

static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    if (err != ERR_OK || newpcb == NULL)
    {
        return ERR_VAL;
    }

    ESP_LOGI(TAG, "New client connected");

    // Set up receive callback
    tcp_recv(newpcb, tcp_server_recv);

    return ERR_OK;
}

static void start_tcp_server(void)
{
    struct tcp_pcb *pcb = tcp_new();
    if (pcb == NULL)
    {
        ESP_LOGE(TAG, "Failed to create TCP PCB");
        return;
    }

    err_t err = tcp_bind(pcb, IP_ADDR_ANY, TCP_PORT);
    if (err != ERR_OK)
    {
        ESP_LOGE(TAG, "Failed to bind TCP server");
        return;
    }

    pcb = tcp_listen(pcb);
    if (pcb == NULL)
    {
        ESP_LOGE(TAG, "Failed to start TCP listen");
        return;
    }

    tcp_accept(pcb, tcp_server_accept);
    ESP_LOGI(TAG, "TCP server started on port %d", TCP_PORT);
}

void app_main(void)
{
    // Initialize SPI
    spi_device_handle_t spi;
    ESP_ERROR_CHECK(spi_init(&spi, 5, 3));

    // Initialize ENC28J60
    ENC28J60 ether_obj(spi);
    ether_obj.enc28j60_reset();
    vTaskDelay(pdMS_TO_TICKS(100));
    ether_obj.init_enc28j60();

    // Configure IP address
    ip4_addr_t ipaddr, netmask, gateway;
    IP4_ADDR(&ipaddr, 192, 168, 18, 52);  // Your desired IP address
    IP4_ADDR(&netmask, 255, 255, 255, 0); // Subnet mask
    IP4_ADDR(&gateway, 192, 168, 18, 1);  // Gateway

    // Initialize network interface
    if (!enc28j60_network_init(&ether_obj, &ipaddr, &netmask, &gateway))
    {
        ESP_LOGE("MAIN", "Failed to initialize network interface");
        return;
    }

    ESP_LOGI("MAIN", "Network interface initialized");
    ESP_LOGI("MAIN", "IP: " IPSTR, IP2STR(&ipaddr));
    // start_tcp_server();
    // Main loop
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        // LWIP and network interface will handle everything in background tasks
    }
}


bool enc28j60_network_init(ENC28J60 *driver, const ip4_addr_t *ipaddr, const ip4_addr_t *netmask, const ip4_addr_t *gw)
{
    enc_driver = driver;

    // Initialize LWIP
    tcpip_init(NULL, NULL);

    // Add our network interface to LWIP
    if (!netif_add(&enc_netif, ipaddr, netmask, gw, driver, enc28j60_netif_init, tcpip_input))
    {
        ESP_LOGE(TAG, "Failed to add netif");
        return false;
    }

    netif_set_default(&enc_netif);
    netif_set_up(&enc_netif);

    // Create input task to handle incoming packets
    xTaskCreate(enc28j60_input_task, "enc28j60_input", 4096, NULL, 5, NULL);

    return true;
}


// Network interface initialization
static err_t enc28j60_netif_init(struct netif *netif)
{
    ESP_LOGI(TAG, "Initializing ENC28J60 network interface");

    netif->name[0] = 'e';
    netif->name[1] = '0';

    // Setup callback functions
    netif->output = etharp_output;             // IPv4 output
    netif->linkoutput = enc28j60_netif_output; // Direct output

    // Set MAC hardware address length
    netif->hwaddr_len = ETHARP_HWADDR_LEN;

    // Set MAC address from your driver's configuration
    // Assuming your MAC address is stored in the driver
    uint8_t mac[6] = {0x74, 0x69, 0x69, 0x2D, 0x30, 0x36}; // Your MAC address
    memcpy(netif->hwaddr, mac, ETHARP_HWADDR_LEN);

    // Maximum transfer unit
    netif->mtu = 1500;

    // Device capabilities
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP;

    return ERR_OK;
}


// Network output function
static err_t enc28j60_netif_output(struct netif *netif, struct pbuf *p)
{
    if (enc_driver == nullptr)
    {
        return ERR_IF;
    }

    // Send packet using your driver
    enc_driver->enc_packet_send((uint8_t *)p->payload, p->tot_len);

    return ERR_OK;
}

// Input task to handle incoming packets
static void enc28j60_input_task(void *pvParameters)
{
    while (1)
    {
        if (enc_driver == nullptr)
        {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        // Read incoming packet
        uint16_t len = enc_driver->Read_buffer_memory();
        if (len > 0)
        {
            // Allocate pbuf
            struct pbuf *p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
            if (p != NULL)
            {
                // Copy received data to pbuf
                pbuf_take(p, enc_driver->ENC_data, len);

                // Send to network interface
                if (enc_netif.input(p, &enc_netif) != ERR_OK)
                {
                    pbuf_free(p);
                }
            }
        }

        // Small delay to prevent tight loop
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}