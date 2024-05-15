#ifndef __ETHERNET_HPP__
#define __ETHERNET_HPP__

#include <stdint.h>
#include <stdio.h>


#define Arp_request 0x0001
#define Arp_response 0x0002


#define ETH_FRAME_TYPE_ARP 0x0806
#define ETH_FRAME_TYPE_IP 0x0800

#pragma pack(push, 1)
struct arp_packet_struct
{
    uint8_t dst_mac[6];
    uint8_t src_mac[6];
    uint8_t ether_type[2];
    uint8_t hwtype[2];
    uint8_t protype[2];
    uint8_t hwsize;
    uint8_t prosize;
    uint8_t opcode[2];
    uint8_t sender_mac[6];
    uint8_t sender_ip[4];
    uint8_t target_mac[6];
    uint8_t target_ip[4];
};
#pragma pack(pop)


void process_arp_request(uint8_t *data);

#endif // __ETHERNET_HPP__
