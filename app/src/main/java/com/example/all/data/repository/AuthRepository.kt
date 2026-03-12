package com.example.all.data.repository

import com.example.all.domain.model.AuthTokens
import com.example.all.domain.model.User

/**
 * 认证仓库接口 —— 封装登录/登出/Token 刷新等认证操作
 *
 * Phase 1 实现：FakeAuthRepository（返回固定值，不发网络请求）
 * Phase 2 实现：RemoteAuthRepository（调用云端 /auth/* API）
 */
interface AuthRepository {

    /**
     * 登录 —— 用手机号和密码换取用户信息和令牌对
     * Phase 1：返回固定调试用户（不会被实际调用，因为无登录页）
     * Phase 2：POST /auth/login
     */
    suspend fun login(phone: String, password: String): Pair<User, AuthTokens>

    /**
     * 登出 —— 通知云端吊销当前设备的 Token
     * Phase 1：静默忽略
     * Phase 2：POST /auth/logout
     */
    suspend fun logout()

    /**
     * 刷新令牌 —— 用 RefreshToken 获取新的令牌对
     * Phase 1：不使用
     * Phase 2：POST /auth/refresh
     */
    suspend fun refreshToken(refreshToken: String): AuthTokens

    /**
     * 修改密码
     * Phase 1：不使用
     * Phase 2：PUT /auth/password
     */
    suspend fun updatePassword(currentPassword: String, newPassword: String)

    /**
     * 注销账号 —— 永久删除账号和关联数据
     * Phase 1：不使用
     * Phase 2：DELETE /auth/account
     */
    suspend fun deleteAccount()
}
