package com.example.all.data.repository

import com.example.all.domain.model.LockOperationResult
import com.example.all.domain.model.OperationType
import com.example.all.domain.model.SmackLockInfo
import kotlinx.coroutines.flow.Flow

/**
 * NFC 仓库接口 —— 封装与 NAC1080 硬件的全部 NFC 通信操作
 *
 * 实现：SmackLockRepository — 基于 SmAcK mailboxApi + 挑战应答协议
 * 六步流程（authenticate 内部包含 challenge-response 步骤 2-4）：
 *   1. observeLock → 发现锁 → LOCK_ID
 *   2-4. authenticate → 挑战应答（CHALLENGE → AES → RESPONSE → AUTH_RESULT）
 *   5. getChargePercent → 充能轮询
 *   6. executeLockCommand → 写 OPERATION → 驱动电机
 */
interface NfcRepository {

    /**
     * 观察锁连接状态
     * 当 NFC Tag 进入/离开感应范围时自动触发
     * @return 锁信息 Flow，null 表示未连接
     */
    fun observeLock(): Flow<SmackLockInfo?>

    /**
     * 读取硬件 UID（用于添加设备时获取序列号）
     * 必须在 observeLock() 已返回非 null 后调用
     * @return UID 的十六进制字符串
     */
    suspend fun readUid(): String

    /**
     * 挑战应答认证：读取硬件随机数 → 本地 AES 加密 → 写回应答 → 验证结果
     * @param lockId 锁唯一标识
     * @param password 用户密码（用于密钥派生）
     * @param userName 用户名（预留）
     * @return 认证是否成功
     */
    suspend fun authenticate(lockId: Long, password: String, userName: String): Boolean

    /**
     * 获取充能百分比
     * 无源锁必须等电容充满才能驱动电机
     * @return 0-100 的充电百分比
     */
    suspend fun getChargePercent(): Int

    /**
     * 执行开锁或关锁操作（需先通过挑战应答认证）
     * @param operationType 开锁/关锁
     * @return 操作结果
     */
    suspend fun executeLockCommand(operationType: OperationType): LockOperationResult
}
