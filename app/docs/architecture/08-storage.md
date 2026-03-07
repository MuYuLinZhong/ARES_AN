# 08 · 本地存储模块：Room 表设计 · DataStore · 缓存策略

> **模块边界**：所有持久化数据的结构设计和读写策略，是其他所有模块的基础设施。  
> **被依赖**：所有需要本地数据的模块（01/02/04/05/06/10）

---

## 1. 存储体系概览

| 存储方案 | 用途 | 加密 |
| :--- | :--- | :--- |
| **DataStore（Proto）** | Token、用户偏好、标识符 | Token 字段加密（Keystore），偏好明文 |
| **Room 数据库** | 设备列表缓存、授权用户缓存、待上报队列 | 明文（Token 等敏感数据不进 Room） |
| **内存（ViewModel StateFlow）** | NFC 操作中的随机数 Challenge、密文、进度 | 不落盘 |

---

## 2. Room 数据库

**文件**：`data/local/AppDatabase.kt`  
**数据库名**：`nac1080_db`

### 2.1 表：device_cache

**文件**：`data/local/entity/DeviceEntity.kt` + `data/local/DeviceDao.kt`

| 字段 | 类型 | 约束 | 说明 |
| :--- | :--- | :--- | :--- |
| `deviceId` | TEXT | PRIMARY KEY | 硬件唯一 ID |
| `nickname` | TEXT | NOT NULL | 用户自定义昵称 |
| `serialNo` | TEXT | NOT NULL | 序列号 |
| `isValid` | INTEGER | NOT NULL, DEFAULT 1 | 1=权限有效，0=权限已撤销 |
| `lastSyncAt` | INTEGER | NOT NULL | 最后同步时间戳（ms） |

**DAO 方法**：
| 方法 | SQL | 说明 |
| :--- | :--- | :--- |
| `observeAll()` | `SELECT * FROM device_cache ORDER BY nickname` | Flow，自动推送变化 |
| `getById(deviceId)` | `SELECT * WHERE deviceId = ?` | 单个设备 |
| `upsertAll(devices)` | `INSERT OR REPLACE` | 批量更新（网络刷新后调用） |
| `markInvalid(deviceId)` | `UPDATE SET isValid=0 WHERE deviceId=?` | 权限撤销时调用 |
| `deleteById(deviceId)` | `DELETE WHERE deviceId=?` | 解绑设备时调用 |
| `deleteAll()` | `DELETE FROM device_cache` | 登出/注销时清空 |

---

### 2.2 表：authorized_user_cache

**文件**：`data/local/entity/AuthorizedUserEntity.kt` + `data/local/AuthorizedUserDao.kt`

| 字段 | 类型 | 约束 | 说明 |
| :--- | :--- | :--- | :--- |
| `id` | INTEGER | PRIMARY KEY AUTOINCREMENT | 自增主键 |
| `userId` | TEXT | NOT NULL | 被授权用户 ID |
| `deviceId` | TEXT | NOT NULL | 关联设备 ID（外键逻辑） |
| `name` | TEXT | NOT NULL | 用户名 |
| `phone` | TEXT | NOT NULL | 手机号（存脱敏后的值） |
| `role` | TEXT | NOT NULL | "Owner" 或 "Guest" |
| `lastSyncAt` | INTEGER | NOT NULL | 最后同步时间戳 |

**索引**：在 `(deviceId)` 上建立索引，加快按设备查询。

**DAO 方法**：
| 方法 | 说明 |
| :--- | :--- |
| `observeByDevice(deviceId)` | Flow，某设备授权用户列表 |
| `replaceForDevice(deviceId, users)` | 整体替换某设备的授权用户（先 DELETE 再 INSERT） |
| `deleteByDevice(deviceId)` | 解绑设备时清除相关授权 |
| `deleteAll()` | 登出/注销时清空 |

---

### 2.3 表：pending_reports

**文件**：`data/local/entity/PendingReportEntity.kt` + `data/local/PendingReportDao.kt`

| 字段 | 类型 | 约束 | 说明 |
| :--- | :--- | :--- | :--- |
| `operationId` | TEXT | PRIMARY KEY | 唯一操作ID（由 HomeViewModel 生成 UUID） |
| `deviceId` | TEXT | NOT NULL | 操作的设备 ID |
| `operationType` | TEXT | NOT NULL | "Unlock" 或 "Lock" |
| `result` | TEXT | NOT NULL | "Cancelled" / "NetworkError" / "NfcError" |
| `failedAtStep` | INTEGER | NOT NULL | 失败时所处步骤（1-5） |
| `createdAt` | INTEGER | NOT NULL | 写入时间戳（ms） |
| `retryCount` | INTEGER | NOT NULL, DEFAULT 0 | 已重试次数 |
| `status` | TEXT | NOT NULL, DEFAULT 'pending' | "pending" / "sent" / "expired" |

