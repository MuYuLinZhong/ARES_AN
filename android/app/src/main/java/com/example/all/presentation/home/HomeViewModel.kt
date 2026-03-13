package com.example.all.presentation.home

import android.nfc.TagLostException
import android.util.Log
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.example.all.data.repository.NfcRepository
import com.example.all.domain.model.LockOperationResult
import com.example.all.domain.model.OperationState
import com.example.all.domain.model.OperationType
import com.example.all.domain.model.SmackLockInfo
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.CancellationException
import kotlinx.coroutines.CompletableDeferred
import kotlinx.coroutines.Job
import kotlinx.coroutines.TimeoutCancellationException
import kotlinx.coroutines.awaitCancellation
import kotlinx.coroutines.coroutineScope
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch
import kotlinx.coroutines.withTimeout
import java.io.IOException
import javax.inject.Inject

/**
 * 首页 ViewModel —— 核心业务：挑战应答协议开/关锁
 *
 * 六步流程（authenticate 内部包含挑战应答步骤 2-4）：
 * 1. 发现锁（observeLock → mailboxApi → LOCK_ID）→ 20%
 * 2. 读取挑战（CHALLENGE → 固件 RNG 16 字节随机数）
 * 3. 本地应答（CryptoRepository.encryptLocal → AES(key, challenge)）
 * 4. 验证结果（RESPONSE → AUTH_RESULT）→ 40%
 * 5. 等待充能（CHARGE_PERCENT 轮询）→ 70%
 * 6. 执行开/关锁（OPERATION → 固件 auth_ok 检查 → H-Bridge）→ 100%
 *
 * 数据流：WebView → AndroidBridge → HomeViewModel → NfcRepository(SmackLockRepository) → SmackSdk.mailboxApi → NAC1080
 */
