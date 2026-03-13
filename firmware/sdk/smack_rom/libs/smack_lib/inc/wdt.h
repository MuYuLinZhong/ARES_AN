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

/*
 * wdt.h
 *
 *  Created on: 19.06.2020
 *      Author: schnemat
 */


#ifndef LIBS_SMACK_LIB_INC_WDT_H_
#define LIBS_SMACK_LIB_INC_WDT_H_


#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WDT_UNLOCK_CODE 0x1ACCE551  //!< Access Code to Unlock the Locked Watchdog

typedef enum clk_scaling_ratio_e //!< enumeration type to define the clock scaling ratio between WDT and CPU
{
    wdt_clk1,    //!< wdt:cpu 1:1
    wdt_clk2,    //!< wdt:cpu 1:2
    wdt_clk4,    //!< wdt:cpu 1:4
    wdt_clk8,    //!< wdt:cpu 1:8
    wdt_clk16    //!< wdt:cpu 1:16
} clk_scaling_ratio_t;


/**
 * @brief This function initializes the watch dog timer.
 *        It must be called before accessing the watchdog timer to enable the WDT clock.
 */
extern void init_wdt(void);


/**
 * @brief This function disables the watch dog timer.
 *        After clearing the enable bit, also the WDT clock is switched off. init_wdt() must be called again before accessing the WDT.
 */
extern void disable_wdt(void);


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
extern void config_wdt(bool enable, bool en_irq, bool en_res, clk_scaling_ratio_t  clk_scaling_ratio);



/**
 * @brief This function locks the watch dog timer, in case the WDT is not in locked state.
 *        That means it it will put a write protection to WDT registers, so that a SW malfunction cannot write the WDT by accident.
 */
extern void lock_wdt(void);


/**
 * @brief This function unlocks the watch dog timer, in case the WDT is  in locked state.
 *        That means it it will remove a write protection from the WDT registers.
 */
extern void unlock_wdt(void);



/**
 * @brief This function will serve the watch dog timer, means the wdt count is reset to 0x0.
 *        It will unlock the WDT, serve and lock the WDT.
 *        If the service is not be done within the valid service window a wdt reset request is provided.
 */
extern void serve_wdt(void);


/**
 * @brief This function is the wdt irq service request procedure.
 *        It will serve the wdt interrupt and is not intended to be called directly.
 *        A user handler can be configured in the entry wdt_hand_addr in the APARAM block. If configured,
 *        it is called by this interrupt service routine after acknowledging the interrupt event in the WDT.
 */
extern void serve_wdtirq(void);


/**
 * @brief This function sets the upper and lower limit of the wdt service window.
 *        The service window is valid if wdt count is bigger/equal than lower limit and smaller than upper limit.
 *        Validity of wdt service window is returned by --> wdt_service_window();
 * @param lower_limit
 * @param upper_limit
 */
extern void set_wdt_limits(uint32_t lower_limit, uint32_t upper_limit);


/**
 * @brief This function checks, if the wdt service window is valid, when called.
 * @return wdt service window state
 *         true --> wdt count value is within the valid wdt service window when calling the function
 *         false --> wdt count value is not within the valid wdt service window when calling the function
 */
extern bool wdt_service_window(void);




#ifdef __cplusplus
}
#endif

#endif /* LIBS_SMACK_LIB_INC_WDT_H_ */
