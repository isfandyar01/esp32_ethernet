#include "ip.hpp"
#include "esp_log.h "
#include "ethernet.hpp"
#include "icmp.hpp"
#include "macros.hpp"
#include "spi_d.hpp"
#include "stdio.h"
#include "string.h"


/**
 * @brief the check sum works by adding all bytes in
 * form of 16 bit data and then taking compliement of it
 * https://www.thegeekstuff.com/2012/05/ip-header-checksum/
 * @param data
 * @param length
 * @return uint16_t
 */
uint16_t ip_checksum_calc(uint8_t *data, uint16_t length)
{


    // 1. Initialization (Preparation)
    uint32_t res = 0; // Initialize the accumulator for checksum calculation (represented as a box)
                      // (Empty box signifies zero initial value)

    // 2. Type Casting and Pointer Setup


    uint16_t *ptr_to_data = (uint16_t *)data; // Cast `data` to a pointer of type `uint16_t*`

    // (Arrow points to the first 16-bit word in `data`


    // 3. Looping through Data (Main Calculation Phase)
    while (length > 1)
    {
        res += (*ptr_to_data); // Add the current 16-bit word to the accumulator
                               // (Value from memory pointed to by `ptr_to_data` is added to the box)
        ptr_to_data++;         // Increment the pointer to move to the next 16-bit word
                               // (Arrow moves to the next 16-bit word in `data`)
        length -= 2;           // Decrement the remaining length by 2 (since 2 bytes were processed)

        // Visualization of Looping:
        //  +-----------+           +-----------+           +-----------+           +-----------+
        //  |  Byte 1  |           |  Byte 2  |           |  Byte 3  |           |  Byte 4  | (data)
        //  +-----------+           +-----------+           +-----------+           +-----------+
        //        ^                      ^                      ^                      ^
        //        |                      |                      |                      |
        //     (ptr_to_data)            (ptr_to_data after 1st iteration)  (ptr_to_data after 2nd iteration) ...
    }

    // 4. Handling Single-Byte Leftover (if applicable)
    if (length > 0)
    {
        res += *(uint8_t *)ptr_to_data; // If there's a single byte remaining, add it (cast to 16-bit for
                                        // consistency) (The single byte value from memory is added to the box)
    }

    // 5. One's Complement Sum Reduction (Wrapping)
    while (res > 0xffff)
    {
        res = (res >> 16) + (res & 0xffff); // Carry the overflow from the high 16 bits to the low 16 bits
                                            // (Value in the box is shifted right by 16 and added to the original
                                            // value, effectively wrapping)

        // Visualization of Reduction:
        //  +-----------+           +-----------+
        //  | Overflow |           |   Result   |
        //  +-----------+           +-----------+
    }

    // 6. Final One's Complement (Inverting Bits)
    return ~((uint16_t)res); // Invert all bits using bitwise NOT

    // Visualization of Final Result:
    //  +-----------+
    //  | Checksum  | (Inverted bits of the final reduced value)
    //  +-----------+
}


uint16_t ip_process(ip_frame_struct *ip_frame, uint16_t length)
{

    // printf("Version and Header Length: 0x%02X\n", ip_frame->verHL);
    // printf("Service Type: 0x%02X\n", ip_frame->service_type);

    // // Print the total_len and data_gram_idenfication as hexadecimal values
    // printf("Total Length: 0x%04X\n", ntohs_16bits(ip_frame->total_len) & 0xFFFF);
    // printf("Data Gram Identification: 0x%04X\n", ntohs_16bits(ip_frame->data_gram_idenfication) & 0xFFFF);

    // // Print the fLags_Frament_offset as a hexadecimal value
    // printf("Flags and Fragment Offset: 0x%04X\n", ntohs_16bits(ip_frame->fLags_Frament_offset) & 0xFFFF);

    // // Print the ttl and protocol as hexadecimal values
    // printf("Time To Live: 0x%02X\n", ip_frame->ttl);
    // printf("Protocol: 0x%02X\n", ip_frame->protocol);

    // // Print the header_checksum as a hexadecimal value
    // printf("Header Checksum: 0x%04X\n", ntohs_16bits(ip_frame->header_checksum) & 0xFFFF);
    // // Print the source and destination IP addresses
    // printf("Source IP: %u.%u.%u.%u\n", ip_frame->source_ip[0], ip_frame->source_ip[1], ip_frame->source_ip[2],
    //        ip_frame->source_ip[3]);
    // printf("Destination IP: %u.%u.%u.%u\n", ip_frame->dest_ip[0], ip_frame->dest_ip[1], ip_frame->dest_ip[2],
    //        ip_frame->dest_ip[3]);


    // ip_frame->total_len = ntohs_16bits(ip_frame->total_len);
    // ip_frame->data_gram_idenfication = ntohs_16bits(ip_frame->data_gram_idenfication);
    // ip_frame->fLags_Frament_offset = ntohs_16bits(ip_frame->fLags_Frament_offset);
    // ip_frame->header_checksum = ntohs_16bits(ip_frame->header_checksum);

    // uint16_t ip_data_len = length - sizeof(ip_frame_struct);
    // for (size_t i = 0; i < ip_data_len; i++)
    // {
    //     printf("data in ip frame  0x%02X\n", ip_frame->data[i]);
    // }

    uint16_t frame_len = 0;
    if (memcmp(ip_frame->dest_ip, ip_address, 4) == 0)
    {
        uint16_t received_checksum = 0;

        received_checksum = ip_frame->header_checksum;

        // printf("received checksum %4X\n", received_checksum);


        ip_frame->header_checksum = 0;


        // printf("size of ip frame struct %d\n", sizeof(ip_frame_struct));

        uint16_t cal_checksum = ip_checksum_calc((uint8_t *)ip_frame, sizeof(ip_frame_struct));


        // printf("cal checksum %4X \n", cal_checksum);


        if (received_checksum == cal_checksum)
        {
            printf("*** ip FRAME CHECKSUM OK ***\n");
            uint16_t data_length_in_ip_frame = length - sizeof(ip_frame_struct);
            uint16_t newdata_len = 0;
            if (ip_frame->protocol == IP_FRAME_PROTOCOL_ICMP)
            {
                newdata_len = icmp_frame_processor((icmp_frame_struct *)ip_frame->data, data_length_in_ip_frame);
            }
            frame_len = newdata_len + sizeof(ip_frame_struct);
            ip_frame->total_len = htons_16bits(frame_len);
            ip_frame->fLags_Frament_offset = 0;
            ip_frame->data_gram_idenfication = 0;
            memcpy(ip_frame->dest_ip, ip_frame->source_ip, 4);
            memcpy(ip_frame->source_ip, ip_address, 4);
            ip_frame->header_checksum = ip_checksum_calc((uint8_t *)ip_frame, sizeof(ip_frame_struct));
        }
    }
    return frame_len;
}