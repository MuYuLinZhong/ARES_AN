package com.example.all.domain.usecase.device

import com.example.all.domain.model.Device
import javax.inject.Inject

/**
 * 搜索设备用例 —— 纯本地按昵称关键词过滤
 *
 * 不涉及网络请求，直接对内存中的设备列表做过滤。
 * 搜索逻辑：昵称包含关键词（不区分大小写）。
 *
 * 调用链：DeviceListViewModel → SearchDevicesUseCase（纯内存计算）
 */
class SearchDevicesUseCase @Inject constructor() {
    /**
     * @param devices 全量设备列表
     * @param keyword 搜索关键词（为空时返回全量列表）
     * @return 过滤后的设备列表
     */
    operator fun invoke(devices: List<Device>, keyword: String): List<Device> {
        if (keyword.isBlank()) return devices
        return devices.filter { device ->
            device.nickname.contains(keyword, ignoreCase = true)
        }
    }
}
