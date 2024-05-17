#ifndef __MACROS_HPP__
#define __MACROS_HPP__
/**
 * @brief macros defined e.g network to host
 *
 */
#define ntohs_16bits(x) (((x) >> 8) | ((x) << 8))

#define ntohs_32bits(x)                                                                                                \
    ((((x)&0xff000000) >> 24) | (((x)&0x00ff0000) >> 8) | (((x)&0x0000ff00) << 8) | (((x)&0x000000ff) << 24))


#define ntohs_8bits(x) (x)


#define htons_16bits(x) ((((x)&0xff00) >> 8) | (((x)&0x00ff) << 8))

#define htons_32bits(x)                                                                                                \
    ((((x)&0xff000000) >> 24) | (((x)&0x00ff0000) >> 8) | (((x)&0x0000ff00) << 8) | (((x)&0x000000ff) << 24))


#endif // __MACROS_HPP__
