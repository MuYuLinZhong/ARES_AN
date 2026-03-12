package com.example.all.data.local.entity

import androidx.room.Entity
import androidx.room.PrimaryKey
import com.example.all.domain.model.Device

/**
 * 设备缓存表实体 —— 对应 Room 数据库中的 device_cache 表
 *
 * Phase 1：存储本地手动添加的设备（通过 NFC 读取 deviceId）
 * Phase 2：同时作为云端设备列表的本地缓存（Cache-Then-Network）
 *
 * @property deviceId   硬件唯一标识，作为主键
 * @property nickname   用户自定义昵称
 * @property serialNo   硬件序列号
 * @property isValid    授权有效性：1=有效，0=已撤销（Phase 2 使用）
 * @property lastSyncAt 最后同步时间戳（毫秒），Phase 1 为本地写入时间
 */
@Entity(tableName = "device_cache")
data class DeviceEntity(
    @PrimaryKey val deviceId: String,
    val nickname: String,
    val serialNo: String,
    val isValid: Int = 1,
    val lastSyncAt: Long = System.currentTimeMillis()
) {
    /** 将数据层实体转换为领域模型 */
    fun toDomain(): Device = Device(
        deviceId = deviceId,
        nickname = nickname,
        serialNo = serialNo,
        isValid = isValid == 1,
        lastSyncAt = lastSyncAt
    )
}
