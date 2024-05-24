#include "icmp.hpp"
#include "ip.hpp"
#include <stdio.h>


uint16_t icmp_frame_processor(icmp_frame_struct *frame, uint16_t lenght)
{
    uint16_t newframelen = 0;
    uint16_t checksum = frame->checksum;
    frame->checksum = 0;
    uint16_t calchecksum = ip_checksum_calc((uint8_t *)frame, lenght);
    if (checksum == calchecksum)
    {
        printf("*** ICMP checkSUM OK ***- \n ");
        if (frame->type == ICMP_FRAME_TYPE_ECHO_REQUEST)
        {
            frame->type = ICMP_FRAME_TYPE_ECHO_REPLY;
            frame->checksum = ip_checksum_calc((uint8_t *)frame, lenght);
            newframelen = lenght;

            printf("&&***&& data apeended &&***&& \n ");
        }
    }
    return newframelen;
}