package com.example.all.data.fake

import com.example.all.data.repository.AuthRepository
import com.example.all.domain.model.AuthTokens
import com.example.all.domain.model.User
import com.example.all.domain.model.UserRole
import javax.inject.Inject

/**
 * 假认证仓库 —— Phase 1 专用，不发起任何网络请求
 *
 * Phase 1 整个认证模块不参与（无登录页、无 Token），
 * 此类仅提供编译所需的接口实现和调试用的固定返回值。
 *
 * Phase 2 由 Hilt DI 替换为 RemoteAuthRepository（调用云端 auth 相关 API）。
 */
class FakeAuthRepository @Inject constructor() : AuthRepository {

    /**
     * 假登录 —— 返回固定的调试用户和假 Token
     * Phase 1 中此方法不会被实际调用（因为没有登录页面）
     */
    override suspend fun login(phone: String, password: String): Pair<User, AuthTokens> {
        return Pair(
            User(
                userId = "debug-user-001",
                phone = "13800000000",
                username = "Debug User",
                role = UserRole.Owner,
                email = null
            ),
            AuthTokens(
                accessToken = "fake-access-token-phase1",
                refreshToken = "fake-refresh-token-phase1"
            )
        )
        // TODO Phase 2: 替换为 RemoteAuthRepository，调用 POST /auth/login
    }

    /** 假登出 —— 静默忽略（Phase 1 无 Token 需要吊销） */
    override suspend fun logout() {
        // TODO Phase 2: POST /auth/logout 通知云端吊销 Token
    }

    /** 刷新令牌 —— Phase 1 不使用 */
    override suspend fun refreshToken(refreshToken: String): AuthTokens {
        throw UnsupportedOperationException("Phase 2: refreshToken 尚未实现")
    }

    /** 修改密码 —— Phase 1 不使用 */
    override suspend fun updatePassword(currentPassword: String, newPassword: String) {
        throw UnsupportedOperationException("Phase 2: updatePassword 尚未实现")
    }

    /** 注销账号 —— Phase 1 不使用 */
    override suspend fun deleteAccount() {
        throw UnsupportedOperationException("Phase 2: deleteAccount 尚未实现")
    }
}
