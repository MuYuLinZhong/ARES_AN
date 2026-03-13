#ifndef LOCK_AUTH_H_
#define LOCK_AUTH_H_

#include <stdint.h>
#include <stdbool.h>
#include "aes_lib.h"

/*
 * ARES Lock Firmware - Challenge-Response Authentication Module
 *
 * Security flow:
 *   1. App reads CHALLENGE  → notify_tx generates 16-byte hardware RNG
 *   2. App writes RESPONSE  → notify_rx verifies AES(storedKey, challenge) == response
 *   3. App reads AUTH_RESULT → 0x01 = success, 0x00 = failure
 *   4. App writes OPERATION → notify_rx checks auth_ok, drives motor, resets auth_ok
 *
 * Anti-replay:
 *   - Challenge is refreshed on every read (hardware RNG)
 *   - Challenge is invalidated after RESPONSE verification
 *   - auth_ok is reset after OPERATION execution
 */

/**
 * Load the pre-shared key from NVM (APARAM.secret[0..15]).
 */
void lock_auth_init(void);

/**
 * Check if authentication was successful in the current session.
 * @return true if auth_ok is set
 */
bool lock_auth_is_authenticated(void);

/**
 * Reset authentication state (called after OPERATION execution).
 */
void lock_auth_reset(void);

/**
 * notify_tx callback for CHALLENGE (0xF100).
 * Generates a fresh 16-byte random number via hardware RNG before it is sent.
 */
void on_challenge_read(uint16_t dp_id);

/**
 * notify_rx callback for RESPONSE (0xF101).
 * Verifies: AES_Encrypt(storedKey, challenge) == received response.
 * Sets auth_ok on success, clears challenge immediately.
 */
void on_response_write(uint16_t dp_id);

#endif /* LOCK_AUTH_H_ */
