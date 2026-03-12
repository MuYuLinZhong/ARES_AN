package com.example.all.domain.usecase.device

import com.example.all.data.repository.DeviceRepository
import javax.inject.Inject

/**
 * 解绑设备用例 —— 从本地删除设备记录
 *
 * Phase 1：仅删除 Room 记录
 * Phase 2：同时调用 DELETE /devices/{id}
 *
 * 调用链：DeviceDetailViewModel → RemoveDeviceUseCase → DeviceRepository → Room
 */
class RemoveDeviceUseCase @Inject constructor(
    private val deviceRepository: DeviceRepository
) {
    /**
     * @param deviceId 要解绑的设备 ID
     */
    suspend operator fun invoke(deviceId: String) =
        deviceRepository.removeDevice(deviceId)
}
