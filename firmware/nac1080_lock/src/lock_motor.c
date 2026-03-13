/**
 * @file    lock_motor.c
 * @brief   ARES Lock - H-Bridge motor control (Challenge-Response Protocol)
 *
 * Single OPERATION data point triggers motor actuation:
 *   0x01 = unlock (forward: HS1+LS2)
 *   0x02 = lock   (backward: HS2+LS1)
 *
 * Requires auth_ok from challenge-response authentication.
 * auth_ok is reset after execution — one auth = one operation.
 */

#include <stdint.h>
#include <stdbool.h>
#include "core_cm0.h"

#include "rom_lib.h"
#include "sys_tim_lib.h"

#include "lock_config.h"
#include "lock_datapoints.h"
#include "lock_auth.h"
#include "lock_motor.h"

#ifndef WAIT_ABOUT_1MS
#define WAIT_ABOUT_1MS  0x8000
#endif

static void drive_motor(bool forward);


void lock_motor_init(void)
{
    for (uint8_t j = 0; j < 4; j++) {
        set_singlegpio_alt(8 + j, 0, 3);
        single_gpio_iocfg(true, false, true, false, false, 8 + j);
    }

    set_hb_eventctrl(false);
    set_hb_switch(false, false, false, false);
}


void on_operation_write(uint16_t dp_id)
{
    (void)dp_id;

    if (!lock_auth_is_authenticated()) {
        dp_lock_status = LOCK_STATUS_ERROR;
        dp_operation = 0;
        return;
    }

    dp_lock_status = LOCK_STATUS_IN_PROGRESS;

    if (dp_operation == OP_UNLOCK) {
        drive_motor(true);
        dp_lock_status = LOCK_STATUS_UNLOCKED;
    } else if (dp_operation == OP_LOCK) {
        drive_motor(false);
        dp_lock_status = LOCK_STATUS_LOCKED;
    } else {
        dp_lock_status = LOCK_STATUS_ERROR;
    }

    dp_operation = 0;
    lock_auth_reset();
}


static void drive_motor(bool forward)
{
    for (uint32_t i = 0; i < MOTOR_STEPS; i++) {
        if (forward) {
            set_hb_switch(true, false, false, true);
        } else {
            set_hb_switch(false, true, true, false);
        }
        sys_tim_singleshot_32(0, WAIT_ABOUT_1MS * MOTOR_STEP_DELAY_MS, 14);
    }
    set_hb_switch(false, false, false, false);
}
