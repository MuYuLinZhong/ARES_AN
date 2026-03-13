package com.example.all.domain.model

/**
 * 设备不符异常 —— 步骤1 NFC 返回的 deviceId 与用户选择的设备不一致时抛出
 *
 * 典型场景：用户选择了"办公室门锁"，但手机靠近的是另一把锁。
 */
class DeviceMismatchException(
    message: String = "检测到的设备与所选设备不符"
) : Exception(message)

/**
 * 表单校验异常 —— 本地格式校验失败时抛出（如手机号格式错误、密码长度不足）
 */
class ValidationException(
    message: String
) : Exception(message)
