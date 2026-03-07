# 07 · WebView 桥接层：JS↔Native 完整接口契约

> **模块边界**：WebView 内 React 页面与 Android 原生层之间的全量通信协议定义。  
> **依赖模块**：所有 ViewModel（调用目标）  
> **被依赖**：前端 React/TSX 代码（调用方）、`MainActivity`（注册桥接器）

---

## 1. 架构概览

```
React/TSX 页面（WebView 内）
       │
       │  window.AndroidBridge.methodName(jsonString)
       ▼
AndroidBridge.kt（@JavascriptInterface）
       │
       │  分发给对应 ViewModel
       ▼
ViewModel → UseCase → Repository
       │
       │  webView.evaluateJavascript("window.callbackName(jsonString)", null)
       ▼
React 页面接收回调，更新 UI 状态
```

**通信约定**：
- JS → Native：所有参数统一用 **JSON 字符串**传入，便于扩展
- Native → JS：所有回调统一用 **JSON 字符串**传出
- 所有 JSON 字段名使用 **camelCase**
- 回调在 **主线程**执行（`evaluateJavascript` 必须在主线程调用）
- Native 不主动调用 JS，只在 ViewModel UiState 变化时（通过 Flow collect）触发回调

---

## 2. JS → Native 接口完整列表

### 2.1 认证相关

#### `onLoginClicked(json)`
```json
{ "phone": "13800138000", "password": "myPassword123" }
```
分发给：`AuthViewModel.login(phone, password)`

---

#### `onForgotPasswordClicked(json)`
```json
{ "phone": "13800138000" }
```
分发给：触发跳转忘记密码流程（当前阶段可以是打开 Web 页面，取决于产品决策）

---

#### `onUpdatePasswordClicked(json)`
```json
{ "currentPassword": "old123", "newPassword": "new456789", "confirmPassword": "new456789" }
```
分发给：`AuthViewModel.updatePassword(current, new, confirm)`

---

#### `onLogout()`
无参数  
分发给：`AuthViewModel.logout()`

---

#### `onDeleteAccountClicked()`
无参数  
分发给：`SettingsViewModel.onDeleteAccountClicked()`

---

#### `confirmDeleteAccount()`
无参数  
分发给：`SettingsViewModel.confirmDeleteAccount()`

---

### 2.2 开/关锁相关

#### `onUnlockClicked(json)`
```json
{ "deviceId": "NAC1080-A1B2C3", "deviceName": "Office Door" }
```
分发给：`HomeViewModel.onUnlockClicked(deviceId, deviceName)`

---

#### `onLockClicked(json)`
```json
{ "deviceId": "NAC1080-A1B2C3", "deviceName": "Office Door" }
```
分发给：`HomeViewModel.onLockClicked(deviceId, deviceName)`

---

#### `onOperationCancelled()`
无参数  
分发给：`HomeViewModel.onOperationCancelled()`

---

### 2.3 设备管理相关

#### `onRefreshDeviceList()`
无参数  
分发给：`DeviceListViewModel.loadDevices()`

---

#### `onSearchKeywordChanged(json)`
```json
{ "keyword": "office" }
```
分发给：`DeviceListViewModel.onSearchKeywordChanged(keyword)`

---

#### `onUnlockFromList(json)`
```json
{ "deviceId": "NAC1080-A1B2C3", "deviceName": "Office Door" }
```
分发给：`DeviceListViewModel.onUnlockFromList(deviceId, deviceName)`

---

#### `onStartNfcScan(json)`
```json
{ "nickname": "Office Door" }
```
分发给：`AddDeviceViewModel.startNfcScan(nickname)`

---

#### `onCancelNfcScan()`
无参数  
分发给：`AddDeviceViewModel.cancelScan()`

---

#### `onLoadDeviceDetail(json)`
```json
{ "deviceId": "NAC1080-A1B2C3" }
```
分发给：`DeviceDetailViewModel.loadDetail(deviceId)`

---

#### `onRemoveDevice(json)`
```json
{ "deviceId": "NAC1080-A1B2C3" }
```
分发给：`DeviceDetailViewModel.removeDevice(deviceId)`

---

### 2.4 权限管理相关

#### `onInviteUser(json)`
```json
{ "deviceId": "NAC1080-A1B2C3", "phone": "13900139000" }
```
分发给：`DeviceDetailViewModel.onInviteClicked(deviceId, phone)`

---

#### `onRevokeUser(json)`
```json
{ "deviceId": "NAC1080-A1B2C3", "userId": "user-uuid-xxx" }
```
分发给：`DeviceDetailViewModel.onRevokeClicked(userId)`

---

#### `confirmRevokeUser()`
无参数  
分发给：`DeviceDetailViewModel.confirmRevoke()`

---

#### `dismissRevokeDialog()`
无参数  
分发给：`DeviceDetailViewModel.dismissRevokeDialog()`

---

### 2.5 设置相关

#### `onToggleOnlineMode(json)`
```json
{ "enabled": true }
```
分发给：`SettingsViewModel.onToggleOnlineMode(enabled)`

---

#### `onToggleVibration(json)`
```json
{ "enabled": false }
```
分发给：`SettingsViewModel.onToggleVibration(enabled)`

---

