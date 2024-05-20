#include "ip.hpp"

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
    uint16_t res = 0; // Initialize the accumulator for checksum calculation (represented as a box)
                      // (Empty box signifies zero initial value)

    // 2. Type Casting and Pointer Setup
    uint16_t *ptr_to_data = (uint16_t *)data; // Cast `data` to a pointer of type `uint16_t*`
                                              // (Arrow points to the first 16-bit word in `data`)

    // 3. Looping through Data (Main Calculation Phase)
    while (length > 1)
    {
        res += *ptr_to_data; // Add the current 16-bit word to the accumulator
                             // (Value from memory pointed to by `ptr_to_data` is added to the box)
        ptr_to_data++;       // Increment the pointer to move to the next 16-bit word
                             // (Arrow moves to the next 16-bit word in `data`)
        length -= 2;         // Decrement the remaining length by 2 (since 2 bytes were processed)

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
