# 04 · 设备管理模块：列表 · 搜索 · 添加绑定 · 详情

> **模块边界**：设备列表展示、搜索过滤、新设备 NFC 绑定、设备详情查看与删除。  
> **依赖模块**：`08-storage`（Room 缓存）、`09-network`（DeviceRepository 远端请求）、`03-nfc-core`（NFC 扫描绑定）  
> **被依赖**：`05-permission`（设备详情中的授权管理）、`07-webview-bridge`（事件调用）

---

## 1. 模块职责

| 职责 | 说明 |
| :--- | :--- |
| 设备列表展示 | 展示当前账号下所有绑定设备，支持离线缓存展示 |
| 搜索过滤 | 按设备昵称关键词实时过滤（纯本地逻辑，不调接口） |
| 快捷开锁 | 设备列表卡片直接触发开锁（通知 HomeViewModel） |
| 新设备绑定 | 输入昵称 → NFC 扫描读取设备 ID → 上传云端建立关联 |
| 设备详情 | 展示设备基本信息 + 授权用户列表 |
| 解绑设备 | Owner 删除 User-Device 绑定关系 |
| 设备有效性标记 | `isValid=false` 的设备在列表中灰色置灰展示，禁用操作 |

---

## 2. 数据模型

### Device（`domain/model/Device.kt`）
| 字段 | 类型 | 说明 |
| :--- | :--- | :--- |
| `deviceId` | String | 硬件唯一 ID（NFC 读取） |
| `serialNo` | String | 设备序列号（展示用） |
| `nickname` | String | 用户自定义昵称 |
| `isValid` | Boolean | 账号对此设备是否仍有权限 |
| `lastSyncAt` | Long | 最后一次云端同步时间戳（毫秒） |

---

## 3. ViewModel：DeviceListViewModel

**文件**：`presentation/devices/DeviceListViewModel.kt`

### UiState
```
data class DeviceListUiState(
    val allDevices: List<Device> = emptyList(),   // Room 缓存的完整列表
    val filteredDevices: List<Device> = emptyList(), // 搜索过滤后的列表（默认等于 allDevices）
    val searchKeyword: String = "",
    val isLoading: Boolean = false,
    val errorMessage: String? = null,
    val isOffline: Boolean = false               // 是否处于离线展示模式
)
```

### 方法
| 方法 | 触发来源 | 职责 |
| :--- | :--- | :--- |
| `loadDevices()` | 页面进入 / 下拉刷新 | 先展示 Room 缓存，后台拉云端刷新 |
| `onSearchKeywordChanged(keyword)` | AndroidBridge | 实时过滤 `allDevices`，更新 `filteredDevices` |
| `onUnlockFromList(deviceId, deviceName)` | AndroidBridge 快捷开锁 | 设置 HomeViewModel 的当前设备，触发开锁流程 |

### 核心逻辑：Cache-Then-Network
```
loadDevices() {
  1. 立即从 Room 读取缓存 → 更新 allDevices（用户不等待白屏）
  2. 后台发起网络请求
  3a. 成功 → 更新 Room → 更新 allDevices（用户看到最新数据）
  3b. 失败 → isOffline = true，保持缓存展示，UI 显示离线横幅
}
```

---

## 4. ViewModel：AddDeviceViewModel

**文件**：`presentation/adddevice/AddDeviceViewModel.kt`

### UiState
```
data class AddDeviceUiState(
    val nickname: String = "",
    val nfcScanState: NfcScanState = NfcScanState.Idle,
    val errorMessage: String? = null
)

sealed class NfcScanState {
    object Idle       : NfcScanState()    // 初始状态，等待用户点击"扫描"
    object Scanning   : NfcScanState()    // 等待 NFC 感应中，显示引导动画
    object Connecting : NfcScanState()    // 已感应到 Tag，建立连接中
    data class Success(val deviceId: String) : NfcScanState()  // 绑定成功
    data class Error(val message: String)   : NfcScanState()   // 失败，可重试
}
```

### 方法
| 方法 | 触发来源 | 职责 |
| :--- | :--- | :--- |
| `onNicknameChanged(text)` | AndroidBridge | 更新昵称输入 |
| `startNfcScan(nickname)` | AndroidBridge | 校验昵称非空 → 切换到 Scanning 状态，等待 Tag |
| `onNfcTagDiscovered(tag)` | MainActivity（NFC Tag 回调） | 调用 AddDeviceUseCase 执行绑定 |
| `cancelScan()` | 用户取消 | 重置状态为 Idle |

---

## 5. ViewModel：DeviceDetailViewModel

**文件**：`presentation/devicedetail/DeviceDetailViewModel.kt`

### UiState
```
data class DeviceDetailUiState(
    val device: Device? = null,
    val authorizedUsers: List<AuthorizedUser> = emptyList(),
    val isLoading: Boolean = false,
    val revokeDialogTarget: AuthorizedUser? = null,  // 非 null 时显示撤销确认弹窗
    val errorMessage: String? = null,
    val inviteStatus: InviteStatus = InviteStatus.Idle
)

sealed class InviteStatus {
    object Idle : InviteStatus()
    object Sending : InviteStatus()
    data class Success(val phone: String) : InviteStatus()
    data class Error(val message: String) : InviteStatus()
}
```

