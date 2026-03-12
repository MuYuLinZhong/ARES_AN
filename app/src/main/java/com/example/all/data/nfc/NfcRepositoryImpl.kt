package com.example.all.data.nfc

import android.nfc.Tag
import android.nfc.tech.IsoDep
import com.example.all.data.repository.NfcRepository
import com.example.all.domain.model.LockResult
import com.example.all.domain.model.OperationType
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import kotlinx.coroutines.withTimeout
import javax.inject.Inject

/**
 * NFC 仓库实现 —— 封装与 NAC1080 无源智能锁的全部硬件通信
 *
 * 使用 IsoDep（ISO 14443-4）协议，通过 APDU 指令与硬件交互。
 * 所有 NFC I/O 操作在 Dispatchers.IO 上执行，避免阻塞主线程。
 *
 * APDU 指令格式说明（需根据 NAC1080 实际协议调整）：
 * - CLA: 0x00（标准类）
 * - INS: 操作码（0xA1=发送操作位, 0xA2=读取challenge, 0xA3=发送密文, 0xA4=读取结果, 0xA5=读取ID）
 * - P1/P2: 参数字节
 * - Lc: 数据长度
 * - Data: 数据载荷
 * - Le: 期望返回长度
 *
 * ⚠️ 下方 APDU 指令为示例格式，需在实际硬件联调时根据 NAC1080 通信协议修正。
 */
class NfcRepositoryImpl @Inject constructor() : NfcRepository {

    companion object {
        /** NFC 操作超时时间（毫秒），对应设置中 NFC 灵敏度 Medium */
        private const val NFC_TIMEOUT_MS = 5000L
        /** IsoDep 连接超时（毫秒） */
        private const val ISODEP_TIMEOUT_MS = 3000
    }

    /**
     * 从 NFC Tag 获取 IsoDep 实例并建立连接
     *
     * @param tag Android 系统发现的 NFC Tag 对象
     * @return 已连接的 IsoDep 实例
     * @throws java.io.IOException NFC 连接失败（设备移开、信号弱等）
     */
    override suspend fun connect(tag: Tag): IsoDep = withContext(Dispatchers.IO) {
        val isoDep = IsoDep.get(tag)
            ?: throw IllegalStateException("该 NFC 标签不支持 IsoDep 协议")
        isoDep.timeout = ISODEP_TIMEOUT_MS
        isoDep.connect()
        isoDep
    }

    /**
     * 读取设备 ID —— 发送 READ_ID 指令（INS=0xA5）
     *
     * @param isoDep 已建立连接的 IsoDep 实例
     * @return 设备 ID 字符串（如 "NAC1080-A1B2C3"）
     */
    override suspend fun readDeviceId(isoDep: IsoDep): String = withContext(Dispatchers.IO) {
        // 构造读取 ID 的 APDU 指令：CLA=00, INS=A5, P1=00, P2=00, Le=00
        val command = byteArrayOf(0x00, 0xA5.toByte(), 0x00, 0x00, 0x00)
        val response = isoDep.transceive(command)
        // 解析响应：去掉最后 2 字节状态字（SW1+SW2），剩余为 deviceId 的 UTF-8 编码
        validateSwBytes(response)
        String(response, 0, response.size - 2, Charsets.UTF_8)
    }

    /** 断开 NFC 连接，安全关闭 IsoDep */
    override suspend fun disconnect(isoDep: IsoDep) = withContext(Dispatchers.IO) {
        try {
            if (isoDep.isConnected) isoDep.close()
        } catch (_: Exception) {
            // 断开时的异常静默忽略（可能设备已移走）
        }
    }

