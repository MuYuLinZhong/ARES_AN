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
 * @file     sense_ctrl.h
 *
 * @brief    Definitions for the Smack Sensing Unit.
 *
 *
 * @version  v1.0
 * @date     2020-05-20
 *
 * @note
 */

#ifndef LIBS_SMACK_LIB_INC_SENSE_CTRL_H_
#define LIBS_SMACK_LIB_INC_SENSE_CTRL_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define sense_ldo_settle 0xFF     //!< Default Settle Time of Sense LDO
#define dac_settle       0x130    //!< Default Power Up Time of DAC
#define adc_sh_settle    0x130    //!< Default Power Up Time of Sample and Hold Stage
#define min_arm_ticks    0xEF     //!< Default minimum sampling time of analog signal at Sample and Hold Stage
#define arm_start_delay  0x1F     //!< Used as Delay Parameter to start Sampling after CPU entered WFI state

/**
 * @brief used to set/ get of Power state of SH, TS, DAC, ADC, I2V
 *
 */
typedef enum sense_power_state_e
{
    sense_power_down,//!< power_down
    sense_power_up,  //!< power_up
} sense_power_state_t;

/**
 * @brief used to set get the input configuration of Sample and Hold Stage
 */
typedef enum i2v_sel_e
{
    i2v_sel_ain = 0x0,//!< ain
    i2v_sel_i2v = 0x1 //!< i2v
} i2v_sel_t;

/**
 * @brief used to select the analog input AIN1...AIN4
 */
typedef enum ain_sel_e
{
    ain_sel_ain1 = 0x1,//!< ain1
    ain_sel_ain2 = 0x2,//!< ain2
    ain_sel_ain3 = 0x3,//!< ain3
    ain_sel_ain4 = 0x4 //!< ain4
} ain_sel_t;

/**
 * @brief enum to describe enable/ disable state
 */
typedef enum sense_en_dis_e
{
    sense_disable = 0x0,//!< disable
    sense_enable  = 0x1 //!< enable
} sense_en_dis_t;

/**
 * @brief used to describe the used sample and Hold Stage SH0/ SH1
 */
typedef enum sample_hold_enum
{
    sample_hold0 = 0x0,//!< sh0
    sample_hold1 = 0x1, //!< sh1
    tsense = 0x2
} s_h_type;

#ifndef _SENSE_CTRL_HAL_H_
typedef enum sense_ctrl_cfg__attn_e     /*!< SH attenuation */
{
    sense_ctrl_cfg__attn_dis = 0u,      /*!< SH attenuation disabled */
    sense_ctrl_cfg__attn_ena = 1u       /*!< SH attenuation enabled */
} sense_ctrl_cfg__attn_t;
#endif

/**
 * @brief used to select the NFC AFE status signal to be converted
 */
typedef enum sense_nfc_sel_e
{
    nfc_sel_rssi, //!< rssi
    nfc_sel_vdd_ca//!< vdd_ca
} sense_nfc_sel_t;

/**
 * @brief result of AD conversion
 */
typedef struct
{
    uint16_t sh_result[2];  //!< result[1] contains AD result of sh1, result[0] contains AD result of sh0
} sh_result_t;

/**
 * @brief Configures the mainly the power on/down state of the components of the sensing unit
 * @param adc_state Power On/ Down of ADC
 * @param sh0_state Power On/ Down of SH0
 * @param sh1_state Power On/ Down of SH1
 * @param dac_state Power On/ Down of DAC
 * @param i2v_state Power On/ Down of I2V
 * @param comp_state Power On/ Down of Comparator
 * @param ts_state Power On/ Down of Temp Sensor
 * @param shts_state Power On/ Down of Sample/ Hold Temp Sensor
 * @param attn_en_dis sense_ctrl_cfg__attn_dis -> SH attenuation disabled (SH gain=1); sense_ctrl_cfg__attn_ena -> sense_ctrl_cfg__attn_dis -> SH attenuation enabled (SH gain=7/8)
 */
extern void sense_ctrl_config(
    sense_power_state_t adc_state,
    sense_power_state_t sh0_state,
    sense_power_state_t sh1_state,
    sense_power_state_t dac_state,
    sense_power_state_t i2v_state,
    sense_power_state_t comp_state,
    sense_power_state_t ts_state,
    sense_power_state_t shts_state,
    sense_en_dis_t attn_en_dis
);

/**
 * @brief This functions sets the value of the DAC output, which might be visible at pin AOUT (--> use function set_aout(...) for that purpose).
 *        The DAC generates an analog value, which can be changed within 1024 steps (10 bit) between 0V and 1.8V.
 *        AOUT= dac_value*(1.8V/2^10)
 * @param dac_value a 16-bit unsigned integer, which represents the analog DAC output voltage. Only the 10-LSBs are considered; useless MSBs will be masked
 */
extern void set_dac_value(uint16_t dac_value);

/**
 * @brief This function will switch on the power supply of sense unit and cares for proper power-on of sense unit.
 *        After the power supply is settled and valid, the sense unit is in reset and power save state.
 *        The function runs about 5us.
 */
