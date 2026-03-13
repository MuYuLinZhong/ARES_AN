/* ============================================================================
** Copyright (c) 2022 Infineon Technologies AG
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
 * @file     rom_lib.h
 *
 * @brief    ROM library interface.
 *
 *           This file provides the definition of the function pointers through which the ROM library
 *           functions are called. It also provides wrapper macros to access those functions through
 *           the pointers similar to a direct function call.
 *
 *           To use the ROM library functions, include this file and call the functions as defined in
 *           their header file. The wrapper macros map function calls to the function pointers.
 *
 * @version  v1.0
 * @date     2020-05-20
 *
 * @note
 */

// /* lint -save -e960 */

#ifndef _ROM_LIB_H_
#define _ROM_LIB_H_


/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Smack
 * @{
 */


/** @addtogroup ROM_library
 * @{
 */

#include <stdint.h>
#include <stdbool.h>

// include all (or most) headers which define functions used in this table
#include "aes_drv.h"
#include "aparam.h"
#include "dand_handler.h"
#include "dparam.h"
//#include "gpio.h"     // gpio.h needs definition of rom_func_table and is included below
#include "handlers.h"
#include "hbctrl_drv.h"
#include "hwdiv.h"
#include "nfc_handler.h"
#include "nvm.h"
#include "pmu.h"
#include "rtc.h"
#include "sense_ctrl.h"
#include "ssp_drv.h"
#include "sys_tim_drv.h"
#include "uart_drv.h"
#include "wdt.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
    // offset (base address: 0x00c0)
    // AES
    void (*m_run_pnk)(void);                                                    // 0x0000
    void (*m_check_aes_busy)(void);                                             // 0x0004
    void (*m_aes_load_key)(const uint32_t* key);                                // 0x0008
    void (*m_calc_aes)(uint32_t* result, const uint32_t* data, aes_operation_type op_type); // 0x000c
    void (*m_generate_random_number)(uint32_t* random_number);                  // 0x0010

    // APARAM
    void     (*m_aparam_init)(void);                                            // 0x0014
    uint16_t (*m_aparam_size_get)(void);                                        // 0x0018
    const Aparams_t* (*m_aparam_pointer_get)(void);                             // 0x001c

    // DAND handler
    void (*m_init_dand)(void);                                                  // 0x0020
    Mailbox_t* (*m_get_mailbox_address)(void);                                  // 0x0024
    uint32_t (*m_get_mailbox_size)(void);                                       // 0x0028
    void (*m_register_function)(uint8_t id, Mailbox_Fct_Ptr_t callback_function); // 0x002c
    void (*m_handle_dand_protocol)(const rx_DAND_t* input, tx_DAND_t* output);  // 0x0030
    void (*m_reset_rx_tx)(rx_DAND_t* input, tx_DAND_t* output);                 // 0x0034
    uint8_t (*m_is_legal_addr)(const uint32_t addr);                            // 0x0038
    uint32_t (*m_extract_addr)(const rx_DAND_t* input);                         // 0x003c
    uint32_t (*m_transmit_mailbox_addr)(const Mailbox_t* mailbox);              // 0x0040
    uint32_t (*m_transmit_mailbox_size)(const Mailbox_t* mailbox);              // 0x0044
    void (*m_read_suffix)(uint32_t return_code_param, uint32_t value, Mailbox_t* mbox); // 0x0048
    uint32_t (*m_mailbox_message_valid)(Mailbox_t* mailbox);                    // 0x004c
    uint32_t (*m_purge_mailbox)(Mailbox_t* mailbox);                            // 0x0050
    uint32_t (*m_message_nvm_program)(const Mailbox_t* mailbox);                // 0x0054
    uint32_t (*m_message_nvm_erase)(const Mailbox_t* mailbox);                  // 0x0058
    uint32_t (*m_message_nvm_switch_on)(const Mailbox_t* Mailbox);              // 0x005c
    uint32_t (*m_message_nvm_switch_off)(const Mailbox_t* Mailbox);             // 0x0060
    uint32_t (*m_message_nvm_open_assi_buffer)(const Mailbox_t* Mailbox);       // 0x0064
    uint32_t (*m_message_set_uart_baudrate)(const Mailbox_t* Mailbox);          // 0x0068
    uint32_t (*m_message_set_gpio)(const Mailbox_t* Mailbox);                   // 0x006c
    uint32_t (*m_message_get_gpio)(const Mailbox_t* Mailbox);                   // 0x0070
    uint32_t (*m_message_single_gpio_cfg)(const Mailbox_t* Mailbox);            // 0x0074
    uint32_t (*m_message_ctrl_hbridge)(const Mailbox_t* Mailbox);               // 0x0078
    uint32_t (*m_message_nvm_normal_read)(const Mailbox_t* Mailbox);            // 0x007c
    uint32_t (*m_message_sense_adc)(const Mailbox_t* Mailbox);                  // 0x0080
    uint32_t (*m_message_get_rssi)(const Mailbox_t* Mailbox);                   // 0x0084
    uint32_t (*m_message_get_vccca)(const Mailbox_t* Mailbox);                  // 0x0088

    // DPARAM
    void (*m_dparam_init)(void);                                                // 0x008c
    uint16_t (*m_dparam_size_get)(void);                                        // 0x0090
    const Dparams_t* (*m_dparam_pointer_get)(void);                             // 0x0094

    // GPIO
    void (*m_read_gpio_nvm_config)(void);                                       // 0x0098
    void (*m_read_gpio_alt_nvm_config)(void);                                   // 0x009c
    uint8_t (*m_all_gpios_iocfg)(const uint16_t out_enable, const uint16_t in_enable, const uint16_t outtype, const uint16_t pup, const uint16_t pdown); // 0x00a0
    uint8_t (*m_single_gpio_iocfg)(const bool out_enable, const bool in_enable, const bool outtype, const bool pup, const bool pdown, uint8_t gpio);     // 0x00a4
    void    (*m_set_allgpios_out)(const uint16_t value);                        // 0x00a8
    void    (*m_set_singlegpio_out)(uint8_t value, uint8_t gpio);               // 0x00ac
    uint16_t (*m_get_allgpios_in)(void);                                        // 0x00b0
    uint8_t (*m_get_singlegpio_in)(uint8_t gpio);                               // 0x00b4
    void    (*m_set_singlegpio_alt)(uint8_t gpio, uint8_t ain_en, uint8_t outsel); // 0x00b8

    // Handler
    void (*m_clear_ev_irq)(uint8_t irq);                                        // 0x00bc
    uint8_t (*m_get_adc_irq)(void);                                             // 0x00c0
    uint8_t (*m_get_uart_irq)(void);                                            // 0x00c4
    uint8_t (*m_get_ssp_irq)(void);                                             // 0x00c8
    uint8_t (*m_get_aes_irq)(void);                                             // 0x00cc
    uint8_t (*m_get_udma_irq)(void);                                            // 0x00d0
    uint8_t (*m_get_i2c_irq)(void);                                             // 0x00d4
    uint8_t (*m_get_gpio0_irq)(void);                                           // 0x00d8
    uint8_t (*m_get_gpio1_irq)(void);                                           // 0x00dc
    uint8_t (*m_get_gpio2_irq)(void);                                           // 0x00e0
    uint8_t (*m_get_gpio3_irq)(void);                                           // 0x00e4
    uint8_t (*m_get_gpio4_irq)(void);                                           // 0x00e8
    uint8_t (*m_get_gpio5_irq)(void);                                           // 0x00ec
    uint8_t (*m_get_gpio6_irq)(void);                                           // 0x00f0
    uint8_t (*m_get_gpio7_irq)(void);                                           // 0x00f4
    void (*m_config_irq_hp_matrix)(void);                                       // 0x00f8
    uint8_t (*m_get_gpio_evnt_irq)(void);                                       // 0x00fc
    uint8_t (*m_get_gpio_evnt_gen_irq)(void);                                   // 0x0100
    uint8_t (*m_get_systim0_irq)(void);                                         // 0x0104
    uint8_t (*m_get_systim1_irq)(void);                                         // 0x0108
    uint8_t (*m_get_systim2_irq)(void);                                         // 0x010c
    uint8_t (*m_get_systim3_irq)(void);                                         // 0x0110
    uint8_t (*m_get_systim4_irq)(void);                                         // 0x0114
    uint8_t (*m_get_systim5_irq)(void);                                         // 0x0118

    // H bridge
    uint32_t (*m_get_hb_stat)(status_type_t stat_req);                          // 0x011c
    void (*m_set_hb_switch)(bool hs1_set, bool ls1_set, bool hs2_set, bool ls2_set); // 0x0120
    void (*m_set_hb_eventctrl)(bool control_switches_by_eventbus);              // 0x0124
    void (*m_set_hb_config)(const hb_config_struct_t* hb_config);               // 0x0128
    void (*m_set_hb_event)(uint32_t event);                                     // 0x012c
    void (*m_discharge_CA)(void);                                               // 0x0130

    // HW div
    uint32_t (*m_calc_div)(uint32_t op1, uint32_t op2, op_type_t op_formats, calc_type_t calc_res); // 0x0134
    void     (*m_clear_div_err)(void);                                          // 0x0138
    bool    (*m_get_hwdiv_div0_state)(void);                                    // 0x013c
    bool    (*m_get_hwdiv_ovf_state)(void);                                     // 0x0140

    // NFC handler
    void (*m_nfc_init)(void);                                                   // 0x0144
    void (*m_read_frame)(void);                                                 // 0x0148
    NFC_Frame_enum_t (*m_classify_frame)(void);                                 // 0x014c
    NFC_Cmd_enum_t (*m_classify_command)(void);                                 // 0x0150
    void (*m_reset_CRC_peripheral)(void);                                       // 0x0154
    void (*m_clear_ret_frame)(void);                                            // 0x0158
    uint8_t (*m_is_CRC_A_correct)(void);                                        // 0x015c
    uint8_t (*m_is_selected)(void);                                             // 0x0160
    void (*m_send_return_frame)(void);                                          // 0x0164
    NFC_State_enum_t (*m_handle_anticollision)(void);                           // 0x0168
    NFC_State_enum_t (*m_handle_DAND_protocol)(void);                           // 0x016c
    void (*m_send_p_error)(void);                                               // 0x0170
    void (*m_nfc_state_machine)(void);                                          // 0x0174
    void (*m_cleanup_routine)(void);                                            // 0x0178
    void (*m_serve_hw_field_off_irq)(void);                                     // 0x017c

    // NVM
    void (*m_switch_on_nvm)(void);                                              // 0x0180
    void (*m_switch_off_nvm)(void);                                             // 0x0184
    void (*m_nvm_config)(void);                                                 // 0x0188
    uint8_t (*m_nvm_open_assembly_buffer)(uint32_t cpu_address);                // 0x018c
    uint8_t (*m_nvm_program_page)(void);                                        // 0x0190
    uint8_t (*m_nvm_program_verify)(void);                                      // 0x0194
    void (*m_nvm_abort_program)(void);                                          // 0x0198
    void (*m_nvm_erase_page)(void);                                             // 0x019c
    access_state_t (*m_get_nvm_access_state)(uint32_t address);                 // 0x01a0
    uint32_t (*m_get_jtag_id)(void);                                            // 0x01a4

    // PMU
    void     (*m_config_stbtm)(bool on_off, uint32_t standby_time);             // 0x01a8
    uint32_t (*m_get_standby_time)(void);                                       // 0x01ac
    wakeup_source_t (*m_get_wakeup_source)(void);                               // 0x01b0
    void     (*m_request_power_saving_mode)(bool wake_by_nfc, bool wake_by_stbtim, bool wake_by_wakeuppin, wakeup_pol_t wakeup_polarity); // 0x01b4
    void     (*m_check_supply_source)(void);                                    // 0x01b8
    void     (*m_single_shot_systick)(uint32_t time);                           // 0x01bc

    // RTC
    void     (*m_rtc_init)(void);                                               // 0x01c0
    void     (*m_rtc_control)(bool rtc_on);                                     // 0x01c4
    uint32_t (*m_rtc_get)(void);                                                // 0x01c8

    // Sense ctrl
    void (*m_sense_ctrl_config)(
        sense_power_state_t adc_state,
        sense_power_state_t sh0_state,
        sense_power_state_t sh1_state,
        sense_power_state_t dac_state,
        sense_power_state_t i2v_state,
        sense_power_state_t comp_state,
        sense_power_state_t ts_state,
        sense_power_state_t shts_state,
        /*sense_ctrl_cfg__attn_t*/ sense_en_dis_t attn_en_dis
    );                                                                          // 0x01cc
    void (*m_set_dac_value)(uint16_t dac_value);                                // 0x01d0
    void (*m_switch_on_sense)(void);                                            // 0x01d4
    void (*m_switch_off_sense)(void);                                           // 0x01d8
    void (*m_sense_sh_config)(s_h_type sample_hold, ain_sel_t ain_sel, i2v_sel_t i2v_sel, sense_en_dis_t auto_arm);        // 0x01dc
    void (*m_sense_comp_config)(uint16_t dac_value, ain_sel_t ain_sel, sense_en_dis_t comp_out_en, uint8_t filter_cycles); // 0x01e0
    sh_result_t (*m_sense_sh)(bool sh0_sense, bool sh1_sense, bool ts_sense);   // 0x01e4
    void (*m_set_aout)(uint16_t dac_value);                                     // 0x01e8
    uint16_t (*m_get_nfc_value)(sense_nfc_sel_t value);                                 // 0x01ec
    // NA:
    //extern sh_result_t sense_sh_lp(bool sh0_sense, bool sh1_sense, uint8_t timer_arm, uint8_t timer_trigger);
    //extern sh_result_t sense_sh_ts(void);
    // RFU: Sense ctrl
    void (*m_serve_adc_irq)(void);                                              // 0x01f0

    // SSP
    void (*m_control_spi)(bool en_dis, bool loop_back);                         // 0x01f4
    void (*m_config_spi)(uint8_t clockrate, ssp_ifmode_t mode, uint8_t number_of_bits, ssp_polarity_t clock_polarity, ssp_phase_t clock_phase); // 0x01f8
    void (*m_config_ssp_irqs)(bool txim, bool rxim, bool rtim, bool rorim, uint8_t irq_num); // 0x01fc

    // RFU: SSP
    void (*m_serve_ssp_irq)(uint8_t irq_num);                                   // 0x0200

    // System timer
    void     (*m_sys_tim_pwm_cfg)(const bool enable, const enum sys_tim_polarity_e polarity, const uint32_t duty); // 0x0204
    uint32_t (*m_get_pwm_status)(void);                                         // 0x0208
    void     (*m_sys_tim_chn_cfg)(const sys_tim_config_struct_t* sys_tim_config, const uint32_t channel);  // 0x020c
    void     (*m_sys_tim_chn_control)(const enum sys_tim_control_E start_stop, const uint32_t channel); // 0x0210
    void     (*m_set_sys_tim_chn_period)(const uint32_t period, const uint32_t channel);        // 0x0214
    uint32_t (*m_get_sys_tim_chn_period)(const uint32_t channel);               // 0x0218
    uint32_t (*m_get_sys_tim_chn_timecount)(const uint32_t channel);            // 0x021c
    uint32_t (*m_get_sys_tim_chn_status)(const uint32_t channel);               // 0x0220
    void     (*m_sys_tim_chn_evnt_cfg)(const uint8_t en_hprio, const uint8_t irq_event, const uint8_t adc_event, const uint32_t event_code, const uint32_t channel); // 0x0224
    void (*m_my_systim_singleshot)(const uint8_t channel, const uint16_t period, const uint8_t irq_number); // 0x0228

    // UART
    void     (*m_set_uart_baudrate)(uint32_t baudrate); //e.g. set_uart_baudrate(9600); // 0x022c
    void     (*m_init_uart)(bool parity_en, parity_t even_odd, bool two_stop_bits, bool fifo_en, uart_data_len_t word_length, bool  stick_parity); // 0x0230
    void     (*m_set_uart_control)(bool enable, bool rx_enable, bool tx_enable); // 0x0234
    void     (*m_transmit_bytes)(void);                                         // 0x0238
    void     (*m_configure_uart_irq)(uint8_t irq_num, uint8_t rx_fifo_level, uint8_t tx_fifo_level, bool rx_int, bool tx_int); // 0x023c

    // RFU: UART
    void     (*m_serve_uart_irq)(uint8_t number);                               // 0x0240

    // WDT
    void (*m_init_wdt)(void);                                                   // 0x0244
    void (*m_disable_wdt)(void);                                                // 0x0248
    void (*m_config_wdt)(bool enable, bool en_irq, bool en_res, clk_scaling_ratio_t  clk_scaling_ratio); // 0x024c
    void (*m_lock_wdt)(void);                                                   // 0x0250
    void (*m_unlock_wdt)(void);                                                 // 0x0254
    void (*m_serve_wdt)(void);                                                  // 0x0258
    void (*m_set_wdt_limits)(uint32_t lower_limit, uint32_t upper_limit);       // 0x025c
    bool (*m_wdt_service_window)(void);                                         // 0x0260


    // test - todo: remove
//    uint32_t fill0;
//    uint32_t fill1;
//    uint32_t fill2;
//    uint32_t fill3;
//    uint32_t fill4;
//    uint32_t fill5;
//    uint32_t fill6;
//    uint32_t fill7;

    // RFU: reserve some vectors for future use
    void (*m_rfu_0)(void);                                                      // 0x0264
    void (*m_rfu_1)(void);                                                      // 0x0268
    void (*m_rfu_2)(void);                                                      // 0x026c
    void (*m_rfu_3)(void);                                                      // 0x0270
    void (*m_rfu_4)(void);                                                      // 0x0274
    void (*m_rfu_5)(void);                                                      // 0x0278
    void (*m_rfu_6)(void);                                                      // 0x027c
    void (*m_rfu_7)(void);                                                      // 0x0280
    void (*m_rfu_8)(void);                                                      // 0x0284
    void (*m_rfu_9)(void);                                                      // 0x0288
    void (*m_rfu_10)(void);                                                     // 0x028c
    void (*m_rfu_11)(void);                                                     // 0x0290
    void (*m_rfu_12)(void);                                                     // 0x0294

    //---------------------------------------------------------------------------------------------------------------------------------------
    // RFU: All of the following functions shall not be used. They are listed temporarily for test purposes and will be removed in the future

    // RFU: GPIO
    void (*m_serve_gpio_event_irq)(void);                                       // 0x0298
    void (*m_serve_gpio_event_gen_irq)(void);                                   // 0x029c
    void (*m_serve_gpin0_irq)(void);                                            // 0x02a0
    void (*m_serve_gpin1_irq)(void);                                            // 0x02a4
    void (*m_serve_gpin2_irq)(void);                                            // 0x02a8
    void (*m_serve_gpin3_irq)(void);                                            // 0x02ac
    void (*m_serve_gpin4_irq)(void);                                            // 0x02b0
    void (*m_serve_gpin5_irq)(void);                                            // 0x02b4
    void (*m_serve_gpin6_irq)(void);                                            // 0x02b8
    void (*m_serve_gpin7_irq)(void);                                            // 0x02bc

    // RFU: Handler
    void (*m_HardFault_Handler)(void);                                          // 0x02c0
    void (*m_SVC_Handler)(void);                                                // 0x02c4
    void (*m_PendSV_Handler)(void);                                             // 0x02c8
    void (*m_SysTick_Handler)(void);                                            // 0x02cc
    void (*m_Cl_uart_Handler)(void);                                            // 0x02d0
    void (*m_Event_Bus1_Handler)(void);                                         // 0x02d4
    void (*m_Event_Bus2_Handler)(void);                                         // 0x02d8
    void (*m_Event_Bus3_Handler)(void);                                         // 0x02dc
    void (*m_Event_Bus4_Handler)(void);                                         // 0x02e0
    void (*m_Event_Bus5_Handler)(void);                                         // 0x02e4
    void (*m_Event_Bus6_Handler)(void);                                         // 0x02e8
    void (*m_Event_Bus7_Handler)(void);                                         // 0x02ec
    void (*m_Event_Bus8_Handler)(void);                                         // 0x02f0
    void (*m_HPrio_Matrix0_Handler)(void);                                      // 0x02f4
    void (*m_HPrio_Matrix1_Handler)(void);                                      // 0x02f8
    void (*m_HPrio_Matrix2_Handler)(void);                                      // 0x02fc
    void (*m_HPrio_Matrix3_Handler)(void);                                      // 0x0300
    void (*m_HPrio_Matrix4_Handler)(void);                                      // 0x0304
    void (*m_HPrio_Matrix5_Handler)(void);                                      // 0x0308
    void (*m_WDT_Handler)(void);                                                // 0x030c
    void (*m_HW_field_off_Handler)(void);                                       // 0x0310
    void (*m_HW_nvm_Handler)(void);                                             // 0x0314

    // RFU: NVM
    void (*m_serve_nvmirq)(void);                                               // 0x0318

    // RFU: PMU
    void     (*m_dft_preamble1)(void);                                          // 0x031c
    void     (*m_dft_preamble2)(void);                                          // 0x0320

    // RFU: System timer
    void (*m_serve_systim0_irq)(void);                                          // 0x0324
    void (*m_serve_systim1_irq)(void);                                          // 0x0328
    void (*m_serve_systim2_irq)(void);                                          // 0x032c
    void (*m_serve_systim3_irq)(void);                                          // 0x0330
    void (*m_serve_systim4_irq)(void);                                          // 0x0334
    void (*m_serve_systim5_irq)(void);                                          // 0x0338

    // RFU: WDT
    void (*m_serve_wdtirq)(void);                                               // 0x033c
} rom_func_table_t;

