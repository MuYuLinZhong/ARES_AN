#ifndef LOCK_DATAPOINTS_H_
#define LOCK_DATAPOINTS_H_

#include <stdint.h>
#include "smack_exchange.h"
#include "lock_config.h"

/*
 * ARES Lock Firmware - Data Point Definitions (Challenge-Response Protocol)
 *
 * Data point IDs are aligned with Android AresDataPoints.kt.
 * All data points are plaintext (no data_point_encrypt flag).
 * Security is handled at application level via challenge-response.
 */

/* ============ Data Point IDs ============ */

/* Basic information (read-only, plaintext) */
#define DP_FIRMWARE_VERSION     0x0001
#define DP_FIRMWARE_NAME        0x0003
#define DP_UID                  0x0004
#define DP_LOCK_ID              0x0005
#define DP_CHARGE_PERCENT       0x0012
#define DP_STATUS               0x0020

/* Challenge-response authentication */
#define DP_CHALLENGE            0xF100
#define DP_RESPONSE             0xF101
#define DP_AUTH_RESULT          0xF102

/* Operation command (requires auth_ok) */
#define DP_OPERATION            0xF103

/* ============ Public API ============ */

void lock_datapoints_init(void);

/* ============ Externally accessible variables ============ */

extern uint32_t dp_firmware_version;
extern uint8_t  dp_firmware_name[];
extern uint64_t dp_uid;
extern uint64_t dp_lock_id;
extern uint8_t  dp_charge_percent;
extern uint8_t  dp_lock_status;
extern uint8_t  dp_challenge[];
extern uint8_t  dp_response[];
extern uint8_t  dp_auth_result;
extern uint8_t  dp_operation;

#endif /* LOCK_DATAPOINTS_H_ */
