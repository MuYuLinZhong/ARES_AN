package com.example.all.presentation.auth

import androidx.lifecycle.ViewModel
import com.example.all.domain.model.User
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import javax.inject.Inject

/**
 * 认证 ViewModel —— Phase 1 Stub 骨架
 *
 * Phase 1 整个认证模块不参与（无登录页、无 Token），
 * 此 ViewModel 仅提供接口骨架，所有方法为空实现。
 *
 * Phase 2 完整实现：登录/登出/修改密码/Token 生命周期管理。
 */
@HiltViewModel
class AuthViewModel @Inject constructor() : ViewModel() {

    private val _uiState = MutableStateFlow<AuthUiState>(AuthUiState.Idle)
    val uiState: StateFlow<AuthUiState> = _uiState.asStateFlow()

    /** 登录 —— Phase 2 实现 */
    fun login(phone: String, password: String) {
        // TODO Phase 2: loginUseCase(phone, password) → saveTokens → GoToHome
    }

    /** 登出 —— Phase 2 实现 */
    fun logout() {
        // TODO Phase 2: logoutUseCase() → clearTokens → GoToLogin
    }

    /** 修改密码 —— Phase 2 实现 */
    fun updatePassword(current: String, newPwd: String, confirm: String) {
        // TODO Phase 2: updatePasswordUseCase(current, newPwd, confirm)
    }

    /** 注销账号 —— Phase 2 实现 */
    fun deleteAccount() {
        // TODO Phase 2: deleteAccountUseCase() → clearAll → GoToLogin
    }
}

/**
 * 认证 UI 状态
 */
sealed class AuthUiState {
    object Idle : AuthUiState()
    object Loading : AuthUiState()
    data class LoggedIn(val user: User) : AuthUiState()
    data class Error(val message: String) : AuthUiState()
    object LoggedOut : AuthUiState()
    object PasswordUpdated : AuthUiState()
}
