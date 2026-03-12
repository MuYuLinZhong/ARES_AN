package com.example.all.data.repository

/**
 * 权限仓库接口 —— 封装设备授权的邀请/撤销操作
 *
 * Phase 1 实现：FakePermissionRepository（全部 stub，不参与）
 * Phase 2 实现：RemotePermissionRepository（调用云端 API）
 */
interface PermissionRepository {

    /**
     * 邀请用户 —— 授权某用户操作指定设备
     * Phase 2：POST /devices/{deviceId}/invite
     */
    suspend fun inviteUser(deviceId: String, phone: String)

    /**
     * 撤销用户权限 —— 取消某用户对指定设备的操作权限
     * Phase 2：DELETE /devices/{deviceId}/users/{userId}
     */
    suspend fun revokeUser(deviceId: String, userId: String)

    /**
     * 拉取权限快照 —— 获取当前用户的所有设备权限状态
     * 用于前台化时刷新设备有效性（Phase 2）
     */
    suspend fun fetchPermissionSnapshot(): List<PermissionSnapshot>
}

/**
 * 权限快照 —— 某台设备对当前用户的授权状态
 * @property deviceId 设备 ID
 * @property isValid  授权是否仍然有效
 */
data class PermissionSnapshot(
    val deviceId: String,
    val isValid: Boolean
)
