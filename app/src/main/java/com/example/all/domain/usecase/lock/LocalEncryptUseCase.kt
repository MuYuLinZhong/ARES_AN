package com.example.all.domain.usecase.lock

import com.example.all.data.repository.CryptoRepository
import javax.inject.Inject

/**
 * 步骤 3 用例（Phase 1 版）：本地加密
 *
 * 用本地预存密钥对硬件随机数进行 AES 加密，生成密文。
 * 此密文将在步骤 4 发送给硬件进行比对验证。
 *
 * Phase 2 演进：此用例被替换为 RequestCipherUseCase（调用云端加密）。
 *               由于 HomeViewModel 通过接口调用，切换时 ViewModel 无需修改。
 *
 * 调用链：HomeViewModel → LocalEncryptUseCase → CryptoRepository(Local) → AES 加密
 */
class LocalEncryptUseCase @Inject constructor(
    private val cryptoRepository: CryptoRepository
) {
    /**
     * @param challenge 步骤 2 获得的硬件随机数
     * @return 成功返回加密密文，失败返回异常
     */
    suspend operator fun invoke(challenge: ByteArray): Result<ByteArray> =
        cryptoRepository.encryptLocal(challenge)
}
