# 03 · NFC 核心模块：开/关锁五步协议 · 进度状态机 · 异常处理

> **模块边界**：从用户点击 Unlock/Lock 到操作最终完成（或取消/失败）的完整流程。  
> **依赖模块**：`07-webview-bridge`（进度回调）、`09-network`（云端加密请求）、`08-storage`（pending_reports 写入）  
> **被依赖**：`07-webview-bridge`（桥接调用入口）

---

## 1. 模块职责

| 职责 | 说明 |
| :--- | :--- |
| 五步开/关锁协议 | 发操作位 → 收随机数 → 云端加密 → 发密文 → 收结果 |
| 进度状态机 | 每步更新进度百分比和 UI 文案，驱动底部弹窗 |
| NFC 连接管理 | IsoDep 连接建立、保持、关闭，NFC ForegroundDispatch |
| 协程作用域管理 | 整个流程在可取消的协程作用域中执行 |
| WakeLock 管理 | 操作中申请屏幕常亮，操作结束释放 |
| 设备 ID 校验 | 防止 NFC 读到错误的锁 |
| 待上报队列写入 | 操作中断时写入 `pending_reports`（见 `10-exception.md`） |

---

## 2. 五步协议详解

```
用户点击 Unlock / Lock
       ↓
   HomeViewModel.onUnlockClicked(deviceId) / onLockClicked(deviceId)
       ↓
   启动协程作用域（可取消），申请 WakeLock，显示底部弹窗
       ↓
[步骤1] SendIntentionBitUseCase
   - 通过 NfcRepository 向 NAC1080 发送操作位
     Unlock → 0x01，Lock → 0x02
   - NFC 握手时读取硬件返回的 deviceId
   - 与用户选中的 deviceId 对比，不匹配 → 中止（"设备不符"）
   - 进度：20%，文案："Sending command to lock..."
       ↓
[步骤2] ReceiveChallengeUseCase
   - 等待 NAC1080 回传随机数 (Challenge)，存入协程局部变量（不落盘）
   - 进度：40%，文案："Waiting for hardware challenge..."
       ↓
[步骤3] RequestCipherUseCase
   - 携带 {deviceId + challenge + operationType} 调用云端加密接口
   - 云端同步记录"处理中"日志（后端逻辑，App 无需关心）
   - 进度：65%，文案："Verifying with cloud..."
       ↓
[步骤4] SendCipherToLockUseCase
   - 将云端返回的密文通过 NFC 发送给 NAC1080
   - 进度：85%，文案："Sending cipher to lock..."
       ↓
[步骤5] ReceiveLockResultUseCase
   - 等待 NAC1080 返回执行结果
   - 关锁时额外包含机械检测结果（门栓是否到位）
   - 进度：100%，文案："Executing..."
       ↓
   ReportOperationResultUseCase（异步，不阻塞 UI）
   - 将最终结果回传云端，补全日志状态
       ↓
   更新 UiState.Success / Error，关闭底部弹窗，释放 WakeLock
```

---

## 3. ViewModel：HomeViewModel

**文件**：`presentation/home/HomeViewModel.kt`

### UiState
```
data class HomeUiState(
    val deviceName: String = "",         // 当前操作设备名（从设备列表传入）
    val isNfcEnabled: Boolean = true,    // NFC 硬件是否可用
    val operationState: OperationState = OperationState.Idle
)

sealed class OperationState {
    object Idle       : OperationState()   // 无操作，按钮可点击
    data class InProgress(
        val progress: Int,                 // 0-100
        val stepText: String              // UI 文案
    ) : OperationState()
    data class Success(val message: String) : OperationState()
    data class Error(val message: String, val isRetryable: Boolean) : OperationState()
}
```

### 方法
| 方法 | 触发来源 | 职责 |
| :--- | :--- | :--- |
| `onUnlockClicked(deviceId)` | AndroidBridge | 启动开锁协程流程 |
| `onLockClicked(deviceId)` | AndroidBridge | 启动关锁协程流程 |
| `onOperationCancelled()` | AndroidBridge / 用户点 Cancel | 取消当前协程，释放 WakeLock，重置状态 |
| `setNfcEnabled(Boolean)` | NfcStateBroadcastReceiver | 更新 NFC 可用状态 |
| `setCurrentDevice(deviceId, deviceName)` | DeviceListViewModel | 设置当前操作的设备 |

