# 07 · WebView 桥接层：JS↔Native 完整接口契约

> **模块边界**：WebView 内 React 页面与 Android 原生层之间的全量通信协议定义。  
> **依赖模块**：所有 ViewModel（调用目标）  
> **被依赖**：前端 React/TSX 代码（调用方）、`MainActivity`（注册桥接器）

---

## Phase 1：子集接口（开锁/关锁/设备/偏好）

### 职责范围

Phase 1 仅暴露与硬件调试直接相关的接口，其余接口在 `AndroidBridge.kt` 中提供 stub 骨架（记录日志 + `TODO("Phase 2")`）。

### 消息分发架构图

```mermaid
flowchart LR
    subgraph webview [WebView React/TSX]
        JS[JavaScript\nwindow.AndroidBridge.xxx]
        CB[JS 回调\nwindow.onXxx]
    end

    subgraph native [Android Native]
        AB[AndroidBridge.kt\n@JavascriptInterface]
        subgraph vms [ViewModels]
            HVM[HomeViewModel]
            DVM[DeviceListViewModel]
            ADVM[AddDeviceViewModel]
            SVM[SettingsViewModel]
            AVM[AuthViewModel\nPhase2+]
            DDVM[DeviceDetailViewModel\nPhase2+]
        end
    end

    JS -- "JSON String" --> AB
    AB -- "分发" --> HVM
    AB -- "分发" --> DVM
    AB -- "分发" --> ADVM
    AB -- "分发" --> SVM
    HVM -- "evaluateJavascript" --> CB
    DVM -- "evaluateJavascript" --> CB
    ADVM -- "evaluateJavascript" --> CB
    SVM -- "evaluateJavascript" --> CB
```

### Phase 1 接口清单

#### JS → Native（Phase 1 实现）

| 方法 | 分发目标 | 说明 |
| :--- | :--- | :--- |
| `onUnlockClicked(json)` | `HomeViewModel.onUnlockClicked` | 开锁 |
| `onLockClicked(json)` | `HomeViewModel.onLockClicked` | 关锁 |
| `onOperationCancelled()` | `HomeViewModel.onOperationCancelled` | 取消操作 |
| `onRefreshDeviceList()` | `DeviceListViewModel.loadDevices` | 刷新列表 |
| `onSearchKeywordChanged(json)` | `DeviceListViewModel.onSearchKeywordChanged` | 搜索 |
| `onUnlockFromList(json)` | `DeviceListViewModel.onUnlockFromList` | 列表快捷开锁 |
| `onStartNfcScan(json)` | `AddDeviceViewModel.startNfcScan` | 开始扫描添加 |
| `onCancelNfcScan()` | `AddDeviceViewModel.cancelScan` | 取消扫描 |
| `onToggleVibration(json)` | `SettingsViewModel.onToggleVibration` | 震动开关 |
| `onNfcSensitivityChanged(json)` | `SettingsViewModel.onNfcSensitivityChanged` | NFC 灵敏度 |

#### Native → JS（Phase 1 实现）

| 回调 | 来源 | 说明 |
| :--- | :--- | :--- |
| `window.onLockProgress(json)` | `HomeViewModel` | 进度更新 |
| `window.onLockOperationResult(json)` | `HomeViewModel` | 操作结果 |
| `window.onNfcStatusChanged(json)` | `NfcStateBroadcastReceiver` | NFC 状态 |
| `window.onDevicesLoaded(json)` | `DeviceListViewModel` | 设备列表 |
| `window.onNfcScanStateChanged(json)` | `AddDeviceViewModel` | 扫描状态 |
| `window.onPreferencesSaved(json)` | `SettingsViewModel` | 偏好保存结果 |

### Phase 1 AndroidBridge.kt 骨架

**文件**：`bridge/AndroidBridge.kt`

