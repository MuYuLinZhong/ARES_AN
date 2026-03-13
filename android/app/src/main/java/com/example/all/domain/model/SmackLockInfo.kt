package com.example.all.domain.model

/**
 * 锁信息 —— 从 SmAcK SDK getLock() 获取
 *
 * @property lockId 锁唯一标识（LOCK_ID 数据点 0x0005）
 * @property isNew 是否全新锁（USER_COUNT == 0 表示从未设置过密钥）
 */
data class SmackLockInfo(
    val lockId: Long,
    val isNew: Boolean
)
