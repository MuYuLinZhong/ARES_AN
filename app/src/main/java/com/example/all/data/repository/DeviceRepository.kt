package com.example.all.data.repository

import com.example.all.domain.model.AuthorizedUser
import com.example.all.domain.model.Device
import kotlinx.coroutines.flow.Flow

/**
 * 设备仓库接口 —— 设备数据的统一入口
 *
 * Phase 1 实现：LocalDeviceRepository（Room 直接读写，无云端）
 * Phase 2 实现：RemoteDeviceRepository（Cache-Then-Network 策略）
 */
interface DeviceRepository {

    /**
     * 观察设备列表 —— 返回 Flow，Room 数据变化时自动推送新列表
     * 用于 DeviceListViewModel 实时刷新 UI
     */
    fun observeDevices(): Flow<List<Device>>

    /**
     * 从云端拉取设备列表并缓存到 Room
     * Phase 1：空实现（无云端）；Phase 2：GET /devices → upsertAll
     */
    suspend fun fetchAndCacheDevices()

    /**
     * 添加新设备 —— NFC 读取 deviceId 后写入本地
     * @param deviceId 硬件唯一标识
     * @param nickname 用户输入的设备昵称
     * @return 创建的设备领域模型
     */
    suspend fun addDevice(deviceId: String, nickname: String): Device

    /**
     * 解绑设备 —— 从本地删除设备记录
     * Phase 2 同时调用 DELETE /devices/{id}
     */
    suspend fun removeDevice(deviceId: String)

    /** 清空本地设备缓存 —— 登出时调用 */
    suspend fun clearLocalCache()

    /** 标记设备为无效 —— 权限被撤销时调用（Phase 2） */
    suspend fun markInvalid(deviceId: String)

    /**
     * 获取设备详情 + 授权用户列表
     * Phase 1：返回基本设备信息 + 空授权列表
     * Phase 2：同时从云端拉取授权用户
     */
    suspend fun getDeviceDetail(deviceId: String): Pair<Device, List<AuthorizedUser>>
}
