package com.example.all.di

import android.content.Context
import androidx.datastore.core.DataStore
import androidx.datastore.core.DataStoreFactory
import androidx.datastore.dataStoreFile
import com.example.all.UserPrefs
import com.example.all.data.local.UserPrefsSerializer
import dagger.Module
import dagger.Provides
import dagger.hilt.InstallIn
import dagger.hilt.android.qualifiers.ApplicationContext
import dagger.hilt.components.SingletonComponent
import javax.inject.Singleton

/**
 * 偏好存储模块 —— 提供 Proto DataStore 单例
 *
 * 使用 Proto DataStore（而非 Preferences DataStore）的原因：
 * 1. 类型安全 —— Proto 消息在编译期校验字段类型
 * 2. 结构化 —— 适合 Phase 2 新增 Token 加密字段
 * 3. 向后兼容 —— Proto 天然支持字段新增（旧数据自动使用默认值）
 */
@Module
@InstallIn(SingletonComponent::class)
object PreferencesModule {

    /**
     * 创建 Proto DataStore 实例
     *
     * 文件路径：/data/data/com.example.all/files/datastore/user_prefs.pb
     * 序列化器：UserPrefsSerializer（定义默认值和序列化/反序列化逻辑）
     */
    @Provides
    @Singleton
    fun provideUserPrefsDataStore(
        @ApplicationContext context: Context
    ): DataStore<UserPrefs> = DataStoreFactory.create(
        serializer = UserPrefsSerializer,
        produceFile = { context.dataStoreFile("user_prefs.pb") }
    )
}
