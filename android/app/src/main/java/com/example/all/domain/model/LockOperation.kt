package com.example.all.domain.model

/**
 * 锁操作类型枚举
 * - Unlock：开锁（OPERATION=0x01）
 * - Lock：关锁（OPERATION=0x02）
 */
enum class OperationType {
    Unlock,
    Lock
}

/**
 * 锁操作结果 —— 由 SmackLockRepository（挑战应答）返回
 */
sealed class LockOperationResult {
    object Success : LockOperationResult()
    data class WrongKey(val message: String = "密码错误") : LockOperationResult()
    data class TagLost(val message: String = "NFC 连接断开") : LockOperationResult()
    data class Error(val message: String) : LockOperationResult()
}

/**
 * 开/关锁操作的进度状态 —— 驱动前端底部弹窗的进度条和文案
 *
 * 挑战应答六步流程对应的进度百分比：
 * Idle(0%) → 发现锁(20%) → 挑战应答(40%) → 充能(70%) → 执行(100%)
 */
sealed class OperationState {
    object Idle : OperationState()

    data class InProgress(
        val progress: Int,
        val stepText: String
    ) : OperationState()

    data class Success(val message: String) : OperationState()

    data class Error(
        val message: String,
        val isRetryable: Boolean
    ) : OperationState()
}