---

## 4. UseCase 清单（开/关锁流程）

所有 UseCase 位于 `domain/usecase/lock/`

### 4.1 SendIntentionBitUseCase
**输入**：`deviceId: String, operationType: OperationType`（enum Unlock/Lock）  
**输出**：`Result<String>`（NFC 握手返回的硬件 deviceId，用于校验）

**关键逻辑**：
- 通过 `NfcRepository.sendCommand(intentionByte)` 发送操作位
- 解析 NFC 响应，提取硬件的 `deviceId`
- 比对入参 `deviceId`，不一致时 throw `DeviceMismatchException`

---

### 4.2 ReceiveChallengeUseCase
**输入**：无（依赖 NFC 连接状态）  
**输出**：`Result<ByteArray>`（随机数 Challenge 的原始字节）

**关键逻辑**：
- 调用 `NfcRepository.readResponse()` 读取 NAC1080 返回的随机数
- 随机数**只存在协程局部变量中**，绝对不落盘（防重放攻击）
- 设置单次读写超时：5 秒

---

### 4.3 RequestCipherUseCase
**输入**：`deviceId: String, challenge: ByteArray, operationType: OperationType`  
**输出**：`Result<ByteArray>`（云端返回的加密密文）

**关键逻辑**：
- 调用 `CryptoRepository.requestCipher(deviceId, challenge, operationType)` → POST `/crypto/encrypt`
- 网络超时：10 秒（在 OkHttp 层配置）
- 网络断开 → throw `NetworkException`（由 HomeViewModel 处理，写入 pending_reports）

---

### 4.4 SendCipherToLockUseCase
**输入**：`cipher: ByteArray`  
**输出**：`Result<Unit>`

**关键逻辑**：
- 调用 `NfcRepository.sendCommand(cipher)` 将密文发送给 NAC1080
- 超时：5 秒

---

### 4.5 ReceiveLockResultUseCase
**输入**：`operationType: OperationType`  
**输出**：`Result<LockResult>`

```
data class LockResult(
    val success: Boolean,
    val errorCode: Int?,        // 失败时的错误码
    val mechanicalOk: Boolean?  // 关锁时：机械检测是否到位，null 表示开锁操作
)
```

**关键逻辑**：
- 调用 `NfcRepository.readResponse()` 获取 NAC1080 最终结果
- 解析返回的状态字节：
  - `0x00` → 成功
  - `0x01` → 密文比对失败（安全拒绝）
  - `0x02` → 机械检测失败（门未关严）
  - 其他 → 通用失败

---

### 4.6 ReportOperationResultUseCase
**输入**：`operationId: String, deviceId: String, result: LockResult`  
**输出**：`Result<Unit>`（不影响 UI，失败写入 pending_reports）

**关键逻辑**：
- 调用 `CryptoRepository.reportResult(operationId, result)` → POST `/crypto/report`
- 网络失败时将操作结果写入 Room 的 `pending_reports` 表（见 `10-exception.md`）
- **异步执行，不 await，不阻塞主流程**

---

## 5. Repository 接口（NfcRepository / CryptoRepository）

### NfcRepository（`data/repository/NfcRepository.kt`）
| 方法 | 参数 | 返回 | 说明 |
| :--- | :--- | :--- | :--- |
| `connect(tag: Tag)` | NFC Tag | `IsoDep` | 建立 IsoDep 连接，设置超时 |
| `sendCommand(apdu: ByteArray)` | ByteArray | `ByteArray` | 发送 APDU 指令，返回响应 |
| `readResponse()` | — | `ByteArray` | 读取硬件主动推送的响应 |
| `disconnect()` | — | `Unit` | 关闭 IsoDep 连接 |

### CryptoRepository（`data/repository/CryptoRepository.kt`）
| 方法 | 参数 | 返回 | 说明 |
| :--- | :--- | :--- | :--- |
| `requestCipher(deviceId, challenge, opType)` | String, ByteArray, OperationType | `ByteArray` | POST `/crypto/encrypt` |
| `reportResult(operationId, result)` | String, LockResult | `Unit` | POST `/crypto/report` |

