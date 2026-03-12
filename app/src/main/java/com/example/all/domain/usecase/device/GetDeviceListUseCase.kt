package com.example.all.domain.usecase.device

import com.example.all.data.repository.DeviceRepository
import com.example.all.domain.model.Device
import kotlinx.coroutines.flow.Flow
import javax.inject.Inject

/**
 * 获取设备列表用例 —— 返回设备列表的响应式 Flow
 *
 * 底层 Room 数据库变化时自动推送新列表。
 * DeviceListViewModel 通过 collect 订阅此 Flow 实现实时刷新。
 *
 * 调用链：DeviceListViewModel → GetDeviceListUseCase → DeviceRepository → Room Flow
 */
class GetDeviceListUseCase @Inject constructor(
    private val deviceRepository: DeviceRepository
) {
    /** @return 设备列表 Flow，按昵称排序 */
    operator fun invoke(): Flow<List<Device>> =
        deviceRepository.observeDevices()
}