**DAO 方法**：
| 方法 | 说明 |
| :--- | :--- |
| `insert(report)` | 中止时写入 |
| `getPending()` | 查询所有 status='pending' 的记录 |
| `markSent(operationId)` | 上报成功后删除或标记 |
| `markExpired(operationId)` | 超过72小时标记过期 |
| `deleteAll()` | 登出/注销时清空 |

---

## 3. DataStore（Proto DataStore）

**文件**：`data/local/DataStorePreferencesRepository.kt`  
**Proto 文件**：`app/src/main/proto/user_prefs.proto`

### 3.1 Proto 字段定义

```protobuf
message UserPrefs {
  bytes encrypted_access_token  = 1;   // 加密后的 AccessToken 字节
  bytes encrypted_refresh_token = 2;   // 加密后的 RefreshToken 字节
  bytes token_iv                = 3;   // AES-GCM 加密用的 IV
  bool  online_mode_enabled     = 4;   // 在线模式开关
  bool  vibration_enabled       = 5;   // 震动反馈开关
  string nfc_sensitivity        = 6;   // "High" / "Medium" / "Low"
  string last_logged_in_user_id = 7;   // 上次登录的用户ID（用于后续登录快速匹配缓存）
}
```

> **说明**：Token 使用 Android Keystore 生成的 AES-GCM 密钥加密，`encrypted_*_token` 存密文字节，`token_iv` 存每次加密生成的随机 IV（IV 明文存储是安全的，密钥在 Keystore 中）。加密细节见 `11-security.md`。

### 3.2 DataStore 读写规范

- **读取**：通过 `dataStore.data` 返回 `Flow<UserPrefs>`，ViewModel 通过 `collectAsState` 响应变化
- **写取 Token**：每次写入前生成新的随机 IV，加密后连同 IV 一起写入
- **清除 Token**：将 `encrypted_access_token` 和 `encrypted_refresh_token` 设为空字节数组，`token_iv` 清空

---

## 4. 数据安全分级

| 数据类型 | 存储位置 | 是否加密 | 理由 |
| :--- | :--- | :--- | :--- |
| AccessToken / RefreshToken | DataStore | ✅ AES-GCM + Keystore | 核心凭证，泄漏即账号被控制 |
| NFC 随机数 Challenge | 内存（协程变量） | 不落盘 | 使用后即销毁，防重放攻击 |
| NFC 密文 | 内存（协程变量） | 不落盘 | 同上 |
| 设备缓存 | Room 明文 | ❌ | 非敏感，deviceId 和昵称不构成安全风险 |
| 授权用户缓存 | Room 明文 | ❌ | 手机号脱敏存储（只存后四位明文） |
| 用户偏好 | DataStore 明文 | ❌ | 非敏感配置 |

---

## 5. 缓存策略

### 5.1 Cache-Then-Network（设备列表）
```
进入设备列表页
  ↓
立即读取 Room device_cache → 展示给用户（0等待）
  ↓ 同时后台
发起网络请求 GET /devices
  成功 → upsertAll(newDevices) → Room Flow 触发 UI 更新
  失败 → 保持现有缓存展示 + isOffline=true 横幅
```

### 5.2 Network-First（设备详情授权用户）
```
进入设备详情页
  ↓
先读 Room device_cache 展示设备基本信息
  ↓ 同时
发起网络请求 GET /devices/{id}/users（授权用户变化频繁，优先最新数据）
  成功 → replaceForDevice() → 展示最新授权用户列表
  失败 → 读 Room authorized_user_cache 展示 + 错误提示
```

### 5.3 缓存有效期规则
| `lastSyncAt` 距今 | 行为 |
| :--- | :--- |
| < 24 小时 | 正常展示，无额外提示 |
| 24 小时 ~ 7 天 | 设备名旁显示"⚠ 数据较旧" |
| > 7 天 | 禁用开锁操作按钮，提示"数据已过期，请联网刷新" |

---

## 6. Room Schema 迁移策略

- 每次改动 Room 实体，**必须**在 `AppDatabase` 中增加 `MIGRATION_X_Y`
- 迁移原则：**宁可清空缓存重建，也不让 App 崩溃**
  - 非必要字段新增：`ALTER TABLE ADD COLUMN ... DEFAULT ...`
  - 表结构大变：`DROP TABLE + CREATE TABLE + 清空旧缓存`（缓存可从云端重拉，不是核心资产）
- `fallbackToDestructiveMigration()` 作为最终兜底（只在开发测试阶段允许，生产不可用）
