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
 * @file     shc_lib.h
 *
 * @brief    This module provides voltage comparison through the SH unit.
 *
 * @version  v1.0
 * @date     2021-08-01
 *
 */

#ifndef SHC_LIB_H_
#define SHC_LIB_H_

/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Smack
 * @{
 */


/** @addtogroup shc_lib
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

// channel selection in shc_compare()
typedef enum shc_channel_e
{
    shc_channel_ma = 3,
    shc_channel_mb = 4,
} shc_channel_t;


/**
 * @brief Initialize the SHC module, e.g. start clock.
 */
extern void shc_init(void);

/**
 * @brief Stop the SHC module, e.g. switch off clock.
 */
extern void shc_close(void);

/**
 * @brief Compare a signal against a voltage.
 *        Note: Do not use SHC and sense unit at the same time, units to route the analog signals are shared between both of the modules.
 * @param channel   signal to compare
 * @param threshold threshold voltage to compare the signal against (not exactly; 1000mV ~ 1024 digits)
 * @return          true: voltage of signal is higher than threshold
 */
extern bool shc_compare(const shc_channel_t channel, const uint16_t threshold);

#ifdef __cplusplus
}
#endif

/** @} */ /* End of group shc_lib */

/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */

#endif /* SHC_LIB_H_ */
