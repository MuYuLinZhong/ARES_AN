/**
 * @file    lock_datapoints.c
 * @brief   ARES Lock - Data point definitions (Challenge-Response Protocol)
 *
 * All data point IDs are aligned with Android AresDataPoints.kt.
 * No data_point_encrypt flags — security is at application level via challenge-response.
 *
 * The smack_exchange library handles reading/writing these data points via NFC.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "core_cm0.h"

#include "rom_lib.h"
#include "nvm_params.h"
#include "aes_lib.h"
#include "smack_exchange.h"

#include "lock_config.h"
#include "lock_datapoints.h"
#include "lock_auth.h"
#include "lock_motor.h"

/* ============ Data Point Storage Variables ============ */

uint32_t dp_firmware_version = 0x00020000;  /* v2.0.0 (challenge-response) */
uint8_t  dp_firmware_name[32] = "ARES_Lock_CR";
uint64_t dp_uid = 0;
uint64_t dp_lock_id = 0;
uint8_t  dp_charge_percent = 0;
uint8_t  dp_lock_status = LOCK_STATUS_LOCKED;

/* Challenge-response buffers */
uint8_t  dp_challenge[CHALLENGE_LENGTH] = {0};
uint8_t  dp_response[RESPONSE_LENGTH] = {0};
uint8_t  dp_auth_result = AUTH_FAIL;
uint8_t  dp_operation = 0;

/* ============ Data Point Table ============
 *
 * MUST be sorted in ascending order of data_point_id.
 * The smack_exchange library uses binary search on this table.
 */

static const data_point_entry_t data_point_list[] =
{
    /* ID               Type                            Len                     Value               notify_rx                   notify_tx */

    /* 0x0001 FIRMWARE_VERSION (UINT32, R) */
    {DP_FIRMWARE_VERSION, data_point_uint32,             sizeof(uint32_t),       &dp_firmware_version, NULL,                     NULL},

    /* 0x0003 FIRMWARE_NAME (STRING, R) */
    {DP_FIRMWARE_NAME,   data_point_string,              sizeof(dp_firmware_name)-1, &dp_firmware_name, NULL,                    NULL},

    /* 0x0004 UID (UINT64, R) */
    {DP_UID,             data_point_uint64,              sizeof(uint64_t),       &dp_uid,             NULL,                      NULL},

    /* 0x0005 LOCK_ID (UINT64, R) */
    {DP_LOCK_ID,         data_point_uint64,              sizeof(uint64_t),       &dp_lock_id,         NULL,                      NULL},

    /* 0x0012 CHARGE_PERCENT (UINT8, R) — plaintext, readable before auth */
    {DP_CHARGE_PERCENT,  data_point_uint8,               sizeof(uint8_t),        &dp_charge_percent,  NULL,                      NULL},

    /* 0x0020 STATUS (UINT8, R) — plaintext */
    {DP_STATUS,          data_point_uint8,               sizeof(uint8_t),        &dp_lock_status,     NULL,                      NULL},

    /* 0xF100 CHALLENGE (ARRAY 16, R) — notify_tx generates fresh random on every read */
    {DP_CHALLENGE,       data_point_array,               CHALLENGE_LENGTH,       &dp_challenge,       NULL,                      on_challenge_read},

    /* 0xF101 RESPONSE (ARRAY 16, W) — notify_rx verifies AES(key, challenge) */
    {DP_RESPONSE,        data_point_array | data_point_write,
                                                         RESPONSE_LENGTH,        &dp_response,        on_response_write,         NULL},

    /* 0xF102 AUTH_RESULT (UINT8, R) */
    {DP_AUTH_RESULT,     data_point_uint8,               sizeof(uint8_t),        &dp_auth_result,     NULL,                      NULL},

    /* 0xF103 OPERATION (UINT8, W) — notify_rx checks auth_ok then drives motor */
    {DP_OPERATION,       data_point_uint8 | data_point_write,
                                                         sizeof(uint8_t),        &dp_operation,       on_operation_write,        NULL},
};

static const uint16_t data_point_count = sizeof(data_point_list) / sizeof(data_point_list[0]);


void lock_datapoints_init(void)
{
    dp_uid = ((uint64_t)dparams.chip_uid.uid[0] << 48) |
             ((uint64_t)dparams.chip_uid.uid[1] << 40) |
             ((uint64_t)dparams.chip_uid.uid[2] << 32) |
             ((uint64_t)dparams.chip_uid.uid[3] << 24) |
             ((uint64_t)dparams.chip_uid.uid[4] << 16) |
             ((uint64_t)dparams.chip_uid.uid[5] <<  8) |
             ((uint64_t)dparams.chip_uid.uid[6] <<  0);

    dp_lock_id = dp_uid;

    smack_exchange_init(data_point_list, data_point_count);
}