extern const rom_func_table_t rom_func_table;


// include all (or most) headers which define functions used in this table
#include "gpio.h"


#ifndef _NO_ROM_FUNC_TABLE_WRAPPER_

#define _ROM_LIB_MAKE_FUNC_NAME(func) rom_func_table.m_##func

// AES
#define run_pnk(...)        _ROM_LIB_MAKE_FUNC_NAME(run_pnk) (__VA_ARGS__)
#define check_aes_busy(...)        _ROM_LIB_MAKE_FUNC_NAME(check_aes_busy) (__VA_ARGS__)
#define aes_load_key(...)        _ROM_LIB_MAKE_FUNC_NAME(aes_load_key) (__VA_ARGS__)
#define calc_aes(...)        _ROM_LIB_MAKE_FUNC_NAME(calc_aes) (__VA_ARGS__)
#define generate_random_number(...)        _ROM_LIB_MAKE_FUNC_NAME(generate_random_number) (__VA_ARGS__)

// APARAM
#define aparam_init(...)        _ROM_LIB_MAKE_FUNC_NAME(aparam_init) (__VA_ARGS__)
#define aparam_size_get(...)    _ROM_LIB_MAKE_FUNC_NAME(aparam_size_get) (__VA_ARGS__)
#define aparam_pointer_get(...) _ROM_LIB_MAKE_FUNC_NAME(aparam_pointer_get) (__VA_ARGS__)

