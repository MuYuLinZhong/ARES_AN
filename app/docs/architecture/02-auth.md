# 02 · 认证模块：登录 · Token 管理 · 单设备策略

> **模块边界**：账号认证的完整生命周期，包括登录、登出、修改密码、Token 存储、静默刷新、换机吊销。  
> **依赖模块**：`08-storage`（DataStore Token 存储）、`09-network`（AuthInterceptor、认证 API）  
> **被依赖**：`01-startup`（Token 有效性判断）、`07-webview-bridge`（登录/登出事件）

---

## 1. 模块职责

| 职责 | 说明 |
| :--- | :--- |
| 账号登录 | 手机号 + 密码验证，成功后存储双 Token |
| 登出 | 清除本地 Token 和 Room 缓存，通知云端吊销 |
| 修改密码 | 三段式校验（当前/新/确认），调用云端接口 |
| Token 有效性管理 | 本地解析 JWT exp，无网络时仍可判断有效性 |
| 静默刷新 | AccessToken 过期时，无感知地用 RefreshToken 续期 |
| 单设备策略 | 用户在新设备登录时，云端吊销旧设备所有 Token |

---

## 2. 数据模型

### User（`domain/model/User.kt`）
| 字段 | 类型 | 说明 |
| :--- | :--- | :--- |
| `userId` | String | 唯一标识 |
| `phone` | String | 手机号（脱敏展示时只显示后四位） |
| `username` | String | 显示名称 |
| `role` | enum（Owner / Guest） | 账号角色 |
| `email` | String? | 邮箱（可选） |

### AuthTokens（Token 持有对象，不落盘原始类，加密后存 DataStore）
| 字段 | 说明 |
| :--- | :--- |
| `accessToken` | 短效 Token（JWT），用于接口认证 |
| `refreshToken` | 长效 Token，仅用于刷新 AccessToken |

---

## 3. ViewModel：AuthViewModel

**文件**：`presentation/auth/AuthViewModel.kt`

### UiState
```
sealed class AuthUiState {
    object Idle          : AuthUiState()   // 初始状态
    object Loading       : AuthUiState()   // 请求中，按钮禁用，显示加载动效
    data class LoggedIn(val user: User) : AuthUiState()  // 登录成功
    data class Error(val message: String) : AuthUiState() // 登录失败，展示错误文案
    object LoggedOut     : AuthUiState()   // 已登出，跳转登录页
    object PasswordUpdated : AuthUiState() // 密码修改成功，返回设置页
}
```

### 方法（由 AndroidBridge 调用）
| 方法 | 触发来源 | 职责 |
| :--- | :--- | :--- |
| `login(phone, password)` | `onLoginClicked` | 校验格式 → LoginUseCase → 更新 UiState |
| `logout()` | `onLogout` | LogoutUseCase → 清除数据 → UiState.LoggedOut |
| `updatePassword(current, new, confirm)` | `onUpdatePasswordClicked` | UpdatePasswordUseCase → UiState |
| `deleteAccount()` | `onDeleteAccount` | DeleteAccountUseCase → 清除全部本地数据 |

---

## 4. UseCase 清单

### 4.1 LoginUseCase（`domain/usecase/auth/LoginUseCase.kt`）

**输入**：`phone: String, password: String`  
**输出**：`Result<User>`

**执行步骤**：
1. 格式校验：手机号 11 位纯数字；密码非空且长度 ≥ 8
2. 调用 `AuthRepository.login(phone, password)` → 获取 `AuthTokens` 和 `User`
3. 调用 `PreferencesRepository.saveTokens(tokens)` 加密存储双 Token
4. 返回 `User`，ViewModel 更新 UiState.LoggedIn

**失败情况**：
- 格式不合法 → 直接返回 `Result.failure(ValidationException("手机号格式错误"))`
- 网络错误 / 密码错误 → `AuthRepository` 抛出对应异常，转换为用户友好文案

---

### 4.2 LogoutUseCase（`domain/usecase/auth/LogoutUseCase.kt`）

**输入**：无  
**输出**：`Result<Unit>`

**执行步骤**：
1. 调用 `AuthRepository.logout()` — 通知云端吊销当前设备 Token（网络失败也继续本地清除）
2. 调用 `PreferencesRepository.clearTokens()` 清除 DataStore 中的双 Token
3. 调用 `DeviceRepository.clearLocalCache()` 清除 Room 设备缓存（logout 时不保留缓存）

---

### 4.3 UpdatePasswordUseCase（`domain/usecase/auth/UpdatePasswordUseCase.kt`）

**输入**：`currentPassword: String, newPassword: String, confirmPassword: String`  
**输出**：`Result<Unit>`

**执行步骤**：
1. 校验 `newPassword == confirmPassword`，否则返回 `ValidationException`
2. 校验 `newPassword.length >= 8`，否则返回 `ValidationException`
3. 调用 `AuthRepository.updatePassword(currentPassword, newPassword)` → 云端验证旧密码并更新
4. 成功后返回 `Result.success(Unit)`

