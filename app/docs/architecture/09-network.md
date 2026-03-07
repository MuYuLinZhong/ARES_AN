# 09 · 网络层：Retrofit 接口 · OkHttp 拦截器 · 错误码处理

> **模块边界**：所有 HTTP 通信的配置、接口定义、拦截器逻辑和错误映射。  
> **依赖模块**：`02-auth`（Token 管理，刷新逻辑）、`11-security`（证书固定）  
> **被依赖**：所有 RemoteRepository 实现

---

## 1. OkHttp 客户端配置

**文件**：`di/NetworkModule.kt`（Hilt 模块，见 `12-di.md`）

### 1.1 超时配置
| 超时类型 | 时间 | 说明 |
| :--- | :--- | :--- |
| Connect Timeout | 10 秒 | TCP 连接建立超时 |
| Read Timeout | 10 秒 | 读取响应超时（加密接口单独 10s） |
| Write Timeout | 10 秒 | 发送请求超时 |
| Call Timeout | 15 秒 | 单次请求端到端总超时（含重试） |

### 1.2 拦截器链（按执行顺序）
```
OkHttpClient
  ├─ CertificatePinner（证书固定，见 11-security.md）
  ├─ AuthInterceptor（Token 注入 + 401 处理）
  ├─ HttpLoggingInterceptor（Debug 模式开启，Release 关闭）
  └─ (Retrofit 默认处理)
```

---

## 2. AuthInterceptor（认证拦截器）

**文件**：`data/remote/AuthInterceptor.kt`

### 职责
1. 每个请求自动注入 `Authorization: Bearer {accessToken}` Header
2. 收到 `401` 响应时，透明刷新 AccessToken，重试原请求一次
3. 刷新失败时，触发强制退出事件

### 流程
```
拦截请求
  ↓
从 DataStore 读取 accessToken（同步读，需在 IO 线程）
  ↓
添加 Header "Authorization: Bearer {token}"
  ↓
放行请求，等待响应
  ↓
收到 401？
  否 → 返回响应
  是 → 进入刷新逻辑（加互斥锁，防止多个请求并发刷新）
         ↓
         用 RefreshToken 请求 POST /auth/refresh
           成功 → 保存新 Token → 用新 Token 重试原请求一次 → 返回响应
           失败 → 清除所有 Token → 通过 EventBus/SharedFlow 发送 ForceLogoutEvent
                  → 返回原始 401 响应（UI 层监听 ForceLogoutEvent 跳转登录页）
```

**互斥锁实现**：使用 `Mutex`（kotlinx.coroutines）或 `synchronized` 确保同一时刻只有一个刷新请求进行。

---

## 3. Retrofit 接口定义

**文件**：`data/api/ApiService.kt`

### 3.1 认证接口
```
POST   /auth/login              → LoginRequest → LoginResponse
POST   /auth/logout             → (无 body) → Unit
POST   /auth/refresh            → RefreshRequest → TokenResponse
PUT    /auth/password           → UpdatePasswordRequest → Unit
GET    /auth/validate           → (无 body) → Unit（仅验证 Token 是否有效）
DELETE /auth/account            → (无 body) → Unit
```

### 3.2 设备管理接口
```
GET    /devices                 → List<DeviceDto>
POST   /devices/bind            → BindDeviceRequest → DeviceDto
DELETE /devices/{deviceId}      → Unit
GET    /devices/{deviceId}      → DeviceDetailDto
GET    /devices/my              → List<PermissionSnapshotDto>（轻量权限快照）
```

### 3.3 授权管理接口
```
GET    /devices/{deviceId}/users           → List<AuthorizedUserDto>
POST   /devices/{deviceId}/invite         → InviteRequest → Unit
DELETE /devices/{deviceId}/users/{userId} → Unit
```

### 3.4 加密接口
```
POST   /crypto/encrypt          → EncryptRequest → EncryptResponse
POST   /crypto/report           → ReportRequest → Unit
```

---

## 4. DTO（网络数据传输对象）

**文件夹**：`data/api/dto/`  
DTO 与 Domain Model 分离，通过 `.toDomain()` 扩展函数转换。

