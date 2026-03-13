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
 * @file     dparam.h
 *
 * @brief    The DPARAM module implements the handling of device specific parameters.
 *
 * @version  v1.0
 * @date     2020-05-20
 *
 * @note
 */

/*lint -save -e960 */

#ifndef _DPARAM_H_
#define _DPARAM_H_

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


/** @addtogroup dparam
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
 * @defgroup group_dparam_interface Interface of the DPARAM module
 * @ingroup group_interface_description
 *
 * @{
 */


/** Both ::Dparams_t @b must use
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

/** chip_uid_t holds an unique ID for each individual chip
 *  for NFC and is comprised of:
 *  - manufacturing code
 *  - chip family code
 *  - lot number
 *  - wafer coordinates
 *  - ... to guarantee unique ID
 *
 * idx are generic data types because the test team has
 * encoded lot number and wafer coordinates in a way the FW does not
 * care about. All the FW needs to do is to provide a 8 byte wide
 * memory location.
 */
typedef struct
{
    param_t     uid[7];
    param_t     rfu;
} chip_uid_t;

/**
 *  Dparams_t holds device-specific parameters which are generated during
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
    /**                                                       byte address range [            ] (     ) bits used                           */
    param_t     sifo[640];                                                  /**< [0x27f:0x000] reserved for SIFO NVM test and config        */
    param_t     tpe[128];                                                   /**< [0x2ff:0x280] reserved for TPE                             */
    param_t     trim_main_osc[2];                                           /**< [0x301:0x300] (10 bit) trimming value for main oscillator  */
    param_t     trim_stb_osc;                                               /**< [0x302:0x302] (5 bit) trimming value for stb oscillator    */
    param_t     trim_bias;                                                  /**< [0x303:0x303] (4 bit) trimming value for bias currents     */
    param_t     trim_ldo_vddd;                                              /**< [0x304:0x304] (4 bit) trimming value for VDDD LDO          */
    param_t     trim_ldo_nvm;                                               /**< [0x305:0x305] (4 bit) trimming value for NVM LDO           */
    param_t     trim_ldo_sens;                                              /**< [0x306:0x306] (5 bit) trimming value for SENS LDO          */
    param_t     trim_prim_ldo;                                              /**< [0x307:0x307] (4 bit) trimming value for PRIM LDO          */
    param_t     trim_i2v;                                                   /**< [0x308:0x308] (6 bit) trimming value for I2V               */
    param_t     trim_nvm_ref;                                               /**< [0x309:0x309] (6 bit) trimming value for NVM REF           */
    param_t     trim_rfu[2];                                                /**< [0x30b:0x30a] trimming, reserved for future usage          */
    param32_t   cal_stb_osc;                                                /**< [0x30f:0x30c] calibration value for stb oscillator         */
    param32_t   cal_main_osc;                                               /**< [0x313:0x310] calibration value for main oscillator        */
    param32_t   ate_nvm_write_flag;                                         /**< [0x317:0x314] NVM erased and programmed by ATE indicator   */
    param_t     cal_adc_ref_voltage[4];                                     /**< [0x31b:0x318] calibration value of adc reference voltage   */
    param_t     rfu0[100];                                                  /**< [0x37f:0x320] 100 bytes reserved for future usage          */
    chip_uid_t  chip_uid;                                                   /**< [0x387:0x380] (56 bit) unique chip-id                      */
    param32_t   jtag_id;                                                    /**< [0x38b:0x388] JTAG_ID = 0x00000222                         */
    param_t     rfu1[20];                                                   /**< [0x39f:0x38c] 20 bytes reserved for future usage           */
    param_t     tag_type_2[96];                                             /**< [0x3a0:0x3ff] 96 Bytes Tag2 area                           */
} Dparams_t;



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
 * dparam_fe_init() tries to retrieve FE records from OTP,
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
extern void dparam_init(void);
extern uint16_t dparam_size_get(void);

/** dparam_pointer_get() returns pointer to
 * the current dparam record, it points to NVM.
 *
 * @return pointer to current dparam record
 */
extern const Dparams_t* dparam_pointer_get(void);


/**
 * @brief This function returns the JTAG-ID of the chip
 * @return 32-bit unsigned integer, representing JTAG-ID
 */
extern uint32_t get_jtag_id(void);


/*
==============================================================================
   6. INLINE FUNCTIONS
==============================================================================
*/

#ifdef __cplusplus
}
#endif

/** @} *//* End of group group_dparam_interface block */

/** @} */ /* End of group dparam block */

/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */


#endif /* _DPARAM_H_ */
