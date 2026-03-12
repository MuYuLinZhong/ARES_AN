package com.example.all.data.repository

import android.nfc.Tag
import android.nfc.tech.IsoDep
import com.example.all.domain.model.LockResult
import com.example.all.domain.model.OperationType

/**
 * NFC 仓库接口 —— 封装与 NAC1080 硬件的全部 NFC 通信操作
 *
 * 所有阶段都使用 NfcRepositoryImpl 真实实现（NFC 是核心硬件交互，不做 Fake）。
 * 内部使用 IsoDep（ISO 14443-4）协议与无源锁通信。
 */
interface NfcRepository {

    /** 从 NFC Tag 获取 IsoDep 实例并建立连接 */
    suspend fun connect(tag: Tag): IsoDep

    /** 读取设备 ID —— 发送专用读取指令，解析硬件返回的 deviceId */
    suspend fun readDeviceId(isoDep: IsoDep): String

    /** 断开 NFC 连接 */
    suspend fun disconnect(isoDep: IsoDep)

    /**
     * 步骤 1：发送操作意图位（0x01 开锁 / 0x02 关锁），接收硬件返回的 deviceId
     * @return 硬件回传的 deviceId，用于校验是否为目标设备
     */
    suspend fun sendIntentionBit(isoDep: IsoDep, operationType: OperationType): String

    /**
     * 步骤 2：接收硬件随机数（challenge）
     * @return 硬件生成的随机字节数组，用于后续加密
     */
    suspend fun receiveChallenge(isoDep: IsoDep): ByteArray

    /**
     * 步骤 4：将加密后的密文发送给硬件
     * @param cipher 经本地或云端加密的密文字节数组
     */
    suspend fun sendCipher(isoDep: IsoDep, cipher: ByteArray)

    /**
     * 步骤 5：接收硬件执行结果
     * @return LockResult 枚举（Success / CipherMismatch / MechanicalFailure）
     */
    suspend fun receiveResult(isoDep: IsoDep): LockResult
}
