# NAC1080 三阶段开发总览

> **本文档定义**：按阶段推进开发与测试的顶层策略。每个阶段有明确的目标、参与模块和验收标准。  
> 各模块详细文档（01～12）中均标注了所属阶段及 stub/todo 实现指引。

---

## 1. 三阶段目标速查

| 阶段 | 核心目标 | 账户/云端 | 密钥来源 | 开锁流程 |
| :--- | :--- | :--- | :--- | :--- |
| **Phase 1** | 硬件设备调试，调通 NFC 开/关锁 | 不涉及登录注销、不涉及云端 | 本地预存密钥 | 直接下发指令，本地加密 |
| **Phase 2** | 云端交互：认证、信息流、存储 | 完整登录/登出/Token | 云端加密 | 云端加密，五步协议 |
| **Phase 3** | 端到端验证，整体链条打通 | 同上 | 同上 | 同上 + 权限、审计、异常 |

---

## 2. Phase 1：硬件设备调试

### 2.1 目标

- 完成与 NAC1080 硬件的 NFC 通信联调
- 本地存储设备密钥，直接下发开锁/关锁指令
- **不涉及**：账户登录/注销、云端 API、Token、权限管理

### 2.2 参与模块

| 模块 | 参与程度 | 说明 |
| :--- | :--- | :--- |
| `01-startup` | 简化 | 仅 NFC 检测 → 直接进首页，无 Token 判断 |
| `02-auth` | 不参与 | 用 stub 或直接跳过，无登录页 |
| `03-nfc-core` | 核心 | 本地密钥加密，跳过云端，直接五步（1→2→本地加密→4→5） |
| `04-device` | 简化 | 本地硬编码/手动添加设备，不调云端 |
| `05-permission` | 不参与 | 不实现 |
| `06-settings` | 简化 | 仅震动、NFC 灵敏度，无登出/注销 |
| `07-webview-bridge` | 简化 | 仅开锁/关锁、设备选择、设置偏好相关接口 |
| `08-storage` | 简化 | 无 Token，仅 device_cache（本地）、UserPreferences |
| `09-network` | 不参与 | 用 stub，不发起真实请求 |
| `10-exception` | 简化 | 无 pending_reports、无 Token 失效 |
| `11-security` | 简化 | 本地密钥存储（非 Keystore Token 加密） |
| `12-di` | 全部 | 绑定 Phase1 专用 Repository 实现 |

### 2.3 Phase 1 开锁流程（本地加密版）

```
用户点击 Unlock / Lock
  ↓
[步骤1] 发送操作位 → NAC1080 返回 deviceId + 随机数
[步骤2] 接收随机数
[步骤3] 本地加密：用本地预存密钥对随机数加密（替代云端 RequestCipher）
[步骤4] 发送密文给 NAC1080
[步骤5] 接收执行结果
  ↓
更新 UI，无云端上报
```

### 2.4 Phase 1 验收标准

- [ ] NFC 能稳定连接 NAC1080
- [ ] 发送操作位后能收到硬件随机数
- [ ] 本地加密密文能被硬件验证通过
- [ ] 开锁/关锁机械动作正常执行
- [ ] 进度条、取消、异常提示 UI 正常

---

## 3. Phase 2：云端交互

### 3.1 目标

- 接入云端 API：认证、设备管理、加密、上报
- 实现登录/登出、Token 管理、静默刷新
- 实现设备列表云端同步、添加/解绑设备
- 开锁流程改为云端加密（五步协议完整版）

### 3.2 参与模块

| 模块 | 参与程度 | 说明 |
| :--- | :--- | :--- |
| `01-startup` | 完整 | Token 判断、Refresh、路由到登录/首页 |
| `02-auth` | 完整 | 登录/登出/修改密码/Token 生命周期 |
| `03-nfc-core` | 完整 | 步骤3 改为 RequestCipherUseCase 调用云端 |
| `04-device` | 完整 | 云端拉取、添加、解绑、Cache-Then-Network |
| `05-permission` | 完整 | 邀请、撤销、权限感知 |
| `06-settings` | 完整 | 登出、注销、在线模式 |
| `07-webview-bridge` | 完整 | 全量接口 |
| `08-storage` | 完整 | Token 加密存储、pending_reports |
| `09-network` | 完整 | Retrofit、AuthInterceptor、证书固定 |
| `10-exception` | 完整 | pending_reports、Token 失效、断网分级 |
| `11-security` | 完整 | Keystore Token 加密、证书固定 |
| `12-di` | 完整 | 绑定 RemoteRepository、NfcRepositoryImpl |

### 3.3 Phase 2 验收标准

- [ ] 登录/登出流程正常
- [ ] Token 刷新、401 处理正常
- [ ] 设备列表从云端拉取并缓存
- [ ] 添加设备（NFC + 云端绑定）正常
- [ ] 开锁流程完整五步（云端加密）正常
- [ ] 邀请/撤销权限正常

---

## 4. Phase 3：端到端验证

### 4.1 目标

- 验证完整业务链条
- 断网、Token 失效、权限撤销等边界场景
- 待上报队列、审计日志、安全加固

### 4.2 参与模块

所有模块完整参与，无 stub。

### 4.3 Phase 3 验收标准

- [ ] 断网三场景（启动/浏览/开锁中）处理正确
- [ ] Token 失效、换机吊销链路正确
- [ ] 权限撤销两道防线生效
- [ ] pending_reports 队列消费正常
- [ ] 证书固定、Token 加密等安全项通过
- [ ] 端到端业务流程无遗漏

---

## 5. 阶段切换与开关

### 5.1 构建/运行开关

建议通过 BuildConfig 或 Gradle 属性控制当前阶段：

```kotlin
// BuildConfig 或 常量
object PhaseConfig {
    const val CURRENT_PHASE = 1  // 1 | 2 | 3
    val isPhase1 get() = CURRENT_PHASE == 1
    val isPhase2OrAbove get() = CURRENT_PHASE >= 2
    val isPhase3 get() = CURRENT_PHASE == 3
}
```

### 5.2 DI 切换

```kotlin
// RepositoryModule.kt
@Binds @Singleton
abstract fun bindCryptoRepository(
    impl: @JvmSuppressWildcards
    if (PhaseConfig.isPhase1) FakeLocalCryptoRepository::class
    else RemoteCryptoRepository::class
): CryptoRepository
```

或通过 `@Qualifier` 区分 Phase1/Phase2 实现，在模块中按阶段选择绑定。

### 5.3 路由与 UI 开关

- Phase 1：Splash 检测 NFC 后直接 `startActivity(HomeActivity)`，无登录页
- Phase 2/3：Splash 检测 Token → 有则首页，无则登录页

---

## 6. 各模块阶段标注索引

| 模块 | Phase 1 | Phase 2 | Phase 3 |
| :--- | :--- | :--- | :--- |
| 01-startup | 简化路由 | 完整 Token 路由 | 完整 |
| 02-auth | 不参与（stub） | 完整 | 完整 |
| 03-nfc-core | 本地加密 | 云端加密 | 完整 |
| 04-device | 本地设备 | 云端同步 | 完整 |
| 05-permission | 不参与 | 完整 | 完整 |
| 06-settings | 简化（无登出） | 完整 | 完整 |
| 07-webview-bridge | 子集接口 | 全量 | 全量 |
| 08-storage | 无 Token | 完整 | 完整 |
| 09-network | stub | 完整 | 完整 |
| 10-exception | 简化 | 完整 | 完整 |
| 11-security | 本地密钥 | 完整 | 完整 |
| 12-di | Phase1 绑定 | Phase2 绑定 | Phase3 绑定 |
