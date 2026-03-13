package com.example.all.data.local

import androidx.datastore.core.DataStore
import com.example.all.UserPrefs
import com.example.all.data.repository.PreferencesRepository
import com.example.all.domain.model.AuthTokens
import com.example.all.domain.model.NfcSensitivity
import com.example.all.domain.model.UserPreferences
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.first
import kotlinx.coroutines.flow.map
import javax.inject.Inject

/**
 * DataStore 偏好仓库 —— PreferencesRepository 接口的 Phase 1 实现
 *
 * 使用 Proto DataStore 持久化用户偏好和本地调试密钥。
 * 比 SharedPreferences 更安全（不会在主线程阻塞、类型安全、支持 Flow 观察）。
 *
 * Phase 1 功能：
 *   - 读写震动开关、NFC 灵敏度
 *   - 读写本地调试密钥
 *
 * Phase 2 新增：
 *   - Token 加密存储（配合 KeystoreManager）
 */
class DataStorePreferencesRepository @Inject constructor(
    private val dataStore: DataStore<UserPrefs>
) : PreferencesRepository {

    /**
     * 观察用户偏好 —— 将 Proto 数据流映射为领域模型流
     * DataStore 数据变化时自动推送新值给所有观察者
     */
    override fun observePreferences(): Flow<UserPreferences> =
        dataStore.data.map { prefs ->
            UserPreferences(
                vibrationEnabled = prefs.vibrationEnabled,
                nfcSensitivity = NfcSensitivity.fromString(prefs.nfcSensitivity)
                // onlineModeEnabled: Phase 2 启用
            )
        }

    /**
     * 保存用户偏好 —— 将领域模型写回 Proto DataStore
     *
     * DataStore.updateData 是原子操作，线程安全。
     */
    override suspend fun savePreferences(preferences: UserPreferences) {
        dataStore.updateData { current ->
            current.toBuilder()
                .setVibrationEnabled(preferences.vibrationEnabled)
                .setNfcSensitivity(preferences.nfcSensitivity.name)
                .build()
        }
    }

    /**
     * 读取本地调试密钥 —— Phase 1 专用
     * @return 十六进制密钥字符串，可能为空（空时使用 BuildConfig 默认值）
     */
    override suspend fun getLocalDeviceKey(): String =
        dataStore.data.first().localDeviceKeyHex

    /**
     * 写入本地调试密钥 —— Phase 1 调试工具调用
     * @param keyHex 新密钥的十六进制字符串
     */
    override suspend fun setLocalDeviceKey(keyHex: String) {
        dataStore.updateData { current ->
            current.toBuilder()
                .setLocalDeviceKeyHex(keyHex)
                .build()
        }
    }

    // ====== Phase 2 Token 存储方法（当前为 stub） ======

    override suspend fun saveTokens(tokens: AuthTokens) {
        // TODO Phase 2: 使用 KeystoreManager AES-GCM 加密后写入 DataStore
    }

    override suspend fun getAccessToken(): String? {
        // TODO Phase 2: 从 DataStore 读取密文，用 KeystoreManager 解密返回
        return null
    }

    override suspend fun getRefreshToken(): String? {
        // TODO Phase 2: 同 getAccessToken
        return null
    }

    override suspend fun clearTokens() {
        // TODO Phase 2: 清除 DataStore 中的 Token 相关字段
    }

    override suspend fun clearAll() {
        // TODO Phase 2: 清除所有 DataStore 数据（注销时调用）
        dataStore.updateData { UserPrefsSerializer.defaultValue }
    }
}
