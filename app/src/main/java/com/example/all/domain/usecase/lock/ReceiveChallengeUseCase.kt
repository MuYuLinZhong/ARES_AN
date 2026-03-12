package com.example.all.domain.usecase.lock

import android.nfc.tech.IsoDep
import com.example.all.data.repository.NfcRepository
import javax.inject.Inject

/**
 * 步骤 2 用例：接收硬件随机数（challenge）
 *
 * 硬件在收到操作意图位后生成 16 字节随机数作为挑战值。
 * 应用读取此随机数，交给加密模块（本地或云端）处理。
 *
 * 调用链：HomeViewModel → ReceiveChallengeUseCase → NfcRepository → 硬件
 */
class ReceiveChallengeUseCase @Inject constructor(
    private val nfcRepository: NfcRepository
) {
    /**
     * @param isoDep 已建立的 NFC 连接
     * @return 硬件生成的 16 字节随机数
     * @throws java.io.IOException NFC 通信中断
     * @throws kotlinx.coroutines.TimeoutCancellationException 5 秒超时
     */
    suspend operator fun invoke(isoDep: IsoDep): ByteArray =
        nfcRepository.receiveChallenge(isoDep)
}
