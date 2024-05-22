#ifndef __IP_HPP__
#define __IP_HPP__

#include <stdint.h>

/**
 * @brief https://benisnous.com/wp-content/uploads/2021/08/IPV4-header-format-in-Computer-Network.jpg
 *
 */
typedef struct
{
    uint8_t verHL;
    uint8_t service_type;
    uint16_t total_len;
    uint16_t data_gram_idenfication;
    uint16_t fLags_Frament_offset;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t header_checksum;
    uint8_t source_ip[4];
    uint8_t dest_ip[4];
    uint8_t data[];
} ip_frame_struct;


uint16_t ip_checksum_calc(uint8_t *data, uint16_t length);
uint16_t ip_process(ip_frame_struct *obj, uint16_t length);

#endif // __IP_HPP__
