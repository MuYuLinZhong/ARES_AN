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

/** @file     smack_dataexchange.c
 *  @brief    Smack example: data declarations for the data exchange library.
 *
 *  This example defines a few datapoints, most of them with no further functionality,
 *  which are offered to an NFC reader through the data exchange library. An NFC reader
 *  can read or write these datapoints using the data exchange protocol.
 *  The reader writes a request frame to the mailbox and triggers evaluation by issuing
 *  a CALL_APP command. The reader and the NAC1080 firmware must agree upon the CALL_APP
 *  number, and the callback function of the data exchange library must be listed in the
 *  appropriate entry in APARAM (see app_prog[0] in sl_aparam.c).
 */

// standard libs
// included by core_cm0.h: #include <stdint.h>
#include <stddef.h>
#include "core_cm0.h"
#include <stdbool.h>
#include <string.h>

// Smack ROM lib
#include "rom_lib.h"
#include "nvm_params.h"

// Smack NVM lib
#include "aes_lib.h"
#include "smack_exchange.h"
#include "inet_lib.h"

// smack_sl project
#include "smack_sl.h"
#include "smack_dataexchange.h"



//-------------------------------------------------------------
// globals/statics

// The following variables are simply placeholders to be listen in data_point_list[]

static uint64_t uid;
static uint64_t scratch64;
static uint8_t scratch8;
static uint8_t scratch_str[100];
static uint8_t count8;

// measured values
static int16_t temperature;
static int16_t humidity;
static int16_t pressure;
static int32_t m_reserved;

static const aes_block_t aes_default_key =
{
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    }
};


//-------------------------------------------------------------
// data point list

static const data_point_entry_t data_point_list[] =
{
    // id               type                                             length             element             notify
    // status
    {0x0004,            data_point_uint64,                               sizeof(uint64_t),  &uid,               NULL, NULL},
    {0x0005,            data_point_uint64,                               sizeof(uint64_t),  &scratch64,         NULL, NULL},
    {0x0030,            data_point_uint8,                                sizeof(uint8_t),   &count8,            NULL, NULL},
    {0x0080,            data_point_int16,                                sizeof(uint16_t),  &temperature,       NULL, NULL},
    {0x0081,            data_point_int16,                                sizeof(uint16_t),  &humidity,          NULL, NULL},
    {0x0082,            data_point_int16,                                sizeof(uint16_t),  &pressure,          NULL, NULL},
    {0x0083,            data_point_int32,                                sizeof(uint32_t),  &m_reserved,        NULL, NULL},
    {0x1800,            data_point_int64  | data_point_write,            sizeof(int64_t),   &scratch64,         NULL, NULL},
    {0x1801,            data_point_string | data_point_write,            sizeof(scratch_str) - 1, &scratch_str, NULL, NULL},
    {0x1900,            data_point_uint8  | data_point_write,            sizeof(uint8_t),   &scratch8,          NULL, NULL},
    {0xF000,            data_point_uint32 | data_point_write,            sizeof(sl_counter), &sl_counter,       NULL, NULL},    // counter modified in smack_sl.c
    {0xF002,            data_point_uint8  | data_point_write,            sizeof(scratch8),  &scratch8,          NULL, NULL},
};
static const uint16_t data_point_count = (sizeof(data_point_list) / sizeof(data_point_list[0]));


//-------------------------------------------------------------

// Initialize the Smack exchange library with our datapoint list.
void vars_init(void)
{
    count8 = 1;
    uid = ((uint64_t)dparams.chip_uid.uid[0] << 48) |
          ((uint64_t)dparams.chip_uid.uid[1] << 40) |
          ((uint64_t)dparams.chip_uid.uid[2] << 32) |
          ((uint64_t)dparams.chip_uid.uid[3] << 24) |
          ((uint64_t)dparams.chip_uid.uid[4] << 16) |
          ((uint64_t)dparams.chip_uid.uid[5] <<  8) |
          ((uint64_t)dparams.chip_uid.uid[6] <<  0);

    // Setup NFC data point exchange
    // The smack_exchange_handler() callback must be configured in the APARAM block
    smack_exchange_init(data_point_list, data_point_count);

    smack_exchange_key_set(&aes_default_key);
}
