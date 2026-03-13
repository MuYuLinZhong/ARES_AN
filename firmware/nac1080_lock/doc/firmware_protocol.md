# ARES Lock Firmware Protocol — Challenge-Response

## 概述

基于 SmAcK `smack_exchange` 数据点机制实现挑战应答认证协议。
固件运行在 NAC1080 芯片上，通过 NFC 与 Android App（`SmackLockRepository` + `mailboxApi`）通信。

核心安全特性：
- 硬件 RNG 生成一次性挑战值
- AES-128 对称密钥验证
- 挑战值用后即废（防重放）
- 认证状态一次性消费（一次认证 = 一次操作）

---

## 数据点规格

### 基础信息（只读，明文）

| ID | 名称 | 类型 | 大小 | 说明 |
|:---|:-----|:-----|:-----|:-----|
| 0x0001 | FIRMWARE_VERSION | UINT32 | 4 | 固件版本（0x00020000 = v2.0.0） |
| 0x0003 | FIRMWARE_NAME | STRING | 31 | 固件名称 "ARES_Lock_CR" |
| 0x0004 | UID | UINT64 | 8 | 芯片唯一 ID |
| 0x0005 | LOCK_ID | UINT64 | 8 | 锁 ID（默认等于 UID） |
| 0x0012 | CHARGE_PERCENT | UINT8 | 1 | 充能百分比 0-100 |
| 0x0020 | STATUS | UINT8 | 1 | 锁状态（见下表） |

STATUS 值：
- `0x00` UNLOCKED — 已开锁
- `0x01` LOCKED — 已锁定
- `0x02` IN_PROGRESS — 操作中
- `0x07` ERROR — 错误

### 挑战应答认证

| ID | 名称 | 类型 | 大小 | R/W | 回调 | 说明 |
|:---|:-----|:-----|:-----|:----|:-----|:-----|
| 0xF100 | CHALLENGE | ARRAY | 16 | R | notify_tx | 每次读取生成新随机数 |
| 0xF101 | RESPONSE | ARRAY | 16 | W | notify_rx | 验证 AES(key, challenge) |
| 0xF102 | AUTH_RESULT | UINT8 | 1 | R | — | 0x00=失败, 0x01=成功 |

### 操作指令

| ID | 名称 | 类型 | 大小 | R/W | 回调 | 说明 |
|:---|:-----|:-----|:-----|:----|:-----|:-----|
| 0xF103 | OPERATION | UINT8 | 1 | W | notify_rx | 0x01=开锁, 0x02=关锁 |

---

## 认证流程

```
App                                  Firmware (NAC1080)
───                                  ──────────────────

1. READ LOCK_ID (0x0005)   ──NFC──>  返回 dp_lock_id
   App 查本地数据库找到密钥

2. READ CHALLENGE (0xF100)  ──NFC──> on_challenge_read():
                                       generate_random_number_lib()
                                       写入 dp_challenge[16]
                                       challenge_valid = true
                             <──NFC── 返回 16 字节随机数

3. App 计算: response = AES(key, challenge)

4. WRITE RESPONSE (0xF101)  ──NFC──> on_response_write():
                                       expected = AES(storedKey, challenge)
                                       恒时比较 expected == response
                                       匹配: auth_ok=true, dp_auth_result=0x01
                                       不匹配: auth_ok=false, dp_auth_result=0x00
                                       清零 dp_challenge（防重放）
                             <──NFC── OK

5. READ AUTH_RESULT (0xF102) ──NFC──> 返回 dp_auth_result (0x01)

6. WRITE OPERATION (0xF103)  ──NFC──> on_operation_write():
                                       检查 auth_ok == true
                                       是: 驱动 H-Bridge 电机
                                       重置 auth_ok = false
                             <──NFC── OK

7. READ STATUS (0x0020)      ──NFC──> 返回 dp_lock_status (0x00 = 已开锁)
```

---

## 密钥管理

### 预共享密钥

- 存储位置：NVM `APARAM.secret[0..15]`（16 字节 AES-128）
- `lock_auth_init()` 启动时加载，全 0xFF 表示未配置
- Android 端对应密钥由 `LocalKeyManager` 管理

