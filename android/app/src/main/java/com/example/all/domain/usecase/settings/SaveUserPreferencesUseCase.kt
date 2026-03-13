package com.example.all.domain.usecase.settings

import com.example.all.data.repository.PreferencesRepository
import com.example.all.domain.model.UserPreferences
import javax.inject.Inject

/**
 * 保存用户偏好用例 —— 将修改后的偏好写入 DataStore
 *
 * 支持函数式更新：传入一个变换函数，基于当前值生成新值。
 * 这样调用方只需关心要改的字段，无需获取完整偏好对象。
 *
 * 调用链：SettingsViewModel → SaveUserPreferencesUseCase → PreferencesRepository → DataStore
 */
class SaveUserPreferencesUseCase @Inject constructor(
    private val preferencesRepository: PreferencesRepository
) {
    /**
     * 直接保存完整偏好对象
     * @param preferences 新的偏好值
     */
    suspend operator fun invoke(preferences: UserPreferences) {
        preferencesRepository.savePreferences(preferences)
    }
}
