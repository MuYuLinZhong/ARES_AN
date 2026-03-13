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
 * @file     smack_sl.h
 *
 * @brief    Smack NVM application code example file.
 *
 * @version  v1.0
 * @date     2020-05-20
 *
 * @note
 */

/*lint -save -e960 */

#ifndef _SMACK_SL_H_
#define _SMACK_SL_H_


/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Smack_sl
 * @{
 */


/** @addtogroup fw_config
 * @{
 */

extern void example_handler(void);
extern void hardfault_handler(void);

extern const uint8_t smack_sl_tag[];

// Offer a counter for external access
extern uint32_t sl_counter;


/** @} */ /* End of group fw_config */


/** @} */ /* End of group Smack_sl */

/** @} */ /* End of group Infineon */

#endif /* _SMACK_SL_H_ */