### 密钥配置方式

Phase 1：通过烧录工具在 APARAM 中预写入默认密钥。
Phase 2：可扩展 PROVISION 数据点实现 NFC 空中密钥交换。

---

## 防重放机制详解

| 机制 | 实现 |
|:-----|:-----|
| 挑战一次性 | `on_challenge_read` 每次调用 `generate_random_number_lib()` 生成新值 |
| 用后即废 | `on_response_write` 验证后立即 `memset(dp_challenge, 0, 16)` + `challenge_valid = false` |
| 认证一次性 | `on_operation_write` 执行后调用 `lock_auth_reset()` 清除 `auth_ok` |
| 恒时比较 | `on_response_write` 使用 `diff \|= expected ^ response` 防止计时侧信道 |

攻击场景分析：
- **嗅探重放**：录制的 RESPONSE 对应旧 CHALLENGE，新 CHALLENGE 不同，重放无效
- **中间人**：截获 CHALLENGE+RESPONSE 对，但不知密钥无法为新 CHALLENGE 生成有效 RESPONSE
- **暴力破解**：每次认证需要 NFC 物理接触，且 CHALLENGE 随机，无法离线穷举

---

## H-Bridge 电机控制

```
         NAC1080 H-Bridge
    ┌─────────────────────┐
    │  HS1 ──┐     ┌── HS2│
    │        │     │      │
    │        ├─ M ─┤      │    M = 直流电机
    │        │     │      │
    │  LS2 ──┘     └── LS1│
    └─────────────────────┘

    开锁 (forward):  HS1=on, LS2=on → 正转
    关锁 (backward): HS2=on, LS1=on → 反转
```

时序：10 步 x 50ms = 500ms 总运行时间。完成后所有开关断开。

---

## Android SDK 映射

| Android (AresDataPoints.kt) | 固件 (lock_datapoints.h) | 通信方式 |
|:----|:----|:----|
| `AresDataPoints.LOCK_ID` | `DP_LOCK_ID (0x0005)` | `mailboxApi.readDataPoint()` |
| `AresDataPoints.CHALLENGE` | `DP_CHALLENGE (0xF100)` | `mailboxApi.readDataPoint(bufferSize=16)` |
| `AresDataPoints.RESPONSE` | `DP_RESPONSE (0xF101)` | `mailboxApi.writeDataPoint()` |
| `AresDataPoints.AUTH_RESULT` | `DP_AUTH_RESULT (0xF102)` | `mailboxApi.readDataPoint()` |
| `AresDataPoints.CHARGE_PERCENT` | `DP_CHARGE_PERCENT (0x0012)` | `mailboxApi.readDataPoint()` |
| `AresDataPoints.OPERATION` | `DP_OPERATION (0xF103)` | `mailboxApi.writeDataPoint()` |
| `AresDataPoints.STATUS` | `DP_STATUS (0x0020)` | `mailboxApi.readDataPoint()` |

所有数据点均为明文传输（不使用 `data_point_encrypt`），安全性由应用层挑战应答保障。

---

## 构建与烧录

```bash
cd nac1080_lock
make clean && make

# 烧录 HEX 文件到 NAC1080
# 使用 Infineon 提供的 NFC Programmer 工具
```

## 文件结构

```
nac1080_lock/
├── inc/
│   ├── lock_config.h        # 全局常量
│   ├── lock_datapoints.h    # 数据点 ID 定义
│   ├── lock_auth.h          # 挑战应答认证 API
│   └── lock_motor.h         # 电机控制 API
├── src/
│   ├── lock_main.c          # 固件入口
│   ├── lock_datapoints.c    # 数据点表 + 初始化
│   ├── lock_auth.c          # RNG 挑战 + AES 验证
│   ├── lock_motor.c         # H-Bridge 电机驱动
│   ├── lock_aparam.c        # NVM APARAM 配置
│   └── startup_smack.c      # 启动代码
└── doc/
    └── firmware_protocol.md  # 本文档
```
