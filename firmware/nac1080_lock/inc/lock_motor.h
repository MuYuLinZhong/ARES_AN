#ifndef LOCK_MOTOR_H_
#define LOCK_MOTOR_H_

#include <stdint.h>

/*
 * ARES Lock Firmware - Motor Control Module (Challenge-Response Protocol)
 *
 * Single OPERATION data point replaces the old ARM+CONTROL two-step.
 * Motor actuation requires auth_ok == true (verified by challenge-response).
 */

/**
 * Initialize H-Bridge GPIO pins and ensure motor is stopped.
 */
void lock_motor_init(void);

/**
 * notify_rx callback for OPERATION (0xF103).
 * Checks auth_ok, drives motor, resets auth_ok.
 * Values: 0x01 = unlock (forward), 0x02 = lock (backward).
 */
void on_operation_write(uint16_t dp_id);

#endif /* LOCK_MOTOR_H_ */
