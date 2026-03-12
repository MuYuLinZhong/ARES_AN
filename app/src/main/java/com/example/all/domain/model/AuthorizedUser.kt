package com.example.all.domain.model

/**
 * 授权用户 —— 某台设备上被授权的用户信息
 *
 * Phase 1 不使用（设备详情中无授权用户列表）。
 * Phase 2 由云端 GET /devices/{id}/users 返回。
 *
 * @property userId 用户唯一 ID
 * @property name   用户昵称
 * @property phone  手机号（脱敏后仅显示后四位）
 * @property role   角色：Owner 或 Guest
 */
data class AuthorizedUser(
    val userId: String,
    val name: String,
    val phone: String,
    val role: UserRole
)
