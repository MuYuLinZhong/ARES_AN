package com.example.all.data.repository

import com.example.all.domain.model.AuthTokens
import com.example.all.domain.model.UserPreferences
import kotlinx.coroutines.flow.Flow

/**
 * 偏好仓库接口 —— 统一管理 DataStore 中的用户偏好和 Token 存储
 *
 * Phase 1 实现：DataStorePreferencesRepository（仅偏好 + 本地密钥）
 * Phase 2 新增：Token 加密存储相关方法
 */
interface PreferencesRepository {

    /**
     * 观察用户偏好 —— 返回 Flow，DataStore 变化时自动推送最新偏好
     */
    fun observePreferences(): Flow<UserPreferences>

    /**
     * 保存用户偏好
     * @param preferences 新的偏好值
     */
    suspend fun savePreferences(preferences: UserPreferences)

    /**
     * Phase 1 专用：读取本地调试密钥（十六进制字符串）
     * Phase 2 此方法将被移除
     */
    suspend fun getLocalDeviceKey(): String

    /**
     * Phase 1 专用：写入本地调试密钥
     * 用于运行时更换不同设备的密钥
     */
    suspend fun setLocalDeviceKey(keyHex: String)

    // ====== Phase 2 Token 存储方法 ======

    /** 加密存储令牌对（Phase 2 启用） */
    suspend fun saveTokens(tokens: AuthTokens)

    /** 读取 AccessToken 明文（Phase 2 启用） */
    suspend fun getAccessToken(): String?

    /** 读取 RefreshToken 明文（Phase 2 启用） */
    suspend fun getRefreshToken(): String?

    /** 清除所有 Token（登出时调用） */
    suspend fun clearTokens()

    /** 清除全部 DataStore 数据（注销时调用） */
    suspend fun clearAll()
}
