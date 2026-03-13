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
 * @file     handlers.h
 *
 * @brief    Interupt service handlers.
 *
 * @version  v1.0
 * @date     2020-05-20
 *
 * @note
 */

/*lint -save -e960 */

#ifndef _HANDLERS_H_
#define _HANDLERS_H_

/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Smack
 * @{
 */


/** @addtogroup handlers
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

//!< Default IRQ Assignment
//!< NFC IRQ 0x0
#define SENSE_ADC_IRQ     0x1 //!< Event Bus
#define TIMER0_IRQ        0x2 //!< Event Bus
#define TIMER1_IRQ        0x3 //!< Event Bus
#define TIMER2_IRQ        0x4 //!< Event Bus
#define TIMER3_IRQ        0x5 //!< Event Bus
#define GPIO_EVNT_IRQ     0x6 //!< Event Bus
#define GPIO_EVNT_GEN_IRQ 0x7 //!< Event Bus
#define TIMER4_IRQ        0x8 //!< Event Bus
#define TIMER5_IRQ        0x9 //!< HP Matrix
#define UART_IRQ          0xA //!< HP Matrix
#define SSP_IRQ           0xB //!< HP Matrix
#define I2C_IRQ           0xC //!< HP Matrix
#define GPIO0_IRQ         0xD //!< HP Matrix
#define GPIO1_IRQ         0xE //!< HP Matrix
//!< wdt_irq 0xF
//!< hwdiv_irq 0x10
//!< nvm_irq 0x11
//!< other IRQ sources, can be put to HP Matrix
#define GPIO2_IRQ         0xF  //!< could be assigned to HP Matrix
#define GPIO3_IRQ         0x10 //!< could be assigned to HP Matrix
#define GPIO4_IRQ         0x11 //!< could be assigned to HP Matrix
#define GPIO5_IRQ         0x12 //!< could be assigned to HP Matrix
#define GPIO6_IRQ         0x13 //!< could be assigned to HP Matrix
#define GPIO7_IRQ         0x14 //!< could be assigned to HP Matrix
#define AES_IRQ           0x15 //!< could be assigned to HP Matrix
#define HFAULT_IDX        0x16 //!< offset index to get NVM address of custom handler function
#define SYSTICK_IDX       0x17 //!< offset index to get NVM address of custom handler function
#define WDT_IDX           0x18 //!< offset index to get NVM address of custom handler function
#define NVM_IDX           0x19 //!< offset index to get NVM address of custom handler function






//!< Define Matrix IRQ Sources


//!< Eventbus IRQ Sources
#define SENSE_ADC_IRQ_HAND     0x1
#define TIMER0_IRQ_HAND        0x2
#define TIMER1_IRQ_HAND        0x3
#define TIMER2_IRQ_HAND        0x4
#define TIMER3_IRQ_HAND        0x5
#define GPIO_EVNT_HAND         0x6
#define GPIO_EVNT_GEN_HAND     0x7
#define TIMER4_IRQ_HAND        0x8
#define TIMER5_IRQ_HAND        0x9

//!< HP Matrix IRQ Sources
//!< content of e.g. HP_IRQ9_COL_CFG_offset
//!< values contain the column value config register
#define UART_IRQ_HAND  0x1
#define SSP_IRQ_HAND   0x2
#define I2C_IRQ_HAND   0x4
#define UDMA_IRQ_HAND  0x8
#define PM0_IRQ_HAND   0x10
#define PM1_IRQ_HAND   0x20
#define PM2_IRQ_HAND   0x40
#define PM3_IRQ_HAND   0x80
#define PM4_IRQ_HAND   0x100
#define PM5_IRQ_HAND   0x200
#define GPIO0_IRQ_HAND 0x400
#define GPIO1_IRQ_HAND 0x800
#define GPIO2_IRQ_HAND 0x1000
#define GPIO3_IRQ_HAND 0x2000
#define GPIO4_IRQ_HAND 0x4000
#define GPIO5_IRQ_HAND 0x8000
#define GPIO6_IRQ_HAND 0x10000
#define GPIO7_IRQ_HAND 0x20000
#define AES_IRQ_HAND   0x40000








/* IRQ handler for hard fault errors */
/**
 * @brief IRQ handler for hard fault errors
 *        It sets an indication flag in SCUS scratch Pad Register: Value 0xACDC (bits 12...15 are used by ROM for further signalling and must not be changed by user)
 *        It checks, if there is a custom code in NVM to be executed (value at address (nvm_base + application_params2_offset + ((hfault_idx - 1) * 4))
 *        Finally a system reset request is performed.
 */
extern void HardFault_Handler(void);

/* IRQ handler for SVC calls */
/**
 * @brief IRQ handler for SVC calls; not used so far; empty function body
 */
extern void SVC_Handler(void);

/* IRQ handler for pend SVC calls */
/**
 * @brief IRQ handler for pend SVC calls; not used so far; empty function body
 */
extern void PendSV_Handler(void);

/* SysTick IRQ handler */
/**
 * @brief SysTick IRQ handler
 *        It checks, if there is a custom code in NVM to be executed (value at address (nvm_base + application_params2_offset + ((systick_idx - 1) * 4))
 */
extern void SysTick_Handler(void);

/**
 * @brief NFC interrupt handler; receives/ transmits data from/ to CL_UART and maintains the NFC state machine (14443-A)
 */
extern void Cl_uart_Handler(void);   //!< IRQ0

/**
 * @brief It clears EventBus IRQ #1 and decodes the related service request IR service routine, associated to this EventBus IRQ
 *        The associated service request IR service routine (e.g. serve_adc_irq()) is a called and executed
 *        The IRQ service request routine decoding is based on value at NVM address nvm_base  + application_params_offset + EVBUS_HANDLER1_SOURCE_offset
 *        If no valid value for decoding is found, then serve_adc_irq() is executed
 */
extern void Event_Bus1_Handler(void); //!< IRQ1

/**
 * @brief It clears EventBus IRQ #2 and decodes the related service request IR service routine, associated to this EventBus IRQ
 *        The associated service request IR service routine (e.g. serve_adc_irq()) is a called and executed
 *        The IRQ service request routine decoding is based on value at NVM address nvm_base  + application_params_offset + EVBUS_HANDLER2_SOURCE_offset
 *        If no valid value for decoding is found, then serve_adc_irq() is executed
 */
extern void Event_Bus2_Handler(void); //!< IRQ2

/**
 * @brief It clears EventBus IRQ #3 and decodes the related service request IR service routine, associated to this EventBus IRQ
 *        The associated service request IR service routine (e.g. serve_adc_irq()) is a called and executed
 *        The IRQ service request routine decoding is based on value at NVM address nvm_base  + application_params_offset + EVBUS_HANDLER3_SOURCE_offset
 *        If no valid value for decoding is found, then serve_adc_irq() is executed
 */
extern void Event_Bus3_Handler(void); //!< IRQ3

/**
 * @brief It clears EventBus IRQ #4 and decodes the related service request IR service routine, associated to this EventBus IRQ
 *        The associated service request IR service routine (e.g. serve_adc_irq()) is a called and executed
 *        The IRQ service request routine decoding is based on value at NVM address nvm_base  + application_params_offset + EVBUS_HANDLER4_SOURCE_offset
 *        If no valid value for decoding is found, then serve_adc_irq() is executed
 */
extern void Event_Bus4_Handler(void); //!< IRQ4

/**
 * @brief It clears EventBus IRQ #5 and decodes the related service request IR service routine, associated to this EventBus IRQ
 *        The associated service request IR service routine (e.g. serve_adc_irq()) is a called and executed
 *        The IRQ service request routine decoding is based on value at NVM address nvm_base  + application_params_offset + EVBUS_HANDLER5_SOURCE_offset
 *        If no valid value for decoding is found, then serve_adc_irq() is executed
 */
extern void Event_Bus5_Handler(void); //!< IRQ5

/**
 * @brief It clears EventBus IRQ #6 and decodes the related service request IR service routine, associated to this EventBus IRQ
 *        The associated service request IR service routine (e.g. serve_adc_irq()) is a called and executed
 *        The IRQ service request routine decoding is based on value at NVM address nvm_base  + application_params_offset + EVBUS_HANDLER6_SOURCE_offset
 *        If no valid value for decoding is found, then serve_adc_irq() is executed
 */
extern void Event_Bus6_Handler(void); //!< IRQ6

/**
 * @brief It clears EventBus IRQ #7 and decodes the related service request IR service routine, associated to this EventBus IRQ
 *        The associated service request IR service routine (e.g. serve_adc_irq()) is a called and executed
 *        The IRQ service request routine decoding is based on value at NVM address nvm_base  + application_params_offset + EVBUS_HANDLER7_SOURCE_offset
 *        If no valid value for decoding is found, then serve_adc_irq() is executed
 */
extern void Event_Bus7_Handler(void); //!< IRQ7

/**
 * @brief It clears EventBus IRQ #8 and decodes the related service request IR service routine, associated to this EventBus IRQ
 *        The associated service request IR service routine (e.g. serve_adc_irq()) is a called and executed
 *        The IRQ service request routine decoding is based on value at NVM address nvm_base  + application_params_offset + EVBUS_HANDLER8_SOURCE_offset
 *        If no valid value for decoding is found, then serve_adc_irq() is executed
 */
extern void Event_Bus8_Handler(void); //!< IRQ8

/**
 * @brief This function decodes the related IR service request routine, associated with this HP Matrix IRQ
 *        The IRQ service request routine is decoded from the value at NVM address (nvm_base  + application_params_offset + HP_IRQ9_CFG_offset)
 *        If no valid value for decoding is found, then serve_adc_irq() is executed
 *        If no valid value for decoding is found, then serve_uart_irq(get_uart_irq()) is executed
 */
extern void HPrio_Matrix0_Handler(void); //!< IRQ9

/**
 * @brief This function decodes the related IR service request routine, associated with this HP Matrix IRQ
 *        The IRQ service request routine is decoded from the value at NVM address (nvm_base  + application_params_offset + HP_IRQ10_CFG_offset)
 *        If no valid value for decoding is found, then serve_adc_irq() is executed
 *        If no valid value for decoding is found, then serve_uart_irq(get_uart_irq()) is executed
 */
extern void HPrio_Matrix1_Handler(void); //!< IRQ10

/**
 * @brief This function decodes the related IR service request routine, associated with this HP Matrix IRQ
 *        The IRQ service request routine is decoded from the value at NVM address (nvm_base  + application_params_offset + HP_IRQ11_CFG_offset)
 *        If no valid value for decoding is found, then serve_adc_irq() is executed
 *        If no valid value for decoding is found, then serve_uart_irq(get_uart_irq()) is executed
 */
extern void HPrio_Matrix2_Handler(void); //!< IRQ11

/**
 * @brief This function decodes the related IR service request routine, associated with this HP Matrix IRQ
 *        The IRQ service request routine is decoded from the value at NVM address (nvm_base  + application_params_offset + HP_IRQ12_CFG_offset)
 *        If no valid value for decoding is found, then serve_adc_irq() is executed
 *        If no valid value for decoding is found, then serve_uart_irq(get_uart_irq()) is executed
 */
extern void HPrio_Matrix3_Handler(void); //!< IRQ12

/**
 * @brief This function decodes the related IR service request routine, associated with this HP Matrix IRQ
 *        The IRQ service request routine is decoded from the value at NVM address (nvm_base  + application_params_offset + HP_IRQ13_CFG_offset)
 *        If no valid value for decoding is found, then serve_adc_irq() is executed
 *        If no valid value for decoding is found, then serve_uart_irq(get_uart_irq()) is executed
 */
extern void HPrio_Matrix4_Handler(void); //!< IRQ13

/**
 * @brief This function decodes the related IR service request routine, associated with this HP Matrix IRQ
 *        The IRQ service request routine is decoded from the value at NVM address (nvm_base  + application_params_offset + HP_IRQ14_CFG_offset)
 *        If no valid value for decoding is found, then serve_adc_irq() is executed
 *        If no valid value for decoding is found, then serve_uart_irq(get_uart_irq()) is executed
 */
extern void HPrio_Matrix5_Handler(void); //!< IRQ14

/**
 * @brief This function calls the watchdog timer service request routine.
 */
extern void WDT_Handler(void); //!< IRQ15

/**
 * @brief This function calls the NFC (RF) field off detection service request routine.
 */
extern void HW_field_off_Handler(void); //!< IRQ16

/**
 * @brief This function calls the NVM service request routine
 */
extern void HW_nvm_Handler(void); //!< IRQ17

/**
 * @brief This function clears the event bus IRQ status
 * @param irq interrupt status number to be cleared
 */
extern void clear_ev_irq(uint8_t irq);

/**
 * @brief This function returns the interrupt number of the ADC IRQ
 * @return ADC interrupt number
 */
extern uint8_t get_adc_irq(void);

/**
 * @brief This function returns the interrupt number of the UART IRQ
 * @return ADC interrupt number
 */
extern uint8_t get_uart_irq(void);

/**
 * @brief This function returns the interrupt number of the SSP IRQ
 * @return UART interrupt number
 */
extern uint8_t get_ssp_irq(void);

/**
 * @brief This function returns the interrupt number of the AES IRQ
 * @return AES interrupt number
 */
extern uint8_t get_aes_irq(void);

/**
 * @brief This function returns the interrupt number of the uDMA IRQ
 * @return uDMA interrupt number
 */
extern uint8_t get_udma_irq(void);

/**
 * @brief This function returns the interrupt number of the I2C IRQ
 * @return I2C interrupt number
 */
extern uint8_t get_i2c_irq(void);

/**
 * @brief This function returns the interrupt number of the GPIO0 IRQ
 * @return GPIO interrupt number
 */
extern uint8_t get_gpio0_irq(void);

/**
 * @brief This function returns the interrupt number of the GPIO1 IRQ
 * @return GPIO1 interrupt number
 */
extern uint8_t get_gpio1_irq(void);

/**
 * @brief This function returns the interrupt number of the GPIO2 IRQ
 * @return GPIO2 interrupt number
 */
extern uint8_t get_gpio2_irq(void);

/**
 * @brief This function returns the interrupt number of the GPIO3 IRQ
 * @return GPIO3 interrupt number
 */
extern uint8_t get_gpio3_irq(void);

/**
 * @brief This function returns the interrupt number of the GPIO4 IRQ
 * @return GPIO4 interrupt number
 */
extern uint8_t get_gpio4_irq(void);

/**
 * @brief This function returns the interrupt number of the GPIO5 IRQ
 * @return GPIO5 interrupt number
 */
extern uint8_t get_gpio5_irq(void);

/**
 * @brief This function returns the interrupt number of the GPIO6 IRQ
 * @return GPIO6 interrupt number
 */
extern uint8_t get_gpio6_irq(void);

/**
 * @brief This function returns the interrupt number of the GPIO7 IRQ
 * @return GPIO7 interrupt number
 */
extern uint8_t get_gpio7_irq(void);

/**
 * @brief This function configures IRQ related Colums of the HP Matrix
 *        It reads the column Configurations from NVM address from (nvm_base  + application_params_offset + HP_IRQ9_COL_CFG_offset + j * 0x4 )
 *        Then it configures the related column  configuration register
 */
extern void config_irq_hp_matrix(void);

//extern uint8_t get_adc_irq(void);

/**
 * @brief This function returns the interrupt number of the GPIO EVnt Bus IRQ
 * @return GPIO Event Bus interrupt number
 */
extern uint8_t get_gpio_evnt_irq(void);

/**
 * @brief This function returns the interrupt number of the GPIO EVnt Bus Generator IRQ
 * @return GPIO Event Bus interrupt generator number
 */
extern uint8_t get_gpio_evnt_gen_irq(void);

/**
 * @brief This function returns the interrupt number of SYSTIM0
 * @return SYSTIM0 interrupt number
 */
extern uint8_t get_systim0_irq(void);

/**
 * @brief This function returns the interrupt number of SYSTIM1
 * @return SYSTIM1 interrupt number
 */
extern uint8_t get_systim1_irq(void);

/**
 * @brief This function returns the interrupt number of SYSTIM2
 * @return SYSTIM2 interrupt number
 */
extern uint8_t get_systim2_irq(void);

/**
 * @brief This function returns the interrupt number of SYSTIM3
 * @return SYSTIM3 interrupt number
 */
extern uint8_t get_systim3_irq(void);

/**
 * @brief This function returns the interrupt number of SYSTIM4
 * @return SYSTIM4 interrupt number
 */
extern uint8_t get_systim4_irq(void);

/**
 * @brief This function returns the interrupt number of SYSTIM5
 * @return SYSTIM5 interrupt number
 */
extern uint8_t get_systim5_irq(void);



#ifdef __cplusplus
}
#endif

/** @} */ /* End of group handlers */


/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */

#endif /* _HANDLERS_H_ */
