/**
 * @file    lock_main.c
 * @brief   ARES Lock Firmware - Entry point
 *
 * Initializes all subsystems and enters the main idle loop.
 * NFC communication is handled entirely by interrupt-driven
 * smack_exchange callbacks registered in the data point table.
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

/**
 * Hard fault handler - spin for a while to allow debugger connection
 * before the watchdog resets the chip.
 */
void hardfault_handler(void)
{
    uint32_t cnt = 30000000;
    while (cnt--) {
        __NOP();
    }
}

/**
 * SysTick handler - can be used for periodic tasks.
 * Currently unused; registered in APARAM for future use.
 */
static volatile uint32_t systick_counter = 0;
void systick_handler(void)
{
    systick_counter++;
}

/**
 * Firmware entry point.
 * Called by startup_smack.c after SystemInit().
 */
void _nvm_start(void);
void _nvm_start(void)
{
    /* 1. Initialize authentication (load key from NVM) */
    lock_auth_init();

    /* 2. Initialize data points (sets up smack_exchange) */
    lock_datapoints_init();

    /* 3. Initialize motor control (configure H-Bridge GPIO) */
    lock_motor_init();

    /* 4. Enter idle loop
     * All NFC communication is handled by smack_exchange_handler()
     * which is called from the APARAM app_prog[0] callback.
     * The WFI instruction puts the CPU to sleep until an interrupt
     * (NFC field event, timer, etc.) wakes it up.
     */
    while (true) {
        __WFI();
    }
}
