# 10 · 异常与边界处理：断网 · 待上报队列 · Token 失效 · 权限撤销

> **模块边界**：跨越多个模块的异常场景的统一处理策略，以及 pending_reports 队列的完整消费机制。  
> **依赖模块**：`08-storage`（Room pending_reports）、`09-network`（NetworkMonitor）  
> **被依赖**：`03-nfc-core`（中止时写队列）、`01-startup`（启动时消费队列）

---

## 1. 断网场景分级处理

断网不是一种状态，是三种完全不同的场景，需要分级对待：

### 场景 A：启动时无网络

```
App 启动 → ConnectivityManager 检测无网络
  ↓
读取 Room 本地缓存
  ↓ 有缓存                           ↓ 无缓存（首次安装/清数据）
展示缓存数据                          展示全屏"无网络连接"占位页
顶部显示"离线模式·数据可能不是最新"    + 重试按钮（等待用户联网后重试）
横幅（非弹窗，不打断操作）
开/关锁按钮置灰：提示"需要联网才能操作"
设置页偏好正常展示（读 DataStore，不需要网络）
```

### 场景 B：浏览中断网

- `DeviceRepository` 网络请求捕获到 `IOException` → **不抛出错误**，静默降级到 Room 缓存
- UI 显示缓存内容 + 顶部出现"网络已断开，显示的是缓存数据"横幅（持久条幅，不是 Toast）
- 横幅右侧提供"重新加载"按钮，等网络恢复后用户可手动刷新
- 横幅在 `NetworkMonitor` 检测到网络恢复时自动消失

### 场景 C：NFC 开/关锁流程中网络中断（最复杂）

发生在 `RequestCipherUseCase`（步骤3，请求云端密文）时：

```
步骤1：发送操作位 ✓
步骤2：收到随机数 ✓
步骤3：请求云端密文 → IOException（网络中断）
  ↓
立即取消整个协程链（NFC 连接同步关闭）
  ↓
写入 pending_reports 表（见第3节）
  ↓
UI：显示"网络中断，操作已取消"，进度条重置为0
```

> 步骤1/2 发生中断：NFC 已断开，直接中止，**不写** pending_reports（未到达云端，无需上报）  
> 步骤3/4/5 发生中断：**必须写** pending_reports（云端已有"处理中"记录，需要上报结果）

---

## 2. 全局异常分类

| 异常类型 | 来源 | 统一处理方式 |
| :--- | :--- | :--- |
| `IOException` | 网络不通/NFC断开 | 降级缓存 or 中止NFC流程 |
| `SocketTimeoutException` | 网络超时 | 视为网络失败，同上 |
| `ApiException(401)` | Token 失效 | `AuthInterceptor` 自动处理，失败则强退 |
| `ApiException(403, 4031)` | 设备权限被撤销 | 更新 Room isValid=false，禁用 UI |
| `ApiException(403, 其他)` | 角色权限不足 | 提示用户无权操作 |
| `ApiException(4xx)` | 客户端参数错误 | 解析 message 展示给用户 |
| `ApiException(5xx)` | 服务端错误 | 展示"服务器异常，请稍后重试" |
| `DeviceMismatchException` | NFC 读到错误设备 | 中止流程，提示"设备不符" |
| `ValidationException` | 本地参数校验失败 | 直接展示校验错误文案 |

---

## 3. 待上报队列（pending_reports）完整设计

### 3.1 写入时机
在 `HomeViewModel` 的协程 `catch` 块中写入，满足以下**所有条件**：
1. 操作已到达步骤3（`RequestCipherUseCase`）或之后
2. 流程被中断（网络异常、NFC 异常、用户取消）

```kotlin
// HomeViewModel 伪代码
viewModelScope.launch {
    val operationId = UUID.randomUUID().toString()
    try {
        // 步骤1-5
    } catch (e: Exception) {
        if (reachedStep >= 3) {
            pendingReportDao.insert(
                PendingReportEntity(
                    operationId = operationId,
                    deviceId = deviceId,
                    operationType = operationType.name,
                    result = e.toResultString(),
                    failedAtStep = currentStep,
                    createdAt = System.currentTimeMillis()
                )
            )
        }
    }
}
```

### 3.2 消费逻辑

**UseCase**：`ConsumePendingReportsUseCase`（`domain/usecase/report/`）

执行步骤：
1. 查询 `pending_reports` 中所有 `status='pending'` 的记录
2. 逐条调用 `CryptoRepository.reportResult(operationId, result={cancelled/error})`
3. 成功：`markSent(operationId)`（或直接删除）
4. 失败（网络问题）：`retryCount + 1`，保留记录等下次消费
5. `createdAt` 超过 72 小时且 `retryCount >= 3`：`markExpired(operationId)`，不再重试

### 3.3 消费触发时机

