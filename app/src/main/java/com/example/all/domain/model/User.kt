package com.example.all.domain.model

/**
 * 用户领域模型 —— 代表登录账号信息
 *
 * Phase 1 中不涉及真实登录，FakeAuthRepository 返回固定调试用户。
 * Phase 2 由云端 /auth/login 返回。
 *
 * @property userId   云端用户唯一 ID
 * @property phone    手机号（展示时脱敏为 138****8000 格式）
 * @property username 用户昵称
 * @property role     角色：Owner（设备拥有者）或 Guest（被授权用户）
 * @property email    邮箱，可选
 */
data class User(
    val userId: String,
    val phone: String,
    val username: String,
    val role: UserRole,
    val email: String? = null
)

/**
 * 用户角色枚举
 * - Owner：设备拥有者，可邀请/撤销其他用户
 * - Guest：被授权用户，仅可操作授权范围内的设备
 */
enum class UserRole {
    Owner, Guest
}