---

### 4.4 ValidateTokenUseCase（`domain/usecase/auth/ValidateTokenUseCase.kt`）

**输入**：无  
**输出**：`TokenStatus`（enum：Valid / Expired / Missing）

**执行步骤**（纯本地，无网络）：
1. 读取 DataStore 中的 AccessToken（解密后）
2. 解析 JWT payload 的 `exp` 字段（Base64 decode，不需要签名验证）
3. 比较 `exp` 与当前时间戳（预留 60 秒缓冲，避免时钟误差）
4. 返回对应状态

---

### 4.5 RefreshTokenUseCase（`domain/usecase/auth/RefreshTokenUseCase.kt`）

**输入**：无  
**输出**：`Result<Unit>`

**执行步骤**：
1. 读取 DataStore 中的 RefreshToken
2. 调用 `AuthRepository.refreshToken(refreshToken)` → 获取新的双 Token
3. 调用 `PreferencesRepository.saveTokens(newTokens)` 覆盖存储
4. 失败时返回 `Result.failure`，调用方（SplashViewModel 或 AuthInterceptor）负责后续清除逻辑

---

### 4.6 DeleteAccountUseCase（`domain/usecase/auth/DeleteAccountUseCase.kt`）

**输入**：无  
**输出**：`Result<Unit>`

**执行步骤**：
1. 调用 `AuthRepository.deleteAccount()` — 云端删除账号、所有设备绑定、所有授权关系
2. `PreferencesRepository.clearAll()` — 清除 DataStore 所有字段
3. 清除全部 Room 表数据（`DeviceDao.deleteAll()`、`AuthorizedUserDao.deleteAll()`、`PendingReportDao.deleteAll()`）

---

## 5. Repository 接口（AuthRepository）

**文件**：`data/repository/AuthRepository.kt`

| 方法 | 参数 | 返回 | 说明 |
| :--- | :--- | :--- | :--- |
| `login(phone, password)` | String, String | `Pair<User, AuthTokens>` | 调用 POST `/auth/login` |
| `logout()` | — | `Unit` | 调用 POST `/auth/logout`，网络失败不抛出 |
| `refreshToken(refreshToken)` | String | `AuthTokens` | 调用 POST `/auth/refresh` |
| `updatePassword(current, new)` | String, String | `Unit` | 调用 PUT `/auth/password` |
| `deleteAccount()` | — | `Unit` | 调用 DELETE `/auth/account` |

---

## 6. Token 生命周期完整流程

### 6.1 Token 存储（加密）
- AccessToken 和 RefreshToken 均通过 Android Keystore 生成的 AES-GCM 密钥加密后存入 DataStore
- 密钥绑定设备，不可导出，不可跨设备解密
- 具体加密实现见 `11-security.md`

### 6.2 Token 使用（自动注入）
- 所有 API 请求通过 OkHttp 的 `AuthInterceptor` 自动注入 `Authorization: Bearer {accessToken}`
- 具体拦截器实现见 `09-network.md`

### 6.3 Token 401 处理（透明续期）
在 `AuthInterceptor` 中处理（见 `09-network.md`）：
```
收到 401 响应
  ↓
尝试 RefreshTokenUseCase（最多一次，加互斥锁防止并发刷新）
  ↓ 成功 → 用新 Token 重试原请求一次
  ↓ 失败 → 清除 Token → 发送"强制退出"事件 → UI 跳转登录页
```

### 6.4 换机登录自动吊销（单设备策略）
- 云端在处理 `/auth/login` 时，会将该账号的旧设备 Token 全部加入黑名单
- 旧设备下次发起请求时，`AuthInterceptor` 收到 `401`，`RefreshToken` 也会失败（已被黑名单）
- 最终触发强制退出，提示："您的账号已在新设备登录，请重新验证"

---

## 7. 后台 Token 有效性校验（App 首页加载后触发）

登录后进入首页时，在 `HomeViewModel.init{}` 中触发一次后台轻量校验：
- 调用 GET `/auth/validate`（仅验证 Token，不返回数据）
- 返回 `200` → 无操作
- 返回 `401` → 强制退出（Token 被后台吊销，即换机场景的旧设备兜底）
- 网络不通 → 静默忽略，允许离线使用

---

## 8. 边界与异常

| 场景 | 处理 |
| :--- | :--- |
| 登录时无网络 | `AuthRepository` 抛出 `IOException`，转换为"网络不可用，请检查连接" |
| 手机号不存在 / 密码错误 | 云端返回 `401`，转换为"手机号或密码错误" |
| RefreshToken 过期（90 天未登录） | `refreshToken` 接口返回 `401`，强制退出 |
| 修改密码时旧密码错误 | 云端返回 `400`，提示"当前密码不正确" |
| 账号注销失败（网络错误） | 允许重试；提示"注销请求失败，请稍后再试" |
| DataStore 加密异常（密钥丢失，极罕见） | catch 异常，清除所有 DataStore 数据，跳转登录页 |
