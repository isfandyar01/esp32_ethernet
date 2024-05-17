#include "ethernet.hpp"
#include "macros.hpp"
#include <stdio.h>
#include <string.h>


uint8_t macAddr[6] = {0x74, 0x69, 0x69, 0x2D, 0x30, 0x36};


void print_mac(const char *label, const uint8_t *mac)
{
    printf("%s: %02X:%02X:%02X:%02X:%02X:%02X\n", label, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void print_ip(const char *label, const uint8_t *ip)
{
    printf("%s: %d.%d.%d.%d\n", label, ip[0], ip[1], ip[2], ip[3]);
}

static void printDestMAC(Eth_frame_struct *eth_frame)
{
    printf("Destination MAC Address: ");
    for (int i = 0; i < 6; i++)
    {
        printf("%02X", eth_frame->srcMACAddr[i]);
        if (i < 5)
            printf(":");
    }
    printf("\n");
}


uint16_t ARP_RESPONSE(arp_packet_struct *arp_frame_data, uint16_t arpFrameLen)
{
    uint16_t frame_len = 0;

    print_mac("destMacAddr", arp_frame_data->destMacAddr);
    if (memcmp(arp_frame_data->destIpAddr, ip_address, 4) == 0)
    {


        arp_frame_data->opCode = ntohs_16bits(arp_frame_data->opCode);


        if (arp_frame_data->opCode == Arp_request)
        {

            memcpy(arp_frame_data->destMacAddr, arp_frame_data->srcMacAddr, 6);
            memcpy(arp_frame_data->srcMacAddr, macAddr, 6);
            memcpy(arp_frame_data->destIpAddr, arp_frame_data->srcIpAddr, 4);
            memcpy(arp_frame_data->srcIpAddr, ip_address, 4);
            {

                arp_frame_data->opCode = htons_16bits(Arp_response);
                printf("op code arp %04X\n", arp_frame_data->opCode);
                frame_len = arpFrameLen;
            }
        }

        return arpFrameLen;
    }
    return 0;
}


void ethernet_process(ENC28J60 *obj)
{

    uint16_t data_len = 0;
    uint16_t reply_len = 0;
    data_len = obj->Read_buffer_memory();
    if (data_len > 0)
    { // copying data array from enc class to ethernet frame

        // enc data is array with all the data including dest mac source mac ethertype and after these packets its tha
        // payload of frame received this is a valid ethernet frame format so in next line we copy all data received
        // into ethernet frame after ether type we can check the payload for type of packet so when we cast the
        // eth_frame data into arp_packet struct the arp packet strucut only get data array of ethernet frame
        Eth_frame_struct *eth_frame = (Eth_frame_struct *)obj->ENC_data;
        // printDestMAC(eth_frame);
        // this shows that i need to do byte shift or reverse since network is in big endian and we are in little endian
        // ether type should be 0806 but it comes out as 0608 so i need to put macrocs next
        uint16_t ether_type = ntohs_16bits(eth_frame->ether_type);


        if (ether_type == ETH_FRAME_TYPE_ARP)
        {

            uint16_t arpFrameLen = data_len - sizeof(eth_frame);
            reply_len = ARP_RESPONSE((arp_packet_struct *)eth_frame->data, arpFrameLen);
        }

        if (reply_len > 0)
        {


            memcpy(eth_frame->destMACAddr, eth_frame->srcMACAddr, 6);
            memcpy(eth_frame->srcMACAddr, macAddr, 6);

            printf("%02X:", reply_len);
            obj->enc_packet_send((uint8_t *)eth_frame, reply_len + sizeof(eth_frame));
            // printf("\t\t hehehe \t \n");

            // printf("Destination MAC: ");
            // for (int i = 0; i < 6; i++)
            // {
            //     printf("%02X:", eth_frame->destMACAddr[i]);
            // }
            // printf("\n");

            // printf("Source MAC: ");
            // for (int i = 0; i < 6; i++)
            // {
            //     printf("%02X:", eth_frame->srcMACAddr[i]);
            // }
            // printf("\n");

            // printf("Ether Type: %04X\n", eth_frame->ether_type);


            // printf("Data: ");
            // for (int i = 0; i < 36; i++)
            // {
            //     printf("%02X ", eth_frame->data[i]); // Print each byte in hex format
            // }
            // printf("\n");
        }

        // now we can process ethernet packet based on its type
    }
}