// DAND handler
#define init_dand(...)        _ROM_LIB_MAKE_FUNC_NAME(init_dand) (__VA_ARGS__)
#define get_mailbox_address(...)        _ROM_LIB_MAKE_FUNC_NAME(get_mailbox_address) (__VA_ARGS__)
#define get_mailbox_size(...)        _ROM_LIB_MAKE_FUNC_NAME(get_mailbox_size) (__VA_ARGS__)
#define register_function(...)        _ROM_LIB_MAKE_FUNC_NAME(register_function) (__VA_ARGS__)
#define handle_dand_protocol(...)        _ROM_LIB_MAKE_FUNC_NAME(handle_dand_protocol) (__VA_ARGS__)
#define reset_rx_tx(...)        _ROM_LIB_MAKE_FUNC_NAME(reset_rx_tx) (__VA_ARGS__)
#define is_legal_addr(...)        _ROM_LIB_MAKE_FUNC_NAME(is_legal_addr) (__VA_ARGS__)
#define extract_addr(...)        _ROM_LIB_MAKE_FUNC_NAME(extract_addr) (__VA_ARGS__)
#define transmit_mailbox_addr(...)        _ROM_LIB_MAKE_FUNC_NAME(transmit_mailbox_addr) (__VA_ARGS__)
#define transmit_mailbox_size(...)        _ROM_LIB_MAKE_FUNC_NAME(transmit_mailbox_size) (__VA_ARGS__)
#define read_suffix(...)        _ROM_LIB_MAKE_FUNC_NAME(read_suffix) (__VA_ARGS__)
#define mailbox_message_valid(...)        _ROM_LIB_MAKE_FUNC_NAME(mailbox_message_valid) (__VA_ARGS__)
#define purge_mailbox(...)        _ROM_LIB_MAKE_FUNC_NAME(purge_mailbox) (__VA_ARGS__)
#define message_nvm_program(...)        _ROM_LIB_MAKE_FUNC_NAME(message_nvm_program) (__VA_ARGS__)
#define message_nvm_erase(...)        _ROM_LIB_MAKE_FUNC_NAME(message_nvm_erase) (__VA_ARGS__)
#define message_nvm_switch_on(...)        _ROM_LIB_MAKE_FUNC_NAME(message_nvm_switch_on) (__VA_ARGS__)
#define message_nvm_switch_off(...)        _ROM_LIB_MAKE_FUNC_NAME(message_nvm_switch_off) (__VA_ARGS__)
#define message_nvm_open_assi_buffer(...)        _ROM_LIB_MAKE_FUNC_NAME(message_nvm_open_assi_buffer) (__VA_ARGS__)
#define message_set_uart_baudrate(...)        _ROM_LIB_MAKE_FUNC_NAME(message_set_uart_baudrate) (__VA_ARGS__)
#define message_set_gpio(...)        _ROM_LIB_MAKE_FUNC_NAME(message_set_gpio) (__VA_ARGS__)
#define message_get_gpio(...)        _ROM_LIB_MAKE_FUNC_NAME(message_get_gpio) (__VA_ARGS__)
#define message_single_gpio_cfg(...)        _ROM_LIB_MAKE_FUNC_NAME(message_single_gpio_cfg) (__VA_ARGS__)
#define message_ctrl_hbridge(...)        _ROM_LIB_MAKE_FUNC_NAME(message_ctrl_hbridge) (__VA_ARGS__)
#define message_nvm_normal_read(...)        _ROM_LIB_MAKE_FUNC_NAME(message_nvm_normal_read) (__VA_ARGS__)
#define message_sense_adc(...)        _ROM_LIB_MAKE_FUNC_NAME(message_sense_adc) (__VA_ARGS__)
#define message_get_rssi(...)        _ROM_LIB_MAKE_FUNC_NAME(message_get_rssi) (__VA_ARGS__)
#define message_get_vccca(...)        _ROM_LIB_MAKE_FUNC_NAME(message_get_vccca) (__VA_ARGS__)