### 方法
| 方法 | 触发来源 | 职责 |
| :--- | :--- | :--- |
| `loadDetail(deviceId)` | 页面进入 | GetDeviceDetailUseCase |
| `onInviteClicked(deviceId, phone)` | AndroidBridge | InviteUserUseCase（见 05-permission.md） |
| `onRevokeClicked(userId)` | AndroidBridge | 设置 `revokeDialogTarget`，显示确认弹窗 |
| `confirmRevoke()` | AndroidBridge | RevokeUserAccessUseCase（见 05-permission.md） |
| `dismissRevokeDialog()` | AndroidBridge | 清除 `revokeDialogTarget` |
| `removeDevice(deviceId)` | AndroidBridge | RemoveDeviceUseCase |

---

## 6. UseCase 清单

### 6.1 GetDeviceListUseCase（`domain/usecase/device/`）
**输入**：无  
**输出**：`Flow<List<Device>>`（响应式，Room 变化时自动推送）

**逻辑**：
- 返回 `DeviceRepository.observeDevices()`（Room 的 Flow）
- ViewModel 收集此 Flow，网络刷新后 Room 更新，Flow 自动触发 UI 刷新

---

### 6.2 SearchDevicesUseCase（`domain/usecase/device/`）
**输入**：`devices: List<Device>, keyword: String`  
**输出**：`List<Device>`

**逻辑**（纯本地，不调接口）：
- `keyword.isBlank()` → 返回原列表
- 否则过滤 `device.nickname.contains(keyword, ignoreCase = true)`

---

### 6.3 AddDeviceUseCase（`domain/usecase/device/`）
**输入**：`nickname: String, tag: Tag`（NFC Tag 对象）  
**输出**：`Result<Device>`

**执行步骤**：
1. 通过 `NfcRepository.connect(tag)` 建立 IsoDep 连接
2. 发送"读取设备ID"指令（非开锁操作位，是绑定专用指令）→ 获取 `deviceId`
3. 关闭 NFC 连接
4. 调用 `DeviceRepository.addDevice(deviceId, nickname)` → POST `/devices/bind`
5. 云端返回完整 Device 信息 → 写入 Room 缓存
6. 返回 `Device`

**失败情况**：
- NFC 读取失败 → 提示"NFC 读取失败，请重新靠近"
- 设备已被绑定（云端返回 `409`）→ 提示"该设备已被其他账号绑定"

---

### 6.4 RemoveDeviceUseCase（`domain/usecase/device/`）
**输入**：`deviceId: String`  
**输出**：`Result<Unit>`

**执行步骤**：
1. 调用 `DeviceRepository.removeDevice(deviceId)` → DELETE `/devices/{deviceId}`
2. 成功后从 Room 删除该设备的 `device_cache` 和 `authorized_user_cache` 记录
3. 刷新设备列表

---

### 6.5 GetDeviceDetailUseCase（`domain/usecase/device/`）
**输入**：`deviceId: String`  
**输出**：`Result<Pair<Device, List<AuthorizedUser>>>`

**执行步骤**：
1. 从 Room 读取 `device_cache` 中的设备信息（立即返回缓存）
2. 同时发起网络请求刷新 `authorized_user_cache`（授权用户列表变化频繁，不用旧缓存）
3. 返回最新 Device + 最新 AuthorizedUsers

---

## 7. Repository 接口（DeviceRepository）

**文件**：`data/repository/DeviceRepository.kt`

| 方法 | 参数 | 返回 | 说明 |
| :--- | :--- | :--- | :--- |
| `observeDevices()` | — | `Flow<List<Device>>` | Room 的 Flow，自动响应变化 |
| `fetchAndCacheDevices()` | — | `Unit` | GET `/devices`，结果写入 Room |
| `addDevice(deviceId, nickname)` | String, String | `Device` | POST `/devices/bind` |
| `removeDevice(deviceId)` | String | `Unit` | DELETE `/devices/{deviceId}` |
| `getDeviceDetail(deviceId)` | String | `Pair<Device, List<AuthorizedUser>>` | GET `/devices/{deviceId}` |
| `clearLocalCache()` | — | `Unit` | 清空 Room 所有设备缓存（登出时调用） |
| `markInvalid(deviceId)` | String | `Unit` | 设置 Room 中 `isValid=false` |

---

## 8. 设备有效性与缓存时效

| 状态 | 触发条件 | UI 表现 |
| :--- | :--- | :--- |
| 正常（isValid=true） | 默认 | 正常显示，Unlock/Manage 可点击 |
| 权限已撤销（isValid=false） | 403 或前台刷新对账 | 红色标记"权限已撤销"，按钮禁用 |
| 数据较旧（lastSyncAt > 24h） | 时间戳检查 | 旁边显示"⚠ 数据较旧" |
| 数据过期（lastSyncAt > 7天） | 时间戳检查 | 强制要求联网刷新，禁用操作按钮 |

---

## 9. 边界与异常

| 场景 | 处理 |
| :--- | :--- |
| 设备列表为空（新账号） | 展示"你还没有绑定任何设备"引导卡，底部导航 Add 按钮高亮提示 |
| NFC 扫描到已绑定的同款锁（同 deviceId） | `addDevice` 返回当前设备信息，提示"该设备已在你的列表中" |
| 删除设备时网络断开 | 提示"网络不可用，请联网后操作"（删除必须联网，不允许离线删除） |
| 设备详情页加载失败 | 展示 Room 缓存的基本信息 + 授权用户列表显示"加载失败，下拉刷新" |
