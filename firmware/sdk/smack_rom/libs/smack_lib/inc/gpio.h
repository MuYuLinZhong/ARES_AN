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
 * @file     gpio.h
 *
 * @brief    HW abstraction layer for GPIOs.
 *
 * @version  v1.0
 * @date     2020-05-20
 *
 * @note
 */

/*lint -save -e960 */

#ifndef _GPIO_H_
#define _GPIO_H_

#include <stdint.h>
#include <stdbool.h>


/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Smack
 * @{
 */


/** @addtogroup gpio
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif


#if !defined _NO_ROM_FUNC_TABLE_WRAPPER_ && !defined _ROM_LIB_ROM_H_

#include "cmsis_compiler.h"

/**********************************************************************
 * The following GPIO functions are preferred for use in NVM firmware projects.
 * They follow a similar syntax as provided in SDKs for similar plarforms.
 */

/**
 * @brief  This function configures the GPIOs with a configuration stored in the NVM. The function checks, if valid config parameter
 *         are available in the NVM and configures the GPIOs accordingly.
 * @param  None
 * @return None
 */
__STATIC_FORCEINLINE void gpio_nvm_config_read(void)
{
    rom_func_table.m_read_gpio_nvm_config();
}


/**
 * @brief  This function configures the ALT Function of GPIOs with a configuration stored in the NVM. The function checks, if valid config parameter
 *         are available in the NVM and configures the ALT functions accordingly.
 * @param  None
 * @return None
 */
__STATIC_FORCEINLINE void gpio_nvm_alt_config_read(void)
{
    rom_func_table.m_read_gpio_alt_nvm_config();
}

/**
 * @brief  This function configures the characteristic of all GPIOs simultaneously.
 *         For each configuration setting, a 16-bit value is provided.
 *         bit #0 represents GPIO0 and bit #15 represents GPIO15 (GPIO corresponds to bit position)
 * @param  out_enable if bit[x] is 0x1, then the output driver of GPIOx is enabled
 * @param  in_enable  if bit[x] is 0x1, then the input buffer of GPIOx is enabled
 * @param  outtype    if bit[x] is 0x1, then the output x is a Push Pull CMOS output, else Open Drain Output
 * @param  pup        if bit[x] is 0x1, then the internal Pull Up of IO x is enabled
 * @param  pdown      if bit[x] is 0x1, then the internal Pull Down of IO x is enabled
 * @return value 0x0 per default, if configuration parameters are correct and don't contradict (e.g. enable of both PU and PD)
 */
__STATIC_FORCEINLINE uint8_t gpio_config_set_all(const uint8_t out_enable, const uint8_t in_enable, const uint32_t outtype, const uint8_t pup, const uint8_t pdown)
{
    return rom_func_table.m_all_gpios_iocfg(out_enable, in_enable, outtype, pup, pdown);
}

/**
 * @brief  This function configures the characteristic of the GPIO specified by parameter uint8_t gpio.
 * @param  gpio       gpio number (should be between 0 to 15)
 * @param  out_enable if true, then the output driver of GPIO is enabled
 * @param  in_enable  if true, then the input buffer of GPIO is enabled
 * @param  outtype    if true, then the output is a Push Pull CMOS output, else Open Drain Output
 * @param  pup        if true, then the internal Pull Up of IO is enabled
 * @param  pdown      if true, then the internal Pull Down of IO is enabled
 * @return value 0x0 per default, if configuration parameters are correct and don't contradict (e.g. enable of both PU and PD)
 */
__STATIC_FORCEINLINE uint8_t  gpio_config_set(uint8_t gpio, const bool out_enable, const bool in_enable, const bool outtype, const bool pup, const bool pdown)
{
    return rom_func_table.m_single_gpio_iocfg(out_enable, in_enable, outtype, pup, pdown, gpio);
}

/**
 * @brief  This function writes the 16GPIOs
 *         It sets the value of the 16 GPIOs to the value specified by uint16_t value.
 *         bit #0 represents GPIO0 and bit #15 represents GPIO15 (GPIO corresponds to bit position)
 *         The value appears at the GPIO, if the GPIO output driver is enabled and no alternate output path is selected
 * @param  value      the ouput value of the 16 GPIOs (GPIO  corresponds to bit position)
 */
__STATIC_FORCEINLINE void     gpio_out_set_all(const uint16_t value)
{
    rom_func_table.m_set_allgpios_out(value);
}

/**
 * @brief  This function writes the GPIO selected by parameter uint8_t gpio
 *         It sets the value of the specified GPIO to the value specified by uint8_t value, which should be 0x0 or 0x1)
 *         The value appears at the GPIO, if the GPIO output driver is enabled and no alternate output path is selected
 * @param  gpio       the GPIO, which shall be set
 * @param  value      the output value of the  GPIO (0x0 or 0x1)
 */
__STATIC_FORCEINLINE void     gpio_out_set(uint8_t gpio, bool value)
{
    rom_func_table.m_set_singlegpio_out(value, gpio);
}

/**
 * @brief  This function reads the status/ logical level of all 16 GPIOs of Smack
 *         It returns a 16- bit integer, where bit #0 represents GPIO0 and bit #15 represents GPIO15 (GPIO corresponds to bit position)
 * @return input status of the 16 GPIOs (GPIO  corresponds to bit position)
 */
__STATIC_FORCEINLINE uint16_t gpio_in_get_all(void)
{
    return rom_func_table.m_get_allgpios_in();
}

/**
 * @brief  This function reads the status/ logical level of the GPIO, selected by uint8_t gpio
 *         e.g. to read GPIO#3 get_singlegpio_in(3);
 * @param  gpio       gpio is the number of the gpio, should be in the range of 0...15
 * @return input status of the selected GPIO by parameter uint8_t gpio; is either 0x0 or 0x1
 */
__STATIC_FORCEINLINE bool     gpio_in_get(uint8_t gpio)
{
    return (rom_func_table.m_get_singlegpio_in(gpio) != 0);
}


/**
 * @brief  This function selects alternate functions for input and output of the GPIO, selected by uint8_t gpio
 * @param  gpio       gpio is the number of the gpio, should be in the range of 0...15
 * @param  ain_en     alternate input (see manual to find the port specific settings)
 * @param  outsel     alternate output (see manual to find the port specific settings)
 */
__STATIC_FORCEINLINE void     gpio_alt_config_set(uint8_t gpio, uint8_t ain_en, uint8_t outsel)
{
    rom_func_table.m_set_singlegpio_alt(gpio, ain_en, outsel);
}

#endif


/**********************************************************************
 * The following GPIO functions shall not be used for future NVM projects.
 * This syntax was used in the first versions of the NAC1080/NGC1081 SDK
 * and the functions are provided for compatibility reasons. They may be
 * removed in future versions of the SDK.
 * Please use the functions above for new firmware projects.
 */


/**
 * @brief This function configures the GPIOs with a configuration stored in the NVM. The function checks, if valid config parameter
 *        are available in the NVM and configures the GPIOs accordingly.
 * @param  None
 * @return None
 */
extern void read_gpio_nvm_config(void);


/**
 * @brief This function configures the ALT Function of GPIOs with a configuration stored in the NVM. The function checks, if valid config parameter
 *        are available in the NVM and configures the ALT functions accordingly.
 * @param  None
 * @return None
 */
extern void read_gpio_alt_nvm_config(void);

/**
 * @brief This function configures the characteristic of all GPIOs simultaneously.
 *        For each configuration setting, a 16-bit value is provided.
 *        bit #0 represents GPIO0 and bit #15 represents GPIO15 (GPIO corresponds to bit position)
 * @param out_enable uint16_t out_enable --> if bit[x] is 0x1, then the output driver of GPIOx is enabled
 * @param in_enable uint16_t in_enable  --> if bit[x] is 0x1, then the input buffer of GPIOx is enabled
 * @param outtype uint16_t outtype    --> if bit[x] is 0x1, then the output x is a Push Pull CMOS output, else Open Drain Output
 * @param pup uint16_t pup        --> if bit[x] is 0x1, then the internal Pull Up of IO x is enabled
 * @param pdown uint16_t pdown      --> if bit[x] is 0x1, then the internal Pull Down of IO x is enabled
 * @return value 0x0 per default, if configuration parameters are correct and don't contradict (e.g. enable of both PU and PD)
 */
extern uint8_t  all_gpios_iocfg(const uint16_t out_enable, const uint16_t in_enable, const uint16_t outtype, const uint16_t pup, const uint16_t pdown );

/**
 * @brief This function configures the characteristic of the GPIO specified by parameter uint8_t gpio.
 * @param out_enable bool out_enable --> if true, then the output driver of GPIO is enabled
 * @param in_enable bool in_enable --> if true, then the input buffer of GPIO is enabled
 * @param outtype bool outtype  --> if true, then the output is a Push Pull CMOS output, else Open Drain Output
 * @param pup bool pup --> if true, then the internal Pull Up of IO is enabled
 * @param pdown bool pdown --> if true, then the internal Pull Down of IO is enabled
 * @param gpio gpio number (should be between 0 to 15)
 * @return value 0x0 per default, if configuration parameters are correct and don't contradict (e.g. enable of both PU and PD)
 */
extern uint8_t  single_gpio_iocfg(const bool out_enable, const bool in_enable, const bool outtype, const bool pup, const bool pdown, uint8_t gpio);

/**
 * @brief This function writes the 16GPIOs
 *        It sets the value of the 16 GPIOs to the value specified by uint16_t value.
 *        bit #0 represents GPIO0 and bit #15 represents GPIO15 (GPIO corresponds to bit position)
 *        The value appears at the GPIO, if the GPIO output driver is enabled and no alternate output path is selected
 * @param value uint16_t value --> the ouput value of the 16 GPIOs (GPIO  corresponds to bit position)
 */
extern void     set_allgpios_out(const uint16_t value);

/**
 * @brief This function writes the GPIO selected by parameter uint8_t gpio
 *        It sets the value of the specified GPIO to the value specified by uint8_t value, which should be 0x0 or 0x1)
 *        The value appears at the GPIO, if the GPIO output driver is enabled and no alternate output path is selected
 * @param value uint8_t value --> the output value of the  GPIO (0x0 or 0x1)
 * @param gpio uint8_t gpio  --> the GPIO, which shall be set
 */
extern void     set_singlegpio_out(uint8_t value, uint8_t gpio);

/**
 * @brief This function reads the status/ logical level of all 16 GPIOs of Smack
 *        It returns a 16- bit integer, where bit #0 represents GPIO0 and bit #15 represents GPIO15 (GPIO corresponds to bit position)
 * @return uint16_t input status of the 16 GPIOs (GPIO  corresponds to bit position)
 */
extern uint16_t get_allgpios_in(void);

/**
 * @brief This function reads the status/ logical level of the GPIO, selected by uint8_t gpio
 *        e.g. to read GPIO#3 get_singlegpio_in(3);
 * @param gpio uint8_t gpio --> gpio is the number of the gpio, should be in the range of 0...15
 * @return uint8_t input status of the selected GPIO by parameter uint8_t gpio; is either 0x0 or 0x1
 */
extern uint8_t  get_singlegpio_in(uint8_t gpio);


/**
 * @brief This function selects alternate functions for input and output of the GPIO, selected by uint8_t gpio
 * @param gpio uint8_t gpio   --> gpio is the number of the gpio, should be in the range of 0...15
 * @param ain_en uint8_t ain_en --> alternate input (see manual to find the port specific settings)
 * @param outsel uint8_t outsel --> alternate output (see manual to find the port specific settings)
 */
extern void     set_singlegpio_alt(uint8_t gpio, uint8_t ain_en, uint8_t outsel);


/***********************************************************************
 * End of deprecated functions
 **********************************************************************/


// IRQs
/**
 * @brief IRQ service request routine for gpio event, called in respective Handler function
 */
extern void serve_gpio_event_irq(void);

/**
 * @brief IRQ service request routine for gpio event generator, called in respective Handler function
 */
extern void serve_gpio_event_gen_irq(void);

/**
 * @brief IRQ service request routine for gpio #0, called in respective Handler function
 */
extern void serve_gpin0_irq(void);

/**
 * @brief IRQ service request routine for gpio #1, called in respective Handler function
 */
extern void serve_gpin1_irq(void);

/**
 * @brief IRQ service request routine for gpio #2, called in respective Handler function
 */
extern void serve_gpin2_irq(void);

/**
 * @brief IRQ service request routine for gpio #3, called in respective Handler function
 */
extern void serve_gpin3_irq(void);

/**
 * @brief IRQ service request routine for gpio #4, called in respective Handler function
 */
extern void serve_gpin4_irq(void);

/**
 * @brief IRQ service request routine for gpio #5, called in respective Handler function
 */
extern void serve_gpin5_irq(void);

/**
 * @brief IRQ service request routine for gpio #6, called in respective Handler function
 */
extern void serve_gpin6_irq(void);

/**
 * @brief IRQ service request routine for gpio #7, called in respective Handler function
 */
extern void serve_gpin7_irq(void);


#ifdef __cplusplus
}
#endif

/** @} */ /* End of group gpio block */


/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */

#endif /* _GPIO_H_ */