### LoginRequest
```
{ "phone": String, "password": String }
```

### LoginResponse
```
{ "userId": String, "username": String, "phone": String, "role": String,
  "email": String?, "accessToken": String, "refreshToken": String }
```

### DeviceDto
```
{ "deviceId": String, "nickname": String, "serialNo": String,
  "isValid": Boolean, "updatedAt": Long }
```

### AuthorizedUserDto
```
{ "userId": String, "name": String, "phone": String, "role": String }
```

### EncryptRequest
```
{ "deviceId": String, "challenge": String,   // Base64 编码的 challenge 字节
  "operationType": String }                   // "Unlock" or "Lock"
```

### EncryptResponse
```
{ "operationId": String,   // 云端生成的唯一操作ID，用于结果上报
  "cipher": String }        // Base64 编码的密文字节
```

### ReportRequest
```
{ "operationId": String, "success": Boolean,
  "errorCode": Int?,      "mechanicalOk": Boolean? }
```

### PermissionSnapshotDto
```
{ "deviceId": String, "isValid": Boolean }
```

---

## 5. 错误码处理规范

### 5.1 HTTP 状态码映射

| HTTP 状态码 | 业务含义 | App 处理策略 |
| :--- | :--- | :--- |
| 200 / 201 | 成功 | 正常处理 |
| 400 | 请求参数错误 | 解析 body 中的 `message` 字段，展示给用户 |
| 401 | Token 无效/过期 | `AuthInterceptor` 处理，刷新或强制退出 |
| 403 | 无权限 | 不同场景区分处理（见下方） |
| 404 | 资源不存在 | 展示"该资源不存在或已被删除" |
| 409 | 资源冲突（如设备已绑定） | 解析 body `message`，展示具体冲突原因 |
| 5xx | 服务端错误 | 展示"服务器异常，请稍后重试" |
| 无响应（IOException） | 网络不通 | 展示"网络不可用，请检查连接" |
| 读取超时 | 服务端响应慢 | 展示"请求超时，请重试" |

### 5.2 403 场景区分

403 在本系统中有两种含义，通过响应 body 中的 `errorCode` 区分：

| `errorCode` | 含义 | 处理方式 |
| :--- | :--- | :--- |
| `4031` | 设备操作权限已被撤销 | 更新 Room `isValid=false`，禁用操作按钮，提示用户 |
| `4032` | 非 Owner 尝试邀请/撤销用户 | 提示"仅设备 Owner 可管理授权" |
| 其他 | 通用权限不足 | 提示"您没有权限执行此操作" |

### 5.3 错误响应体格式约定

所有错误响应使用统一格式：
```json
{
  "errorCode": 4031,
  "message": "Permission revoked",
  "timestamp": 1700000000000
}
```

**ApiException（统一异常类）**：
```
data class ApiException(
    val httpCode: Int,
    val errorCode: Int,
    val message: String
) : Exception(message)
```

Repository 层将网络异常统一转换为 `ApiException` 或 `IOException`，UseCase 层处理业务逻辑，ViewModel 层转换为用户友好文案。

---

## 6. 网络状态监听

**文件**：`data/remote/NetworkMonitor.kt`

通过 `ConnectivityManager.NetworkCallback` 监听网络状态变化，暴露为 `Flow<Boolean>`（true = 有网络）：
- 网络恢复时触发 `pending_reports` 消费（见 `10-exception.md`）
- 网络断开时各 Repository 静默降级到缓存

---

## 7. FakeRepository 实现策略（第一阶段）

**文件夹**：`data/fake/`

- 所有网络请求替换为 `delay(500)` 模拟网络延迟 + 返回硬写数据
- `FakeAuthRepository.login()` 返回固定 User 和 Token
- `FakeDeviceRepository.fetchDevices()` 返回固定设备列表
- `FakeCryptoRepository.requestCipher()` 返回固定密文字节（用于 NFC 联调测试）
- 通过 Hilt 的 `RepositoryModule` 切换绑定（见 `12-di.md`）
