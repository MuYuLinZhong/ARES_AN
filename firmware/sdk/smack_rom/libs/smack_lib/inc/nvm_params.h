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
 * @file     nvm_params.h
 *
 * @brief    Definitions for the Smack NVM.
 *           The offset parameter definitions define the offset address to NVM start for NVM parameter fields
 *           This parameter fields contain system-, application-, production- data of the chip.
 *
 *           Caveat: Most of the definitions in this files, e.g. the offsets within APARAM and DPARAM, are
 *           obsolete and will be removed in future. Please use the members of the Aparams_t and Dparams_t
 *           instead when reading their values.
 *
 * @version  v1.0
 * @date     2020-05-20
 *
 * @note
 */

/* lint -save -e960 */


#ifndef _NVM_PARAMS_H_
#define _NVM_PARAMS_H_

#include "dparam.h"
#include "aparam.h"
#include "nvm.h"        // definition of nvm_base

/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Smack
 * @{
 */


/** @addtogroup NVM_PARAMS
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif


#define DEBUGGER_OFF                  (0x5deb0ff5)      //!< 0x5deb0ff5 will block debugger access
#define BYPASS_MAILBOX                (0xfca1fcb0)      //!< 0xfca1fcb0 will bypass mailbox address check
#define DISABLE_DEFAULT_UART          (0x11223344)      //!< 0x11223344 disable UART per ROM code
#define MESSAGE_AND_CALL_ENABLE       (0xffffffff)      //!< <>0xffffffff: ignore external send message req (enabled when NVM is erased to enable access to new devices - configure to a different value for products)
#define GPIO_VALUE_VALID              (0xabba)          //!< Default unique value for valid indication of GPIO params is gpio_value_valid
#define GPIO_ALT_VALID                (0xabbaabba)      //!< default value 0xABBAABBA, indicates a valid alternate function config used by read_gpio_alt_nvm_config()

/**
 * @brief Configuration values for accessing memory space through mailbox commands.
 *        These values apply to the APARAM fields prot_rom1 through prot_hw3.
 */
enum nvm_prot_e
{
    nvm_prot_no_access  = 0x00,
    nvm_prot_read_only  = 0xf0,
    nvm_prot_write_only = 0x0f,
    nvm_prot_read_write = 0xff
};

extern Dparams_t const dparams __attribute__ ((section (".nvm.DPARAMS")));
extern Aparams_t const aparams __attribute__ ((section (".nvm.APARAMS")));


#ifdef __cplusplus
}
#endif

/** @} */ /* End of group NVM_PARAMS */


/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */

#endif /* _NVM_PARAMS_H_ */
