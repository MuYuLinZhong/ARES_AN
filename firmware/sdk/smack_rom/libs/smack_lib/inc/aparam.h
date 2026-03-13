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
 * @file     aparam.h
 *
 * @brief    The APARAM module implements the handling of application specific parameters.
 *
 * @version  v1.0
 * @date     2020-05-20
 *
 * @note
 */

/*lint -save -e960 */

#ifndef _APARAM_H_
#define _APARAM_H_

/*
 *
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/

#include "stdint.h"

/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Smack
 * @{
 */


/** @addtogroup aparam
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*

==============================================================================
   2. DEFINITIONS
==============================================================================
*/

/*
==============================================================================
   3. TYPES
==============================================================================
*/
/**
 * @defgroup group_aparam_interface Interface of the APARAM module
 * @ingroup group_interface_description
 *
 * @{
 */


/** Both ::Aparams_t @b must use
 * param_t as member types.
 * As per request from the tester team: During development and bring-up of the
 * actual test program, it is beneficial to work with a byte stream of
 * members which all are made from the same data type - it helps
 * to separate the individual members on byte level.
 * If members would be made of different basic data types, it is more error-prone
 * to distinguish the various members on byte level - the tester machine does not
 * support access on symbol level.
 *
 * Obviously, this is not required when using a debugger
 * like Lauterbach or alike: A debugger converts width of
 * member types in human readable information in its GUI. But if you
 * don't have such a debugger at the tester machine, you have to do
 * the byte-to-member translation yourself ...
 */
typedef uint8_t param_t;

/* for parameters which are handled as native 32bit values */
typedef uint32_t param32_t;

/* For function pointers, an atomic type is used, e.g. the pointer has not to be split into param_t fields.
 */
typedef uint32_t param_func_ptr_t;

/* Same for data pointers.
 */
typedef uint32_t param_ptr_t;

/**
 *  Aparams_t holds device-specific parameters which are generated during
 *  testing.
 *  Todo: Update comment
 *  The FE test is passing such parameters as a byte stream through the (proprietary)
 *  test interface into the device (ie. some memory) and calls a FW routine which in turn
 *  burns such parameters as DPARAM record into OTP. For more details see
 *  _dparams_record_store() in production.c.
 *
 *  More specifically, the test parameters are
 * - trimming values: they are copied to dedicated registers
 * - production specific values: they do not influence registers or firmware modules
 *
 */
