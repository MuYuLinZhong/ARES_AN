# 01 · 启动模块：SplashActivity · 路由决策 · NFC 状态监听

> **模块边界**：App 冷启动到"进入第一个业务页面"之间的全部逻辑。  
> **依赖模块**：`02-auth`（读取 Token）、`08-storage`（DataStore）  
> **输出**：路由到登录页 或 首页（HomeActivity/WebView）

---

## 1. 模块职责

| 职责 | 说明 |
| :--- | :--- |
| NFC 硬件检测 | 启动时检测设备是否支持 NFC、NFC 是否已开启 |
| Token 判断与刷新 | 读 DataStore 中的 Token，判断有效性，必要时静默刷新 |
| 路由决策 | 根据上述结果决定跳转登录页还是首页 |
| 待上报队列消费 | 启动时检查并消费 `pending_reports` 队列（不阻塞主流程） |
| NFC 状态广播监听 | 运行中监听用户手动开/关 NFC，实时更新首页按钮状态 |

---

## 2. 启动决策树（完整流程）

```
App 冷启动
  ↓
SplashActivity.onCreate()（原生 Activity，不走 WebView）
  ↓
① NFC 硬件是否存在？
     NfcAdapter.getDefaultAdapter(context) == null
  ↓ 是 → 弹对话框："此设备不支持 NFC，无法使用本应用"
           点击"确认"→ finish() 退出 App
  ↓ 否
② NFC 是否已开启？
     !nfcAdapter.isEnabled
  ↓ 是 → 弹对话框："请先开启 NFC 功能"
           点击"去设置"→ startActivity(Settings.ACTION_NFC_SETTINGS)
           点击"取消"   → finish() 退出 App（不强制，但无法使用）
           用户返回后重新检测
  ↓ 否（NFC 可用）
③ DataStore 中是否有 AccessToken？
  ↓ 无 → 跳转登录页，结束启动流程
  ↓ 有
④ 本地解析 JWT exp 字段，Token 是否已过期？
  ↓ 未过期 → 跳首页（展示缓存数据，后台校验有效性）
  ↓ 已过期
⑤ 是否有 RefreshToken？
  ↓ 无 → 清除所有 Token → 跳登录页
  ↓ 有 → 发起静默 Refresh 请求
         成功 → 更新 DataStore → 跳首页
         失败 → 清除所有 Token 和 Room 缓存 → 跳登录页（提示"登录已过期"）
  ↓
⑥ 后台触发 pending_reports 队列消费（不阻塞启动）
```

**时序要求**：整个决策树需在 **1.5 秒**内完成，避免用户感知卡顿。NFC 检测是同步操作，Token 解析是本地操作，均不耗时；仅 RefreshToken 请求是网络操作（设置 8 秒超时，超时直接跳登录页）。

---

## 3. SplashActivity 实现要点

### 3.1 文件位置
```
ui/SplashActivity.kt
```

### 3.2 核心逻辑说明

- 继承 `AppCompatActivity`，**不使用 WebView**，是纯原生界面
- 设置全屏无 ActionBar 主题（`Theme.App.Splash`）
- 在 `onCreate()` 中用协程执行决策树，超时保护设置为 3 秒（决策树整体），避免白屏过长
- 使用 `lifecycleScope.launch` 保证 Activity 销毁时协程自动取消
- 所有弹窗使用 `AlertDialog.Builder`，在主线程弹出

### 3.3 ViewModel 依赖
`SplashViewModel`（新增）：
- 持有 `StartupUiState`（Checking / GoToLogin / GoToHome / NfcNotAvailable / NfcDisabled）
- 调用 `ValidateTokenUseCase` 判断 Token 有效性
- 调用 `RefreshTokenUseCase` 发起静默刷新

### 3.4 UseCase
| UseCase | 位置 | 职责 |
| :--- | :--- | :--- |
| `ValidateTokenUseCase` | `domain/usecase/auth/` | 读取 Token，本地解析 JWT exp，返回有效/过期/无Token |
| `RefreshTokenUseCase` | `domain/usecase/auth/` | 携带 RefreshToken 调用 `/auth/refresh`，更新 DataStore |
| `ConsumePendingReportsUseCase` | `domain/usecase/report/` | 检查并消费 `pending_reports` 表（调用见 `10-exception.md`） |

---

## 4. NFC 状态运行时监听

### 4.1 监听位置
在 `MainActivity`（WebView 壳）中注册，不在 SplashActivity（SplashActivity 生命周期极短）。

### 4.2 实现方式

```
注册 BroadcastReceiver，Action = NfcAdapter.ACTION_ADAPTER_STATE_CHANGED
监听 EXTRA_ADAPTER_STATE：
  STATE_ON  (3) → NFC 开启 → 通知 HomeViewModel：NFC 可用，恢复按钮
  STATE_OFF (1) → NFC 关闭 → 通知 HomeViewModel：NFC 不可用，禁用 Unlock/Lock 按钮
  STATE_TURNING_ON/OFF → 过渡状态，忽略
```

### 4.3 HomeViewModel 中的 NFC 状态字段
`HomeUiState` 中包含 `isNfcEnabled: Boolean`，WebView 根据此字段控制按钮的可用性。

### 4.4 注册与注销时机
- `onResume()` 注册（与 NFC ForegroundDispatch 一起）
- `onPause()` 注销（系统要求，避免内存泄漏）

---

## 5. App 从后台切回前台时的静默刷新

**触发时机**：`ProcessLifecycleOwner.get().lifecycle` 监听 `Lifecycle.Event.ON_START`

**执行内容**（在 `MainViewModel` 或 `AppLifecycleObserver` 中）：
1. 静默拉取云端设备权限列表，与 Room 缓存对比（见 `05-permission.md`）
2. 刷新 Room 缓存中的 `isValid` 字段
3. 驱动 `DeviceListViewModel` 刷新 UI

**注意**：此操作是"每次回到前台都做"，网络失败时静默忽略，不影响用户操作。

---

## 6. 涉及的数据流

```
SplashActivity
  └─ SplashViewModel
       ├─ ValidateTokenUseCase → DataStore (08-storage)
       ├─ RefreshTokenUseCase  → AuthRepository → /auth/refresh (09-network)
       └─ ConsumePendingReportsUseCase → PendingReportDao (08-storage)

MainActivity
  └─ NfcStateBroadcastReceiver
       └─ HomeViewModel.setNfcEnabled(Boolean)
```

---

## 7. 边界与异常

| 场景 | 处理 |
| :--- | :--- |
| NFC 未开启，用户去系统设置开启后返回 | `onResume()` 重新检测 NFC 状态，若已开启则继续流程 |
| RefreshToken 请求超时（8秒） | 视为刷新失败，清 Token，跳登录页 |
| DataStore 读取异常（极少） | catch IOException，视为无 Token，跳登录页 |
| 决策树整体超过 3 秒 | 强制超时保护，跳登录页（不做任何假设） |
| 运行中 NFC 被关闭且正在执行 NFC 操作 | 中止协程，提示"NFC 已关闭，操作已取消"（具体见 `03-nfc-core.md`） |
