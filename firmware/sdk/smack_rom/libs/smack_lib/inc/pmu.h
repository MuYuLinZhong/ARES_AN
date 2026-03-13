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
 * @file     pmu.h
 *
 * @brief    Driver for the power management unit.
 *
 * @version  v1.0
 * @date     2020-05-20
 *
 * @note
 */

/*lint -save -e960 */

#ifndef _PMU_H_
#define _PMU_H_

#include <stdbool.h>

#define WAIT_ABOUT_2MS   0x10000  //!< clock tick constant ~2ms @ 28MHz
#define WAIT_ABOUT_1MS   0x8000   //!< clock tick constant ~1ms @ 28MHz
#define WAIT_ABOUT_500US 0x4000   //!< clock tick constant ~500us @ 28MHz


/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Smack
 * @{
 */


/** @addtogroup pmu
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum wakeup_source_e  //!< enum type of sources for transition power saving Mode to Operating Mode
{
    wakeup_stb_tim,  //!< Standby Timer
    wakeup_nfc,      //!< NFC Field presence
    wakeup_pin,  //!< Wake Up Pin
    wakeup_unclear     //!< only useful for getting the wakeup status, is used if simultaneous wakeup at different enabled wakeup sources occurs
} wakeup_source_t;

typedef enum wakeup_pol_e   //!< defines the polarity of edge transition at wakeup pin to trigger a wakeup event
{
    wakeup_rising,
    wakeup_falling
} wakeup_pol_t;



//extern void     get_wake_up_state(void);

/**
 * @brief This function configures the standby timer. It enables/ disables the timer function in power save mode and sets the standby wake time.
 * @param bool on_off --> if set to to true, then the standby timer will be active during power save mode (value out of true or false)
 * @param uint32_t standby_time --> 32-bit values which represents the standby wake up time in clock ticks of slow clock
 */
extern void     config_stbtm(bool on_off, uint32_t standby_time);


/**
 * @brief This function returns the standby time in clock ticks of the slow clock of the previous power save mode. It is useful, if wake up sources beyond the standby timer are configured.
 * @return uint32_t --> standby time in clock ticks of the slow clock of the previous power save mode
 */
extern uint32_t get_standby_time(void);

/**
 * @brief This function returns the standby time in clock ticks of the slow clock of the previous power save mode. It is useful, if wake up sources beyond the standby timer are configured.
 * @return wakeup_source_t wake up source of recent wake/up return from power save mode (can be of value stb_tim, nfc, wakeup_pin, unclear (unclear means simultaneous set of  wake up events))
 */
extern wakeup_source_t get_wakeup_source(void);

/**
 * @brief request_power_saving_mode(bool wake_by_nfc, bool wake_by_stbtim, bool wake_by_wakeuppin, wakeup_pol_t wakeup_polarity)
 * @param bool wake_by_nfc --> if true, the wake up by nfc is enabled
 * @param bool wake_by_stbtim --> if true, the wake up by standby timer is enabled
 * @param bool wake_by_wakeuppin --> if true, the wake up by wakeup pin  is enable
 * @param wakeup_pol_t wakeup_polarity --> polarity of wakeup pin wake up event; can be rising or falling
 */
extern void     request_power_saving_mode(bool wake_by_nfc, bool wake_by_stbtim, bool wake_by_wakeuppin, wakeup_pol_t wakeup_polarity);


/**
 * @brief IFX specific routine for manufacturing and maintenance, to be executed after power on reset.
 */
extern void     dft_preamble1(void);

/**
 * @brief IFX specific routine for manufacturing and maintenance, to be executed after power on reset.
 */
extern void     dft_preamble2(void);

/**
 * @brief This function is checking the power supply source after a power on reset cycle. Smack might be supplied from NFC field or external supply (or both).
 *        The function detects the supply scenario and cares for the proper PMU setting after power on reset.
 */
extern void     check_supply_source(void);


/**
 * @brief This function sets up and starts the systick timer in a single shot mode. After the systick is started, the CPU is set into sleep by "WFI".
 *        If called within a handler/IRQ routine, make sure, that systick IRQ prio is high enough to leave WFI
 * @param uint32_t time the number of clock ticks of timer cycle
 */
extern void     single_shot_systick(uint32_t time);



#ifdef __cplusplus
}
#endif

/** @} */ /* End of group pmu block */


/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */


#endif /* _PMU_H_ */
