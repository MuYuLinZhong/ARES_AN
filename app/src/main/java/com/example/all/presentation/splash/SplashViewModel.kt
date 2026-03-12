package com.example.all.presentation.splash

import android.nfc.NfcAdapter
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.launch
import javax.inject.Inject

/**
 * 启动路由 ViewModel —— 控制 App 冷启动后的导航决策
 *
 * Phase 1 职责：
 * 1. 检测设备是否支持 NFC 硬件
 * 2. 检测 NFC 是否已开启
 * 3. NFC 可用 → 直接路由到首页（无 Token 判断）
 *
 * Phase 2 演进：NFC 检测后增加 Token 有效性判断，
 *               决定跳转首页还是登录页。
 *
 * 数据流：SplashActivity ← collect ← uiState ← checkNfcAndRoute()
 */
@HiltViewModel
class SplashViewModel @Inject constructor(
    private val nfcAdapter: NfcAdapter?
) : ViewModel() {

    /** UI 状态流，SplashActivity 通过 collect 订阅 */
    private val _uiState = MutableStateFlow<StartupUiState>(StartupUiState.Checking)
    val uiState: StateFlow<StartupUiState> = _uiState.asStateFlow()

    init {
        // ViewModel 创建时立即执行 NFC 检测
        viewModelScope.launch { checkNfcAndRoute() }
    }

    /**
     * NFC 检测与路由决策
     *
     * 判断顺序：
     * 1. nfcAdapter == null → 设备无 NFC 芯片 → 弹窗退出
     * 2. !nfcAdapter.isEnabled → NFC 未开启 → 引导去设置
     * 3. NFC 可用 → Phase 1 直接进首页
     */
    private fun checkNfcAndRoute() {
        when {
            nfcAdapter == null -> {
                _uiState.value = StartupUiState.NfcNotAvailable
            }
            !nfcAdapter.isEnabled -> {
                _uiState.value = StartupUiState.NfcDisabled
            }
            else -> {
                // Phase 1：NFC 可用，直接进首页，不判断 Token
                _uiState.value = StartupUiState.GoToHome
                // TODO Phase 2：调用 validateTokenUseCase → 决定 GoToHome 或 GoToLogin
            }
        }
    }

    /** 用户从 NFC 设置页返回后重新检测 */
    fun recheckNfc() {
        _uiState.value = StartupUiState.Checking
        checkNfcAndRoute()
    }
}

/**
 * 启动页 UI 状态 —— 控制 SplashActivity 的显示和跳转行为
 */
sealed class StartupUiState {
    /** 检测中，显示 Splash 界面 */
    object Checking : StartupUiState()

    /** NFC 可用，跳转首页 */
    object GoToHome : StartupUiState()

    /** Phase 2：Token 无效，跳转登录页 */
    object GoToLogin : StartupUiState()

    /** 设备不支持 NFC，显示错误弹窗后退出 */
    object NfcNotAvailable : StartupUiState()

    /** NFC 未开启，显示引导弹窗去设置 */
    object NfcDisabled : StartupUiState()
}
