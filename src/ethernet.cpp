#include "ethernet.hpp"
#include "ip.hpp"
#include "macros.hpp"
#include <stdio.h>
#include <string.h>


uint8_t macAddr[6] = {0x74, 0x69, 0x69, 0x2D, 0x30, 0x36};


uint16_t ARP_RESPONSE(arp_packet_struct *arp_frame_data, uint16_t arpFrameLen)
{

    uint16_t frame_len = 0;


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

                frame_len = arpFrameLen;
            }
        }

        return frame_len;
    }
    return frame_len;
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

        // data len is minimum 64 bytes i think so it get padded with zeros and to get actual frame lenght we minus the
        // etherframe - data length


        uint16_t ethdata_len = data_len - sizeof(Eth_frame_struct);

        // for (size_t i = 0; i < ethdata_len; i++)
        // {
        //     printf("frame data %4X \n", eth_frame->data[i]);
        // }


        // printf("eth data len %d\n", ethdata_len);
        if (ether_type == ETH_FRAME_TYPE_ARP)
        {
            // printf("\t\treceived frame is arp type\n");
            reply_len = ARP_RESPONSE((arp_packet_struct *)eth_frame->data, ethdata_len);
        }
        if (ether_type == ETH_FRAME_TYPE_IP)
        {
            // printf("\t\treceived frame is ip type\n");
            reply_len = ip_process((ip_frame_struct *)eth_frame->data, ethdata_len);
        }
        if (reply_len > 0)
        {


            memcpy(eth_frame->destMACAddr, eth_frame->srcMACAddr, 6);
            memcpy(eth_frame->srcMACAddr, macAddr, 6);


            obj->enc_packet_send((uint8_t *)eth_frame, reply_len + sizeof(Eth_frame_struct));
        }

        // now we can process ethernet packet based on its type
    }
}
