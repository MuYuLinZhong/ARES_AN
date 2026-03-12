package com.example.all.presentation.home

import android.nfc.Tag
import android.nfc.tech.IsoDep
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.example.all.data.repository.NfcRepository
import com.example.all.domain.model.DeviceMismatchException
import com.example.all.domain.model.LockResult
import com.example.all.domain.model.OperationState
import com.example.all.domain.model.OperationType
import com.example.all.domain.usecase.lock.LocalEncryptUseCase
import com.example.all.domain.usecase.lock.ReceiveChallengeUseCase
import com.example.all.domain.usecase.lock.ReceiveLockResultUseCase
import com.example.all.domain.usecase.lock.SendCipherToLockUseCase
import com.example.all.domain.usecase.lock.SendIntentionBitUseCase
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.CancellationException
import kotlinx.coroutines.Job
import kotlinx.coroutines.TimeoutCancellationException
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch
import java.io.IOException
import javax.inject.Inject

/**
 * 首页 ViewModel —— 核心业务：NFC 五步开/关锁协议
 *
 * Phase 1 完整实现：
 * 1. 接收 WebView 传来的开锁/关锁指令
 * 2. 启动五步协程（发送意图→接收挑战→本地加密→发送密文→接收结果）
 * 3. 每步更新进度百分比，通过 Bridge 推送给前端显示进度条
 * 4. 异常处理：NFC 断开、超时、设备不符、用户取消
 *
 * Phase 2 演进：步骤 3 由 LocalEncryptUseCase 替换为 RequestCipherUseCase。
 *
 * 数据流：WebView → AndroidBridge → HomeViewModel → NfcRepository → 硬件
 *        HomeViewModel → AndroidBridge → WebView（进度/结果回调）
 */