// DPARAM
#define dparam_init(...)        _ROM_LIB_MAKE_FUNC_NAME(dparam_init) (__VA_ARGS__)
#define dparam_size_get(...)        _ROM_LIB_MAKE_FUNC_NAME(dparam_size_get) (__VA_ARGS__)
#define dparam_pointer_get(...)        _ROM_LIB_MAKE_FUNC_NAME(dparam_pointer_get) (__VA_ARGS__)

// GPIO
#define read_gpio_nvm_config(...)       _ROM_LIB_MAKE_FUNC_NAME(read_gpio_nvm_config) (__VA_ARGS__)
#define read_gpio_alt_nvm_config(...)   _ROM_LIB_MAKE_FUNC_NAME(read_gpio_alt_nvm_config) (__VA_ARGS__)
#define all_gpios_iocfg(...)    _ROM_LIB_MAKE_FUNC_NAME(all_gpios_iocfg) (__VA_ARGS__)
#define single_gpio_iocfg(...)  _ROM_LIB_MAKE_FUNC_NAME(single_gpio_iocfg) (__VA_ARGS__)
#define set_allgpios_out(...)   _ROM_LIB_MAKE_FUNC_NAME(set_allgpios_out) (__VA_ARGS__)
#define set_singlegpio_out(...) _ROM_LIB_MAKE_FUNC_NAME(set_singlegpio_out) (__VA_ARGS__)
#define get_allgpios_in(...)    _ROM_LIB_MAKE_FUNC_NAME(get_allgpios_in) (__VA_ARGS__)
#define get_singlegpio_in(...)  _ROM_LIB_MAKE_FUNC_NAME(get_singlegpio_in) (__VA_ARGS__)
#define set_singlegpio_alt(...) _ROM_LIB_MAKE_FUNC_NAME(set_singlegpio_alt) (__VA_ARGS__)

