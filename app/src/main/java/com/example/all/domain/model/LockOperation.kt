package com.example.all.domain.model

/**
 * 锁操作类型枚举
 * - Unlock：开锁，对应 NFC 操作位 0x01
 * - Lock：关锁，对应 NFC 操作位 0x02
 */
enum class OperationType(val commandByte: Byte) {
    Unlock(0x01),
    Lock(0x02)
}

/**
 * 硬件执行结果枚举
 * - Success：0x00，操作成功（开锁/关锁机械动作完成）
 * - CipherMismatch：0x01，密文比对失败（密钥错误）
 * - MechanicalFailure：0x02，机械检测失败（如关锁时门未关严）
 */
enum class LockResult(val resultByte: Byte) {
    Success(0x00),
    CipherMismatch(0x01),
    MechanicalFailure(0x02);

    companion object {
        /** 从硬件返回的字节值解析为枚举 */
        fun fromByte(b: Byte): LockResult = entries.first { it.resultByte == b }
    }
}

/**
 * 开/关锁操作的进度状态 —— 驱动前端底部弹窗的进度条和文案
 *
 * 五步协议对应的进度百分比：
 * Idle(0%) → Step1(20%) → Step2(40%) → Step3(65%) → Step4(85%) → Step5(100%)
 */
sealed class OperationState {
    /** 空闲，无操作进行中 */
    object Idle : OperationState()

    /** 操作进行中 */
    data class InProgress(
        val progress: Int,
        val stepText: String
    ) : OperationState()

    /** 操作成功 */
    data class Success(val message: String) : OperationState()

    /** 操作失败 */
    data class Error(
        val message: String,
        val isRetryable: Boolean
    ) : OperationState()
}
