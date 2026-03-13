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
 * @file     gpio_lib.h
 *
 * @brief    This module extends the GPIO driver of the ROM library.
 *
 * @version  v1.0
 * @date     2021-08-01
 *
 */

#ifndef INC_GPIO_LIB_H_
#define INC_GPIO_LIB_H_

/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Smack
 * @{
 */


/** @addtogroup gpio_lib
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif


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
extern uint8_t  gpio_config_set_lib(uint8_t gpio, const bool out_enable, const bool in_enable, const bool outtype, const bool pup, const bool pdown);


/**
 * @brief  This function configures the GPIO event interrupt generator.
 *         One out of up to 16 GPIO inputs can be selected as the source to the GPIO event interrupt generator that is able to detect edges
 *         in the signal at the GPIO input and use them as events to generate interrupts.
 *         The input signal is filtered with a configurable filter setting to suppress spikes.
 *         The GPIO event generator can be configured to generate an interrupt only on the nth occurance of an event. The event counter
 *         must be reset manually in the interrupt service routine.
 * @param  gpio       GPIO to select as the input to the GPIO event interrupt generator
 * @param  irq        interrupt number: 1...8 for event bus interrupts
 * @param  polarity_falling  changes polarity of the edge of the input signal that generates an event from rising to falling
 * @param  threshold  event counter threshold (20 bit). Set to 0 to disable the event counter and generate an interrupt on every GPIO event.
 * @param  filter     filter setting of the input filter to the GPIO event controller (valid values: 0...15, default: 0)
 * @return interrupt number if configuration was successful, 0 otherwise
 */
extern uint8_t gpio_int_edge_config_set(uint8_t gpio, uint8_t irq, bool polarity_falling, uint32_t threshold, uint32_t filter);


/**
 * @brief  This function clears the event counter in the GPIO event interrupt generator.
 *         When the GPIO event interrupt generator is configured with a threshold to generate an interrupt only on the nth event, the GPIO
 *         events are counted. Once the counter reaches the configured threshold, an interrupt is generated, but the counter just continues
 *         counting. In order to have an interrupt after the next n GPIO events, the event counter must be manually reset in the interrupt
 *         service routine by calling this function.
 */
extern void gpio_int_edge_counter_clear(void);


/**
 * @brief  This function disables the generation of further interrupts by the GPIO event interrupt generator.
 *         Generation of interrupts can be resumed by configuring them again with gpio_int_edge_config_set().
 */
extern void gpio_int_edge_disable(void);


/**
 * @brief  This function configures the GPIO level sensitive interrupts.
 *         The first two GPIO inputs can be configured as a source for level sensitive interrupts.
 *         An interrupt is generated as long as the input signal is a logic "1".
 *         To disable the interrupt generation, call this function with the same value for irq, and with gpio_mask set to 0.
 * @param  gpio_mask  GPIOs to select as the input to the GPIO level sensitive interrupt
 * @param  irq        interrupt number: only interrupt 9 is valid for the GPIO level sensitive interrupts
 * @return interrupt number if configuration was successful, 0 otherwise
 */
extern uint8_t gpio_int_level_config_set(uint32_t gpio_mask, uint8_t irq);


#ifdef __cplusplus
}
#endif

/** @} */ /* End of group gpio_lib */

/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */

#endif /* INC_GPIO_LIB_H_ */
