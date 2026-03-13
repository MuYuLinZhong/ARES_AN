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
 * @file     sys_tim_drv.h
 *
 * @brief    Driver for the system timer block.
 *
 * @version  v1.0
 * @date     2020-05-20
 *
 * @note
 */

/*lint -save -e960 */

#ifndef _SYS_TIM_DRV_H_
#define _SYS_TIM_DRV_H_

#include <stdint.h>
#include <stdbool.h>

/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Smack
 * @{
 */


/** @addtogroup sys_tim
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Polarity of PWM Signal
 */
enum sys_tim_polarity_e
{
    sys_tim_normal, //!< sys_tim_normal
    sys_tim_inverted//!< sys_tim_inverted
};

/**
 * @brief Type to Define the Event Control based Start/ Stop Control
 */
enum sys_tim_cntrl_e
{
    sys_tim_event,  //!< event bus
    sys_tim_hardware//!< hardware means High Prio Matrix Event
};

/**
 * @brief Type to Define the Timer Operation Mode
 */
enum sys_tim_mode_e
{
    sys_tim_continous, //!< continous
    sys_tim_single_shot//!< single_shot
};

/**
 * @brief data structure type to configure the system timer operation
 */
typedef struct sys_tim_config_struct
{
    bool enable;                         //!< timer enable
    enum sys_tim_cntrl_e start_control;          //!< start of timer event source (event bus or matrix)
    enum sys_tim_cntrl_e stop_control;           //!< stop of timer event source (event bus or matrix)
    bool en_start;                       //!< enable eventbus/ matrix controled timer start
    bool en_stop;                        //!< enable eventbus/ matrix controled timer stop
    enum sys_tim_mode_e tim_mode;            //!< timer mode --> continous or single shot
    bool chain;                          //!< if true, channel chaining is enabled
} sys_tim_config_struct_t;

/**
 * @brief Used to control the timer per software access
 */
enum sys_tim_control_E
{
    sys_tim_start,    //!< start
    sys_tim_stop      //!< stop
};


//!< Constants to be used for event control register programming --> irq section
#define NO_INT  0x00
#define EV_NMI  0x40
#define EV_IRQ1 0x21
#define EV_IRQ2 0x22
#define EV_IRQ3 0x23
#define EV_IRQ4 0x24
#define EV_IRQ5 0x25
#define EV_IRQ6 0x26
#define EV_IRQ7 0x27
#define EV_IRQ8 0x28

//!< Constants to be used for event control register programming --> adc section
#define NO_ADC              0x00
#define EV_ADC_ARM_SH0      0x81
#define EV_ADC_ARM_SH1      0x82
#define EV_ADC_ARM_SH0_SH1  0x83
#define EV_ADC_TRIG_SH0     0xC1
#define EV_ADC_TRIG_SH1     0xC2
#define EV_ADC_TRIG_SH0_SH1 0xC3



/**
* @brief This function enables the PWM signal generation of timer channel#0
* @param bool enable --> if true, then the PWM signal generation is enabled
* @param enum polarity_t polarity --> can be normal or inverted; normal means PWM is HIGH if timercount < duty; inverted means PWM is LOW if timercount < duty;
* @param uint32_t duty --> duty cycle in ticks, should be smaller then period value of timer 0 to make sense
* @return None
*/
extern void     sys_tim_pwm_cfg(const bool enable, const enum sys_tim_polarity_e polarity, const uint32_t duty);

/**
* @brief This function returns back the current status of the PWM signal
* @param   None
* @return an 32-bit unsigned integer; can be either 0x0 or 0x1
*/
extern uint32_t get_pwm_status(void);

/**
 * @brief This function configures the system timer channel, selected by uint32_t channel
 * @param sys_tim_config --> channel configuration structure
 *                           bool enable --> if true, then timer channel enabled
 *                           enum cntrl_t start_control --> timer start source  either event bus or direct HW signal/line
 *                           enum cntrl_t stop_control; --> timer stop either by ...as 1 line above
 *                           bool en_start --> if true, then start source HW line or eventbus is enabled
 *                           bool en_stop --> if true, then stop source HW line or eventbus is enabled
 *                           enum tim_mode_t tim_mode --> can be single shoot or contionous
 *                           bool chain --> if true, then the channel is chainned with neighboured channel to increase timer period value from 16 to 32, 48, ... bit
 * @param channel --> system timer channel (can be between 0x0 to 0x5)
 */
extern void     sys_tim_chn_cfg(const sys_tim_config_struct_t* sys_tim_config, const uint32_t channel);

/**
 * @brief This function controls the system timer channel, selected by uint32_t channel
 * @param start_stop --> controls the FW start/ stop, can bei either start (will start timer) or stop (will stop timer)
 * @param channel --> system timer channel (can be between 0x0 to 0x5)
 */
extern void     sys_tim_chn_control(const enum sys_tim_control_E start_stop, const uint32_t channel);

/**
 * @brief This function sets the timer period  of the selected channel, 16 valid bits
 * @param period --> number of clock ticks
 * @param channel --> system timer channel (can be between 0x0 to 0x5)
 */
extern void     set_sys_tim_chn_period(const uint32_t period, const uint32_t channel);

/**
 * @brief This function returns the timer period  of the selected channel, 16 valid bits
 * @param channel --> system timer channel (can be between 0x0 to 0x5)
 */
extern uint32_t get_sys_tim_chn_period(const uint32_t channel);

/**
 * @brief This function returns the value of the timer counter of the selected channel
 * @param channel --> system timer channel (can be between 0x0 to 0x5)
 * @return an unsigned 32-bit value representing the current timer value
 */
extern uint32_t get_sys_tim_chn_timecount(const uint32_t channel);

/**
 * @brief This function returns the activity status of the selected timer channel
 * @param channel --> system timer channel (can be between 0x0 to 0x5)
 * @return an unsigned 32-bit value representing the current activity status ; 0x0 --> timer stopped, 0x1 --> timer running
 */
extern uint32_t get_sys_tim_chn_status(const uint32_t channel);

/**
 * @brief this functions configures the events to be transmitted  to event bus by the selected channel, if channel period matches
 * @param en_prio --> enables the period match output of timer channel to hprio matrix
 * @param irq_event --> see constant definition --> irq section
 * @param adc_event --> see constant definition --> adc section
 * @param event_code --> see event.h the list of possible codes
 * @param channel  --> system timer channel (can be between 0x0 to 0x5)
 */
extern void     sys_tim_chn_evnt_cfg(const uint8_t en_hprio, const uint8_t irq_event, const uint8_t adc_event, const uint32_t event_code, const uint32_t channel);

/**
 * @brief This is a single test function, which starts a selected timer channel in single shot mode and creates a HP matrix IRQ, when timer expires
 * @param channel --> system timer channel (can be between 0x0 to 0x5)
 * @param period --> period in clock ticks
 * @param irq_number --> irq number
 */
extern void my_systim_singleshot(const uint8_t channel, const uint16_t period, const uint8_t irq_number);

/**
 * @brief IRQ service request function of timer channel 0. called in respective Handler Function. Might execute custom code in NVM, if available.
 */
extern void serve_systim0_irq(void);

/**
 * @brief IRQ service request function of timer channel 1. called in respective Handler Function. Might execute custom code in NVM, if available.
 */
extern void serve_systim1_irq(void);

/**
 * @brief IRQ service request function of timer channel 2. called in respective Handler Function. Might execute custom code in NVM, if available.
 */
extern void serve_systim2_irq(void);

/**
 * @brief IRQ service request function of timer channel 3. called in respective Handler Function. Might execute custom code in NVM, if available.
 */
extern void serve_systim3_irq(void);

/**
 * @brief IRQ service request function of timer channel 4. called in respective Handler Function. Might execute custom code in NVM, if available.
 */
extern void serve_systim4_irq(void);

/**
 * @brief IRQ service request function of timer channel 5. called in respective Handler Function. Might execute custom code in NVM, if available.
 */
extern void serve_systim5_irq(void);

#ifdef __cplusplus
}
#endif

/** @} */ /* End of group sys_tim block */


/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */


#endif /* _SYS_TIM_DRV_H_ */
