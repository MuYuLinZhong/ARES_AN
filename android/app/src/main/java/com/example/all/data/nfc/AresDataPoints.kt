package com.example.all.data.nfc

import com.infineon.smack.sdk.mailbox.datapoint.DataPoint
import com.infineon.smack.sdk.mailbox.datapoint.DataPointType

/**
 * ARES 挑战应答协议自定义数据点 — ID 与 NAC1080 固件 lock_datapoints.h 一一对齐
 *
 * 数据点分三组：
 *   基础信息（明文只读）  → UID, LOCK_ID, CHARGE_PERCENT, STATUS
 *   挑战应答（核心安全）  → CHALLENGE, RESPONSE, AUTH_RESULT
 *   操作指令（需认证后写） → OPERATION
 */
@Suppress("MagicNumber")
enum class AresDataPoints(
    override val value: UShort,
    override val dataType: DataPointType
) : DataPoint {

    UID(0x0004u, DataPointType.UINT64),
    LOCK_ID(0x0005u, DataPointType.UINT64),
    CHARGE_PERCENT(0x0012u, DataPointType.UINT8),
    STATUS(0x0020u, DataPointType.UINT8),

    CHALLENGE(0xF100u, DataPointType.ARRAY),
    RESPONSE(0xF101u, DataPointType.ARRAY),
    AUTH_RESULT(0xF102u, DataPointType.UINT8),

    OPERATION(0xF103u, DataPointType.UINT8);

    companion object {
        const val CHALLENGE_LENGTH: Byte = 16
        const val RESPONSE_LENGTH: Byte = 16

        const val AUTH_OK: Byte = 0x01
        const val AUTH_FAIL: Byte = 0x00

        const val OP_UNLOCK: Byte = 0x01
        const val OP_LOCK: Byte = 0x02
    }
}