// RFU: GPIO
#define serve_gpio_event_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(serve_gpio_event_irq) (__VA_ARGS__)
#define serve_gpio_event_gen_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(serve_gpio_event_gen_irq) (__VA_ARGS__)
#define serve_gpin0_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(serve_gpin0_irq) (__VA_ARGS__)
#define serve_gpin1_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(serve_gpin1_irq) (__VA_ARGS__)
#define serve_gpin2_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(serve_gpin2_irq) (__VA_ARGS__)
#define serve_gpin3_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(serve_gpin3_irq) (__VA_ARGS__)
#define serve_gpin4_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(serve_gpin4_irq) (__VA_ARGS__)
#define serve_gpin5_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(serve_gpin5_irq) (__VA_ARGS__)
#define serve_gpin6_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(serve_gpin6_irq) (__VA_ARGS__)
#define serve_gpin7_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(serve_gpin7_irq) (__VA_ARGS__)

// Handler
#define clear_ev_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(clear_ev_irq) (__VA_ARGS__)
#define get_adc_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(get_adc_irq) (__VA_ARGS__)
#define get_uart_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(get_uart_irq) (__VA_ARGS__)
#define get_ssp_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(get_ssp_irq) (__VA_ARGS__)
#define get_aes_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(get_aes_irq) (__VA_ARGS__)
#define get_udma_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(get_udma_irq) (__VA_ARGS__)
#define get_i2c_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(get_i2c_irq) (__VA_ARGS__)
#define get_gpio0_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(get_gpio0_irq) (__VA_ARGS__)
#define get_gpio1_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(get_gpio1_irq) (__VA_ARGS__)
#define get_gpio2_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(get_gpio2_irq) (__VA_ARGS__)
#define get_gpio3_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(get_gpio3_irq) (__VA_ARGS__)
#define get_gpio4_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(get_gpio4_irq) (__VA_ARGS__)
#define get_gpio5_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(get_gpio5_irq) (__VA_ARGS__)
#define get_gpio6_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(get_gpio6_irq) (__VA_ARGS__)
#define get_gpio7_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(get_gpio7_irq) (__VA_ARGS__)
#define config_irq_hp_matrix(...)        _ROM_LIB_MAKE_FUNC_NAME(config_irq_hp_matrix) (__VA_ARGS__)
#define get_gpio_evnt_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(get_gpio_evnt_irq) (__VA_ARGS__)
#define get_gpio_evnt_gen_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(get_gpio_evnt_gen_irq) (__VA_ARGS__)
#define get_systim0_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(get_systim0_irq) (__VA_ARGS__)
#define get_systim1_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(get_systim1_irq) (__VA_ARGS__)
#define get_systim2_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(get_systim2_irq) (__VA_ARGS__)
#define get_systim3_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(get_systim3_irq) (__VA_ARGS__)
#define get_systim4_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(get_systim4_irq) (__VA_ARGS__)
#define get_systim5_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(get_systim5_irq) (__VA_ARGS__)