@HiltViewModel
class HomeViewModel @Inject constructor(
    private val nfcRepository: NfcRepository,
    private val sendIntentionBitUseCase: SendIntentionBitUseCase,
    private val receiveChallengeUseCase: ReceiveChallengeUseCase,
    private val localEncryptUseCase: LocalEncryptUseCase,
    private val sendCipherToLockUseCase: SendCipherToLockUseCase,
    private val receiveLockResultUseCase: ReceiveLockResultUseCase
) : ViewModel() {

    /** 首页 UI 状态 */
    private val _uiState = MutableStateFlow(HomeUiState())
    val uiState: StateFlow<HomeUiState> = _uiState.asStateFlow()

    /** 当前开/关锁协程 Job，用于取消操作 */
    private var lockJob: Job? = null

    /** 当前 NFC 连接，用于异常时安全关闭 */
    private var currentIsoDep: IsoDep? = null

    /**
     * 开锁 —— 由 WebView Bridge 调用
     * @param deviceId   目标设备 ID
     * @param deviceName 设备昵称（用于日志/提示）
     * @param tag        NFC Tag（由 ForegroundDispatch 获得）
     */
    fun onUnlockClicked(deviceId: String, deviceName: String, tag: Tag) {
        executeLockOperation(deviceId, OperationType.Unlock, tag)
    }

    /**
     * 关锁 —— 由 WebView Bridge 调用
     */
    fun onLockClicked(deviceId: String, deviceName: String, tag: Tag) {
        executeLockOperation(deviceId, OperationType.Lock, tag)
    }

    /**
     * 用户取消操作 —— 由 WebView Bridge 调用
     * 取消当前协程 + 关闭 NFC 连接 + 重置进度
     */
    fun onOperationCancelled() {
        lockJob?.cancel()
        lockJob = null
        safeDisconnectNfc()
        _uiState.update { it.copy(operationState = OperationState.Idle) }
    }

    /**
     * 通知有新的 NFC Tag 被发现（ForegroundDispatch 回调）
     * 若当前有待处理的操作请求，使用此 Tag 建立连接
     */
    fun onNfcTagDiscovered(tag: Tag) {
        val pending = _uiState.value.pendingOperation ?: return
        executeLockOperation(pending.deviceId, pending.operationType, tag)
        _uiState.update { it.copy(pendingOperation = null) }
    }

    /**
     * 请求开/关锁（在 NFC Tag 到达前先记录意图）
     * 用于 WebView 发起操作时还没检测到 NFC Tag 的场景
     */
    fun requestOperation(deviceId: String, operationType: OperationType) {
        _uiState.update {
            it.copy(
                pendingOperation = PendingOperation(deviceId, operationType),
                operationState = OperationState.InProgress(0, "请将手机靠近门锁...")
            )
        }
    }

    /**
     * 核心：执行五步开/关锁协议
     *
     * 进度对应关系：
     * 步骤 1（发送操作位）→ 20%
     * 步骤 2（接收随机数）→ 40%
     * 步骤 3（本地加密）  → 65%
     * 步骤 4（发送密文）  → 85%
     * 步骤 5（接收结果）  → 100%
     */
    private fun executeLockOperation(
        deviceId: String,
        operationType: OperationType,
        tag: Tag
    ) {
        // 如果已有操作在进行，先取消
        lockJob?.cancel()

        lockJob = viewModelScope.launch {
            try {
                // 建立 NFC 连接
                val isoDep = nfcRepository.connect(tag)
                currentIsoDep = isoDep

                // 步骤 1：发送操作意图位，接收硬件 deviceId
                updateProgress(20, "正在连接门锁...")
                val hardwareDeviceId = sendIntentionBitUseCase(isoDep, operationType)
                // 校验 deviceId 是否与用户选择的设备匹配
                if (hardwareDeviceId != deviceId) {
                    throw DeviceMismatchException()
                }

                // 步骤 2：接收硬件随机数
                updateProgress(40, "正在等待硬件响应...")
                val challenge = receiveChallengeUseCase(isoDep)

                // 步骤 3：本地加密（Phase 1）
                // Phase 2 替换为：val cipher = requestCipherUseCase(deviceId, challenge, operationType)
                updateProgress(65, "正在加密验证...")
                val cipher = localEncryptUseCase(challenge).getOrThrow()

                // 步骤 4：发送密文给硬件
                updateProgress(85, "正在发送验证码...")
                sendCipherToLockUseCase(isoDep, cipher)

                // 步骤 5：接收执行结果
                updateProgress(100, "正在执行...")
                val result = receiveLockResultUseCase(isoDep)

                // 根据结果更新 UI
                when (result) {
                    LockResult.Success -> {
                        _uiState.update {
                            it.copy(operationState = OperationState.Success(
                                if (operationType == OperationType.Unlock) "开锁成功" else "关锁成功"
                            ))
                        }
                    }
                    LockResult.CipherMismatch -> {
                        _uiState.update {
                            it.copy(operationState = OperationState.Error("验证失败，请重试", true))
                        }
                    }
                    LockResult.MechanicalFailure -> {
                        _uiState.update {
                            it.copy(operationState = OperationState.Error("关锁失败：门未完全关闭", true))
                        }
                    }
                }

                // Phase 1：无 reportResult（无云端上报）
                // TODO Phase 2: viewModelScope.launch { reportResultUseCase(operationId, result) }

            } catch (e: DeviceMismatchException) {
                _uiState.update {
                    it.copy(operationState = OperationState.Error("检测到的设备与所选设备不符", false))
                }
            } catch (e: IOException) {
                _uiState.update {
                    it.copy(operationState = OperationState.Error("NFC 信号中断，请重新靠近", true))
                }
            } catch (e: TimeoutCancellationException) {
                _uiState.update {
                    it.copy(operationState = OperationState.Error("NFC 响应超时，请重试", true))
                }
            } catch (e: CancellationException) {
                // 用户主动取消，重置为空闲状态
                _uiState.update { it.copy(operationState = OperationState.Idle) }
            } catch (e: Exception) {
                _uiState.update {
                    it.copy(operationState = OperationState.Error(
                        e.message ?: "操作失败", true
                    ))
                }
            } finally {
                // 无论成功或失败，都安全关闭 NFC 连接
                safeDisconnectNfc()
            }
        }
    }

    /** 更新进度状态 */
    private fun updateProgress(progress: Int, stepText: String) {
        _uiState.update {
            it.copy(operationState = OperationState.InProgress(progress, stepText))
        }
    }

    /** 安全断开 NFC，忽略断开时的异常 */
    private fun safeDisconnectNfc() {
        currentIsoDep?.let { isoDep ->
            viewModelScope.launch {
                try { nfcRepository.disconnect(isoDep) } catch (_: Exception) {}
            }
        }
        currentIsoDep = null
    }
}

/**
 * 首页 UI 状态
 * @property operationState 当前操作状态（空闲/进行中/成功/失败）
 * @property pendingOperation 待处理的操作请求（等待 NFC Tag 到达）
 */
data class HomeUiState(
    val operationState: OperationState = OperationState.Idle,
    val pendingOperation: PendingOperation? = null
)

/**
 * 待处理操作 —— 用户已点击开/关锁但 NFC Tag 尚未到达
 */
data class PendingOperation(
    val deviceId: String,
    val operationType: OperationType
)
