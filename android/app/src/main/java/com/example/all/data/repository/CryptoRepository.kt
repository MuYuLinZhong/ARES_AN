package com.example.all.data.repository

import com.example.all.domain.model.LockOperationResult
import com.example.all.domain.model.OperationType

/**
 * 加密仓库接口 —— 统一加密操作的入口
 *
 * Phase 1 实现：LocalCryptoRepository（本地预存密钥做 AES 加密）
 * Phase 2 实现：RemoteCryptoRepository（调用云端 POST /crypto/encrypt）
 *
 * 通过 Hilt DI 切换实现类，UseCase / ViewModel 无需修改。
 */
interface CryptoRepository {

    /**
     * Phase 1：本地加密 —— 用预存密钥对硬件随机数进行 AES 加密
     * @param challenge 硬件返回的随机数（字节数组）
     * @return 加密后的密文字节数组
     */
    suspend fun encryptLocal(challenge: ByteArray): Result<ByteArray>

    /**
     * Phase 2：云端加密 —— 将随机数发送到云端，由服务器用 HSM 密钥加密
     * @param deviceId      目标设备 ID
     * @param challenge     硬件随机数
     * @param operationType 操作类型（开锁/关锁）
     * @return 云端返回的密文和操作 ID
     */
    suspend fun requestCipher(
        deviceId: String,
        challenge: ByteArray,
        operationType: OperationType
    ): Result<CipherResponse>

    /**
     * Phase 2：上报操作结果 —— 将开/关锁结果异步上报到云端审计日志
     * @param operationId 云端加密时返回的操作 ID
     * @param result      硬件执行结果
     */
    suspend fun reportResult(operationId: String, result: LockOperationResult): Result<Unit>
}

/**
 * 云端加密响应 —— Phase 2 使用
 * @property operationId 云端分配的操作 ID（用于后续上报）
 * @property cipher      加密后的密文字节数组
 */
data class CipherResponse(
    val operationId: String,
    val cipher: ByteArray
)