// RFU: Handler
#define HardFault_Handler(...)        _ROM_LIB_MAKE_FUNC_NAME(HardFault_Handler) (__VA_ARGS__)
#define SVC_Handler(...)        _ROM_LIB_MAKE_FUNC_NAME(SVC_Handler) (__VA_ARGS__)
#define PendSV_Handler(...)        _ROM_LIB_MAKE_FUNC_NAME(PendSV_Handler) (__VA_ARGS__)
#define SysTick_Handler(...)        _ROM_LIB_MAKE_FUNC_NAME(SysTick_Handler) (__VA_ARGS__)
#define Cl_uart_Handler(...)        _ROM_LIB_MAKE_FUNC_NAME(Cl_uart_Handler) (__VA_ARGS__)
#define Event_Bus1_Handler(...)        _ROM_LIB_MAKE_FUNC_NAME(Event_Bus1_Handler) (__VA_ARGS__)
#define Event_Bus2_Handler(...)        _ROM_LIB_MAKE_FUNC_NAME(Event_Bus2_Handler) (__VA_ARGS__)
#define Event_Bus3_Handler(...)        _ROM_LIB_MAKE_FUNC_NAME(Event_Bus3_Handler) (__VA_ARGS__)
#define Event_Bus4_Handler(...)        _ROM_LIB_MAKE_FUNC_NAME(Event_Bus4_Handler) (__VA_ARGS__)
#define Event_Bus5_Handler(...)        _ROM_LIB_MAKE_FUNC_NAME(Event_Bus5_Handler) (__VA_ARGS__)
#define Event_Bus6_Handler(...)        _ROM_LIB_MAKE_FUNC_NAME(Event_Bus6_Handler) (__VA_ARGS__)
#define Event_Bus7_Handler(...)        _ROM_LIB_MAKE_FUNC_NAME(Event_Bus7_Handler) (__VA_ARGS__)
#define Event_Bus8_Handler(...)        _ROM_LIB_MAKE_FUNC_NAME(Event_Bus8_Handler) (__VA_ARGS__)
#define HPrio_Matrix0_Handler(...)        _ROM_LIB_MAKE_FUNC_NAME(HPrio_Matrix0_Handler) (__VA_ARGS__)
#define HPrio_Matrix1_Handler(...)        _ROM_LIB_MAKE_FUNC_NAME(HPrio_Matrix1_Handler) (__VA_ARGS__)
#define HPrio_Matrix2_Handler(...)        _ROM_LIB_MAKE_FUNC_NAME(HPrio_Matrix2_Handler) (__VA_ARGS__)
#define HPrio_Matrix3_Handler(...)        _ROM_LIB_MAKE_FUNC_NAME(HPrio_Matrix3_Handler) (__VA_ARGS__)
#define HPrio_Matrix4_Handler(...)        _ROM_LIB_MAKE_FUNC_NAME(HPrio_Matrix4_Handler) (__VA_ARGS__)
#define HPrio_Matrix5_Handler(...)        _ROM_LIB_MAKE_FUNC_NAME(HPrio_Matrix5_Handler) (__VA_ARGS__)
#define WDT_Handler(...)        _ROM_LIB_MAKE_FUNC_NAME(WDT_Handler) (__VA_ARGS__)
#define HW_field_off_Handler(...)        _ROM_LIB_MAKE_FUNC_NAME(HW_field_off_Handler) (__VA_ARGS__)
#define HW_nvm_Handler(...)        _ROM_LIB_MAKE_FUNC_NAME(HW_nvm_Handler) (__VA_ARGS__)

// H bridge
#define m_get_hb_stat(...)      _ROM_LIB_MAKE_FUNC_NAME(m_get_hb_stat) (__VA_ARGS__)
#define set_hb_switch(...)      _ROM_LIB_MAKE_FUNC_NAME(set_hb_switch) (__VA_ARGS__)
#define set_hb_eventctrl(...)   _ROM_LIB_MAKE_FUNC_NAME(set_hb_eventctrl) (__VA_ARGS__)
#define set_hb_config(...)      _ROM_LIB_MAKE_FUNC_NAME(set_hb_config) (__VA_ARGS__)
#define set_hb_event(...)       _ROM_LIB_MAKE_FUNC_NAME(set_hb_event) (__VA_ARGS__)
#define discharge_CA(...)       _ROM_LIB_MAKE_FUNC_NAME(discharge_CA) (__VA_ARGS__)

