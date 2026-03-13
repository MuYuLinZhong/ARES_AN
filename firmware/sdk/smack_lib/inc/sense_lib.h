/* ============================================================================
** Copyright (c) 2024 Infineon Technologies AG
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
 * @file     sense_lib.h
 *
 * @brief    This module provides functions for the sensing unit.
 *
 * @version  v1.0
 * @date     2024-07-09
 *
 */

#ifndef SENSE_LIB_H_
#define SENSE_LIB_H_

/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Smack
 * @{
 */


/** @addtogroup sense_lib
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief  Calculate temperature from raw value of internal temperature sensor measured by sensing unit.
 * @param  raw   raw ADC value as measured by sensing unit
 * @return temperature value in unit 0.1 degree Celsius, e.g., a raw value of 1876 that stands for a temperature
 *         of 21.9 degrees Celsius will be converted to and returned as a signed integer value of +219.
 */
static inline int32_t temperature_raw2celsius(const uint16_t raw)
{
    return (((int32_t)raw - 1238) * 1100 / 3193);
}


/**
 * @brief  Calculate temperature from raw value of internal temperature sensor measured by sensing unit.
 * @param  raw   raw ADC value as measured by sensing unit
 * @return temperature value in unit 0.1 degree Fahrenheit, e.g., a raw value of 1876 that stands for a temperature
 *         of 71.5 degrees Fahrenheit will be converted to and returned as a signed integer value of +715.
 */
static inline int32_t temperature_raw2fahrenheit(const uint16_t raw)
{
    return (((int32_t)raw - 722) * 1980 / 3193);
}


#ifdef __cplusplus
}
#endif

/** @} */ /* End of group sense_lib */

/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */

#endif /* SENSE_LIB_H_ */
