package com.example.all.domain.model

/**
 * 通用 UI 三态模型 —— 所有 ViewModel 统一使用
 *
 * 让 WebView 端的处理逻辑统一：
 * - Loading → 显示加载动效（骨架屏/菊花）
 * - Success → 展示数据
 * - Error   → 展示错误提示 + 可选重试按钮
 *
 * 使用密封类保证 when 表达式穷举。
 */
sealed class UiState<out T> {
    /** 处理中，显示加载动效 */
    object Loading : UiState<Nothing>()

    /** 成功，携带返回数据 */
    data class Success<T>(val data: T) : UiState<T>()

    /** 失败，携带错误消息和可选错误码 */
    data class Error(
        val message: String,
        val code: Int = 0
    ) : UiState<Nothing>()
}
