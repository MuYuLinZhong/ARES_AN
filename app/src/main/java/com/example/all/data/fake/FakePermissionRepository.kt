package com.example.all.data.fake

import com.example.all.data.repository.PermissionRepository
import com.example.all.data.repository.PermissionSnapshot
import javax.inject.Inject

/**
 * 假权限仓库 —— Phase 1 专用，权限管理功能完全不参与
 *
 * 所有方法要么抛 UnsupportedOperationException（不应被调用的），
 * 要么返回安全的空值/默认值（可能被间接触发的）。
 *
 * Phase 2 由 Hilt DI 替换为 RemotePermissionRepository。
 */
class FakePermissionRepository @Inject constructor() : PermissionRepository {

    /** 邀请用户 —— Phase 1 不使用 */
    override suspend fun inviteUser(deviceId: String, phone: String) {
        throw UnsupportedOperationException("Phase 2: inviteUser 尚未实现")
    }

    /** 撤销权限 —— Phase 1 不使用 */
    override suspend fun revokeUser(deviceId: String, userId: String) {
        throw UnsupportedOperationException("Phase 2: revokeUser 尚未实现")
    }

    /**
     * 获取权限快照 —— 返回空列表（Phase 1 无权限感知需求）
     * 此方法可能在 App 前台化时被间接调用，因此返回空列表而非抛异常
     */
    override suspend fun fetchPermissionSnapshot(): List<PermissionSnapshot> {
        // TODO Phase 2: 调用 GET /devices/my 获取最新权限状态
        return emptyList()
    }
}
