package com.example.all.domain.usecase.lock

import android.nfc.tech.IsoDep
import com.example.all.data.repository.NfcRepository
import com.example.all.domain.model.OperationType
import javax.inject.Inject

/**
 * 步骤 1 用例：发送操作意图位
 *
 * 向 NAC1080 发送开锁(0x01)或关锁(0x02)操作位，
 * 硬件收到后返回自身的 deviceId，供应用校验是否为目标设备。
 *
 * 调用链：HomeViewModel → SendIntentionBitUseCase → NfcRepository → 硬件
 *
 * @param nfcRepository NFC 仓库，封装了底层 APDU 通信
 */
class SendIntentionBitUseCase @Inject constructor(
    private val nfcRepository: NfcRepository
) {
    /**
     * @param isoDep        已建立的 NFC 连接
     * @param operationType 操作类型（开锁/关锁）
     * @return 硬件返回的 deviceId 字符串
     * @throws java.io.IOException NFC 通信中断
     */
    suspend operator fun invoke(
        isoDep: IsoDep,
        operationType: OperationType
    ): String = nfcRepository.sendIntentionBit(isoDep, operationType)
}
