/* ============================================================================
** Copyright (c) 2021 Infineon Technologies AG
**               All rights reserved.
**               www.infineon.com
** ============================================================================
*
*/

/**************************************************************************//**
 * @file     smack.h
 * @brief    CMSIS Core Peripheral Access Layer Header File for
 *           smack Device
 * @version  V5.3.1
 * @date     09. July 2018
 ******************************************************************************/
/*
 * Copyright (c) 2009-2018 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SMACK_H
#define SMACK_H

#ifdef __cplusplus
extern "C" {
#endif


/* -------------------------  Interrupt Number Definition  ------------------------ */

typedef enum IRQn
{
/* -------------------  Processor Exceptions Numbers  ----------------------------- */
  NonMaskableInt_IRQn           = -14,     /*  2 Non Maskable Interrupt */
  HardFault_IRQn                = -13,     /*  3 HardFault Interrupt */



  SVCall_IRQn                   =  -5,     /* 11 SV Call Interrupt */

  PendSV_IRQn                   =  -2,     /* 14 Pend SV Interrupt */
  SysTick_IRQn                  =  -1,     /* 15 System Tick Interrupt */

/* -------------------  Processor Interrupt Numbers  ------------------------------ */
  Cl_uart_IRQn                  =   0,
  Event_Bus1_IRQn               =   1,
  Event_Bus2_IRQn               =   2,
  Event_Bus3_IRQn               =   3,
  Event_Bus4_IRQn               =   4,
  Event_Bus5_IRQn               =   5,
  Event_Bus6_IRQn               =   6,
  Event_Bus7_IRQn               =   7,
  Event_Bus8_IRQn               =   8,
  HPrio_Matrix0_IRQn            =   9,
  HPrio_Matrix1_IRQn            =   10,
  HPrio_Matrix2_IRQn            =   11,
  HPrio_Matrix3_IRQn            =   12,
  HPrio_Matrix4_IRQn            =   13,
  HPrio_Matrix5_IRQn            =   14,
  WDT_IRQn                      =   15,
  HW_field_off_IRQn             =   16,    // NFC Field loss
  HW_nvm_IRQn                   =   17,
  Unused1_IRQn                  =   18,
  Unused2_IRQn                  =   19,
  Unused3_IRQn                  =   20,
  Unused4_IRQn                  =   21,
  Unused5_IRQn                  =   22,
  Unused6_IRQn                  =   23,
  Unused7_IRQn                  =   24,
  Unused8_IRQn                  =   25,
  Unused9_IRQn                  =   26,
  Unused10_IRQn                 =   27,
  Unused11_IRQn                 =   28,
  Unused12_IRQn                 =   29,
  Unused13_IRQn                 =   30,
  Unused14_IRQn                 =   31
} IRQn_Type;


/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* -------  Start of section using anonymous unions and disabling warnings  ------- */
#if   defined (__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined (__ICCARM__)
  #pragma language=extended
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wc11-extensions"
  #pragma clang diagnostic ignored "-Wreserved-id-macro"
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning 586
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
  #warning Not supported compiler type
#endif


/* --------  Configuration of Smack Parameter  ----------------------------------- */
/* define once here: */
#define  XTAL            (28000000UL)     /* Oscillator frequency */

/* --------  Configuration of Core Peripherals  ----------------------------------- */
#define __CM0_REV                 0x0000U   /* Core revision r0p0 */
#define __MPU_PRESENT             0U        /* no MPU present */
#define __VTOR_PRESENT            0U        /* no VTOR present */
#define __NVIC_PRIO_BITS          2U        /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0U        /* Set to 1 if different SysTick Config is used */

#include "core_cm0.h"                       /* Processor and core peripherals */
#include "system_smack.h"                   /* System Header */


/* --------  End of section using anonymous unions and disabling warnings  -------- */
#if   defined (__CC_ARM)
  #pragma pop
#elif defined (__ICCARM__)
  /* leave anonymous unions enabled */
#elif (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
  #pragma clang diagnostic pop
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning restore
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
  #warning Not supported compiler type
#endif


#ifdef __cplusplus
}
#endif

#endif  /* SMACK_H */
