package com.example.all.di

import android.content.Context
import androidx.room.Room
import com.example.all.data.local.AppDatabase
import com.example.all.data.local.DeviceDao
import dagger.Module
import dagger.Provides
import dagger.hilt.InstallIn
import dagger.hilt.android.qualifiers.ApplicationContext
import dagger.hilt.components.SingletonComponent
import javax.inject.Singleton

/**
 * 数据库模块 —— 提供 Room 数据库实例和 DAO
 *
 * Phase 1：仅 device_cache 一张表
 * Phase 2 新增：authorized_user_cache 表（需要 Migration）
 * Phase 3 新增：pending_reports 表（需要 Migration）
 *
 * 使用 @Singleton 保证全应用生命周期内只有一个数据库连接。
 */
@Module
@InstallIn(SingletonComponent::class)
object DatabaseModule {

    /**
     * 创建 Room 数据库实例
     *
     * 数据库名 "ares_database" 在设备上对应 /data/data/com.example.all/databases/ares_database
     * fallbackToDestructiveMigration：开发期间表结构变更时自动重建（生产环境应使用 Migration）
     */
    @Provides
    @Singleton
    fun provideDatabase(@ApplicationContext context: Context): AppDatabase =
        Room.databaseBuilder(
            context,
            AppDatabase::class.java,
            "ares_database"
        )
            .fallbackToDestructiveMigration()
            .build()

    /**
     * 从数据库实例中获取 DeviceDao
     * Hilt 会自动将 AppDatabase 注入进来（因为上方已 @Provides）
     */
    @Provides
    @Singleton
    fun provideDeviceDao(database: AppDatabase): DeviceDao =
        database.deviceDao()
}
