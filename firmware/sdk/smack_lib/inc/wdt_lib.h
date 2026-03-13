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
 * @file     wdt_lib.h
 *
 * @brief    This module provides helper functions for system access.
 *
 * @version  v1.0
 * @date     2021-08-01
 *
 */

#ifndef INC_WDT_LIB_H_
#define INC_WDT_LIB_H_

/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Smack
 * @{
 */


/** @addtogroup wdt_lib
 * @{
 */


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief This function disables the watch dog timer.
 *        After clearing the enable bit, also the WDT clock is switched off. init_wdt() must be called again before accessing the WDT.
 */
extern void wdt_disable_lib(void);


/**
 * @brief This function configures the watch dog timer unit.
 * @param bool enable --> if true, the wdt is enabled and counts
 * @param bool en_irq --> if true, the wdt irq generation is enabled. An IRQ is requested, if CPU is in sleep (WFI) and wdt upper limit is reached.
 * @param bool en_res --> if true, an reset request is issued, if wdt is served outside the valid wdt window or the wdt counter exceeds the upper limit
 * @param clk_scaling_ratio_t clk_scaling_ratio --> this defines the ratio between CPU clock and wdt counter clock
 *                                                  possible settings:
 *                                                  clk1 --> ratio wdt:cpu 1:1
 *                                                  clk2 --> ratio wdt:cpu 1:2
 *                                                  clk4 --> ratio wdt:cpu 1:4
 *                                                  clk8 --> ratio wdt:cpu 1:8
 *                                                  clk16--> ratio wdt:cpu 1:16
 */
extern void wdt_config_lib(bool enable, bool en_irq, bool en_res, clk_scaling_ratio_t  clk_scaling_ratio);


/**
 * @brief This function unlocks the watch dog timer, in case the WDT is  in locked state.
 *        That means it it will remove a write protection from the WDT registers.
 */
extern void wdt_unlock_lib(void);


#ifdef __cplusplus
}
#endif

/** @} */ /* End of group wdt_lib */

/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */

#endif /* INC_WDT_LIB_H_ */