// HW div
#define calc_div(...)        _ROM_LIB_MAKE_FUNC_NAME(calc_div) (__VA_ARGS__)
#define clear_div_err(...)        _ROM_LIB_MAKE_FUNC_NAME(clear_div_err) (__VA_ARGS__)
#define get_hwdiv_div0_state(...)        _ROM_LIB_MAKE_FUNC_NAME(get_hwdiv_div0_state) (__VA_ARGS__)
#define get_hwdiv_ovf_state(...)        _ROM_LIB_MAKE_FUNC_NAME(get_hwdiv_ovf_state) (__VA_ARGS__)

// NFC handler
#define nfc_init(...)        _ROM_LIB_MAKE_FUNC_NAME(nfc_init) (__VA_ARGS__)
#define read_frame(...)        _ROM_LIB_MAKE_FUNC_NAME(read_frame) (__VA_ARGS__)
#define classify_frame(...)        _ROM_LIB_MAKE_FUNC_NAME(classify_frame) (__VA_ARGS__)
#define classify_command(...)        _ROM_LIB_MAKE_FUNC_NAME(classify_command) (__VA_ARGS__)
#define reset_CRC_peripheral(...)        _ROM_LIB_MAKE_FUNC_NAME(reset_CRC_peripheral) (__VA_ARGS__)
#define clear_ret_frame(...)        _ROM_LIB_MAKE_FUNC_NAME(clear_ret_frame) (__VA_ARGS__)
#define is_CRC_A_correct(...)        _ROM_LIB_MAKE_FUNC_NAME(is_CRC_A_correct) (__VA_ARGS__)
#define is_selected(...)        _ROM_LIB_MAKE_FUNC_NAME(is_selected) (__VA_ARGS__)
#define send_return_frame(...)        _ROM_LIB_MAKE_FUNC_NAME(send_return_frame) (__VA_ARGS__)
#define handle_anticollision(...)        _ROM_LIB_MAKE_FUNC_NAME(handle_anticollision) (__VA_ARGS__)
#define handle_DAND_protocol(...)        _ROM_LIB_MAKE_FUNC_NAME(handle_DAND_protocol) (__VA_ARGS__)
#define send_p_error(...)        _ROM_LIB_MAKE_FUNC_NAME(send_p_error) (__VA_ARGS__)
#define nfc_state_machine(...)        _ROM_LIB_MAKE_FUNC_NAME(nfc_state_machine) (__VA_ARGS__)
#define cleanup_routine(...)        _ROM_LIB_MAKE_FUNC_NAME(cleanup_routine) (__VA_ARGS__)
#define serve_hw_field_off_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(serve_hw_field_off_irq) (__VA_ARGS__)

// NVM
#define switch_on_nvm(...)        _ROM_LIB_MAKE_FUNC_NAME(switch_on_nvm) (__VA_ARGS__)
#define switch_off_nvm(...)        _ROM_LIB_MAKE_FUNC_NAME(switch_off_nvm) (__VA_ARGS__)
#define nvm_config(...)        _ROM_LIB_MAKE_FUNC_NAME(nvm_config) (__VA_ARGS__)
#define nvm_open_assembly_buffer(...)        _ROM_LIB_MAKE_FUNC_NAME(nvm_open_assembly_buffer) (__VA_ARGS__)
#define nvm_program_page(...)        _ROM_LIB_MAKE_FUNC_NAME(nvm_program_page) (__VA_ARGS__)
#define nvm_program_verify(...)        _ROM_LIB_MAKE_FUNC_NAME(nvm_program_verify) (__VA_ARGS__)
#define nvm_abort_program(...)        _ROM_LIB_MAKE_FUNC_NAME(nvm_abort_program) (__VA_ARGS__)
#define nvm_erase_page(...)        _ROM_LIB_MAKE_FUNC_NAME(nvm_erase_page) (__VA_ARGS__)
#define get_nvm_access_state(...)        _ROM_LIB_MAKE_FUNC_NAME(get_nvm_access_state) (__VA_ARGS__)
#define get_jtag_id(...)        _ROM_LIB_MAKE_FUNC_NAME(get_jtag_id) (__VA_ARGS__)

// RFU: NVM
#define serve_nvmirq(...)        _ROM_LIB_MAKE_FUNC_NAME(serve_nvmirq) (__VA_ARGS__)

// PMU
#define config_stbtm(...)        _ROM_LIB_MAKE_FUNC_NAME(config_stbtm) (__VA_ARGS__)
#define get_standby_time(...)        _ROM_LIB_MAKE_FUNC_NAME(get_standby_time) (__VA_ARGS__)
#define get_wakeup_source(...)        _ROM_LIB_MAKE_FUNC_NAME(get_wakeup_source) (__VA_ARGS__)
#define request_power_saving_mode(...)        _ROM_LIB_MAKE_FUNC_NAME(request_power_saving_mode) (__VA_ARGS__)
#define check_supply_source(...)        _ROM_LIB_MAKE_FUNC_NAME(check_supply_source) (__VA_ARGS__)
#define single_shot_systick(...)        _ROM_LIB_MAKE_FUNC_NAME(single_shot_systick) (__VA_ARGS__)

// RFU: PMU
#define dft_preamble1(...)        _ROM_LIB_MAKE_FUNC_NAME(dft_preamble1) (__VA_ARGS__)
#define dft_preamble2(...)        _ROM_LIB_MAKE_FUNC_NAME(dft_preamble2) (__VA_ARGS__)

// RTC
#define rtc_init(...)        _ROM_LIB_MAKE_FUNC_NAME(rtc_init) (__VA_ARGS__)
#define rtc_control(...)        _ROM_LIB_MAKE_FUNC_NAME(rtc_control) (__VA_ARGS__)
#define rtc_get(...)        _ROM_LIB_MAKE_FUNC_NAME(rtc_get) (__VA_ARGS__)