| 触发点 | 实现方式 | 说明 |
| :--- | :--- | :--- |
| 网络恢复时（即时） | `NetworkMonitor` Flow 收到 `true` → 触发一次性 WorkManager 任务 | 最及时 |
| App 下次启动时（兜底） | `SplashActivity` 决策树最后，后台启动消费 | 保证不丢 |
| 定期后台任务（保险） | WorkManager 每 6 小时周期任务 | 最终兜底 |

### 3.4 幂等性保证
- 每条记录的 `operationId` 全局唯一（UUID）
- 云端 `/crypto/report` 接口设计为幂等：同一 `operationId` 重复提交只处理一次，不产生重复日志

---

## 4. Token 失效处理（完整链路）

### 4.1 AccessToken 在请求中途过期
见 `09-network.md AuthInterceptor`：自动刷新，用户无感知。

### 4.2 RefreshToken 也过期
```
AuthInterceptor 刷新失败
  ↓
PreferencesRepository.clearTokens()
AppDatabase.clearAllTables()（清除 Room 所有缓存）
  ↓
发送 ForceLogoutEvent（通过 ApplicationScope 的 SharedFlow 广播）
  ↓
MainActivity 监听到 ForceLogoutEvent → WebView 调用 window.onForceLogout(json)
  ↓
前端跳转登录页，清除所有本地状态
```

### 4.3 Token 被后台吊销（换机场景）
```
App 进入首页后，HomeViewModel.init{} 发起 GET /auth/validate
  ↓
服务端返回 401（旧设备 Token 已被换机吊销）
  ↓
AuthInterceptor 尝试 RefreshToken 刷新 → 同样 401
  ↓
触发 ForceLogoutEvent，提示："账号已在新设备登录，请重新验证"
```

---

## 5. 权限撤销感知链路

见 `05-permission.md` 的两道防线，这里补充完整的数据流：

```
Owner 撤销用户权限
  ↓ （云端完成撤销）
被撤销用户的 App：
  ├─ 第一道防线：下次进入前台时
  │     AppLifecycleObserver.ON_START
  │       → SyncPermissionsUseCase → GET /devices/my
  │       → 对比 Room isValid，发现变化
  │       → markInvalid(deviceId)
  │       → Room Flow 触发 DeviceListViewModel 刷新
  │       → WebView 收到 onDevicesLoaded，卡片显示"权限已撤销"
  │
  └─ 第二道防线：操作时
        用户点击 Unlock → RequestCipherUseCase → POST /crypto/encrypt
          → 服务端返回 403（errorCode: 4031）
          → DeviceRepository.markInvalid(deviceId)
          → Room Flow 触发刷新
          → NFC 流程中止，提示"您已失去该设备操作权限"
```

---

## 6. 离线横幅 UI 规范

横幅是持久性 UI 元素，不是 Toast，通过 WebView 回调控制：

| 事件 | 调用 JS 方法 | 前端行为 |
| :--- | :--- | :--- |
| 检测到无网络 | `window.onNetworkStatusChanged({"online": false})` | 顶部显示橙色横幅 |
| 网络恢复 | `window.onNetworkStatusChanged({"online": true})` | 横幅淡出消失，自动刷新数据 |

横幅文案：**"网络已断开 · 当前显示缓存数据"**，右侧"刷新"按钮。

---

## 7. 边界场景速查

| 场景 | 触发时机 | 处理策略 | UI 表现 |
| :--- | :--- | :--- | :--- |
| 启动时无网络 + 有缓存 | App 冷启动 | 展示缓存，禁用操作按钮 | 顶部离线横幅 |
| 启动时无网络 + 无缓存 | 首次安装/清数据 | 无数据可展示 | 全屏无网络提示+重试 |
| 浏览中断网 | 页面刷新/列表加载 | 静默降级缓存 | 顶部横幅，不打断 |
| 开锁中断网（步骤3+） | 请求云端密文时 | 中止流程，写入 pending_reports | 错误提示，进度重置 |
| Token 过期（使用中） | 任意接口返回 401 | 静默刷新，失败则强退 | 无感知 / 弹登录页 |
| 账号在新设备登录（旧设备被踢） | /auth/validate 返回 401 | 强制退出 | 提示"账号已在新设备登录" |
| 设备权限被撤销 | 前台刷新 / 操作 403 | 更新缓存，禁用操作 | 红色权限标记 |
| 本地缓存设备已被云端删除 | 后续登录对账 | isValid=false | "设备已移除"标记 |
| NFC 操作时 App 切后台 | onPause 触发 | 协程取消，NFC 断开 | 操作取消，提示重试 |
| NFC 操作时 NFC 被关闭 | 广播 STATE_OFF | 同上 | "NFC 已关闭，操作取消" |
| NFC 读到错误设备 | 步骤1 deviceId 不匹配 | 中止，不写队列 | "检测到的设备与所选不符" |
| pending_reports 超 72h 未消费 | WorkManager 检查 | 标记 expired | 无感知（后台静默） |
