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

/** @file     smack_sl.c
 *  @brief    holds smack_sl() which serves as the entry point for the application code after all the low-level ROM boot code is done
 */

// standard libs
// included by core_cm0.h: #include <stdint.h>
#include "core_cm0.h"
#include <stdbool.h>

// Smack ROM lib
#include "rom_lib.h"

// Smack NVM lib
#include "sys_tim_lib.h"

// smack_sl project
#include "smack_sl.h"
#include "smack_dataexchange.h"


#ifndef WAIT_ABOUT_1MS
#define WAIT_ABOUT_1MS   0x8000   //!< clock tick constant ~1ms @ 28MHz
#endif


/* NDEF tag defined by user.
 * To activate this tag, set the field "tag_type_2_ptr" in aparams.
 */
const uint8_t smack_sl_tag[] =                    /**< [0x3a0:0x3ff] 96 Bytes Tag2 area          */
{
    0x05, 0xc0, 0xbe, 0xef,      /**< BLOCK0 UID0,1,2,3,4 */
    0xde, 0xad, 0x00, 0x00,      /**< BLOCK1 UID5,6,7,0x00 */
    0xff, 0xff, 0xff, 0xff,      /**< BLOCK2 Internal Lock  Byte0 and Byte1 are relevant 0xff means read only*/
    0xE1,                        /**< Capability Container CC_0 Magic Number fixed to 0xE1 for type 2 Tag */
    0x10,                        /**< Capability Container CC_1 Mapping version default 0x10 */
    0x0b,                        /**< Capability Container CC_2 size 22 Blocks --> 88 Bytes / 8 = 11d */
    0x0f,                        /**< Capability Container CC_3 Access Conditions , 0x0f  for read only tag w/o security*/
    0x03, 0x4f, 0xd1, 0x01,      /**< BLOCK4  NDEF, Length, MB + ME + Well known, ID length */
    0x4b, 0x54, 0x02,  'e',      /**< BLOCK5  Payload lenth, ID, language length, lang0 */
    'n',   'I',  'n',  'f',      /**< BLOCK6  lang1, payload .... */
    'i',   'n',  'e',  'o',      /**< BLOCK7  */
    'n',   ' ',  'T',  'e',      /**< BLOCK8  */
    'c',   'h',  'n',  'o',      /**< BLOCK9  */
    'l',   'o',  'g',  'i',      /**< BLOCK10 */
    'e',   's',  ' ',  'A',      /**< BLOCK11 */
    'G',   ' ',  ' ',  'N',      /**< BLOCK12 */
    'G',   'C',  '1',  '0',      /**< BLOCK13 */
    '8',   '0',  ' ',  ' ',      /**< BLOCK14 */
    'S',   'm',  'A',  'c',      /**< BLOCK15 */
    'K',   ' ',  ' ',  '0',      /**< BLOCK16 */
    '5',   'C',  '0',  'B',      /**< BLOCK17 */
    'E',   'E',  'F',  'D',      /**< BLOCK18 */
    'E',   'A',  'D',  '0',      /**< BLOCK19 */
    '0',   '0',  '0',  ' ',      /**< BLOCK20 */
    ' ',   'V',  '4',  '.',      /**< BLOCK21 */
    '0',   '.',  '1',  ' ',      /**< BLOCK22 */
    '(',   'N',  'V',  'M',      /**< BLOCK23 */
    ')',  0xfe, 0xff, 0xff,      /**< BLOCK24 */
    0xff, 0xff, 0xff, 0xff       /**< BLOCK25 not used */
};


// Globals:

// Offer a counter for external access
uint32_t sl_counter;


/**
 * @brief  Example for application specific interrupt service routine
 *         A function pointer to this routine must be registered in the sl_aparam.c file that defines the APARAM content.
 *         The routine will serve the sys_tick interrupts and will increment a variable which can be easily monitored
 *         by the attached debugger.
 *         If not used, remove this handler from APARAM in the sl_aparam.c file by setting the pointer to 0xffffffff
 *         (default content of an erased NVM).
 */
volatile uint32_t example_counter = 0;
void example_handler(void)
{
    example_counter++;
}

// In case of a Hardfault, spin in a loop for a while before resetting so that a debugger may connect and erase/update the faulty application
void hardfault_handler(void)
{
    uint32_t cnt = 30000000;

    while (cnt--)
    {
        __NOP();
    }
}


/**
 * @brief
 * _nvm_start() is the main() routine of the application code:
 * - when building the image (rom or ram), it is called by Reset_Handler() (see startup_smack.c) after SystemInit().
 * - when building and running the unit tests, it is not called, it is not subject to unit testing.
 * - when building and running integration and/or system tests, it is called by sc_main() upon simulation start.
 * This is also the reason of why it is called _nvm_start() and not main(): The VP has a higher layer main() which
 * calls sc_main() and _nvm_start(). Having two main's will fail when linking the VP executable.
 *
 * @note _nvm_start() cannot be made static, it is referenced by startup_smack.c and also by the interface
 * to the Virtual Prototype.
 *
 * @return nothing
 */
void _nvm_start(void);
void _nvm_start(void)
{

    // *******************Test of hb_ctrl; LED Blinking at HB Pins *********************

    uint32_t time  = 511;  // Controls the switching frequency, the timer will be loaded by multiples of 1ms (e.g., WAIT_ABOUT_1MS * time)
    uint32_t count = 300;
    uint32_t  i;
    /*lint -esym(550,dummy) variable dummy not accessed */
    uint8_t  dummy __attribute__((unused));

    // bring the 4 bit output of hb_ctrl to GPIO 11 downto 8
    for (uint8_t j = 0; j < 4; j++)
    {
        // to switch the HB switch control lines to GPIO8-GPIO11 as well
        set_singlegpio_alt(8 + j, 0, 3);
        dummy = single_gpio_iocfg( true, false, true, false, false, 8 + j);
    }

    sl_counter = 0;

    set_hb_eventctrl(false);

    // initialize the data exchange library
    vars_init();

    //set_hb_switch(bool hs1_set, bool ls1_set, bool hs2_set, bool ls2_set)
    i = 0;

    while (i < count)
    {
        set_hb_switch(true, false, false, false);
        set_hb_switch(true, false, false, true);
        sys_tim_singleshot_32(0, WAIT_ABOUT_1MS * time, 14);
        sl_counter++;

        set_hb_switch(true, false, false, false);
        set_hb_switch(true, false, true, false);
        sys_tim_singleshot_32(0, WAIT_ABOUT_1MS * time, 14);
        sl_counter++;

        set_hb_switch(false, false, true, false);
        set_hb_switch(false, true, true, false);
        sys_tim_singleshot_32(0, WAIT_ABOUT_1MS * time, 14);
        sl_counter++;

        set_hb_switch(false, false, true, false);
        set_hb_switch(true, false, true, false);
        sys_tim_singleshot_32(0, WAIT_ABOUT_1MS * time, 14);
        sl_counter++;

        i++;
    }

    set_hb_switch(false, false, false, false);

    // ****************** END BLINKING DEMO ********************


    // background task is just an endless
    /*lint -e(716) while(1) */
    while (true)
    {
        __WFI();
    }

}

