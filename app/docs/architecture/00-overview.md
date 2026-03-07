# 00 · 总览：架构理念 · 技术选型 · 目录结构

> **本文件职责**：全局视角。阅读此文件可理解整个系统的分层逻辑、各模块边界和协作约定。  
> 每个子模块文件（01～12）都可独立阅读并独立实现，本文件是它们的"索引与公约"。

---

## 1. 项目背景

NAC1080 是一款**无源 NFC 智能锁**。App 的核心职责：

1. 承载现有 React/TSX WebView UI（不重写前端）
2. 通过 Android NFC API 与锁具通信，完成"先发指令→硬件出题→云端加密→硬件验证"的五步开/关锁协议
3. 管理设备绑定、用户授权、账号认证

---

## 2. 整体分层（四层积木）

```
┌──────────────────────────────────────────────────────┐
│  UI 层                                                │
│  WebView（React/TSX）+ JavascriptInterface 桥         │
│  职责：只显示状态，把用户操作事件转发给 ViewModel      │
├──────────────────────────────────────────────────────┤
│  Presentation 层                                      │
│  ViewModel × 6  +  AndroidBridge（桥接器）            │
│  职责：持有 UiState（StateFlow），调用 UseCase，驱动 UI │
├──────────────────────────────────────────────────────┤
│  Domain 层                                            │
│  UseCase × N  +  Model（纯数据类）                    │
│  职责：业务规则，不依赖 Android 框架，可独立单元测试   │
├──────────────────────────────────────────────────────┤
│  Data 层                                              │
│  Repository 接口 + Fake 实现 + Remote/NFC 实现        │
│  职责：数据来源统一入口，先假数据跑通，后换真接口      │
└──────────────────────────────────────────────────────┘
```

**核心原则：数据单向流动 → 仓库 → UseCase → ViewModel → UI，绝不反向。**

---

## 3. 技术选型速查

| 关注点 | 选型 | 说明 |
| :--- | :--- | :--- |
| UI 渲染 | WebView + React/TSX | 现有前端不重写，原生做壳 |
| 原生语言 | Kotlin | 官方推荐 |
| 状态管理 | StateFlow | 协程原生，比 LiveData 更适合异步 NFC 流程 |
| 依赖注入 | Hilt | Google 官方推荐，注解简洁 |
| 网络 | Retrofit + OkHttp | 事实标准，易 Mock，拦截器灵活 |
| 本地 DB | Room | 设备列表离线缓存 |
| 键值存储 | DataStore（Proto） | 替代 SharedPreferences，Token 加密存储 |
| NFC | Android NFC API（IsoDep） | 支持 ISO 14443-4，适配 NAC1080 |
| JS ↔ Native | WebView JavascriptInterface | WebView 内事件通知原生 |
| 异步 | Kotlin Coroutines + Flow | 天然适合多步骤 NFC 串行流程 |
| 安全存储 | Android Keystore | Token 加密密钥硬件级保护 |
| 后台任务 | WorkManager | 待上报队列消费 |

---

## 4. 模块文件索引

| 文件 | 模块 | 负责人 / Agent | 依赖模块 |
| :--- | :--- | :--- | :--- |
| `01-startup.md` | 启动与路由 | — | 02-auth, 08-storage |
| `02-auth.md` | 认证（登录/Token） | — | 08-storage, 09-network |
| `03-nfc-core.md` | NFC 开/关锁核心流程 | — | 07-bridge, 09-network |
| `04-device.md` | 设备管理 | — | 08-storage, 09-network |
| `05-permission.md` | 权限邀请与撤销 | — | 09-network, 08-storage |
| `06-settings.md` | 设置页与用户偏好 | — | 08-storage |
| `07-webview-bridge.md` | WebView JS↔Native 桥 | — | 所有 ViewModel |
| `08-storage.md` | 本地存储（Room+DataStore） | — | — （基础层）|
| `09-network.md` | 网络层（Retrofit+拦截器） | — | 02-auth |
| `10-exception.md` | 异常与边界处理 | — | 全部模块 |
| `11-security.md` | 安全加固 | — | 08-storage, 09-network |
| `12-di.md` | 依赖注入（Hilt） | — | 全部模块 |

---

## 5. 目录结构规划

