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
 * @file     system_lib.h
 *
 * @brief    This module provides helper functions for system access.
 *
 * @version  v1.0
 * @date     2021-08-01
 *
 */

#ifndef INC_SYSTEM_LIB_H_
#define INC_SYSTEM_LIB_H_

/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Smack
 * @{
 */


/** @addtogroup system_lib
 * @{
 */

#include "pmu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Check if RF field is present.
 * @return true: RF field present
 */
extern bool check_rf_field(void);


/**
 * @brief  Read current vclamp setting.
 * @return vclamp setting
 */
extern uint8_t vclamp_get(void);


/**
 * @brief  Set vclamp setting.
 * @param  vclamp value (0...2 to select 3.0V, 3.3V or 3.6V)
 */
extern void vclamp_set(uint8_t value);


/**
 * @brief This function returns the wakeup source which caused to leave power save mode.
 * @return wakeup_source_t wake up source of recent wake/up return from power save mode (can be of value stb_tim, nfc, wakeup_pin, unclear (unclear means simultaneous set of  wake up events))
 */
extern wakeup_source_t get_wakeup_source_lib(void);

/**
 * @brief  This function returns the value stored in the 16-bit standby scratch pad register.
 *         This register retains its value during power save mode as long as the device is powered and can be used
 *         to store the state in which the firmware shall start when returning from power save mode.
 *         Bit 15 is modified by the ROM code and may return a value different from what was written with stb_scratch_pad_set().
 * @return 16-bit value of the standby scratch pad register.
 */
extern uint32_t stb_scratch_pad_get(void);

/**
 * @brief  This function sets the value in the 16-bit standby scratch pad register.
 *         Bit 15 is modified by the ROM code and may not return the written value when the register content is read with stb_scratch_pad_get().
 * @param  16-bit value of the standby scratch pad register.
 */
extern void stb_scratch_pad_set(uint32_t value);

/**
 * @brief request_power_saving_mode(bool wake_by_nfc, bool wake_by_stbtim, bool wake_by_wakeuppin, wakeup_pol_t wakeup_polarity)
 * @param bool wake_by_nfc --> if true, the wake up by nfc is enabled
 * @param bool wake_by_stbtim --> if true, the wake up by standby timer is enabled
 * @param bool wake_by_wakeuppin --> if true, the wake up by wakeup pin  is enable
 * @param wakeup_pol_t wakeup_polarity --> polarity of wakeup pin wake up event; can be rising or falling
 */
extern void request_power_saving_mode_lib(bool wake_by_nfc, bool wake_by_stbtim, bool wake_by_wakeuppin, wakeup_pol_t wakeup_polarity);


/**
 * @brief  This function reads the current setting of the high priority interrupt matrix for the given interrupt.
 * @param  irq   high prio interrupt number, valid values: 9...14
 * @return interrupt matrix setting
 */
extern uint32_t sys_int_hp_matrix_get(uint8_t irq);


/**
 * @brief  This function sets a new configuration in the high priority interrupt matrix of the given interrupt.
 * @param  irq   high prio interrupt number, valid values: 9...14
 * @param  value high prio matrix setting for interrupt number "irq"
 */
extern void sys_int_hp_matrix_set(uint8_t irq, uint32_t value);


#ifdef __cplusplus
}
#endif

/** @} */ /* End of group system_lib */

/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */

#endif /* INC_SYSTEM_LIB_H_ */
