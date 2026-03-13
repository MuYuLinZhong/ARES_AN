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
 * @file     rtc.h
 *
 * @brief    Driver for the RTC.
 *
 * @version  v1.0
 * @date     2020-05-20
 *
 * @note
 */

/*lint -save -e960 */

#ifndef _RTC_H_
#define _RTC_H_

#include <stdbool.h>

/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Smack
 * @{
 */


/** @addtogroup rtc
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function initializes the RTC. Depending on the clock source, it initializes the 1-second number of clock ticks value register
 *        If external crystal is clock source, then the calibration value register is set to 0x7FFF.
 *        If internal clock source is choosen the ATE calibration value, stored in Flash is used.
 */
extern void     rtc_init(void);

/**
 * @brief Switches the RTC second counting on/off
 * @param bool rtc_on --> if true, then the RTC starts counting, if false, then the second counter is reset to 0x0 and no counting takes place.
 */
extern void     rtc_control(bool rtc_on);

/**
 * @brief This function reads the RTC second counter and returns  the current value of the seconds counter.
 * @return uint32_t, which represents the current value of the second counter
 */
extern uint32_t rtc_get(void);


#ifdef __cplusplus
}
#endif

/** @} */ /* End of group rtc block */


/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */

#endif /* _RTC_H_ */