---

## 6. NFC 连接生命周期管理

### 6.1 ForegroundDispatch（前台调度）
NFC 在 Android 上必须通过 ForegroundDispatch 或 NfcAdapter.enableReaderMode 接收 Tag。

- `MainActivity.onResume()` → `nfcAdapter.enableForegroundDispatch(...)` 注册过滤器
- `MainActivity.onPause()` → `nfcAdapter.disableForegroundDispatch(this)` 注销
- 收到 `ACTION_TECH_DISCOVERED` Intent → 通过 `HomeViewModel` 或 `AddDeviceViewModel` 处理（根据当前页面状态判断）

### 6.2 连接保持
- 整个五步流程在**同一个 IsoDep 实例**上完成，中间不断开
- 如果 `IsoDep.isConnected == false`（系统强制断开），`sendCommand` 会抛出 `IOException`，触发流程中止

### 6.3 协程作用域
```
// HomeViewModel 中
private var lockJob: Job? = null

fun onUnlockClicked(deviceId: String) {
    lockJob = viewModelScope.launch {
        // 申请 WakeLock
        // 执行五步 UseCase，每步更新 UiState
        // 完成后释放 WakeLock
    }
}

fun onOperationCancelled() {
    lockJob?.cancel()
    // 释放 WakeLock，重置 UiState
}
```

---

## 7. WakeLock 管理

- **申请时机**：底部弹窗显示时（步骤1开始前）
- **级别**：`PowerManager.SCREEN_DIM_WAKE_LOCK`（屏幕调暗但不熄灭）
- **释放时机**：
  - 操作成功完成
  - 用户点击 Cancel
  - 任意步骤发生异常（协程取消时在 `finally` 块中释放）
- **防泄漏**：用 `use {}` 或 `try/finally` 确保释放，即使协程取消也不泄漏

---

## 8. 进度状态映射

| 步骤 | 进度 | UI 文案 | 异常后 UI |
| :--- | :--- | :--- | :--- |
| 发送操作位 | 20% | Sending command to lock... | 设备不符 / NFC 未感应到 |
| 等待随机数 | 40% | Waiting for hardware challenge... | NFC 断开 / 超时 |
| 请求云端密文 | 65% | Verifying with cloud... | 网络超时 / 权限已撤销(403) |
| 发送密文 | 85% | Sending cipher to lock... | NFC 断开 |
| 等待执行结果 | 100% | Executing... | 比对失败 / 机械卡死 |

---

## 9. 异常处理策略

| 异常类型 | 触发步骤 | 处理方式 | UI 表现 |
| :--- | :--- | :--- | :--- |
| `DeviceMismatchException` | 步骤1 | 中止，不写 pending_reports | "检测到的设备与所选设备不符" |
| NFC `IOException` | 任意步骤 | 中止，步骤3后写 pending_reports | "NFC 信号中断，请重新靠近" |
| NFC 超时（5s） | 步骤2/4/5 | 同上 | "NFC 响应超时，请重试" |
| 网络超时（10s） | 步骤3 | 中止，写 pending_reports | "网络超时，操作已取消" |
| 网络断开 | 步骤3 | 中止，写 pending_reports | "网络中断，操作已取消" |
| `403 Forbidden` | 步骤3 | 中止，更新 Room `isValid=false` | "您已失去该设备操作权限" |
| 密文比对失败（0x01） | 步骤5 | 中止，reportResult 上报失败 | "验证失败，请重试" |
| 机械检测失败（0x02） | 步骤5 | 中止，reportResult 上报失败 | "关锁失败：门未完全关闭" |
| 用户取消 | 任意时刻 | 取消协程，步骤3后写 pending_reports | 弹窗关闭，进度重置 |
| NFC 被关闭（广播） | 操作中途 | 同 NFC IOException | "NFC 已关闭，操作取消" |
| App 切后台（onPause） | 操作中途 | ForegroundDispatch 注销 → IOException | 同 NFC IOException |
