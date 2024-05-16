#include "ethernet.hpp"


static void printDestMAC(Eth_frame_struct *eth_frame)
{
    printf("Destination MAC Address: ");
    for (int i = 0; i < 6; i++)
    {
        printf("%02X", eth_frame->destMACAddr[i]);
        if (i < 5)
            printf(":");
    }
    printf("\n");
}

void ethernet_process(ENC28J60 *obj)
{

    uint16_t data_len = 0;
    data_len = obj->Read_buffer_memory();
    if (data_len > 0)
    {
        Eth_frame_struct *eth_frame = (Eth_frame_struct *)obj->ENC_data;
        printDestMAC(eth_frame);
        printf("ethertype %02X\n", eth_frame->ether_type);
    }
}