extern void switch_on_sense(void);


/**
 * @brief This function will switch off the power supply of sense unit and cares for safe power-off of sense unit.
 *        The function runs about 5us.
 */
extern void switch_off_sense(void);


/**
 * @brief This function configures completely a SH (either SH0 or SH1) stage of ADC of sense unit.
 * @param sample_hold -->elects the SH stage to be configured (can be of value sh0 or sh1)
 * @param ain_sel --> selects the analog input of SH (can be value ain1 or ain2 for SH0; ain3 or ain4 for SH1 --> in case wrong parameter is given, the function corrects it to a reasonable value)
 * @param i2v_sel --> selects either the selected analog input ain* to SH input or the current to voltage converter i2v (can be value of ain or i2v)
 * @param auto_arm --> advanced feature for ADC control (can be value of disable or enable, RECOMENDATION: set it to disable)
 */
extern void sense_sh_config(s_h_type sample_hold, ain_sel_t ain_sel, i2v_sel_t i2v_sel, sense_en_dis_t auto_arm);


/**
 * @brief This function configures completely the comparator of the sensing unit and enables the comparator output (--> comp output valid/active).
 * @param dac_value --> reference value of comparator provided by sense unit DAC
 * @param ain_sel ain_sel  --> selects the analog input, which is compared to DAC reference (can be value of ain1/ ain2/ ain3/ ain4)
 * @param comp_out_en comp_out_en --> enables the internal comparator output  (can be value disable/ enable)
 * @param filter_cycles --> configures the depth of the digital low pass filter at comparator output to prevent ringing of comp result (as bigger as bigger averaging/ filter effect)
 */
extern void sense_comp_config(uint16_t dac_value, ain_sel_t ain_sel, sense_en_dis_t comp_out_en, uint8_t filter_cycles);

/**
 * @brief This function provides an analog to digital conversion at sample&hold SH0 and SH1.
 *        SH0 and SH1 should be configured before by sense_sh_config(...)
 *        The function cares for power up of ADC and SH0/ SH1, if not done, before calling.
 *        The function care for controlling the sample and hold and ADC and capturing the result
 *        The function sets up an event bus driven controlling of the ADC for sampling and converting.
 *        The CPU is set into sleep mode (WFI) during sampling and converting.
 *        The ADC interrupt has to be assigned to an IRQ, which is associated to eventbus IRQ request unit (-->#define sense_adc_irq in handlers.h)
 *        SH0 and SH1 are sampled simultaneously and converted sequentially.
 *        If NFC communication is expected to happen while this function is executed, please disable interrupts 0 and 16 in the NVIC before calling this function,
 *        and enable them again when this function returns. If there are other interrupts active (e.g., UART), please disable them as well. Do not disable interrupts
 *        globally as this function relies on some of the interrupts.
 * @param sh0_sense --> if true, then sh0 is sampled and converted
 * @param sh1_sense --> if true, then sh1 is sampled and converted
 * @param ts_sense  --> if true, then sh_ts is sampled and converted. The value is reported in sh_result[0], so combining a read of ts with a read of sh0 does not make sense
 * @return a struct, which contains an array of two unsigned 16-bit integers, which contain the conversion result; --> sh_result[0] is the conversion result of SH0; sh_result[1] is the conversion result of SH1
 */
extern sh_result_t sense_sh(bool sh0_sense, bool sh1_sense, bool ts_sense);


/**
 * @brief This function is intended to be triggered, when a sense unit related function executes WFI and waits to be triggered from sense unit
 *        Basically the function would need a "return;" only
 *        The ADC interrupt has to be assigned to an IRQ, which is associated to eventbus IRQ request unit (-->#define sense_adc_irq in handlers.h)
 */
extern void serve_adc_irq(void);

/**
 * @brief This functions sets the value to the DAC output and enables the analog output AOUT.
 *        The DAC generates an analog value, which can be changed within 1024 steps (10 bit) between 0V and 1.8V.
 *        AOUT= dac_value*(1.8V/2^10)
 * @param dac_value a 16-bit unsigned integer, which represents the analog DAC output voltage; Only the 10-LSBs are considered; useless MSBs will be masked
 */
extern void set_aout(uint16_t dac_value);

/**
 * @brief This function is sensing the receive strength signal indication of NFC and converts it into a digital represantation.
 *        The RSSI analog signal of NFC Receiver is connected to SH0 of ADC and converted.
 *        The function cares for power up of ADC and SH0, if not done, before calling.
 *        The function care for controlling the sample and hold and ADC and capturing the result
 *        The function sets up an event bus driven controlling of the ADC for sampling and converting.
 *        The CPU is set into sleep mode (WFI) during sampling and converting.
 *        The ADC interrupt has to be assigned to an IRQ, which is associated to eventbus IRQ request unit (-->#define sense_adc_irq in handlers.h)
 * @param value ; can be rssi, vddca
 * @return conversion result of value
 */
extern uint16_t get_nfc_value(sense_nfc_sel_t value);


#ifdef __cplusplus
}
#endif

#endif /* LIBS_SMACK_LIB_INC_SENSE_CTRL_H_ */
