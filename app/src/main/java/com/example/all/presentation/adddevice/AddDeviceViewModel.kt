package com.example.all.presentation.adddevice

import android.nfc.Tag
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.example.all.domain.usecase.device.AddDeviceUseCase
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch
import javax.inject.Inject

/**
 * 添加设备 ViewModel —— 管理 NFC 扫描添加设备的流程
 *
 * 完整交互流程：
 * 1. 用户输入设备昵称
 * 2. 点击"Approach and Add"按钮
 * 3. 进入 NFC 扫描等待状态
 * 4. 用户将手机靠近门锁
 * 5. ForegroundDispatch 检测到 NFC Tag → onNfcTagDiscovered
 * 6. 读取 deviceId → 写入 Room → 返回成功
 *
 * 数据流：WebView → Bridge → AddDeviceViewModel → AddDeviceUseCase → NFC+Room
 */
@HiltViewModel
class AddDeviceViewModel @Inject constructor(
    private val addDeviceUseCase: AddDeviceUseCase
) : ViewModel() {

    private val _uiState = MutableStateFlow(AddDeviceUiState())
    val uiState: StateFlow<AddDeviceUiState> = _uiState.asStateFlow()

    /**
     * 开始 NFC 扫描 —— 由 WebView Bridge 调用
     *
     * 校验昵称非空后进入扫描等待状态。
     * 前端应显示"请靠近门锁"的提示动画。
     *
     * @param nickname 用户输入的设备昵称
     */
    fun startNfcScan(nickname: String) {
        if (nickname.isBlank()) {
            _uiState.update { it.copy(errorMessage = "请输入设备昵称") }
            return
        }
        _uiState.update {
            it.copy(
                nickname = nickname,
                nfcScanState = NfcScanState.Scanning,
                errorMessage = null
            )
        }
        // 此时等待 MainActivity 通过 ForegroundDispatch 回调 onNfcTagDiscovered
    }

    /**
     * NFC Tag 到达回调 —— 由 MainActivity.onNewIntent 触发
     *
     * 收到 Tag 后立即调用 AddDeviceUseCase 执行：
     * 连接 → 读取 deviceId → 断开 → 写入 Room
     *
     * @param tag NFC Tag 对象
     */
    fun onNfcTagDiscovered(tag: Tag) {
        val nickname = _uiState.value.nickname
        if (nickname.isBlank()) return

        viewModelScope.launch {
            _uiState.update { it.copy(nfcScanState = NfcScanState.Connecting) }
            addDeviceUseCase(nickname, tag).fold(
                onSuccess = { device ->
                    _uiState.update {
                        it.copy(nfcScanState = NfcScanState.Success(device.deviceId))
                    }
                },
                onFailure = { e ->
                    _uiState.update {
                        it.copy(nfcScanState = NfcScanState.Error(e.message ?: "添加失败"))
                    }
                }
            )
        }
    }

    /** 取消扫描 —— 重置为空闲状态 */
    fun cancelScan() {
        _uiState.update {
            it.copy(nfcScanState = NfcScanState.Idle, nickname = "")
        }
    }
}

/**
 * 添加设备 UI 状态
 */
data class AddDeviceUiState(
    val nickname: String = "",
    val nfcScanState: NfcScanState = NfcScanState.Idle,
    val errorMessage: String? = null
)

/**
 * NFC 扫描状态
 * - Idle: 未开始扫描
 * - Scanning: 等待用户靠近门锁
 * - Connecting: 已检测到 Tag，正在读取
 * - Success: 读取成功，携带 deviceId
 * - Error: 读取失败，携带错误消息
 */
sealed class NfcScanState {
    object Idle : NfcScanState()
    object Scanning : NfcScanState()
    object Connecting : NfcScanState()
    data class Success(val deviceId: String) : NfcScanState()
    data class Error(val message: String) : NfcScanState()
}
