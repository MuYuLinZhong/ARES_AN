/* ============================================================================
** Copyright (c) 2021 Infineon Technologies AG
**               All rights reserved.
**               www.infineon.com
** ============================================================================
**
** ============================================================================
** Redistribution and use of this software only permitted to the extent
** expressly agreed with Infineon Technologies AG.
** ============================================================================
*
*/

/**
 * @file     inet_lib.h
 *
 * @brief    This module provides functions to convert endianess which are usually found in "inet.h"
 *
 *
 * @version  v1.0
 * @date     2021-08-01
 *
 */

#ifndef INC_INET_LIB_H_
#define INC_INET_LIB_H_

/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Smack
 * @{
 */


/** @addtogroup inet_lib
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

// endian conversion

__STATIC_FORCEINLINE uint32_t ntohl(uint32_t l)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return __REV(l);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return l;
#else
#error endian
#endif
}

__STATIC_FORCEINLINE uint16_t ntohs(uint16_t s)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return __REVSH(s);          // cmsis header: signed 16-bit
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return s;
#else
#error endian
#endif
}

__STATIC_FORCEINLINE uint64_t ntohll(uint64_t ll)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    union
    {
        uint64_t ll;
        uint32_t w[2];
    } in, out;
    in.ll = ll;
    out.w[0] = ntohl(in.w[1]);
    out.w[1] = ntohl(in.w[0]);
    return out.ll;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return ll;
#else
#error endian
#endif
}

__STATIC_FORCEINLINE uint32_t htonl(uint32_t l)
{
    return ntohl(l);
}

__STATIC_FORCEINLINE uint16_t htons(uint16_t s)
{
    return ntohs(s);
}

__STATIC_FORCEINLINE uint64_t htonll(uint64_t ll)
{
    return ntohll(ll);
}

#ifdef __cplusplus
}
#endif

/** @} */ /* End of group inet_lib */

/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */

#endif /* INC_INET_LIB_H_ */
