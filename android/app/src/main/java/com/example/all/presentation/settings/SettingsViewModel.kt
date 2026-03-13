package com.example.all.presentation.settings

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.example.all.domain.model.NfcSensitivity
import com.example.all.domain.model.User
import com.example.all.domain.model.UserPreferences
import com.example.all.domain.usecase.settings.GetUserPreferencesUseCase
import com.example.all.domain.usecase.settings.SaveUserPreferencesUseCase
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch
import javax.inject.Inject

/**
 * 设置页 ViewModel —— 管理用户偏好和账号操作
 *
 * Phase 1 功能：
 * 1. 读取/保存震动反馈开关
 * 2. 读取/保存 NFC 灵敏度
 *
 * Phase 2 演进：
 * - 新增用户信息展示（头像/昵称/角色）
 * - 新增登出、注销、修改密码入口
 * - 新增在线模式开关
 *
 * 数据流：DataStore Flow → GetUserPreferencesUseCase → SettingsViewModel → Bridge → WebView
 */
@HiltViewModel
class SettingsViewModel @Inject constructor(
    private val getUserPreferencesUseCase: GetUserPreferencesUseCase,
    private val saveUserPreferencesUseCase: SaveUserPreferencesUseCase
) : ViewModel() {

    private val _uiState = MutableStateFlow(SettingsUiState())
    val uiState: StateFlow<SettingsUiState> = _uiState.asStateFlow()

    init {
        // 订阅 DataStore 偏好 Flow，数据变化时自动更新 UI
        viewModelScope.launch {
            getUserPreferencesUseCase().collect { prefs ->
                _uiState.update { it.copy(preferences = prefs) }
            }
        }
    }

    /**
     * 切换震动反馈开关
     * @param enabled true=开启震动, false=关闭震动
     */
    fun onToggleVibration(enabled: Boolean) {
        viewModelScope.launch {
            val current = _uiState.value.preferences
            saveUserPreferencesUseCase(current.copy(vibrationEnabled = enabled))
        }
    }

    /**
     * 修改 NFC 灵敏度
     * @param level 灵敏度等级字符串："High" / "Medium" / "Low"
     */
    fun onNfcSensitivityChanged(level: String) {
        viewModelScope.launch {
            val current = _uiState.value.preferences
            saveUserPreferencesUseCase(
                current.copy(nfcSensitivity = NfcSensitivity.fromString(level))
            )
        }
    }

    // ====== Phase 2 Stub 方法 ======

    /** 切换在线模式 —— Phase 2 启用 */
    fun onToggleOnlineMode(enabled: Boolean) {
        // TODO Phase 2: saveUserPreferencesUseCase(current.copy(onlineModeEnabled = enabled))
    }

    /** 登出 —— Phase 2 启用 */
    fun onLogout() {
        // TODO Phase 2: logoutUseCase() → 清 Token + Room → UiState.LogoutDone
    }

    /** 注销账号确认弹窗 —— Phase 2 启用 */
    fun onDeleteAccountClicked() {
        // TODO Phase 2: 显示二次确认弹窗
    }

    /** 确认注销 —— Phase 2 启用 */
    fun confirmDeleteAccount() {
        // TODO Phase 2: deleteAccountUseCase() → 清除全部数据
    }
}

/**
 * 设置页 UI 状态
 * @property user        用户信息（Phase 2 填充，Phase 1 为 null）
 * @property preferences 用户偏好设置
 * @property isLoading   是否加载中
 */
data class SettingsUiState(
    val user: User? = null,
    val preferences: UserPreferences = UserPreferences(),
    val isLoading: Boolean = false
)
