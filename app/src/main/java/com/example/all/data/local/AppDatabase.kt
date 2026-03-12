package com.example.all.data.local

import androidx.room.Database
import androidx.room.RoomDatabase
import com.example.all.data.local.entity.DeviceEntity

/**
 * Room 数据库定义 —— 本地持久化存储的统一入口
 *
 * Phase 1：仅包含 device_cache 一张表（设备列表缓存）
 * Phase 2 新增：authorized_user_cache 表（授权用户缓存）
 * Phase 3 新增：pending_reports 表（待上报操作队列）
 *
 * 版本号从 1 开始，每次表结构变更需递增并编写 Migration。
 */
@Database(
    entities = [DeviceEntity::class],
    version = 1,
    exportSchema = false
)
abstract class AppDatabase : RoomDatabase() {

    /** 获取设备表的 DAO 实例 */
    abstract fun deviceDao(): DeviceDao
}
