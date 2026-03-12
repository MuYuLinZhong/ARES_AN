package com.example.all.domain.usecase.lock

import android.nfc.tech.IsoDep
import com.example.all.data.repository.NfcRepository
import com.example.all.domain.model.LockResult
import javax.inject.Inject

/**
 * 步骤 5 用例：接收硬件执行结果
 *
 * 硬件比对密文后执行机械动作（开锁/关锁），然后返回执行结果：
 * - 0x00 Success：操作成功
 * - 0x01 CipherMismatch：密文比对失败（密钥错误）
 * - 0x02 MechanicalFailure：机械检测失败（如门未关严）
 *
 * 调用链：HomeViewModel → ReceiveLockResultUseCase → NfcRepository → 硬件
 */
class ReceiveLockResultUseCase @Inject constructor(
    private val nfcRepository: NfcRepository
) {
    /**
     * @param isoDep 已建立的 NFC 连接
     * @return 硬件执行结果枚举
     * @throws java.io.IOException NFC 通信中断
     * @throws kotlinx.coroutines.TimeoutCancellationException 5 秒超时
     */
    suspend operator fun invoke(isoDep: IsoDep): LockResult =
        nfcRepository.receiveResult(isoDep)
}
