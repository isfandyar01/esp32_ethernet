#include "spi_d.hpp"
#include "string.h"
#include <iostream>
#include <stdio.h>

esp_err_t spi_init(spi_device_handle_t *spi_handle, uint8_t count_address_bits, uint8_t count_command_bits)
{

    esp_err_t ret;

    spi_bus_config_t bus_config = {
        .mosi_io_num = MOSI_PIN,
        .miso_io_num = MISO_PIN,
        .sclk_io_num = CLK_PIN,
        .quadwp_io_num = -1,  // Not used
        .quadhd_io_num = -1,  // Not used
        .max_transfer_sz = 0, // Default maximum transfer size
    };

    ret = spi_bus_initialize(VSPI_HOST, &bus_config, SPI_DMA_CH1);
    if (ret != ESP_OK)
    {
        printf("Failed to initialize SPI bus: %d\n", ret);
        return ret;
    }

    spi_device_interface_config_t dev_config = {
        .command_bits = count_command_bits,
        .address_bits = count_address_bits,
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
    if (ret != ESP_OK)
    {
        printf("Failed to add SPI device: %d\n", ret);
        spi_bus_free(VSPI_HOST); // Free SPI bus in case of failure
        return ret;
    }

    return ESP_OK;
}

esp_err_t transfer_and_read_byte(spi_device_handle_t spi_handle, uint8_t *rx_data, uint8_t tx_data, uint8_t command,
                                 uint8_t reg_address)
{

    spi_transaction_t t;
    memset(&t, 0, sizeof(t)); // Zero out the transaction object
    t.flags = SPI_TRANS_USE_TXDATA;
    t.length = 8;
    t.cmd = command;
    t.addr = reg_address;
    t.tx_data[0] = tx_data;
    t.rx_buffer = rx_data;
    esp_err_t ret = spi_device_transmit(spi_handle, &t); // Transmit and receive
    if (ret != ESP_OK)
    {
        printf("SPI transaction failed: %d\n", ret);
        return ret; // Or handle error as appropriate
    }
    return ESP_OK;
}
esp_err_t transfer_and_read_MultiplesBytes(spi_device_handle_t spi_handle, const uint8_t reg_addr, uint8_t *tx_buf,
                                           uint8_t *rx_buf, size_t data_length, const uint8_t command)
{

    spi_transaction_t t;
    memset(&t, 0, sizeof(t)); // Zero out the transaction object
    t.flags = 0;
    t.length = data_length * 8;
    t.cmd = command;
    t.addr = reg_addr;
    t.tx_buffer = tx_buf;
    t.rx_buffer = rx_buf;
    esp_err_t ret = spi_device_transmit(spi_handle, &t); // Transmit and receive
    if (ret != ESP_OK)
    {
        printf("SPI transaction failed: %d\n", ret);
        return ret; // Or handle error as appropriate
    }
    
    return ESP_OK;
}
