package com.example.all.data.nfc

import android.nfc.TagLostException
import android.util.Log
import com.example.all.data.repository.CryptoRepository
import com.example.all.data.repository.NfcRepository
import com.example.all.domain.model.LockOperationResult
import com.example.all.domain.model.OperationType
import com.example.all.domain.model.SmackLockInfo
import com.infineon.smack.sdk.SmackSdk
import com.infineon.smack.sdk.mailbox.SmackMailbox
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.transformLatest
import java.nio.ByteBuffer
import javax.inject.Inject

/**
 * 挑战应答 NFC 仓库 —— 通过 mailboxApi 自定义数据点实现防重放认证
 *
 * 六步协议（authenticate 内部包含步骤 2-4）：
 * 1. 发现锁 → mailboxApi.mailbox → 读 LOCK_ID
 * 2. 读 CHALLENGE → 固件 RNG 生成 16 字节随机数
 * 3. 本地 AES(key, challenge) → 写 RESPONSE
 * 4. 读 AUTH_RESULT → 确认认证成功
 * 5. 轮询 CHARGE_PERCENT → 充能就绪
 * 6. 写 OPERATION → 固件检查 auth_ok → 驱动电机
 */
class SmackLockRepository @Inject constructor(
    private val smackSdk: SmackSdk,
    private val cryptoRepository: CryptoRepository
) : NfcRepository {

    companion object {
        private const val TAG = "SmackLockRepo"
    }

    private var currentMailbox: SmackMailbox? = null

    @OptIn(kotlinx.coroutines.ExperimentalCoroutinesApi::class)
    override fun observeLock(): Flow<SmackLockInfo?> {
        return smackSdk.mailboxApi.mailbox.transformLatest { mailbox ->
            if (mailbox != null) {
                Log.d(TAG, "NFC Tag 已连接，读取 LOCK_ID...")
                try {
                    val lockIdBytes = smackSdk.mailboxApi.readDataPoint(
                        mailbox, AresDataPoints.LOCK_ID
                    )
                    if (lockIdBytes.isEmpty()) {
                        Log.w(TAG, "LOCK_ID 返回空数据")
                        currentMailbox = null
                        emit(null)
                        return@transformLatest
                    }
                    val lockId = ByteBuffer.wrap(lockIdBytes).getLong()
                    Log.d(TAG, "读取到 LOCK_ID=0x${lockId.toString(16).uppercase()}")
                    currentMailbox = mailbox
                    emit(SmackLockInfo(lockId = lockId, isNew = false))
                    kotlinx.coroutines.awaitCancellation()
                } catch (e: TagLostException) {
                    Log.w(TAG, "读取 LOCK_ID 时 Tag 断开", e)
                    currentMailbox = null
                    emit(null)
                } catch (e: Exception) {
                    Log.e(TAG, "读取 LOCK_ID 失败: ${e.message}", e)
                    currentMailbox = null
                    emit(null)
                }
            } else {
                Log.d(TAG, "NFC Tag 未连接")
                currentMailbox = null
                emit(null)
            }
        }
    }

    override suspend fun readUid(): String {
        val mailbox = requireMailbox()
        val uidBytes = smackSdk.mailboxApi.readDataPoint(mailbox, AresDataPoints.UID)
        val hex = uidBytes.joinToString("") { String.format("%02X", it) }
        Log.d(TAG, "读取 UID=$hex")
        return hex
    }

    /**
     * 挑战应答认证：
     * 1. 读 CHALLENGE → 固件 notify_tx 生成新随机数
     * 2. CryptoRepository.encryptLocal(challenge) → AES(key, challenge)
     * 3. 写 RESPONSE → 固件 notify_rx 验证
     * 4. 读 AUTH_RESULT → 0x01 表示成功
     */
    override suspend fun authenticate(lockId: Long, password: String, userName: String): Boolean {
        val mailbox = requireMailbox()

        // Step 2: 读取 CHALLENGE
        Log.d(TAG, "读取 CHALLENGE (${AresDataPoints.CHALLENGE_LENGTH} bytes)...")
        val challenge = smackSdk.mailboxApi.readDataPoint(
            mailbox, AresDataPoints.CHALLENGE, bufferSize = AresDataPoints.CHALLENGE_LENGTH
        )
        if (challenge.isEmpty()) {
            Log.e(TAG, "CHALLENGE 返回空数据 — 固件可能未烧写挑战应答协议")
            return false
        }
        Log.d(TAG, "收到 CHALLENGE: ${challenge.toHexString()}")

        // Step 3: 本地 AES 加密
        Log.d(TAG, "AES 加密 challenge...")
        val responseResult = cryptoRepository.encryptLocal(challenge)
        val response = responseResult.getOrElse { e ->
            Log.e(TAG, "AES 加密失败", e)
            return false
        }
        Log.d(TAG, "加密完成，写入 RESPONSE...")

        // 写 RESPONSE
        smackSdk.mailboxApi.writeDataPoint(mailbox, AresDataPoints.RESPONSE, response)
        Log.d(TAG, "RESPONSE 已写入，读取 AUTH_RESULT...")

        // Step 4: 读 AUTH_RESULT
        val authResultBytes = smackSdk.mailboxApi.readDataPoint(
            mailbox, AresDataPoints.AUTH_RESULT
        )
        if (authResultBytes.isEmpty()) {
            Log.e(TAG, "AUTH_RESULT 返回空数据")
            return false
        }

        val authOk = authResultBytes[0] == AresDataPoints.AUTH_OK
        Log.d(TAG, "AUTH_RESULT=0x${String.format("%02X", authResultBytes[0])}, 认证${if (authOk) "成功" else "失败"}")
        return authOk
    }

    override suspend fun getChargePercent(): Int {
        val mailbox = requireMailbox()
        val bytes = smackSdk.mailboxApi.readDataPoint(mailbox, AresDataPoints.CHARGE_PERCENT)
        val percent = if (bytes.isNotEmpty()) bytes[0].toInt() and 0xFF else 0
        Log.d(TAG, "CHARGE_PERCENT=$percent%")
        return percent
    }

    override suspend fun executeLockCommand(operationType: OperationType): LockOperationResult {
        val mailbox = currentMailbox ?: return LockOperationResult.Error("锁未连接")
        return try {
            val opByte = when (operationType) {
                OperationType.Unlock -> AresDataPoints.OP_UNLOCK
                OperationType.Lock -> AresDataPoints.OP_LOCK
            }
            Log.d(TAG, "写入 OPERATION=0x${String.format("%02X", opByte)}")
            smackSdk.mailboxApi.writeDataPoint(
                mailbox, AresDataPoints.OPERATION, byteArrayOf(opByte)
            )
            Log.d(TAG, "OPERATION 写入成功")
            LockOperationResult.Success
        } catch (e: TagLostException) {
            Log.w(TAG, "执行操作时 Tag 断开", e)
            LockOperationResult.TagLost()
        } catch (e: Exception) {
            Log.e(TAG, "执行操作失败", e)
            LockOperationResult.Error(e.message ?: "操作失败")
        }
    }

    private fun requireMailbox(): SmackMailbox {
        return currentMailbox
            ?: throw IllegalStateException("锁未连接 — 请确保手机已靠近 NFC Tag")
    }
}

private fun ByteArray.toHexString(): String =
    joinToString("") { String.format("%02X", it) }
