package com.example.all.domain.model

/**
 * 认证令牌对 —— 封装访问令牌和刷新令牌
 *
 * Phase 1 不使用（FakeAuthRepository 返回假值）。
 * Phase 2 由云端 POST /auth/login 返回，经 Keystore 加密后存入 DataStore。
 *
 * @property accessToken  短期访问令牌（JWT），用于 API 请求的 Authorization 头
 * @property refreshToken 长期刷新令牌，用于在 accessToken 过期后静默获取新令牌
 */
data class AuthTokens(
    val accessToken: String,
    val refreshToken: String
)

/**
 * Token 有效性状态 —— 用于启动路由决策
 * - Valid：Token 存在且未过期，可直接进入首页
 * - Expired：Token 存在但已过期，需尝试刷新
 * - Missing：Token 不存在（首次安装或已登出），跳转登录页
 */
enum class TokenStatus {
    Valid, Expired, Missing
}
