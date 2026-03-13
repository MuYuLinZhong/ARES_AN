# NAC1080 安卓 App 架构设计文档 · 导航索引

> 本文档已重构为多模块独立文件，位于 `docs/architecture/` 目录下。  
> 每个文件覆盖一个独立功能模块，可由不同 Agent 或开发人员并行认领实现。  
> **三阶段开发**：按 [phases-overview.md](./phases-overview.md) 推进 Phase 1（硬件调试）→ Phase 2（云端交互）→ Phase 3（端到端验证）。

---

## 三阶段开发导航（必读）

| 阶段 | 目标 | 文档 |
| :--- | :--- | :--- |
| **Phase 1** | 硬件设备调试：本地密钥、直接开/关锁，不涉及登录与云端 | [`phases-overview.md`](./phases-overview.md) §2 |
| **Phase 2** | 云端交互：认证、信息流、存储、完整五步协议 | [`phases-overview.md`](./phases-overview.md) §3 |
| **Phase 3** | 端到端验证：断网、Token 失效、权限、审计 | [`phases-overview.md`](./phases-overview.md) §4 |

各模块文档（01～12）中均标注了 **Phase 1 / 2 / 3** 的参与程度及 stub/todo 实现指引。

---

## 模块文件一览

| 文件 | 模块名称 | 核心内容 | Phase 1 | Phase 2 | Phase 3 |
| :--- | :--- | :--- | :--- | :--- | :--- |
| [`00-overview.md`](./architecture/00-overview.md) | **总览** | 架构理念、技术选型、目录结构、三阶段开发顺序 | 必读 | 必读 | 必读 |
| [`01-startup.md`](./architecture/01-startup.md) | **启动与路由** | SplashActivity 决策树、NFC 检测、Token 验证、路由跳转 | 简化 | 完整 | 完整 |
| [`02-auth.md`](./architecture/02-auth.md) | **认证模块** | 登录/登出/修改密码、Token 生命周期、换机吊销、强退机制 | 不参与 | 完整 | 完整 |
| [`03-nfc-core.md`](./architecture/03-nfc-core.md) | **NFC 核心流程** | 五步开/关锁协议、进度状态机、WakeLock、异常处理 | 本地加密 | 云端加密 | 完整 |
| [`04-device.md`](./architecture/04-device.md) | **设备管理** | 设备列表缓存、搜索、NFC 绑定新设备、设备详情、解绑 | 本地设备 | 完整 | 完整 |
| [`05-permission.md`](./architecture/05-permission.md) | **权限管理** | 邀请用户、撤销授权、两道防线权限感知刷新 | 不参与 | 完整 | 完整 |
| [`06-settings.md`](./architecture/06-settings.md) | **设置页** | 用户偏好（在线模式/震动/NFC灵敏度）、账号注销 | 简化 | 完整 | 完整 |
| [`07-webview-bridge.md`](./architecture/07-webview-bridge.md) | **WebView 桥接层** | 全量 JS→Native 接口 + Native→JS 回调契约 | 子集 | 全量 | 全量 |
| [`08-storage.md`](./architecture/08-storage.md) | **本地存储层** | Room 三表设计、DataStore Proto 字段、缓存策略 | 简化 | 完整 | 完整 |
| [`09-network.md`](./architecture/09-network.md) | **网络层** | Retrofit 全量接口、OkHttp 配置、AuthInterceptor、错误码映射 | stub | 完整 | 完整 |
| [`10-exception.md`](./architecture/10-exception.md) | **异常与边界处理** | 断网三场景、pending_reports 队列消费、Token 失效链路 | 简化 | 完整 | 完整 |
| [`11-security.md`](./architecture/11-security.md) | **安全加固** | 证书固定（Certificate Pinning）、Keystore Token 加密 | 本地密钥 | 完整 | 完整 |
| [`12-di.md`](./architecture/12-di.md) | **依赖注入** | Hilt 五个模块、Fake/Real 切换策略、完整依赖图 | Phase1 绑定 | Phase2 绑定 | Phase3 绑定 |

---

## 协作分工建议（按阶段）

### Phase 1：硬件调试

| 方向 | 推荐认领的文件 | 说明 |
| :--- | :--- | :--- |
| **Phase1 骨架** | 08（简化）、12 | 本地存储 + DI，绑定 Phase1 Repository |
| **启动简化** | 01 | 仅 NFC 检测 → 直接进首页 |
| **NFC 核心（本地加密）** | 03 | 五步协议，步骤3 用本地密钥加密 |
| **设备与桥接** | 04（简化）、07（子集） | 本地设备列表 + 开锁/关锁桥接 |
| **设置简化** | 06 | 仅震动、NFC 灵敏度 |

### Phase 2：云端交互

| 方向 | 推荐认领的文件 | 说明 |
| :--- | :--- | :--- |
| **基础设施完整** | 08、09、11 | Token 存储、网络、证书固定 |
| **启动与认证** | 01、02 | Token 路由、登录/登出 |
| **NFC 云端加密** | 03 | 步骤3 改为云端 RequestCipher |
| **业务功能** | 04、05、06 | 设备、权限、设置完整实现 |
| **桥接全量** | 07 | 全量 JS↔Native 接口 |

### Phase 3：端到端验证

| 方向 | 推荐认领的文件 | 说明 |
| :--- | :--- | :--- |
| **稳定性保障** | 10、11 | 断网、pending_reports、Token 失效、安全加固 |
| **整体联调** | 全部 | 端到端业务流程验收 |

---

## 快速导航

- **三阶段开发总览** → [`phases-overview.md`](./phases-overview.md)
- 想了解整体架构思路 → [`00-overview.md`](./architecture/00-overview.md)
- Phase 1 调设备 → [`03-nfc-core.md`](./architecture/03-nfc-core.md)（本地加密版）
- Phase 2 实现登录 → [`02-auth.md`](./architecture/02-auth.md)
- 想实现开锁功能 → [`03-nfc-core.md`](./architecture/03-nfc-core.md)
- 想接入前端页面 → [`07-webview-bridge.md`](./architecture/07-webview-bridge.md)
- 想设计数据库 → [`08-storage.md`](./architecture/08-storage.md)
- 想处理断网场景 → [`10-exception.md`](./architecture/10-exception.md)