```kotlin
class AndroidBridge @Inject constructor(
    private val homeViewModel: HomeViewModel,
    private val deviceListViewModel: DeviceListViewModel,
    private val addDeviceViewModel: AddDeviceViewModel,
    private val settingsViewModel: SettingsViewModel,
    // Phase 2+ 注入：private val authViewModel: AuthViewModel,
    // Phase 2+ 注入：private val deviceDetailViewModel: DeviceDetailViewModel,
) {
    private lateinit var webView: WebView
    fun attachWebView(wv: WebView) { webView = wv }

    // ===================== Phase 1 实现 =====================

    @JavascriptInterface
    fun onUnlockClicked(json: String) {
        val payload = json.fromJson<UnlockPayload>()
        mainThread { homeViewModel.onUnlockClicked(payload.deviceId, payload.deviceName) }
    }

    @JavascriptInterface
    fun onLockClicked(json: String) {
        val payload = json.fromJson<LockPayload>()
        mainThread { homeViewModel.onLockClicked(payload.deviceId, payload.deviceName) }
    }

    @JavascriptInterface
    fun onOperationCancelled() {
        mainThread { homeViewModel.onOperationCancelled() }
    }

    @JavascriptInterface
    fun onRefreshDeviceList() {
        mainThread { deviceListViewModel.loadDevices() }
    }

    @JavascriptInterface
    fun onSearchKeywordChanged(json: String) {
        val payload = json.fromJson<SearchPayload>()
        mainThread { deviceListViewModel.onSearchKeywordChanged(payload.keyword) }
    }

    @JavascriptInterface
    fun onUnlockFromList(json: String) {
        val payload = json.fromJson<UnlockPayload>()
        mainThread { deviceListViewModel.onUnlockFromList(payload.deviceId, payload.deviceName) }
    }

    @JavascriptInterface
    fun onStartNfcScan(json: String) {
        val payload = json.fromJson<NfcScanPayload>()
        mainThread { addDeviceViewModel.startNfcScan(payload.nickname) }
    }

    @JavascriptInterface
    fun onCancelNfcScan() {
        mainThread { addDeviceViewModel.cancelScan() }
    }

    @JavascriptInterface
    fun onToggleVibration(json: String) {
        val payload = json.fromJson<TogglePayload>()
        mainThread { settingsViewModel.onToggleVibration(payload.enabled) }
    }

    @JavascriptInterface
    fun onNfcSensitivityChanged(json: String) {
        val payload = json.fromJson<SensitivityPayload>()
        mainThread { settingsViewModel.onNfcSensitivityChanged(payload.level) }
    }

    // ===================== Phase 2 Stub =====================

    @JavascriptInterface
    fun onLoginClicked(json: String) {
        // TODO("Phase 2: authViewModel.login(phone, password)")
        Log.d("Bridge", "onLoginClicked: Phase 2 not implemented")
    }

    @JavascriptInterface
    fun onLogout() {
        // TODO("Phase 2: authViewModel.logout()")
        Log.d("Bridge", "onLogout: Phase 2 not implemented")
        // Phase 1 假流程：直接回调 onLogoutDone（如前端需要测试登出 UI）
        // pushToJs("onLogoutDone", "")
    }

    @JavascriptInterface
    fun onUpdatePasswordClicked(json: String) {
        // TODO("Phase 2: authViewModel.updatePassword(current, new, confirm)")
        Log.d("Bridge", "onUpdatePasswordClicked: Phase 2 not implemented")
    }

    @JavascriptInterface
    fun onDeleteAccountClicked() {
        // TODO("Phase 2: settingsViewModel.onDeleteAccountClicked()")
        Log.d("Bridge", "onDeleteAccountClicked: Phase 2 not implemented")
    }

    @JavascriptInterface
    fun onLoadDeviceDetail(json: String) {
        // TODO("Phase 2: deviceDetailViewModel.loadDetail(deviceId)")
        Log.d("Bridge", "onLoadDeviceDetail: Phase 2 not implemented")
    }

    @JavascriptInterface
    fun onInviteUser(json: String) {
        // TODO("Phase 2: deviceDetailViewModel.onInviteClicked(deviceId, phone)")
        Log.d("Bridge", "onInviteUser: Phase 2 not implemented")
    }

    @JavascriptInterface
    fun onRevokeUser(json: String) {
        // TODO("Phase 2: deviceDetailViewModel.onRevokeClicked(userId)")
        Log.d("Bridge", "onRevokeUser: Phase 2 not implemented")
    }

    @JavascriptInterface
    fun confirmRevokeUser() {
        // TODO("Phase 2: deviceDetailViewModel.confirmRevoke()")
        Log.d("Bridge", "confirmRevokeUser: Phase 2 not implemented")
    }

    @JavascriptInterface
    fun onRemoveDevice(json: String) {
        // TODO("Phase 2: deviceDetailViewModel.removeDevice(deviceId)")
        Log.d("Bridge", "onRemoveDevice: Phase 2 not implemented")
    }

    @JavascriptInterface
    fun onToggleOnlineMode(json: String) {
        // TODO("Phase 2: settingsViewModel.onToggleOnlineMode(enabled)")
        Log.d("Bridge", "onToggleOnlineMode: Phase 2 not implemented")
    }

    // ===================== 通用工具 =====================

    fun pushToJs(callbackName: String, jsonPayload: String) {
        mainThread {
            webView.evaluateJavascript("window.$callbackName($jsonPayload)", null)
        }
    }

    private fun mainThread(block: () -> Unit) {
        Handler(Looper.getMainLooper()).post(block)
    }
}
```

### 验收要点（Phase 1）

- [ ] `onUnlockClicked` / `onLockClicked` 正确触发 HomeViewModel
- [ ] 进度回调 `window.onLockProgress` 前端收到并更新进度条
- [ ] 操作结果 `window.onLockOperationResult` 前端收到
- [ ] 设备列表 `window.onDevicesLoaded` 前端正确渲染
- [ ] NFC 扫描状态 `window.onNfcScanStateChanged` 前端正确响应
- [ ] Phase 2 stub 方法不崩溃，只打日志

---

## Phase 2：全量接口

### 新增接口说明

