package com.example.all.domain.usecase.lock

import android.nfc.tech.IsoDep
import com.example.all.data.repository.NfcRepository
import javax.inject.Inject

/**
 * 步骤 4 用例：将密文发送给硬件
 *
 * 将步骤 3 加密得到的密文通过 NFC 发送给 NAC1080，
 * 硬件将用自身密钥独立计算并与收到的密文比对。
 *
 * 调用链：HomeViewModel → SendCipherToLockUseCase → NfcRepository → 硬件
 */
class SendCipherToLockUseCase @Inject constructor(
    private val nfcRepository: NfcRepository
) {
    /**
     * @param isoDep 已建立的 NFC 连接
     * @param cipher 加密后的密文字节数组
     * @throws java.io.IOException NFC 通信中断
     */
    suspend operator fun invoke(isoDep: IsoDep, cipher: ByteArray) =
        nfcRepository.sendCipher(isoDep, cipher)
}
