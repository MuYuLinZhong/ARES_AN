/* ============================================================================
** Copyright (c) 2021 Infineon Technologies AG
**               All rights reserved.
**               www.infineon.com
** ============================================================================
*
*/

/******************************************************************************
 * @file     startup_smack.c
 * @brief    CMSIS-Core(M) Device Startup File for a Cortex-M0 Device
 * @version  V2.0.0
 * @date     04. June 2019
 ******************************************************************************/
/*
 * Copyright (c) 2009-2019 Arm Limited. All rights reserved.
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

#include "smack.h"

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler Function Prototype
 *----------------------------------------------------------------------------*/
typedef void( *pFunc )( void );

/*----------------------------------------------------------------------------
  External References
 *----------------------------------------------------------------------------*/
extern uint32_t __INITIAL_SP;

extern __NO_RETURN void __NVM_PROGRAM_START(void);

/*----------------------------------------------------------------------------
  Internal References
 *----------------------------------------------------------------------------*/
void             Default_Handler(void);
void __NO_RETURN NVM_Reset_Handler  (void);

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/
/* Exceptions */
void NMI_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));

void Cl_uart_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void Event_Bus1_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void Event_Bus2_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void Event_Bus3_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void Event_Bus4_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void Event_Bus5_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void Event_Bus6_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void Event_Bus7_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void Event_Bus8_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void HPrio_Matrix0_Handler  (void) __attribute__ ((weak, alias("Default_Handler")));
void HPrio_Matrix1_Handler  (void) __attribute__ ((weak, alias("Default_Handler")));
void HPrio_Matrix2_Handler  (void) __attribute__ ((weak, alias("Default_Handler")));
void HPrio_Matrix3_Handler  (void) __attribute__ ((weak, alias("Default_Handler")));
void HPrio_Matrix4_Handler  (void) __attribute__ ((weak, alias("Default_Handler")));
void HPrio_Matrix5_Handler  (void) __attribute__ ((weak, alias("Default_Handler")));
void WDT_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void HW_field_off_Handler   (void) __attribute__ ((weak, alias("Default_Handler")));
void HW_nvm_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void Unused1_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void Unused2_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void Unused3_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void Unused4_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void Unused5_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void Unused6_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void Unused7_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void Unused8_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void Unused9_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void Unused10_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void Unused11_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void Unused12_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void Unused13_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void Unused14_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));



/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

/* In the NAC1080/NGC1081 a copy of the vector table is used to determine the presence
 * of an NVM application. Two fields are evaluated:
 * - Initial stack pointer
 * - Reset vector
 * The initial stack pointer must point to the same location as the ROM's initial stack
 * pointer, that is the end of RAM1 (0x22000).
 * The NVM reset vector determines the entry point of the NVM application. Usually, this
 * is NVM_Reset_Handler(), which performs some initialization stuff and then calls
 * _nvm_start(), which is the first user defined function that is called.
 * The remaining entries of this copy of the vector table are not used and may be omitted
 * to save space.
 */
extern const pFunc __VECTOR_TABLE[48];
const pFunc __VECTOR_TABLE[48] __VECTOR_TABLE_ATTRIBUTE =
{
    /*lint -e(611) warning suspicious cast */
    (pFunc)(&__INITIAL_SP),                   /*     Initial Stack Pointer */
    NVM_Reset_Handler,                        /*     Reset Handler */
    NMI_Handler,                              /* -14 NMI Handler */
    HardFault_Handler,                        /* -13 Hard Fault Handler */
    0,                                        /*     Reserved */
    0,                                        /*     Reserved */
    0,                                        /*     Reserved */
    0,                                        /*     Reserved */
    0,                                        /*     Reserved */
    0,                                        /*     Reserved */
    0,                                        /*     Reserved */
    SVC_Handler,                              /*  -5 SVCall Handler */
    0,                                        /*     Reserved */
    0,                                        /*     Reserved */
    PendSV_Handler,                           /*  -2 PendSV Handler */
    SysTick_Handler,                          /*  -1 SysTick Handler */

    /* Interrupts */
    Cl_uart_Handler,                          /*   0 Interrupt 0 */
    Event_Bus1_Handler,                       /*   1 Interrupt 1 */
    Event_Bus2_Handler,                       /*   2 Interrupt 2 */
    Event_Bus3_Handler,                       /*   3 Interrupt 3 */
    Event_Bus4_Handler,                       /*   4 Interrupt 4 */
    Event_Bus5_Handler,                       /*   5 Interrupt 5 */
    Event_Bus6_Handler,                       /*   6 Interrupt 6 */
    Event_Bus7_Handler,                       /*   7 Interrupt 7 */
    Event_Bus8_Handler,                       /*   8 Interrupt 8 */
    HPrio_Matrix0_Handler,                    /*   9 Interrupt 9 */
    HPrio_Matrix1_Handler,                    /*  10 Interrupt 10 */
    HPrio_Matrix2_Handler,                    /*  11 Interrupt 11 */
    HPrio_Matrix3_Handler,                    /*  12 Interrupt 12 */
    HPrio_Matrix4_Handler,                    /*  13 Interrupt 13 */
    HPrio_Matrix5_Handler,                    /*  14 Interrupt 14 */
    WDT_Handler,                              /*  15 Interrupt 15 */
    HW_field_off_Handler,                     /*  16 Interrupt 16 */
    HW_nvm_Handler,                           /*  17 Interrupt 17 */
    Unused1_Handler,                          /*  18 Interrupt 18 */
    Unused2_Handler,                          /*  19 Interrupt 19 */
    Unused3_Handler,                          /*  20 Interrupt 20 */
    Unused4_Handler,                          /*  21 Interrupt 21 */
    Unused5_Handler,                          /*  22 Interrupt 22 */
    Unused6_Handler,                          /*  23 Interrupt 23 */
    Unused7_Handler,                          /*  24 Interrupt 24 */
    Unused8_Handler,                          /*  25 Interrupt 25 */
    Unused9_Handler,                          /*  26 Interrupt 26 */
    Unused10_Handler,                         /*  27 Interrupt 27 */
    Unused11_Handler,                         /*  28 Interrupt 28 */
    Unused12_Handler,                         /*  29 Interrupt 29 */
    Unused13_Handler,                         /*  30 Interrupt 30 */
    Unused14_Handler                          /*  31 Interrupt 31 */
};

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
void NVM_Reset_Handler(void)
{
    __NVM_PROGRAM_START();                    /* Enter NVM PreMain (C library entry point) */
}

/*----------------------------------------------------------------------------
  Default Handler for Exceptions / Interrupts
 *----------------------------------------------------------------------------*/
void Default_Handler(void)
{
    /*lint -e(716) while(1) */
    while (1)
    {
        ;
    }
}