typedef struct
{
    /**                                                       byte address range [         ] (     ) bits used                                    */
    param_t             prot_rom1;                                          /**< [0x403:0x400] (32) ROM1/ROM2/RAM1/RAM2 protection privilegs      */
    param_t             prot_rom2;                                          /**< [0x403:0x400] (32) ROM1/ROM2/RAM1/RAM2 protection privilegs      */
    param_t             prot_ram1;                                          /**< [0x403:0x400] (32) ROM1/ROM2/RAM1/RAM2 protection privilegs      */
    param_t             prot_ram2;                                          /**< [0x403:0x400] (32) ROM1/ROM2/RAM1/RAM2 protection privilegs      */
    param_t             prot_hw1;                                           /**< [0x407:0x404] (32) HW1/HW2/HW3 protection privilegs              */
    param_t             prot_hw2;                                           /**< [0x407:0x404] (32) HW1/HW2/HW3 protection privilegs              */
    param_t             prot_hw3;                                           /**< [0x407:0x404] (32) HW1/HW2/HW3 protection privilegs              */
    param_t             prot_rfu;                                           /**< [0x407:0x404] (32) HW1/HW2/HW3 protection privilegs              */
    param_func_ptr_t    app_prog[16];                                       /**< [0x447:0x408] (32 * 16) absolute address App function 0 through 15 */
    param32_t           bypass_mailbox;                                     /**< [0x44b:0x448] (32) 0xfca1fcb0 will bypass mailbox address check  */
    param32_t           default_uart_baudrate;                              /**< [0x44f:0x44c] (24) default UART baudrate in baud                 */
    param32_t           kill_debugger;                                      /**< [0x453:0x450] (32) 0x5deb0ff5 will block debugger access         */
    param32_t           disable_default_uart;                               /**< [0x457:0x454] (32) 0x11223344 disable UART per ROM code          */
    param32_t           message_disable;                                    /**< [0x45b:0x458] (32) <>0xffffffff: ignore external send message req */
    param_t             rfu0[36];                                           /**< [0x47f:0x45C] (288) reserved for future usage                    */

    param32_t           gpio_out_en;                                        /**< [0x483:0x480] (32)  0xabba + GPIO output enable                  */
    param32_t           gpio_in_en;                                         /**< [0x487:0x484] (32)  0xabba + GPIO input enable                   */
    param32_t           gpio_out_type;                                      /**< [0x48b:0x488] (32)  0xabba + GPIO output type                    */
    param32_t           gpio_pup_en;                                        /**< [0x48f:0x48c] (32)  0xabba + GPIO pull-up enable                 */
    param32_t           gpio_pdown_en;                                      /**< [0x493:0x490] (32)  0xabba + GPIO power-down enable              */
    param32_t           gpio_out_value;                                     /**< [0x497:0x494] (32)  0xabba + GPIO output value                   */
    param32_t           gpio_alt_valid;                                     /**< [0x49b:0x498] (32)  GPIO Alt Valid                               */
    param_t             gpio_alt[16];                                       /**< [0x49f:0x49c] (32)  GPIO Alt 0..3                                */
    /* */                                                                   /**< [0x4a3:0x4a0] (32)  GPIO Alt 4..7                                */
    /* */                                                                   /**< [0x4a7:0x4a4] (32)  GPIO Alt 8..11                               */
    /* */                                                                   /**< [0x4ab:0x4a8] (32)  GPIO Alt 12..15                              */
    param32_t           evbus_handler1_source;                              /**< [0x4af:0x4ac] (32)  0x00 + custom source of evbus1 irq           */
    param32_t           evbus_handler2_source;                              /**< [0x4b3:0x4b0] (32)  0x00 + custom source of evbus2 irq           */
    param32_t           evbus_handler3_source;                              /**< [0x4b7:0x4b4] (32)  0x00 + custom source of evbus3 irq           */
    param32_t           evbus_handler4_source;                              /**< [0x4bb:0x4b8] (32)  0x00 + custom source of evbus4 irq           */
    param32_t           evbus_handler5_source;                              /**< [0x4bf:0x4bc] (32)  0x00 + custom source of evbus5 irq           */
    param32_t           evbus_handler6_source;                              /**< [0x4c3:0x4c0] (32)  0x00 + custom source of evbus6 irq           */
    param32_t           evbus_handler7_source;                              /**< [0x4c7:0x4c4] (32)  0x00 + custom source of evbus7 irq           */
    param32_t           evbus_handler8_source;                              /**< [0x4cb:0x4c8] (32)  0x00 + custom source of evbus8 irq           */
    param32_t           hp_irq9_cfg;                                        /**< [0x4cf:0x4cc] (32)  0x00 + irq source of matrix irq              */
    param32_t           hp_irq10_cfg;                                       /**< [0x4d3:0x4d0] (32)  0x00 + irq source of matrix irq              */
    param32_t           hp_irq11_cfg;                                       /**< [0x4d7:0x4d4] (32)  0x00 + irq source of matrix irq              */
    param32_t           hp_irq12_cfg;                                       /**< [0x4db:0x4d8] (32)  0x00 + irq source of matrix irq              */
    param32_t           hp_irq13_cfg;                                       /**< [0x4df:0x4dc] (32)  0x00 + irq source of matrix irq              */
    param32_t           hp_irq14_cfg;                                       /**< [0x4e3:0x4e0] (32)  0x00 + irq source of matrix irq              */
    param32_t           hp_irq9_col_cfg;                                    /**< [0x4e7:0x4e4] (32)  0x00 + column config register                */
    param32_t           hp_irq10_col_cfg;                                   /**< [0x4eb:0x4e8] (32)  0x00 + column config register                */
    param32_t           hp_irq11_col_cfg;                                   /**< [0x4ef:0x4ec] (32)  0x00 + column config register                */
    param32_t           hp_irq12_col_cfg;                                   /**< [0x4f3:0x4f0] (32)  0x00 + column config register                */
    param32_t           hp_irq13_col_cfg;                                   /**< [0x4f7:0x4f4] (32)  0x00 + column config register                */
    param32_t           hp_irq14_col_cfg;                                   /**< [0x4fb:0x4f8] (32)  0x00 + column config register                */
    param_t             rfu1[4];                                            /**< [0x4ff:0x4fc] (32)  0x00 + column config register                */

    param_func_ptr_t    sense_adc_hand_addr;                                /**< [0x503:0x500] (32)  absolute address of custom hander            */
    param_func_ptr_t    timer0_hand_addr;                                   /**< [0x507:0x504] (32)  absolute address of custom hander            */
    param_func_ptr_t    timer1_hand_addr;                                   /**< [0x50b:0x508] (32)  absolute address of custom hander            */
    param_func_ptr_t    timer2_hand_addr;                                   /**< [0x50f:0x50c] (32)  absolute address of custom hander            */
    param_func_ptr_t    timer3_hand_addr;                                   /**< [0x513:0x510] (32)  absolute address of custom hander            */
    param_func_ptr_t    gpio_evnt_hand_addr;                                /**< [0x517:0x514] (32)  absolute address of custom hander            */
    param_func_ptr_t    gpio_evnt_gen_hand_addr;                            /**< [0x51b:0x518] (32)  absolute address of custom hander            */
    param_func_ptr_t    timer4_hand_addr;                                   /**< [0x51f:0x51c] (32)  absolute address of custom hander            */
    param_func_ptr_t    timer5_hand_addr;                                   /**< [0x523:0x520] (32)  absolute address of custom hander            */
    param_func_ptr_t    uart_hand_addr;                                     /**< [0x527:0x524] (32)  absolute address of custom hander            */
    param_func_ptr_t    ssp_hand_addr;                                      /**< [0x52b:0x528] (32)  absolute address of custom hander            */
    param_func_ptr_t    i2c_hand_addr;                                      /**< [0x52f:0x52c] (32)  absolute address of custom hander            */
    param_func_ptr_t    gpio0_hand_addr;                                    /**< [0x533:0x530] (32)  absolute address of custom hander            */
    param_func_ptr_t    gpio1_hand_addr;                                    /**< [0x537:0x534] (32)  absolute address of custom hander            */
    param_func_ptr_t    gpio2_hand_addr;                                    /**< [0x53b:0x538] (32)  absolute address of custom hander            */
    param_func_ptr_t    gpio3_hand_addr;                                    /**< [0x53f:0x53c] (32)  absolute address of custom hander            */
    param_func_ptr_t    gpio4_hand_addr;                                    /**< [0x543:0x540] (32)  absolute address of custom hander            */
    param_func_ptr_t    gpio5_hand_addr;                                    /**< [0x547:0x544] (32)  absolute address of custom hander            */
    param_func_ptr_t    gpio6_hand_addr;                                    /**< [0x54b:0x548] (32)  absolute address of custom hander            */
    param_func_ptr_t    gpio7_hand_addr;                                    /**< [0x54f:0x54c] (32)  absolute address of custom hander            */
    param_func_ptr_t    aes_hand_addr;                                      /**< [0x553:0x550] (32)  absolute address of custom hander            */
    param_func_ptr_t    hard_fault_hand_addr;                               /**< [0x557:0x554] (32)  absolute address of custom hander            */
    param_func_ptr_t    systick_hand_addr;                                  /**< [0x55b:0x558] (32)  absolute address of custom hander            */
    param_func_ptr_t    wdt_hand_addr;                                      /**< [0x55f:0x55c] (32)  absolute address of custom hander            */
    param_func_ptr_t    nvm_hand_addr;                                      /**< [0x563:0x560] (32)  absolute address of custom hander            */
    param_t             rfu2[24];                                           /**< [0x57b:0x564] (192) reserved for future usage                    */
    param_ptr_t         tag_type_2_ptr;                                     /**< [0x57f:0x57c] (32)  address of NFC tag information in NVM        */

    param_t             nvm_prot_sect[120];                                 /**< [0x5f7:0x580] (120*8) R/W protection of NVM pages 0..119         */
    param_t             rfu3[8];                                            /**< [0x5ff:0x5f8] (64)  reserved for future usage                    */

    param_t             secret[256];                                        /**< [0x6ff:0x600] reserved for secret application data               */
    param_t             public[256];                                        /**< [0x7ff:0x700] reserved for public application data               */
} Aparams_t;

