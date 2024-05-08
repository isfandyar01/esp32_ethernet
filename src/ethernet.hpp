#ifndef __ETHERNET_HPP__
#define __ETHERNET_HPP__

#include <stdint.h>
#include <stdio.h>

#pragma pack(push, 1)
struct arp_packet_struct
{
    uint8_t dst_mac[7];
    uint8_t src_mac[6];
    uint16_t ether_type;
    uint16_t hwtype;
    uint16_t protype;
    uint8_t hwsize;
    uint8_t prosize;
    uint16_t opcode;
    uint8_t sender_mac[6];
    uint32_t sender_ip;
    uint8_t target_mac[6];
    uint32_t target_ip;
};
#pragma pack(pop)


#endif // __ETHERNET_HPP__
