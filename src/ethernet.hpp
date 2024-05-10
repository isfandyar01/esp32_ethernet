#ifndef __ETHERNET_HPP__
#define __ETHERNET_HPP__

#include <stdint.h>
#include <stdio.h>

#pragma pack(push, 1)
struct arp_packet_struct
{
    uint8_t dst_mac[7];
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


#endif // __ETHERNET_HPP__