```
app/src/main/java/com/example/all/
│
├── ui/
│   └── MainActivity.kt              # WebView 壳 + JavascriptInterface 注册
│
├── bridge/
│   └── AndroidBridge.kt             # 所有 JS→Native 方法入口，分发给对应 ViewModel
│
├── presentation/
│   ├── auth/          AuthViewModel.kt
│   ├── home/          HomeViewModel.kt
│   ├── devices/       DeviceListViewModel.kt
│   ├── adddevice/     AddDeviceViewModel.kt
│   ├── devicedetail/  DeviceDetailViewModel.kt
│   └── settings/      SettingsViewModel.kt
│
├── domain/
│   ├── model/
│   │   ├── User.kt
│   │   ├── Device.kt
│   │   ├── AuthorizedUser.kt
│   │   ├── LockOperation.kt
│   │   └── UserPreferences.kt
│   └── usecase/
│       ├── auth/       LoginUseCase, LogoutUseCase, UpdatePasswordUseCase
│       ├── lock/       SendIntentionBitUseCase, ReceiveChallengeUseCase,
│       │               RequestCipherUseCase, SendCipherToLockUseCase,
│       │               ReceiveLockResultUseCase, ReportOperationResultUseCase
│       ├── device/     GetDeviceListUseCase, SearchDevicesUseCase,
│       │               AddDeviceUseCase, RemoveDeviceUseCase, GetDeviceDetailUseCase
│       ├── permission/ InviteUserUseCase, RevokeUserAccessUseCase
│       └── settings/   GetUserPreferencesUseCase, SaveUserPreferencesUseCase
│
├── data/
│   ├── repository/    （接口定义）
│   │   ├── AuthRepository.kt
│   │   ├── DeviceRepository.kt
│   │   ├── NfcRepository.kt
│   │   ├── CryptoRepository.kt
│   │   └── PreferencesRepository.kt
│   ├── fake/          （假数据实现，第一阶段使用）
│   ├── remote/        （真实网络实现）
│   ├── nfc/           NfcRepositoryImpl.kt
│   └── local/
│       ├── AppDatabase.kt
│       ├── DeviceDao.kt
│       ├── AuthorizedUserDao.kt
│       ├── PendingReportDao.kt
│       └── DataStorePreferencesRepository.kt
│
└── di/
    ├── NetworkModule.kt
    ├── DatabaseModule.kt
    ├── RepositoryModule.kt
    ├── NfcModule.kt
    └── PreferencesModule.kt
```

---

## 6. 通用 UiState 三态约定

所有 ViewModel 都遵守同一套状态模型，让 WebView 端的处理逻辑统一：

```
sealed class UiState<out T> {
    object Loading : UiState<Nothing>()            // 处理中，显示加载动效
    data class Success<T>(val data: T)             // 成功，展示数据
    data class Error(val message: String,          // 失败，展示错误提示
                     val code: Int = 0)
}
```

---

## 7. 两阶段实现策略（先假后真）

| 阶段 | 说明 | 改动范围 |
| :--- | :--- | :--- |
| 第一阶段（假数据） | 所有 Repository 用 Fake 实现，返回硬写数据。目标：整个 MVVM 流程跑通，进度条状态机走完 | 只改 `RepositoryModule` 绑定 |
| 第二阶段（真数据） | 逐个将 FakeRepository 换成 RemoteRepository + NfcRepositoryImpl | 只改 `RepositoryModule` 绑定，其他层零改动 |

---

## 8. 开发推进顺序建议

| 阶段 | 目标 | 关键任务 |
| :--- | :--- | :--- |
| P0 | 架构骨架 | 目录 + 接口定义 + Hilt 配置 + FakeRepository |
| P1 | 认证流程 | AuthViewModel + LoginUseCase + WebView 桥登录事件 |
| P2 | 设备列表 | DeviceListViewModel + 搜索过滤 + Room 缓存 |
| P3 | 开/关锁核心 | HomeViewModel + 6个锁操作 UseCase + 进度状态机 |
| P4 | 授权管理 | DeviceDetailViewModel + 邀请/撤销 + 权限感知 |
| P5 | 设置页 | SettingsViewModel + DataStore 读写 |
| P6 | 接入真实 API | 逐个替换 FakeRepository → RemoteRepository |
| P7 | 接入真实 NFC | FakeNfcRepository → NfcRepositoryImpl，联调硬件 |
