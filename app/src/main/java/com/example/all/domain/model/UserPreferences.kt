package com.example.all.domain.model

/**
 * 用户偏好设置 —— 持久化在 DataStore 中
 *
 * Phase 1 仅使用 vibrationEnabled 和 nfcSensitivity。
 * Phase 2 新增 onlineModeEnabled。
 *
 * @property vibrationEnabled  震动反馈开关，开锁/关锁成功时是否震动提示
 * @property nfcSensitivity    NFC 灵敏度级别，影响 IsoDep 超时时间
 * @property onlineModeEnabled 在线模式开关（Phase 2 启用，控制是否走云端加密）
 */
data class UserPreferences(
    val vibrationEnabled: Boolean = true,
    val nfcSensitivity: NfcSensitivity = NfcSensitivity.Medium,
    val onlineModeEnabled: Boolean = false
)

/**
 * NFC 灵敏度等级
 * - High：高灵敏度，超时时间最短（响应快但信号弱时易失败）
 * - Medium：中等灵敏度，平衡速度与稳定性（默认推荐）
 * - Low：低灵敏度，超时时间最长（信号差环境下更稳定）
 */
enum class NfcSensitivity {
    High, Medium, Low;

    companion object {
        /** 从字符串安全解析，无法识别时降级为 Medium */
        fun fromString(value: String): NfcSensitivity =
            entries.find { it.name.equals(value, ignoreCase = true) } ?: Medium
    }
}
