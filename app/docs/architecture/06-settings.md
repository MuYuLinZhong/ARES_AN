# 06 · 设置页模块：用户偏好 · 在线/离线模式 · 账号管理

> **模块边界**：用户偏好持久化（在线模式/震动/NFC灵敏度）、账号信息展示、修改密码入口、账号注销。  
> **依赖模块**：`08-storage`（DataStore 读写）、`02-auth`（登出/注销）  
> **被依赖**：`07-webview-bridge`（设置事件调用）

---

## 1. 模块职责

| 职责 | 说明 |
| :--- | :--- |
| 用户信息展示 | 头像、用户名、角色标签、邮箱（来自登录时存储的 User） |
| 修改密码 | 入口跳转，执行逻辑在 `02-auth.md` |
| 在线/离线模式切换 | Toggle 开关，持久化到 DataStore |
| 震动反馈开关 | Toggle 开关，持久化到 DataStore |
| NFC 灵敏度设置 | 下拉选择 High/Medium/Low，持久化到 DataStore |
| 退出登录 | 清除 Token 和本地缓存，跳登录页 |
| 账号注销 | 二次确认后调用云端注销接口，清除全部本地数据 |

---

## 2. 数据模型

### UserPreferences（`domain/model/UserPreferences.kt`）
| 字段 | 类型 | 默认值 | 说明 |
| :--- | :--- | :--- | :--- |
| `onlineModeEnabled` | Boolean | `true` | 开启时强制联网加密；关闭时降级离线模式（开发阶段预留，当前仅存储，不改变流程） |
| `vibrationEnabled` | Boolean | `true` | NFC 感应成功/操作完成时触发震动 |
| `nfcSensitivity` | enum（High / Medium / Low） | `Medium` | 传递给 `NfcAdapter` 的参数（当前 Android API 不直接支持灵敏度调节，此字段用于 UI 展示预留） |

---

## 3. ViewModel：SettingsViewModel

**文件**：`presentation/settings/SettingsViewModel.kt`

### UiState
```
data class SettingsUiState(
    val user: User? = null,                         // 当前账号信息
    val preferences: UserPreferences = UserPreferences(),
    val isLoading: Boolean = false,
    val logoutState: LogoutState = LogoutState.Idle,
    val deleteAccountState: DeleteAccountState = DeleteAccountState.Idle,
    val showDeleteConfirmDialog: Boolean = false    // 控制注销二次确认弹窗
)

sealed class LogoutState {
    object Idle : LogoutState()
    object Loading : LogoutState()
    object Done : LogoutState()   // 完成后跳转登录页
}

sealed class DeleteAccountState {
    object Idle : DeleteAccountState()
    object Loading : DeleteAccountState()
    object Done : DeleteAccountState()
    data class Error(val message: String) : DeleteAccountState()
}
```

### 方法
| 方法 | 触发来源 | 职责 |
| :--- | :--- | :--- |
| `loadSettings()` | 页面进入 | 读取 DataStore + User，更新 UiState |
| `onToggleOnlineMode(enabled)` | AndroidBridge | SaveUserPreferencesUseCase |
| `onToggleVibration(enabled)` | AndroidBridge | SaveUserPreferencesUseCase |
| `onNfcSensitivityChanged(level)` | AndroidBridge | SaveUserPreferencesUseCase |
| `onLogout()` | AndroidBridge | LogoutUseCase（见 02-auth.md） |
| `onDeleteAccountClicked()` | AndroidBridge | 设置 `showDeleteConfirmDialog = true` |
| `confirmDeleteAccount()` | AndroidBridge | DeleteAccountUseCase（见 02-auth.md） |
| `dismissDeleteDialog()` | AndroidBridge | 关闭弹窗，取消注销 |

---

## 4. UseCase 清单

### 4.1 GetUserPreferencesUseCase（`domain/usecase/settings/`）

**输入**：无  
**输出**：`Flow<UserPreferences>`（响应式，DataStore 变化时自动推送）

**逻辑**：
- 直接返回 `PreferencesRepository.observePreferences()`
- ViewModel 通过 `collectAsState` 驱动 UI 实时响应

---

### 4.2 SaveUserPreferencesUseCase（`domain/usecase/settings/`）

**输入**：`update: UserPreferences.() -> UserPreferences`（函数式更新）  
**输出**：`Result<Unit>`

**逻辑**：
- 读取当前 preferences
- 应用 `update` 函数得到新值
- 调用 `PreferencesRepository.savePreferences(newPreferences)` 写入 DataStore
- DataStore 变化会自动触发 `GetUserPreferencesUseCase` 的 Flow，UI 自动刷新

---

## 5. Repository 接口（PreferencesRepository）

**文件**：`data/repository/PreferencesRepository.kt`

| 方法 | 参数 | 返回 | 说明 |
| :--- | :--- | :--- | :--- |
| `observePreferences()` | — | `Flow<UserPreferences>` | DataStore 的 Flow，自动响应变化 |
| `savePreferences(prefs)` | UserPreferences | `Unit` | 写入 DataStore |
| `saveTokens(tokens)` | AuthTokens | `Unit` | 加密存储 Token（跨模块调用） |
| `clearTokens()` | — | `Unit` | 清除 Token |
| `clearAll()` | — | `Unit` | 注销时清除全部 DataStore 数据 |

---

## 6. 在线/离线模式说明

| 模式 | 开/关锁行为 | 当前阶段 |
| :--- | :--- | :--- |
| 在线模式（默认） | 必须联网，请求云端加密密文，安全级别最高 | 正常业务流程 |
| 离线模式（预留） | 跳过云端加密步骤，使用本地预存密钥（降级方案，安全级别降低） | 仅存储字段，流程不改变，后期硬件支持时接入 |

> **注意**：当前阶段开/关锁流程**始终走云端加密**，`onlineModeEnabled` 字段仅供未来离线模式扩展使用，现阶段不影响业务逻辑。

---

## 7. 震动反馈触发点

`vibrationEnabled` 字段由原生层读取，在以下时机触发震动（`Vibrator` 或 `VibrationEffect`）：
- NFC Tag 感应成功（首次建立连接）
- 开/关锁操作成功
- 开/关锁操作失败

震动逻辑封装在 `HapticFeedbackHelper`（工具类），读取 DataStore 中的 `vibrationEnabled` 决定是否执行。

---

## 8. 边界与异常

| 场景 | 处理 |
| :--- | :--- |
| DataStore 读取失败（极罕见） | 使用默认值 `UserPreferences()` 展示，不报错 |
| 账号注销时网络断开 | 提示"注销失败，请检查网络后重试"；**不清除本地数据**（等成功后再清） |
| 注销弹窗误触 | 提供 Cancel 按钮关闭弹窗，不触发任何操作 |
| 登出时无网络 | 允许本地登出（清 Token + 缓存），云端吊销异步发送（失败也无影响，Token 会自然过期） |
