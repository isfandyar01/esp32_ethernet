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
    { // copying data array from enc class to ethernet frame
        Eth_frame_struct *eth_frame = (Eth_frame_struct *)obj->ENC_data;
        printDestMAC(eth_frame);
        // this shows that i need to do byte shift or reverse since network is in big endian and we are in little endian
        // ether type should be 0806 but it comes out as 0608 so i need to put macrocs next

        printf("ethertype %02X\n", eth_frame->ether_type);
    }
}
