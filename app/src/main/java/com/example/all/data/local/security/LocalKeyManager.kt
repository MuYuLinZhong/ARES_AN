package com.example.all.data.local.security

import com.example.all.BuildConfig
import com.example.all.data.repository.PreferencesRepository
import javax.inject.Inject

/**
 * 本地密钥管理器 —— Phase 1 专用，管理 NFC 调试密钥
 *
 * 职责：
 * 1. 优先从 DataStore 读取用户运行时配置的密钥
 * 2. 若 DataStore 中无密钥，降级使用 BuildConfig 中的默认调试密钥
 * 3. 提供密钥更新能力（方便调试不同硬件设备）
 *
 * ⚠️ Phase 1 密钥为明文存储，仅用于开发调试。
 *    Phase 2 后设备密钥由云端 HSM 管理，本类将被移除。
 */
class LocalKeyManager @Inject constructor(
    private val preferencesRepository: PreferencesRepository
) {
    companion object {
        /**
         * 默认调试密钥 —— 在 build.gradle 的 buildConfigField 中配置
         * 16 字节 AES-128 密钥的十六进制表示（32 个字符）
         */
        const val DEFAULT_DEBUG_KEY_HEX: String = BuildConfig.DEBUG_DEVICE_KEY
    }

    /**
     * 获取当前设备密钥的字节数组
     *
     * 查找顺序：DataStore 自定义密钥 → BuildConfig 默认密钥
     * @return AES 密钥的原始字节数组
     */
    suspend fun getDeviceKey(): ByteArray {
        val keyHex = preferencesRepository.getLocalDeviceKey()
            .ifBlank { DEFAULT_DEBUG_KEY_HEX }
        return keyHex.hexToByteArray()
    }

    /**
     * 更新调试密钥 —— 运行时替换密钥（仅 Debug 构建使用）
     *
     * @param keyHex 新密钥的十六进制字符串
     * @throws IllegalArgumentException 密钥长度不为 32（AES-128）或 64（AES-256）
     */
    suspend fun updateDeviceKey(keyHex: String) {
        require(keyHex.length == 32 || keyHex.length == 64) {
            "密钥长度必须为 32（AES-128）或 64（AES-256）个十六进制字符"
        }
        preferencesRepository.setLocalDeviceKey(keyHex)
    }
}

/**
 * 十六进制字符串转字节数组的扩展函数
 *
 * 例："0123456789abcdef" → [0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF]
 * @throws IllegalStateException 字符串长度为奇数时抛出
 */
fun String.hexToByteArray(): ByteArray {
    check(length % 2 == 0) { "十六进制字符串长度必须为偶数" }
    return ByteArray(length / 2) { i ->
        substring(i * 2, i * 2 + 2).toInt(16).toByte()
    }
}
