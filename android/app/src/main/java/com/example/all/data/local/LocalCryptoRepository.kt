package com.example.all.data.local

import com.example.all.data.local.security.LocalKeyManager
import com.example.all.data.repository.CipherResponse
import com.example.all.data.repository.CryptoRepository
import com.example.all.domain.model.LockOperationResult
import com.example.all.domain.model.OperationType
import javax.crypto.Cipher
import javax.crypto.spec.SecretKeySpec
import javax.inject.Inject

/**
 * 本地加密仓库 —— Phase 1 专用的 CryptoRepository 实现
 *
 * 职责：用本地预存密钥（AES-128）对硬件随机数进行加密，
 *       替代 Phase 2 的云端加密（POST /crypto/encrypt）。
 *
 * 加密算法：AES/ECB/NoPadding（根据 NAC1080 硬件协议要求）
 * 密钥来源：LocalKeyManager → DataStore 或 BuildConfig
 *
 * ⚠️ Phase 2 时整体替换为 RemoteCryptoRepository，通过 DI 绑定切换。
 */
class LocalCryptoRepository @Inject constructor(
    private val localKeyManager: LocalKeyManager
) : CryptoRepository {

    /**
     * 本地加密：读取预存密钥 → 构造 AES SecretKey → 加密 challenge
     *
     * @param challenge 硬件返回的随机字节数组（步骤 2 获得）
     * @return 成功返回密文字节数组，失败返回异常（如密钥格式错误）
     */
    override suspend fun encryptLocal(challenge: ByteArray): Result<ByteArray> {
        return try {
            // 从 LocalKeyManager 获取密钥字节数组
            val keyBytes = localKeyManager.getDeviceKey()
            // 构造 AES 密钥规范
            val secretKey = SecretKeySpec(keyBytes, "AES")
            // 创建 AES/ECB/NoPadding 加密器（NAC1080 协议要求，无填充）
            val cipher = Cipher.getInstance("AES/ECB/NoPadding")
            cipher.init(Cipher.ENCRYPT_MODE, secretKey)
            // 执行加密，返回密文
            Result.success(cipher.doFinal(challenge))
        } catch (e: Exception) {
            Result.failure(e)
        }
    }

    /**
     * 云端加密 —— Phase 1 不使用，调用时抛异常
     * Phase 2 由 RemoteCryptoRepository 实现
     */
    override suspend fun requestCipher(
        deviceId: String,
        challenge: ByteArray,
        operationType: OperationType
    ): Result<CipherResponse> {
        // TODO Phase 2: 替换为 RemoteCryptoRepository，调用 POST /crypto/encrypt
        throw UnsupportedOperationException("Phase 2 云端加密尚未实现")
    }

    /**
     * 上报结果 —— Phase 1 静默忽略（无云端上报需求）
     * Phase 2 由 RemoteCryptoRepository 实现
     */
    override suspend fun reportResult(operationId: String, result: LockOperationResult): Result<Unit> {
        // TODO Phase 2: 替换为 RemoteCryptoRepository，调用 POST /crypto/report
        return Result.success(Unit)
    }
}