| 新增 JS→Native | 分发目标 |
| :--- | :--- |
| `onLoginClicked` | `AuthViewModel.login` |
| `onForgotPasswordClicked` | 跳转忘记密码页 |
| `onUpdatePasswordClicked` | `AuthViewModel.updatePassword` |
| `onLogout` | `AuthViewModel.logout` |
| `onDeleteAccountClicked` / `confirmDeleteAccount` | `SettingsViewModel.onDeleteAccountClicked/confirmDeleteAccount` |
| `onLoadDeviceDetail` | `DeviceDetailViewModel.loadDetail` |
| `onRemoveDevice` | `DeviceDetailViewModel.removeDevice` |
| `onInviteUser` | `DeviceDetailViewModel.onInviteClicked` |
| `onRevokeUser` / `confirmRevokeUser` / `dismissRevokeDialog` | `DeviceDetailViewModel` |
| `onToggleOnlineMode` | `SettingsViewModel.onToggleOnlineMode` |

| 新增 Native→JS | 来源 |
| :--- | :--- |
| `window.onLoginResult` | `AuthViewModel` |
| `window.onUpdatePasswordResult` | `AuthViewModel` |
| `window.onLogoutDone` | `AuthViewModel` |
| `window.onForceLogout` | `AuthInterceptor` → 广播 |
| `window.onDeviceDetailLoaded` | `DeviceDetailViewModel` |
| `window.onRemoveDeviceResult` | `DeviceDetailViewModel` |
| `window.onInviteResult` | `DeviceDetailViewModel` |
| `window.onRevokeResult` | `DeviceDetailViewModel` |
| `window.onSettingsLoaded` | `SettingsViewModel` |
| `window.onNetworkStatusChanged` | `NetworkMonitor` |

### JSON 契约（完整）

#### JS → Native 参数

```json
// onLoginClicked
{ "phone": "13800138000", "password": "myPassword123" }

// onUnlockClicked / onLockClicked / onUnlockFromList
{ "deviceId": "NAC1080-A1B2C3", "deviceName": "Office Door" }

// onSearchKeywordChanged
{ "keyword": "office" }

// onStartNfcScan
{ "nickname": "Office Door" }

// onLoadDeviceDetail / onRemoveDevice
{ "deviceId": "NAC1080-A1B2C3" }

// onInviteUser
{ "deviceId": "NAC1080-A1B2C3", "phone": "13900139000" }

// onRevokeUser
{ "deviceId": "NAC1080-A1B2C3", "userId": "user-uuid-xxx" }

// onToggleVibration / onToggleOnlineMode
{ "enabled": true }

// onNfcSensitivityChanged
{ "level": "High" }

// onUpdatePasswordClicked
{ "currentPassword": "old123", "newPassword": "new456789", "confirmPassword": "new456789" }
```

#### Native → JS 回调

```json
// onLockProgress
{ "progress": 65, "stepText": "Verifying with cloud..." }

// onLockOperationResult
{ "success": true, "message": "Door unlocked successfully" }
// 或
{ "success": false, "message": "关锁失败：门未完全关闭", "isRetryable": true }

// onNfcStatusChanged
{ "enabled": false }

// onDevicesLoaded
{
  "devices": [
    { "deviceId": "NAC1080-A1B2C3", "nickname": "Office Door", "serialNo": "SN001",
      "isValid": true, "lastSyncAt": 1700000000000 }
  ],
  "isOffline": false
}

// onNfcScanStateChanged
{ "state": "scanning" }
{ "state": "success", "deviceId": "NAC1080-A1B2C3" }
{ "state": "error", "message": "NFC 读取失败，请重试" }

// onLoginResult
{ "success": true, "user": { "userId": "xxx", "username": "Alex", "phone": "138****8000", "role": "Owner" } }

// onForceLogout
{ "reason": "您的账号已在新设备登录，请重新验证" }

// onDeviceDetailLoaded
{
  "device": { "deviceId": "...", "nickname": "...", "serialNo": "...", "isValid": true },
  "authorizedUsers": [
    { "userId": "u1", "name": "Alex", "phone": "138****8000", "role": "Owner" },
    { "userId": "u2", "name": "Bob",  "phone": "139****9000", "role": "Guest" }
  ]
}

// onSettingsLoaded
{
  "user": { "userId": "xxx", "username": "Alex", "phone": "138****8000", "role": "Owner", "email": "alex@example.com" },
  "preferences": { "onlineModeEnabled": true, "vibrationEnabled": true, "nfcSensitivity": "Medium" }
}

// onNetworkStatusChanged
{ "online": false }
```

### 验收要点（Phase 2）

- [ ] 登录/登出流程：`onLoginClicked` → 云端验证 → `window.onLoginResult`
- [ ] 强制退出：`window.onForceLogout` 前端跳登录页
- [ ] 设备详情：`window.onDeviceDetailLoaded` 含授权用户列表
- [ ] 断网横幅：`window.onNetworkStatusChanged({"online": false})` 前端显示
- [ ] 所有回调在主线程执行，无 WebView 崩溃

---

## Phase 3：无新增接口

Phase 3 主要验证端到端稳定性，桥接接口本身不变。重点验收：

- [ ] `window.onForceLogout` 在 Token 被后台吊销时正确触发
- [ ] `window.onNetworkStatusChanged` 准确反映网络状态变化
- [ ] 快速多次点击 Unlock 不产生竞态（`onOperationCancelled` 机制正常）
