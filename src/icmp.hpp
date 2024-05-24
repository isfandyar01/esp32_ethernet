#ifndef __ICMP_HPP__
#define __ICMP_HPP__

#include <stdint.h>

#define ICMP_FRAME_TYPE_ECHO_REQUEST 0x08
#define ICMP_FRAME_TYPE_ECHO_REPLY 0x00

typedef struct
{
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint16_t identifier;
    uint16_t sequence_number;
    uint8_t data[];

} icmp_frame_struct;


uint16_t icmp_frame_processor(icmp_frame_struct *frame, uint16_t lenght);

#endif // __ICMP_HPP__
