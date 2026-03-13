/**
 * @file    lock_auth.c
 * @brief   ARES Lock - Challenge-Response Authentication
 *
 * Implements hardware-RNG challenge generation and AES-128 response verification.
 *
 * Security properties:
 *   - CHALLENGE refreshed on every read via generate_random_number_lib() (hardware RNG)
 *   - CHALLENGE invalidated immediately after RESPONSE verification
 *   - auth_ok allows exactly ONE operation, then resets
 *   - Pre-shared key stored in NVM APARAM.secret[0..15]
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

/* Pre-shared key loaded from NVM */
static aes_block_t stored_key;
static bool key_loaded = false;

/* Current challenge (invalidated after verification) */
static bool challenge_valid = false;

/* Authentication state (reset after operation execution) */
static bool auth_ok = false;


void lock_auth_init(void)
{
    memcpy(&stored_key, aparams.secret, sizeof(aes_block_t));

    /* All-0xFF means erased NVM = no key provisioned */
    key_loaded = false;
    for (int i = 0; i < 16; i++) {
        if (stored_key.b[i] != 0xFF) {
            key_loaded = true;
            break;
        }
    }

    challenge_valid = false;
    auth_ok = false;
}


bool lock_auth_is_authenticated(void)
{
    return auth_ok;
}


void lock_auth_reset(void)
{
    auth_ok = false;
}


/**
 * notify_tx: Called BEFORE dp_challenge is sent to the reader.
 * Generate a fresh 16-byte random number using hardware RNG.
 */
void on_challenge_read(uint16_t dp_id)
{
    (void)dp_id;

    if (!key_loaded) {
        memset(dp_challenge, 0, CHALLENGE_LENGTH);
        challenge_valid = false;
        return;
    }

    aes_block_t rng;
    generate_random_number_lib(&rng);
    memcpy(dp_challenge, &rng, CHALLENGE_LENGTH);
    challenge_valid = true;

    /* Reset auth state for new challenge cycle */
    auth_ok = false;
    dp_auth_result = AUTH_FAIL;
}


/**
 * notify_rx: Called AFTER dp_response has been written by the reader.
 * Verify: AES_Encrypt(storedKey, challenge) == response.
 */
void on_response_write(uint16_t dp_id)
{
    (void)dp_id;

    dp_auth_result = AUTH_FAIL;

    if (!key_loaded || !challenge_valid) {
        auth_ok = false;
        goto cleanup;
    }

    /* Compute expected = AES_Encrypt(storedKey, challenge) */
    aes_block_t expected;
    aes_block_t challenge_block;
    memcpy(&challenge_block, dp_challenge, CHALLENGE_LENGTH);

    aes_load_key_ba(&stored_key);
    calc_aes_ba(&expected, &challenge_block, encrypt);

    /* Constant-time comparison to prevent timing attacks */
    uint8_t diff = 0;
    for (int i = 0; i < RESPONSE_LENGTH; i++) {
        diff |= expected.b[i] ^ dp_response[i];
    }

    if (diff == 0) {
        auth_ok = true;
        dp_auth_result = AUTH_OK;
    } else {
        auth_ok = false;
    }

cleanup:
    /* Invalidate challenge immediately — prevents replay */
    memset(dp_challenge, 0, CHALLENGE_LENGTH);
    challenge_valid = false;
}
