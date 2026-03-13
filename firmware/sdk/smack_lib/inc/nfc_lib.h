/* ============================================================================
** Copyright (c) 2023 Infineon Technologies AG
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


#ifndef _NFC_LIB_H_
#define _NFC_LIB_H_


/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Smack
 * @{
 */


/** @addtogroup nfc_lib
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief  This function enables or disables NFC energy harvesting.
 *         Note: Disabling NFC energy harvesting when running in passive mode (without
 *         external power supply through VCC) cuts the power supply to the device.
 * @param  harvest_enable: enable NFC energy harvesting
 */
extern void nfc_harvest(bool harvest_enable);

/**
 * @brief  This function returns the state of the NFC field.
 * @return true:  NFC field is present
 *         false: NFC field not present
 */
extern bool nfc_field_status_get(void);


#ifdef __cplusplus
}
#endif

/** @} */ /* End of group nfc_lib */

/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */

#endif
