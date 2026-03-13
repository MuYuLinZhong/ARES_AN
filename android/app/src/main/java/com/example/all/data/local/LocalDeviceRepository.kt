package com.example.all.data.local

import com.example.all.data.local.entity.DeviceEntity
import com.example.all.data.repository.DeviceRepository
import com.example.all.domain.model.AuthorizedUser
import com.example.all.domain.model.Device
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.map
import javax.inject.Inject

/**
 * 本地设备仓库 —— Phase 1 专用的 DeviceRepository 实现
 *
 * 职责：直接读写 Room 数据库，不发起任何网络请求。
 * 设备列表完全由本地 NFC 添加操作维护。
 *
 * Phase 2 替换为 RemoteDeviceRepository（Cache-Then-Network 策略）。
 */
class LocalDeviceRepository @Inject constructor(
    private val deviceDao: DeviceDao
) : DeviceRepository {

    /**
     * 观察设备列表 —— 将 Room Entity 流转换为 Domain Model 流
     * DeviceDao.observeAll() 返回按昵称排序的 Flow，
     * 任何表数据变化都会自动推送新列表到 UI。
     */
    override fun observeDevices(): Flow<List<Device>> =
        deviceDao.observeAll().map { entities ->
            entities.map { it.toDomain() }
        }

    /**
     * 云端同步 —— Phase 1 空实现
     * Phase 2: GET /devices → 解析 → deviceDao.upsertAll()
     */
    override suspend fun fetchAndCacheDevices() {
        // TODO Phase 2: 从云端拉取设备列表，写入 Room 缓存
    }

    /**
     * 添加设备 —— NFC 读取 deviceId 后直接写入 Room
     *
     * @param deviceId NFC 从硬件读取的唯一标识
     * @param nickname 用户输入的设备昵称
     * @return 新创建的设备领域模型
     */
    override suspend fun addDevice(deviceId: String, nickname: String): Device {
        val entity = DeviceEntity(
            deviceId = deviceId,
            nickname = nickname,
            serialNo = "SN-$deviceId",
            isValid = 1,
            lastSyncAt = System.currentTimeMillis()
        )
        // 写入 Room（如已存在则覆盖更新）
        deviceDao.upsert(entity)
        return entity.toDomain()
        // TODO Phase 2: 同时调用 POST /devices/bind 向云端注册
    }

    /** 解绑设备 —— 从 Room 删除记录 */
    override suspend fun removeDevice(deviceId: String) {
        deviceDao.deleteById(deviceId)
        // TODO Phase 2: 同时调用 DELETE /devices/{deviceId}
    }

    /** 清空本地缓存 —— 登出时调用 */
    override suspend fun clearLocalCache() = deviceDao.deleteAll()

    /** 标记设备无效 —— Phase 2 权限撤销时使用 */
    override suspend fun markInvalid(deviceId: String) = deviceDao.markInvalid(deviceId)

    /**
     * 获取设备详情 —— Phase 1 返回基本信息 + 空授权列表
     * Phase 2: 同时调用 GET /devices/{id}/users 获取授权用户列表
     */
    override suspend fun getDeviceDetail(deviceId: String): Pair<Device, List<AuthorizedUser>> {
        val device = deviceDao.getById(deviceId)?.toDomain()
            ?: throw NoSuchElementException("设备 $deviceId 不存在")
        // Phase 1：无授权用户管理功能，返回空列表
        return Pair(device, emptyList())
    }
}
