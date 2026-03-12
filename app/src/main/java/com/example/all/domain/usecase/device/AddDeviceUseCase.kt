package com.example.all.domain.usecase.device

import android.nfc.Tag
import com.example.all.data.repository.DeviceRepository
import com.example.all.data.repository.NfcRepository
import com.example.all.domain.model.Device
import java.io.IOException
import javax.inject.Inject

/**
 * 添加设备用例 —— NFC 读取 deviceId 后写入本地
 *
 * 完整流程：
 * 1. 从 NFC Tag 建立 IsoDep 连接
 * 2. 发送读取 ID 指令获取 deviceId
 * 3. 断开 NFC 连接
 * 4. 将设备信息写入 Room 数据库
 *
 * Phase 2 演进：步骤 4 同时调用 POST /devices/bind 向云端注册。
 *
 * 调用链：AddDeviceViewModel → AddDeviceUseCase → NfcRepository + DeviceRepository
 */
class AddDeviceUseCase @Inject constructor(
    private val nfcRepository: NfcRepository,
    private val deviceRepository: DeviceRepository
) {
    /**
     * @param nickname 用户输入的设备昵称
     * @param tag      Android 系统发现的 NFC Tag
     * @return 成功返回新设备，失败返回异常
     */
    suspend operator fun invoke(nickname: String, tag: Tag): Result<Device> {
        return try {
            // 1. 建立 IsoDep 连接
            val isoDep = nfcRepository.connect(tag)
            // 2. 读取硬件 deviceId
            val deviceId = nfcRepository.readDeviceId(isoDep)
            // 3. 断开连接（读取完成即可释放 NFC）
            nfcRepository.disconnect(isoDep)
            // 4. 写入 Room 数据库
            val device = deviceRepository.addDevice(deviceId, nickname)
            // TODO Phase 2: 同时调用 POST /devices/bind 向云端注册
            Result.success(device)
        } catch (e: IOException) {
            Result.failure(Exception("NFC 读取失败，请重新靠近设备"))
        } catch (e: Exception) {
            Result.failure(e)
        }
    }
}