@HiltViewModel
class HomeViewModel @Inject constructor(
    private val nfcRepository: NfcRepository
) : ViewModel() {

    private val _uiState = MutableStateFlow(HomeUiState())
    val uiState: StateFlow<HomeUiState> = _uiState.asStateFlow()

    private var lockJob: Job? = null

    companion object {
        private const val TAG = "HomeViewModel"
        private const val LOCK_DISCOVER_TIMEOUT_MS = 15_000L
        private const val CHARGE_POLL_INTERVAL_MS = 500L
        private const val CHARGE_TIMEOUT_MS = 30_000L
        private const val MIN_CHARGE_PERCENT = 80
        private const val DEFAULT_PASSWORD = "00000000"
        private const val DEFAULT_USERNAME = "ARES_User"
    }

    /**
     * 请求开/关锁 —— 由 WebView Bridge 调用
     * 启动协程，等待 SmAcK SDK 自动发现 NFC Tag，然后执行挑战应答认证
     */
    fun requestOperation(deviceId: String, operationType: OperationType) {
        lockJob?.cancel()

        _uiState.update {
            it.copy(
                pendingOperation = PendingOperation(deviceId, operationType),
                operationState = OperationState.InProgress(0, "请将手机靠近门锁...")
            )
        }

        lockJob = viewModelScope.launch {
            try {
                executeLockOperation(deviceId, operationType)
            } catch (e: TimeoutCancellationException) {
                Log.w(TAG, "操作超时", e)
                _uiState.update {
                    it.copy(operationState = OperationState.Error("操作超时，请重新靠近门锁", true))
                }
            } catch (e: CancellationException) {
                _uiState.update { it.copy(operationState = OperationState.Idle) }
            } catch (e: TagLostException) {
                Log.w(TAG, "NFC Tag 断开", e)
                _uiState.update {
                    it.copy(operationState = OperationState.Error("NFC 连接断开，请重新靠近", true))
                }
            } catch (e: IOException) {
                Log.w(TAG, "NFC IO 异常", e)
                _uiState.update {
                    it.copy(operationState = OperationState.Error("NFC 信号中断，请重新靠近", true))
                }
            } catch (e: Exception) {
                Log.e(TAG, "操作失败", e)
                _uiState.update {
                    it.copy(operationState = OperationState.Error(
                        e.message ?: "操作失败", true
                    ))
                }
            } finally {
                _uiState.update { it.copy(pendingOperation = null) }
            }
        }
    }

    fun onOperationCancelled() {
        lockJob?.cancel()
        lockJob = null
        _uiState.update { it.copy(operationState = OperationState.Idle, pendingOperation = null) }
    }

    /**
     * 挑战应答协议执行
     *
     * 关键设计：通过 coroutineScope + 后台 Job 保持 observeLock() 的 Flow
     * 在整个操作期间持续收集。这样 SmAcK SDK 内部的 NFC 连接不会因为
     * Flow 取消而被关闭，currentMailbox 在后续步骤中始终有效。
     *
     * 步骤 1（发现锁）    → 20%
     * 步骤 2-4（挑战应答） → 40%
     * 步骤 5（充能）      → 70%
     * 步骤 6（执行）      → 100%
     */
    private suspend fun executeLockOperation(
        deviceId: String,
        operationType: OperationType
    ) = coroutineScope {

        val lockInfoDeferred = CompletableDeferred<SmackLockInfo>()

        val nfcObserveJob = launch {
            nfcRepository.observeLock().collect { info ->
                if (info != null && !lockInfoDeferred.isCompleted) {
                    lockInfoDeferred.complete(info)
                }
            }
        }

        try {
            // Step 1: 发现锁 — 等待 SmAcK SDK 通过 NFC 检测到 NAC1080 Tag
            updateProgress(10, "等待检测门锁...")
            Log.d(TAG, "Step 1: 等待 NFC Tag（超时 ${LOCK_DISCOVER_TIMEOUT_MS}ms）")
            val lockInfo: SmackLockInfo = withTimeout(LOCK_DISCOVER_TIMEOUT_MS) {
                lockInfoDeferred.await()
            }
            Log.d(TAG, "Step 1: 检测到锁 lockId=${lockInfo.lockId}")

            // 校验发现的锁是否与用户选择的设备匹配
            val discoveredDeviceId = lockInfo.lockId.toString(16).uppercase()
            if (deviceId.isNotBlank() && !deviceId.equals(discoveredDeviceId, ignoreCase = true)) {
                Log.w(TAG, "设备 ID 不匹配: 请求=$deviceId, 实际=$discoveredDeviceId")
                _uiState.update {
                    it.copy(operationState = OperationState.Error(
                        "检测到的设备与选择的不一致，请确认靠近正确的锁", true
                    ))
                }
                return@coroutineScope
            }
            updateProgress(20, "已检测到门锁")

            // Step 2-4: 挑战应答认证（CHALLENGE → AES → RESPONSE → AUTH_RESULT）
            updateProgress(30, "正在挑战应答认证...")
            Log.d(TAG, "Step 2-4: 开始挑战应答认证")
            val authenticated = nfcRepository.authenticate(
                lockId = lockInfo.lockId,
                password = DEFAULT_PASSWORD,
                userName = DEFAULT_USERNAME
            )
            if (!authenticated) {
                Log.w(TAG, "Step 2-4: 挑战应答认证失败")
                _uiState.update {
                    it.copy(operationState = OperationState.Error("挑战应答认证失败", true))
                }
                return@coroutineScope
            }
            Log.d(TAG, "Step 2-4: 认证成功")
            updateProgress(40, "认证成功")

            // Step 5: 等待充能 — 无源锁靠 NFC 场供电，需充能到 80% 以上
            updateProgress(50, "正在充能...")
            Log.d(TAG, "Step 5: 开始充能轮询（阈值 ${MIN_CHARGE_PERCENT}%，超时 ${CHARGE_TIMEOUT_MS}ms）")
            val charged = waitForCharge()
            if (!charged) {
                Log.w(TAG, "Step 5: 充能超时")
                _uiState.update {
                    it.copy(operationState = OperationState.Error("充能超时，请保持手机靠近", true))
                }
                return@coroutineScope
            }
            Log.d(TAG, "Step 5: 充能完成")
            updateProgress(70, "充能完成")

            // Step 6: 执行开/关锁（写 OPERATION → 固件 auth_ok 检查 → 驱动 H-Bridge 电机）
            val actionText = if (operationType == OperationType.Unlock) "开锁" else "关锁"
            updateProgress(85, "正在执行${actionText}...")
            Log.d(TAG, "Step 6: 执行 $actionText（OPERATION=${if (operationType == OperationType.Unlock) "0x01" else "0x02"}）")
            val result = nfcRepository.executeLockCommand(operationType)

            Log.d(TAG, "Step 6: 结果=$result")
            when (result) {
                is LockOperationResult.Success -> {
                    updateProgress(100, "${actionText}成功")
                    _uiState.update {
                        it.copy(operationState = OperationState.Success("${actionText}成功"))
                    }
                }
                is LockOperationResult.WrongKey -> {
                    _uiState.update {
                        it.copy(operationState = OperationState.Error(result.message, true))
                    }
                }
                is LockOperationResult.TagLost -> {
                    _uiState.update {
                        it.copy(operationState = OperationState.Error(result.message, true))
                    }
                }
                is LockOperationResult.Error -> {
                    _uiState.update {
                        it.copy(operationState = OperationState.Error(result.message, true))
                    }
                }
            }
        } finally {
            nfcObserveJob.cancel()
        }
    }

    /**
     * 轮询充能百分比，直到达到阈值或超时
     * 无源锁靠 NFC 场供电，需等电容充满才能驱动电机
     */
    private suspend fun waitForCharge(): Boolean {
        return try {
            withTimeout(CHARGE_TIMEOUT_MS) {
                while (true) {
                    val percent = nfcRepository.getChargePercent()
                    updateProgress(
                        50 + (percent * 20 / 100),
                        "充能中 ${percent}%..."
                    )
                    if (percent >= MIN_CHARGE_PERCENT) return@withTimeout true
                    delay(CHARGE_POLL_INTERVAL_MS)
                }
                @Suppress("UNREACHABLE_CODE")
                false
            }
        } catch (e: CancellationException) {
            throw e
        } catch (e: Exception) {
            false
        }
    }

    private fun updateProgress(progress: Int, stepText: String) {
        _uiState.update {
            it.copy(operationState = OperationState.InProgress(progress, stepText))
        }
    }
}

data class HomeUiState(
    val operationState: OperationState = OperationState.Idle,
    val pendingOperation: PendingOperation? = null
)

data class PendingOperation(
    val deviceId: String,
    val operationType: OperationType
)
