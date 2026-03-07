# NAC1080 安卓 App 架构设计文档 · 导航索引

> 本文档已重构为多模块独立文件，位于 `docs/architecture/` 目录下。  
> 每个文件覆盖一个独立功能模块，可由不同 Agent 或开发人员并行认领实现。

---

## 模块文件一览

| 文件 | 模块名称 | 核心内容 | 优先级 |
| :--- | :--- | :--- | :--- |
| [`00-overview.md`](./architecture/00-overview.md) | **总览** | 架构理念、技术选型、目录结构、开发顺序 | 必读 |
| [`01-startup.md`](./architecture/01-startup.md) | **启动与路由** | SplashActivity 决策树、NFC 检测、Token 验证、路由跳转 | P0 |
| [`02-auth.md`](./architecture/02-auth.md) | **认证模块** | 登录/登出/修改密码、Token 生命周期、换机吊销、强退机制 | P0 |
| [`03-nfc-core.md`](./architecture/03-nfc-core.md) | **NFC 核心流程** | 五步开/关锁协议、进度状态机、WakeLock、异常处理 | P0 |
| [`04-device.md`](./architecture/04-device.md) | **设备管理** | 设备列表缓存、搜索、NFC 绑定新设备、设备详情、解绑 | P1 |
| [`05-permission.md`](./architecture/05-permission.md) | **权限管理** | 邀请用户、撤销授权、两道防线权限感知刷新 | P1 |
| [`06-settings.md`](./architecture/06-settings.md) | **设置页** | 用户偏好（在线模式/震动/NFC灵敏度）、账号注销 | P2 |
| [`07-webview-bridge.md`](./architecture/07-webview-bridge.md) | **WebView 桥接层** | 全量 JS→Native 接口 + Native→JS 回调契约 | P0 |
| [`08-storage.md`](./architecture/08-storage.md) | **本地存储层** | Room 三表设计、DataStore Proto 字段、缓存策略 | P0（基础设施）|
| [`09-network.md`](./architecture/09-network.md) | **网络层** | Retrofit 全量接口、OkHttp 配置、AuthInterceptor、错误码映射 | P0（基础设施）|
| [`10-exception.md`](./architecture/10-exception.md) | **异常与边界处理** | 断网三场景、pending_reports 队列消费、Token 失效链路 | P1 |
| [`11-security.md`](./architecture/11-security.md) | **安全加固** | 证书固定（Certificate Pinning）、Keystore Token 加密 | P1 |
| [`12-di.md`](./architecture/12-di.md) | **依赖注入** | Hilt 五个模块、Fake/Real 切换策略、完整依赖图 | P0（基础设施）|

---

## 协作分工建议

| 方向 | 推荐认领的文件 | 说明 |
| :--- | :--- | :--- |
| **基础设施搭建** | 08、09、12 | 先建好骨架（存储 + 网络 + DI），其他模块依赖它 |
| **启动与认证** | 01、02 | 完成后整个 App 才能跑起来 |
| **NFC 核心** | 03 | 最复杂的模块，建议最有经验的成员认领 |
| **业务功能** | 04、05、06 | 设备管理、权限、设置，可并行开发 |
| **前端对接** | 07 | WebView 桥接层，需要和前端 React 同学共同确认接口 |
| **稳定性保障** | 10、11 | 异常处理和安全加固，贯穿多个模块，建议最后一个阶段统一验收 |

---

## 快速导航

- 想了解整体架构思路 → [`00-overview.md`](./architecture/00-overview.md)
- 想实现登录功能 → [`02-auth.md`](./architecture/02-auth.md)
- 想实现开锁功能 → [`03-nfc-core.md`](./architecture/03-nfc-core.md)
- 想接入前端页面 → [`07-webview-bridge.md`](./architecture/07-webview-bridge.md)
- 想设计数据库 → [`08-storage.md`](./architecture/08-storage.md)
- 想处理断网场景 → [`10-exception.md`](./architecture/10-exception.md)
