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

/**
 * @file     hbctrl_lib.h
 *
 * @brief    This module provides functions to control the H-bridge.
 *
 * @version  v1.0
 * @date     2023-11-15
 *
 */

#ifndef HBCTRL_LIB_H_
#define HBCTRL_LIB_H_

/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Smack
 * @{
 */


/** @addtogroup hbctrl_lib
 * @{
 */

#include "hbctrl_drv.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function sets configuration parameters of the H-Bridge. The parameters are passed in a struct.
 *        Please note that some of the configuration parameters require up to 20us to take effect (e.g., activating ACL).
 * @param hb_config  struct holding configuration values
 */
extern void     hb_config_set_lib(const hb_config_struct_t* hb_config);

/* wrapper with old-style function name */
static inline void set_hb_config_lib(const hb_config_struct_t* hb_config)
{
    hb_config_set_lib(hb_config);
}

#ifdef __cplusplus
}
#endif

/** @} */ /* End of group hbctrl_lib */

/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */

#endif /* HBCTRL_LIB_H_ */