/*
==============================================================================
   4. EXPORTED DATA
==============================================================================
*/

/*
==============================================================================
   5. FUNCTION PROTOTYPES
==============================================================================
*/

/**
 * aparam_init() tries to retrieve FE records from OTP,
 * copy them to their respective instance (::dparam_fe) and
 * populate the ptrs to such instance (::dparam_fe_ptr).
 * For the FE parameters a limit check is done.
 *
 * In case no ::OTP_FS_DPARAM_FE record is found in OTP, an exception is thrown.
 * In case the limit check for certain members of ::OTP_FS_DPARAM_FE fails, an
 * exception is thrown. This is required because the FE parameters are required
 * for correct operation of the device.
 *
 * @throws ::EXCEPTION_OUT_OF_RESOURCE    no ::OTP_FS_DPARAM_FE is found in OTP.
 */
extern void     aparam_init(void);
extern uint16_t aparam_size_get(void);

/** dparam_pointer_get() returns pointer to
 * the current dparam record, it points to NVM.
 *
 * @return pointer to current dparam record
 */
extern const Aparams_t* aparam_pointer_get(void);

/*
==============================================================================
   6. INLINE FUNCTIONS
==============================================================================
*/

#ifdef __cplusplus
}
#endif

/** @} *//* End of group group_aparam_interface block */

/** @} */ /* End of group aparam block */

/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */


#endif /* _APARAM_H_ */
