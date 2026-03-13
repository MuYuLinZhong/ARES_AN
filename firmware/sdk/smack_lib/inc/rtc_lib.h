/* ============================================================================
** Copyright (c) 2024 Infineon Technologies AG
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
 * @file     rtc_lib.h
 *
 * @brief    This module provides supporting functions for the RTC and the standby clock.
 *
 * @version  v1.0
 * @date     2024-07-09
 *
 */

#ifndef RTC_LIB_H_
#define RTC_LIB_H_

/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Smack
 * @{
 */


/** @addtogroup rtc_lib
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief  This function initializes the RTC. Depending on the clock source given in the parameter,
 *         it initializes the 1-second number of clock ticks value register.
 *         If external crystal is clock source, then the calibration value register is set to 0x7FFF.
 *         If internal clock source is choosen the ATE calibration value, stored in DPARAM, is used.
 *         This function does *not* start the RTC. Please call rtc_control_lib() to start the clock.
 *         Warning: Do not set ext_xtal if no external crystal is connected!
 * @param  ext_xtal  true:  external crystal is used
 *                   false: internal slow clock is used
 */
extern void rtc_init_lib(bool ext_xtal);


/**
 * @brief  Switches the RTC second counting on/off. If the external crystal was selected in the call to
 *         rtc_init_lib(), the crystal oscillator is switched on as well.
 *         Note: The ROM lib function rtc_control() will not switch on the crystal oscillator.
 * @param  rtc_on  true:  the RTC starts counting,
 *                 false: the second counter is reset to 0x0 and no counting takes place.
 */
extern void rtc_control_lib(bool rtc_on);


#ifdef __cplusplus
}
#endif

/** @} */ /* End of group rtc_lib */

/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */

#endif /* RTC_LIB_H_ */
