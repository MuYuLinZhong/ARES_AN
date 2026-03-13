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
 * @file     sys_tick_lib.h
 *
 * @brief    This module provides functions around the SysTick timer.
 *
 * @version  v1.0
 * @date     2021-08-01
 *
 */

#ifndef _SYS_TICK_LIB_H_
#define _SYS_TICK_LIB_H_

/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Smack
 * @{
 */


/** @addtogroup sys_tick_lib
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ms2ticks
#define ms2ticks(ms)    (ms * (XTAL / 1000UL))
#endif

/**
 * @brief Wait for "ticks" clock cycles on the SysTick timer using WFI to conserve power
 *        Note: The SysTick timer has a width of 24 bits. With a clock of 28MHz, the max. delay tme is approx. 500ms.
 * @param ticks number of clock cycles to wait
 */
extern void systick_singleshot_lib(const uint32_t ticks);


#ifdef __cplusplus
}
#endif

/** @} */ /* End of group sys_tick_lib */

/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */

#endif /* _SYS_TIM_WRAPPER_H_ */
