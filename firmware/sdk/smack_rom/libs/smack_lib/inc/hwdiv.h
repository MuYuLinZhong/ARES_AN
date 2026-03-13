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
 * @file     hwdiv.h
 *
 * @brief    Driver for the hardware divider block.
 *
 * @version  v1.0
 * @date     2020-05-20
 *
 * @note
 */

/*lint -save -e960 */

#ifndef _HWDIV_H_
#define _HWDIV_H_

#include <stdbool.h>

/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Smack
 * @{
 */


/** @addtogroup divider
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief type of operands for divide operation. (signed or unsigned integer)
 */
typedef enum op_type_e
{
    div_u_u      = 0, //!< op1 op2 unsigned , unsigned
    div_u_s      = 1, //!< op1 op2 unsigned , signed
    div_s_u      = 2, //!< op1 op2 signed, unsigned
    div_s_s      = 3  //!< op1 op2 signed, signed
} op_type_t;

/**
 * @brief type of operation (divide or modulo)
 */
typedef enum calc_type_e
{
    division,//!< divide op1/op2
    modulo   //!< modulo op1 mod op2
} calc_type_t;



/**
 *
 * @brief This function calculates the value op1/op2 or (op1 mod op2). It returns the result of the selected calculation type (by calc_res)
 * @param op1 Operand 1 of operation op1/op2 or op1 mod op2
 * @param op2 Operand 2 of operation op1/op2 or op1 mod op2
 * @param op_formats  u_u (op1 unsigned, op2 unsigned), u_s (op1 unsigned, op2 signed), s_u (op1 signed, op2 unsigned), s_s (op1 signed, op2 signed)
 * @param calc_res    division --> result=op1/op2, modulo   --> result=op1 mod mod2
 * @return op1/op2 or (op1 mod op2) as uint32_t (depending on param clac_res)
 */
extern uint32_t calc_div(uint32_t op1, uint32_t op2, op_type_t op_formats, calc_type_t calc_res);

/**
 * @brief This function clears the error status flag of divider. (might be set by division through 0)
 * @param None
 * @return None
 */
extern void     clear_div_err(void);

/**
 *
 * @brief This function returns the division by 0  status error flag of divider. (It is set be set by division through 0)
 * @param None
 * @return bool --> true, means that division by 0 was requested to calculate
 */
extern bool    get_hwdiv_div0_state(void);

/**
 *
 * @brief This function returns the division overflow  status error flag of divider. (It is set be set, if the divider overflows, means cannot resolve the result).
 * @param None
 * @return bool --> true, means overflow happened
 */
extern bool    get_hwdiv_ovf_state(void);



#ifdef __cplusplus
}
#endif

/** @} */ /* End of group divider block */


/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */

#endif /* _HWDIV_H_ */
