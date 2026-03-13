package com.example.all.presentation.adddevice

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.example.all.domain.usecase.device.AddDeviceUseCase
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.Job
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch
import javax.inject.Inject

/**
 * 添加设备 ViewModel —— 管理 NFC 扫描添加设备的流程
 *
 * SmAcK SDK 自动管理 NFC Tag 发现，不再需要手动传递 Tag 对象。
 * 流程：
 * 1. 用户输入设备昵称
 * 2. 点击 "Approach and Add" 按钮
 * 3. SmAcK SDK 等待 NFC Tag 进入范围
 * 4. lockApi.getLock() 返回锁信息 → 读取 lockId → 写入 Room
 */
@HiltViewModel
class AddDeviceViewModel @Inject constructor(
    private val addDeviceUseCase: AddDeviceUseCase
) : ViewModel() {

    private val _uiState = MutableStateFlow(AddDeviceUiState())
    val uiState: StateFlow<AddDeviceUiState> = _uiState.asStateFlow()

    private var scanJob: Job? = null

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

        scanJob = viewModelScope.launch {
            _uiState.update { it.copy(nfcScanState = NfcScanState.Connecting) }
            addDeviceUseCase(nickname).fold(
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

    fun cancelScan() {
        scanJob?.cancel()
        scanJob = null
        _uiState.update {
            it.copy(nfcScanState = NfcScanState.Idle, nickname = "")
        }
    }
}

data class AddDeviceUiState(
    val nickname: String = "",
    val nfcScanState: NfcScanState = NfcScanState.Idle,
    val errorMessage: String? = null
)

sealed class NfcScanState {
    object Idle : NfcScanState()
    object Scanning : NfcScanState()
    object Connecting : NfcScanState()
    data class Success(val deviceId: String) : NfcScanState()
    data class Error(val message: String) : NfcScanState()
}
