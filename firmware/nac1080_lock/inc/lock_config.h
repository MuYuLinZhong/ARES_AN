#ifndef LOCK_CONFIG_H_
#define LOCK_CONFIG_H_

#include <stdint.h>

/*
 * ARES Lock Firmware - Global Configuration (Challenge-Response Protocol)
 */

/* AES-128 key / challenge / response length in bytes */
#define LOCK_KEY_LENGTH         16
#define CHALLENGE_LENGTH        16
#define RESPONSE_LENGTH         16

/* Motor control timing (milliseconds, approximated via delay loops) */
#define MOTOR_STEP_DELAY_MS     50
#define MOTOR_STEPS             10

/* Charge level thresholds */
#define CHARGE_MIN_PERCENT      80
#define CHARGE_FULL_PERCENT     100

/* Lock status values (STATUS data point 0x0020) */
#define LOCK_STATUS_UNLOCKED    0x00
#define LOCK_STATUS_LOCKED      0x01
#define LOCK_STATUS_IN_PROGRESS 0x02
#define LOCK_STATUS_ERROR       0x07

/* Operation command values (OPERATION data point 0xF103) */
#define OP_UNLOCK               0x01
#define OP_LOCK                 0x02

/* Authentication result values (AUTH_RESULT data point 0xF102) */
#define AUTH_OK                 0x01
#define AUTH_FAIL               0x00

#endif /* LOCK_CONFIG_H_ */
