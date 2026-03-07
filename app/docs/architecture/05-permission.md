# 05 · 权限管理模块：邀请用户 · 撤销授权 · 权限感知刷新

> **模块边界**：设备授权用户的增删、权限状态的主动感知（前台刷新 + 操作时403兜底）。  
> **依赖模块**：`08-storage`（Room `authorized_user_cache`、`device_cache.isValid`）、`09-network`（权限相关 API）  
> **被依赖**：`04-device`（设备详情页调用邀请/撤销）、`07-webview-bridge`（事件调用）

---

## 1. 模块职责

| 职责 | 说明 |
| :--- | :--- |
| 邀请用户 | Owner 通过手机号邀请，云端触发 SMS，被邀请方注册后自动获得权限 |
| 撤销授权 | Owner 删除已授权用户，被撤销方立即失去操作权限 |
| 权限感知 · 第一道防线 | App 每次回到前台时，静默拉取权限列表，更新 Room 缓存 |
| 权限感知 · 第二道防线 | 操作时云端返回 403，实时更新 isValid 并禁用 UI |

---

## 2. 数据模型

### AuthorizedUser（`domain/model/AuthorizedUser.kt`）
| 字段 | 类型 | 说明 |
| :--- | :--- | :--- |
| `userId` | String | 被授权用户 ID |
| `deviceId` | String | 关联的设备 ID |
| `name` | String | 用户名 |
| `phone` | String | 手机号（脱敏展示） |
| `role` | enum（Owner / Guest） | Owner 显示"Default"标签，不可删除 |

---

## 3. UseCase 清单

### 3.1 InviteUserUseCase（`domain/usecase/permission/InviteUserUseCase.kt`）

**输入**：`deviceId: String, phone: String`  
**输出**：`Result<Unit>`

**执行步骤**：
1. 格式校验：手机号 11 位纯数字，不能是自己的手机号
2. 检查是否已在授权列表中（本地缓存查重，避免重复邀请）
3. 调用 `DeviceRepository.inviteUser(deviceId, phone)` → POST `/devices/{deviceId}/invite`
4. 云端自动触发 SMS 短信（后端逻辑，App 无需处理）
5. 成功后刷新授权用户列表缓存

**失败情况**：
- 用户不存在（手机号未注册）→ 云端返回 `404`，提示"该手机号尚未注册"
- 已在授权列表 → 提示"该用户已有权限"
- 非 Owner 操作 → 云端返回 `403`，提示"只有设备 Owner 才能邀请用户"

---

### 3.2 RevokeUserAccessUseCase（`domain/usecase/permission/RevokeUserAccessUseCase.kt`）

**输入**：`deviceId: String, userId: String`  
**输出**：`Result<Unit>`

**执行步骤**：
1. 安全校验：不允许撤销 Owner 自身（本地校验，防止误调用）
2. 调用 `DeviceRepository.revokeUser(deviceId, userId)` → DELETE `/devices/{deviceId}/users/{userId}`
3. 云端撤销成功后：
   - 从 Room `authorized_user_cache` 中删除该用户记录
   - 刷新 DeviceDetailViewModel 的 UiState

---

### 3.3 SyncPermissionsUseCase（`domain/usecase/permission/SyncPermissionsUseCase.kt`）

> **第一道防线**的核心 UseCase，App 进入前台时调用。

**输入**：无  
**输出**：`Result<List<PermissionChange>>`（有变化的设备列表）

**执行步骤**：
1. 调用 `DeviceRepository.fetchPermissionSnapshot()` → GET `/devices/my`（只返回账号下设备ID + isValid 状态，轻量接口）
2. 将返回的云端状态与 Room `device_cache.isValid` 逐一对比
3. 发现差异（云端 isValid=false 但本地 isValid=true）：
   - 更新 Room `isValid = false`
   - 将该设备ID加入返回的变更列表
4. ViewModel 根据变更列表刷新对应设备的 UI

---

## 4. 权限感知两道防线实现

### 第一道防线：前台刷新

**触发位置**：`AppLifecycleObserver`（监听 `ProcessLifecycleOwner ON_START`）

```
ON_START 事件触发
  ↓
SyncPermissionsUseCase.execute()
  ↓ 有权限变化
DeviceListViewModel.notifyPermissionChanges(changedDeviceIds)
  ↓
设备列表中对应设备刷新 isValid=false 状态 → UI 显示红色"权限已撤销"标记
  ↓ 无变化 / 网络失败（静默忽略）
不做任何操作
```

### 第二道防线：操作时 403 兜底

**触发位置**：`RequestCipherUseCase`（NFC 五步流程第三步）

```
云端加密接口返回 403
  ↓
DeviceRepository.markInvalid(deviceId)
  ↓
DeviceListViewModel + DeviceDetailViewModel 自动刷新（Room Flow 触发）
  ↓
UI：禁用操作按钮，显示"您已失去该设备操作权限"提示
  ↓
NFC 流程中止
```

---

## 5. Repository 接口补充（DeviceRepository 权限相关方法）

| 方法 | 参数 | 返回 | 说明 |
| :--- | :--- | :--- | :--- |
| `inviteUser(deviceId, phone)` | String, String | `Unit` | POST `/devices/{deviceId}/invite` |
| `revokeUser(deviceId, userId)` | String, String | `Unit` | DELETE `/devices/{deviceId}/users/{userId}` |
| `fetchPermissionSnapshot()` | — | `List<PermissionSnapshot>` | GET `/devices/my`（轻量权限快照） |
| `getAuthorizedUsers(deviceId)` | String | `List<AuthorizedUser>` | GET `/devices/{deviceId}/users` |

```
data class PermissionSnapshot(
    val deviceId: String,
    val isValid: Boolean
)
```

---

## 6. UI 权限状态表现

| 状态 | 设备列表卡片 | 设备详情页 | 操作按钮 |
| :--- | :--- | :--- | :--- |
| 权限正常 | 正常蓝色显示 | 正常显示 | Unlock / Lock 可点击 |
| 权限已撤销（已感知） | 红色"权限已撤销"角标 | 操作区显示警告文案 | 全部按钮禁用 |
| 操作时感知（403） | 403 后立即更新为已撤销状态 | 同上 | 操作失败后禁用 |

---

## 7. 边界与异常

| 场景 | 处理 |
| :--- | :--- |
| 邀请时网络断开 | 提示"邀请发送失败，请检查网络后重试"；不写 pending_reports（邀请失败无需补报） |
| 撤销时网络断开 | 提示"撤销失败，请联网后操作"；本地 Room 不提前修改（等真正成功再修改） |
| 前台刷新时网络失败 | 静默忽略，等下次前台刷新或操作时的 403 兜底 |
| 同一设备被多次撤销相同用户 | 云端幂等处理（已撤销的 userId 再撤销返回 200，不报错） |
| Guest 尝试邀请/撤销其他用户 | 本地按角色校验，不发请求；提示"只有 Owner 才能管理授权" |
