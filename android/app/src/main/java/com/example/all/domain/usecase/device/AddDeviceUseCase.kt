package com.example.all.domain.usecase.device

import android.util.Log
import com.example.all.data.repository.DeviceRepository
import com.example.all.data.repository.NfcRepository
import com.example.all.domain.model.Device
import kotlinx.coroutines.CompletableDeferred
import kotlinx.coroutines.coroutineScope
import kotlinx.coroutines.launch
import kotlinx.coroutines.withTimeout
import javax.inject.Inject

/**
 * 添加设备用例 —— 通过 SmAcK SDK 发现锁，读取信息后写入本地
 *
 * 完整流程：
 * 1. 通过 observeLock() 等待 NFC Tag 被 SmAcK SDK 发现
 * 2. 获取 lockId 作为 deviceId，读取 UID 作为序列号
 * 3. 将设备信息写入 Room 数据库
 */
class AddDeviceUseCase @Inject constructor(
    private val nfcRepository: NfcRepository,
    private val deviceRepository: DeviceRepository
) {
    companion object {
        private const val TAG = "AddDeviceUseCase"
        private const val DISCOVER_TIMEOUT_MS = 15_000L
    }

    /**
     * @param nickname 用户输入的设备昵称
     * @return 成功返回新设备，失败返回异常
     */
    suspend operator fun invoke(nickname: String): Result<Device> = coroutineScope {
        try {
            Log.d(TAG, "等待 NFC Tag 发现（超时 ${DISCOVER_TIMEOUT_MS}ms）...")

            val lockInfoDeferred = CompletableDeferred<com.example.all.domain.model.SmackLockInfo>()
            val nfcJob = launch {
                nfcRepository.observeLock().collect { info ->
                    if (info != null && !lockInfoDeferred.isCompleted) {
                        lockInfoDeferred.complete(info)
                    }
                }
            }

            try {
                val lockInfo = withTimeout(DISCOVER_TIMEOUT_MS) {
                    lockInfoDeferred.await()
                }
                val deviceId = lockInfo.lockId.toString(16).uppercase()
                Log.d(TAG, "发现锁 deviceId=$deviceId")

                val serialNo = try {
                    nfcRepository.readUid()
                } catch (e: Exception) {
                    Log.w(TAG, "读取 UID 失败，使用 deviceId 作为序列号", e)
                    deviceId
                }
                Log.d(TAG, "序列号=$serialNo，保存设备 nickname=$nickname")

                val device = deviceRepository.addDevice(deviceId, nickname)
                Result.success(device)
            } finally {
                nfcJob.cancel()
            }
        } catch (e: Exception) {
            Log.e(TAG, "添加设备失败", e)
            Result.failure(Exception("NFC 读取失败，请重新靠近设备: ${e.message}"))
        }
    }
}
