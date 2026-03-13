package com.example.all.domain.usecase.settings

import com.example.all.data.repository.PreferencesRepository
import com.example.all.domain.model.UserPreferences
import kotlinx.coroutines.flow.Flow
import javax.inject.Inject

/**
 * 获取用户偏好用例 —— 返回偏好设置的响应式 Flow
 *
 * DataStore 数据变化时自动推送新偏好值。
 * SettingsViewModel 通过 collect 订阅此 Flow。
 *
 * 调用链：SettingsViewModel → GetUserPreferencesUseCase → PreferencesRepository → DataStore Flow
 */
class GetUserPreferencesUseCase @Inject constructor(
    private val preferencesRepository: PreferencesRepository
) {
    /** @return 用户偏好 Flow */
    operator fun invoke(): Flow<UserPreferences> =
        preferencesRepository.observePreferences()
}