    /**
     * 步骤 1：发送操作意图位，接收硬件回传的 deviceId
     *
     * APDU: CLA=00, INS=A1, P1=操作位(01开锁/02关锁), P2=00, Le=00
     * 响应: [deviceId bytes] + [SW1 SW2]
     *
     * @param isoDep 已连接的 IsoDep
     * @param operationType 开锁(0x01) 或 关锁(0x02)
     * @return 硬件返回的 deviceId，用于与用户选择的设备做匹配校验
     */
    override suspend fun sendIntentionBit(
        isoDep: IsoDep,
        operationType: OperationType
    ): String = withContext(Dispatchers.IO) {
        withTimeout(NFC_TIMEOUT_MS) {
            val command = byteArrayOf(
                0x00,
                0xA1.toByte(),
                operationType.commandByte,
                0x00,
                0x00
            )
            val response = isoDep.transceive(command)
            validateSwBytes(response)
            String(response, 0, response.size - 2, Charsets.UTF_8)
        }
    }

    /**
     * 步骤 2：接收硬件随机数 challenge
     *
     * APDU: CLA=00, INS=A2, P1=00, P2=00, Le=10(期望 16 字节)
     * 响应: [16 字节随机数] + [SW1 SW2]
     *
     * @return 硬件生成的 16 字节随机数，供后续加密使用
     */
    override suspend fun receiveChallenge(isoDep: IsoDep): ByteArray =
        withContext(Dispatchers.IO) {
            withTimeout(NFC_TIMEOUT_MS) {
                val command = byteArrayOf(0x00, 0xA2.toByte(), 0x00, 0x00, 0x10)
                val response = isoDep.transceive(command)
                validateSwBytes(response)
                // 去掉最后 2 字节状态字，返回纯 challenge 数据
                response.copyOfRange(0, response.size - 2)
            }
        }

    /**
     * 步骤 4：发送加密后的密文给硬件
     *
     * APDU: CLA=00, INS=A3, P1=00, P2=00, Lc=密文长度, Data=密文
     * 响应: [SW1 SW2]（仅状态字，无数据）
     *
     * @param cipher 经本地或云端加密后的密文字节数组
     */
    override suspend fun sendCipher(isoDep: IsoDep, cipher: ByteArray): Unit =
        withContext(Dispatchers.IO) {
            withTimeout(NFC_TIMEOUT_MS) {
                // 拼接 APDU 头 + 数据长度 + 密文数据
                val header = byteArrayOf(
                    0x00,
                    0xA3.toByte(),
                    0x00,
                    0x00,
                    cipher.size.toByte()
                )
                val command = header + cipher
                val response = isoDep.transceive(command)
                validateSwBytes(response)
            }
        }

    /**
     * 步骤 5：接收硬件执行结果
     *
     * APDU: CLA=00, INS=A4, P1=00, P2=00, Le=01(期望 1 字节结果码)
     * 响应: [1 字节结果码] + [SW1 SW2]
     * 结果码: 0x00=成功, 0x01=密文比对失败, 0x02=机械检测失败
     *
     * @return LockResult 枚举值
     */
    override suspend fun receiveResult(isoDep: IsoDep): LockResult =
        withContext(Dispatchers.IO) {
            withTimeout(NFC_TIMEOUT_MS) {
                val command = byteArrayOf(0x00, 0xA4.toByte(), 0x00, 0x00, 0x01)
                val response = isoDep.transceive(command)
                validateSwBytes(response)
                // 第一个字节是结果码
                LockResult.fromByte(response[0])
            }
        }

    /**
     * 校验 APDU 响应的状态字（SW1 + SW2）
     *
     * ISO 7816 标准：SW1=0x90, SW2=0x00 表示成功
     * 其他值表示硬件返回错误
     *
     * @throws IllegalStateException 状态字不为 9000 时抛出
     */
    private fun validateSwBytes(response: ByteArray) {
        if (response.size < 2) {
            throw IllegalStateException("NFC 响应数据过短（少于 2 字节）")
        }
        val sw1 = response[response.size - 2].toInt() and 0xFF
        val sw2 = response[response.size - 1].toInt() and 0xFF
        if (sw1 != 0x90 || sw2 != 0x00) {
            throw IllegalStateException(
                "NFC 硬件返回错误状态: SW=${String.format("%02X%02X", sw1, sw2)}"
            )
        }
    }
}
