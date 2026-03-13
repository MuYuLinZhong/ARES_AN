package com.example.all.domain.model

/**
 * 设备领域模型 —— 代表一把 NAC1080 NFC 智能锁
 *
 * 该数据类在整个 Domain / Presentation 层流转，
 * 与数据层的 DeviceEntity（Room 表行）一一映射但互不耦合。
 *
 * @property deviceId   硬件唯一标识，由 NFC 读取获得（如 "NAC1080-A1B2C3"）
 * @property nickname   用户自定义昵称（如 "办公室门锁"）
 * @property serialNo   硬件序列号
 * @property isValid    设备授权是否有效；Phase 1 始终为 true，Phase 2 由云端权限控制
 * @property lastSyncAt 最后一次与云端同步的时间戳（毫秒）；Phase 1 为本地写入时间
 */
data class Device(
    val deviceId: String,
    val nickname: String,
    val serialNo: String,
    val isValid: Boolean = true,
    val lastSyncAt: Long = System.currentTimeMillis()
)
