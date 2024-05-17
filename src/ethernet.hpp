#ifndef __ETHERNET_HPP__
#define __ETHERNET_HPP__

#include "enc28j60.hpp"
#include <stdint.h>
#include <stdio.h>


#define Arp_request 0x0001
#define Arp_response 0x0002


#define ETH_FRAME_TYPE_ARP 0x0806
#define ETH_FRAME_TYPE_IP 0x0800

#pragma pack(push, 1)
struct arp_packet_struct
{
    uint16_t hType;
    uint16_t pType;
    uint8_t hLen;
    uint8_t pLen;
    uint16_t opCode;
    uint8_t srcMacAddr[6];
    uint8_t srcIpAddr[4];
    uint8_t destMacAddr[6];
    uint8_t destIpAddr[4];
};
#pragma pack(pop)

static uint8_t ip_address[4] = {192, 168, 18, 52};

typedef struct
{
    uint8_t destMACAddr[6];
    uint8_t srcMACAddr[6];
    uint16_t ether_type;
    uint8_t data[];
} Eth_frame_struct;

void ethernet_process(ENC28J60 *obj);

uint16_t ARP_RESPONSE(arp_packet_struct *arp_frame_data, uint16_t arpFrameLen);

#endif // __ETHERNET_HPP__