#### `onNfcSensitivityChanged(json)`
```json
{ "level": "High" }
```
分发给：`SettingsViewModel.onNfcSensitivityChanged(level)`

---

## 3. Native → JS 回调完整列表

### 3.1 认证回调

#### `window.onLoginResult(json)`
```json
// 成功
{ "success": true, "user": { "userId": "xxx", "username": "Alex", "phone": "138****8000", "role": "Owner" } }
// 失败
{ "success": false, "message": "手机号或密码错误" }
```

---

#### `window.onUpdatePasswordResult(json)`
```json
{ "success": true }
// 或
{ "success": false, "message": "当前密码不正确" }
```

---

#### `window.onLogoutDone()`
无参数。前端收到后跳转到登录页。

---

#### `window.onForceLogout(json)`
```json
{ "reason": "您的账号已在新设备登录，请重新验证" }
```
Token 被吊销时推送，前端强制跳转登录页。

---

### 3.2 开/关锁回调

#### `window.onLockProgress(json)`
```json
{ "progress": 65, "stepText": "Verifying with cloud..." }
```
每个步骤完成时推送，前端更新底部弹窗进度条。

---

#### `window.onLockOperationResult(json)`
```json
// 成功
{ "success": true, "message": "Door unlocked successfully" }
// 失败
{ "success": false, "message": "关锁失败：门未完全关闭", "isRetryable": true }
```

---

#### `window.onNfcStatusChanged(json)`
```json
{ "enabled": false }
```
NFC 被关闭或开启时推送，前端更新 Unlock/Lock 按钮可用状态。

---

### 3.3 设备管理回调

#### `window.onDevicesLoaded(json)`
```json
{
  "devices": [
    { "deviceId": "NAC1080-A1B2C3", "nickname": "Office Door", "serialNo": "SN001", "isValid": true, "lastSyncAt": 1700000000000 }
  ],
  "isOffline": false
}
```

---

#### `window.onNfcScanStateChanged(json)`
```json
// 扫描中
{ "state": "scanning" }
// 成功
{ "state": "success", "deviceId": "NAC1080-A1B2C3" }
// 失败
{ "state": "error", "message": "NFC 读取失败，请重试" }
```

---

#### `window.onDeviceDetailLoaded(json)`
```json
{
  "device": { "deviceId": "...", "nickname": "...", "serialNo": "...", "isValid": true },
  "authorizedUsers": [
    { "userId": "u1", "name": "Alex", "phone": "138****8000", "role": "Owner" },
    { "userId": "u2", "name": "Bob",  "phone": "139****9000", "role": "Guest" }
  ]
}
```

---

#### `window.onRemoveDeviceResult(json)`
```json
{ "success": true }
// 或
{ "success": false, "message": "删除失败，请重试" }
```

---

### 3.4 权限管理回调

#### `window.onInviteResult(json)`
```json
{ "success": true, "message": "邀请已发送，等待对方接受" }
// 或
{ "success": false, "message": "该手机号尚未注册" }
```

---

#### `window.onRevokeResult(json)`
```json
{ "success": true, "revokedUserId": "user-uuid-xxx" }
// 或
{ "success": false, "message": "撤销失败，请重试" }
```

---

### 3.5 设置回调

#### `window.onSettingsLoaded(json)`
```json
{
  "user": { "userId": "xxx", "username": "Alex", "phone": "138****8000", "role": "Owner", "email": "alex@example.com" },
  "preferences": { "onlineModeEnabled": true, "vibrationEnabled": true, "nfcSensitivity": "Medium" }
}
```

---

#### `window.onPreferencesSaved(json)`
```json
{ "success": true }
```

---

## 4. AndroidBridge.kt 实现要点

**文件**：`bridge/AndroidBridge.kt`

### 4.1 注册方式
```
// MainActivity.kt 中
webView.addJavascriptInterface(androidBridge, "AndroidBridge")
```

### 4.2 线程安全
- `@JavascriptInterface` 注解的方法运行在 **JavaBridge 线程**（非主线程）
- 所有 ViewModel 方法调用需切换到主线程：使用 `Handler(Looper.getMainLooper()).post {}` 或 `viewModelScope.launch(Dispatchers.Main)`

### 4.3 JSON 解析
- 使用 `kotlinx.serialization` 或 `Gson` 解析入参 JSON 字符串
- 解析失败时（格式错误）记录日志，不崩溃，向 JS 返回错误响应

### 4.4 回调推送方式
```
// 在 ViewModel 的 UiState collect 中调用
fun pushToJs(callbackName: String, jsonPayload: String) {
    mainHandler.post {
        webView.evaluateJavascript("window.$callbackName($jsonPayload)", null)
    }
}
```

---

## 5. 前端对接约定

前端 React 页面需在初始化时注册所有回调：
```javascript
// 示例
window.onLoginResult = (json) => {
  const result = JSON.parse(json);
  // 处理登录结果
};

window.onLockProgress = (json) => {
  const { progress, stepText } = JSON.parse(json);
  // 更新进度条
};
```

所有调用原生方法前需检查 `window.AndroidBridge` 是否存在（Web 调试模式下可能为空）：
```javascript
if (window.AndroidBridge) {
  window.AndroidBridge.onUnlockClicked(JSON.stringify({ deviceId, deviceName }));
}
```