// Sense ctrl
#define sense_ctrl_config(...)        _ROM_LIB_MAKE_FUNC_NAME(sense_ctrl_config) (__VA_ARGS__)
#define set_dac_value(...)        _ROM_LIB_MAKE_FUNC_NAME(set_dac_value) (__VA_ARGS__)
#define switch_on_sense(...)        _ROM_LIB_MAKE_FUNC_NAME(switch_on_sense) (__VA_ARGS__)
#define switch_off_sense(...)        _ROM_LIB_MAKE_FUNC_NAME(switch_off_sense) (__VA_ARGS__)
#define sense_sh_config(...)        _ROM_LIB_MAKE_FUNC_NAME(sense_sh_config) (__VA_ARGS__)
#define sense_comp_config(...)        _ROM_LIB_MAKE_FUNC_NAME(sense_comp_config) (__VA_ARGS__)
#define sense_sh(...)        _ROM_LIB_MAKE_FUNC_NAME(sense_sh) (__VA_ARGS__)
#define set_aout(...)        _ROM_LIB_MAKE_FUNC_NAME(set_aout) (__VA_ARGS__)
#define get_nfc_value(...)        _ROM_LIB_MAKE_FUNC_NAME(get_nfc_value) (__VA_ARGS__)

// RFU: Sense ctrl
#define serve_adc_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(serve_adc_irq) (__VA_ARGS__)

// SSP
#define control_spi(...)        _ROM_LIB_MAKE_FUNC_NAME(control_spi) (__VA_ARGS__)
#define config_spi(...)        _ROM_LIB_MAKE_FUNC_NAME(config_spi) (__VA_ARGS__)
#define config_ssp_irqs(...)        _ROM_LIB_MAKE_FUNC_NAME(config_ssp_irqs) (__VA_ARGS__)

// RFU: SSP
#define serve_ssp_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(serve_ssp_irq) (__VA_ARGS__)

// System timer
#define sys_tim_pwm_cfg(...)       _ROM_LIB_MAKE_FUNC_NAME(sys_tim_pwm_cfg) (__VA_ARGS__)
#define get_pwm_status(...)       _ROM_LIB_MAKE_FUNC_NAME(get_pwm_status) (__VA_ARGS__)
#define sys_tim_chn_cfg(...)       _ROM_LIB_MAKE_FUNC_NAME(sys_tim_chn_cfg) (__VA_ARGS__)
#define sys_tim_chn_control(...)       _ROM_LIB_MAKE_FUNC_NAME(sys_tim_chn_control) (__VA_ARGS__)
#define set_sys_tim_chn_period(...)       _ROM_LIB_MAKE_FUNC_NAME(set_sys_tim_chn_period) (__VA_ARGS__)
#define get_sys_tim_chn_period(...)       _ROM_LIB_MAKE_FUNC_NAME(get_sys_tim_chn_period) (__VA_ARGS__)
#define get_sys_tim_chn_timecount(...)       _ROM_LIB_MAKE_FUNC_NAME(get_sys_tim_chn_timecount) (__VA_ARGS__)
#define get_sys_tim_chn_status(...)       _ROM_LIB_MAKE_FUNC_NAME(get_sys_tim_chn_status) (__VA_ARGS__)
#define sys_tim_chn_evnt_cfg(...)       _ROM_LIB_MAKE_FUNC_NAME(sys_tim_chn_evnt_cfg) (__VA_ARGS__)
#define my_systim_singleshot(...)       _ROM_LIB_MAKE_FUNC_NAME(my_systim_singleshot) (__VA_ARGS__)

// RFU: System timer
#define serve_systim0_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(serve_systim0_irq) (__VA_ARGS__)
#define serve_systim1_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(serve_systim1_irq) (__VA_ARGS__)
#define serve_systim2_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(serve_systim2_irq) (__VA_ARGS__)
#define serve_systim3_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(serve_systim3_irq) (__VA_ARGS__)
#define serve_systim4_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(serve_systim4_irq) (__VA_ARGS__)
#define serve_systim5_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(serve_systim5_irq) (__VA_ARGS__)

// UART
#define set_uart_baudrate(...)        _ROM_LIB_MAKE_FUNC_NAME(set_uart_baudrate) (__VA_ARGS__)
#define init_uart(...)        _ROM_LIB_MAKE_FUNC_NAME(init_uart) (__VA_ARGS__)
#define set_uart_control(...)        _ROM_LIB_MAKE_FUNC_NAME(set_uart_control) (__VA_ARGS__)
#define transmit_bytes(...)        _ROM_LIB_MAKE_FUNC_NAME(transmit_bytes) (__VA_ARGS__)
#define configure_uart_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(configure_uart_irq) (__VA_ARGS__)

// RFU: UART
#define serve_uart_irq(...)        _ROM_LIB_MAKE_FUNC_NAME(serve_uart_irq) (__VA_ARGS__)

// WDT
#define init_wdt(...)        _ROM_LIB_MAKE_FUNC_NAME(init_wdt) (__VA_ARGS__)
#define disable_wdt(...)        _ROM_LIB_MAKE_FUNC_NAME(disable_wdt) (__VA_ARGS__)
#define config_wdt(...)        _ROM_LIB_MAKE_FUNC_NAME(config_wdt) (__VA_ARGS__)
#define lock_wdt(...)        _ROM_LIB_MAKE_FUNC_NAME(lock_wdt) (__VA_ARGS__)
#define unlock_wdt(...)        _ROM_LIB_MAKE_FUNC_NAME(unlock_wdt) (__VA_ARGS__)
#define serve_wdt(...)        _ROM_LIB_MAKE_FUNC_NAME(serve_wdt) (__VA_ARGS__)
#define set_wdt_limits(...)        _ROM_LIB_MAKE_FUNC_NAME(set_wdt_limits) (__VA_ARGS__)
#define wdt_service_window(...)        _ROM_LIB_MAKE_FUNC_NAME(wdt_service_window) (__VA_ARGS__)

// RFU: WDT
#define serve_wdtirq(...)        _ROM_LIB_MAKE_FUNC_NAME(serve_wdtirq) (__VA_ARGS__)

//__STATIC_FORCEINLINE void set_hb_switch(bool hs1_set, bool ls1_set, bool hs2_set, bool ls2_set)
//{
//    rom_func_table.m_set_hb_switch(hs1_set, ls1_set, hs2_set, ls2_set);
//}

#endif


#ifdef __cplusplus
}
#endif

/** @} */ /* End of group ROM_library block */

/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */

#endif /* _ROM_LIB_H_ */
