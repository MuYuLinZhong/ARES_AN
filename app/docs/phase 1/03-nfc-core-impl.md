# 03 NFC 核心模块 Phase 1 实现总结

## 功能概述

完整实现五步开/关锁协议（步骤 3 使用本地密钥加密替代云端）：
1. 发送操作位 → 接收 deviceId
2. 接收硬件随机数 challenge
3. 本地 AES 加密 challenge → 生成密文
4. 发送密文给硬件
5. 接收执行结果（成功/密钥错误/机械失败）

## 调用流程

```mermaid
flowchart TD
    A["用户点击 Unlock/Lock"] --> B["WebView → Bridge → HomeViewModel"]
    B --> C["requestOperation()\n记录意图，等待 Tag"]
    C --> D["ForegroundDispatch 检测到 Tag"]
    D --> E["executeLockOperation() 启动协程"]

    E --> F["步骤1: SendIntentionBitUseCase\n→ NfcRepository.sendIntentionBit\n进度 20%"]
    F --> G{"deviceId 匹配?"}
    G -- 否 --> H["DeviceMismatchException\n中止"]
    G -- 是 --> I["步骤2: ReceiveChallengeUseCase\n→ NfcRepository.receiveChallenge\n进度 40%"]
    I --> J["步骤3: LocalEncryptUseCase\n→ CryptoRepository.encryptLocal\n进度 65%"]
    J --> K["步骤4: SendCipherToLockUseCase\n→ NfcRepository.sendCipher\n进度 85%"]
    K --> L["步骤5: ReceiveLockResultUseCase\n→ NfcRepository.receiveResult\n进度 100%"]
    L --> M{"结果"}
    M -- "0x00" --> N["Success → UI 更新"]
    M -- "0x01" --> O["CipherMismatch → 重试"]
    M -- "0x02" --> P["MechanicalFailure → 门未关严"]
```

## 数据流

```mermaid
sequenceDiagram
    participant VM as HomeViewModel
    participant NFC as NfcRepositoryImpl
    participant HW as NAC1080 硬件
    participant Crypto as LocalCryptoRepository
    participant KeyMgr as LocalKeyManager
    participant DS as DataStore

    VM->>NFC: connect(tag)
    NFC->>HW: IsoDep.connect()

    VM->>NFC: sendIntentionBit(0x01)
    NFC->>HW: APDU [00 A1 01 00 00]
    HW-->>NFC: [deviceId bytes] [90 00]

    VM->>NFC: receiveChallenge()
    NFC->>HW: APDU [00 A2 00 00 10]
    HW-->>NFC: [16 字节 challenge] [90 00]

    VM->>Crypto: encryptLocal(challenge)
    Crypto->>KeyMgr: getDeviceKey()
    KeyMgr->>DS: getLocalDeviceKey()
    DS-->>KeyMgr: "0123456789abcdef..."
    KeyMgr-->>Crypto: ByteArray 密钥
    Crypto->>Crypto: AES/ECB/NoPadding 加密
    Crypto-->>VM: Result(cipher)

    VM->>NFC: sendCipher(cipher)
    NFC->>HW: APDU [00 A3 00 00 10 cipher...]
    HW-->>NFC: [90 00]

    VM->>NFC: receiveResult()
    NFC->>HW: APDU [00 A4 00 00 01]
    HW-->>NFC: [0x00] [90 00]
    NFC-->>VM: LockResult.Success
```

## 进度状态机

```mermaid
stateDiagram-v2
    [*] --> Idle
    Idle --> InProgress20 : 开始操作
    InProgress20 --> InProgress40 : 步骤1 成功
    InProgress40 --> InProgress65 : 步骤2 成功
    InProgress65 --> InProgress85 : 步骤3 成功
    InProgress85 --> InProgress100 : 步骤4 成功
    InProgress100 --> Success : 步骤5 成功
    InProgress100 --> Error : 步骤5 失败
    InProgress20 --> Error : NFC 异常
    InProgress40 --> Error : NFC 超时
    InProgress65 --> Error : 加密异常
    InProgress85 --> Error : NFC 异常
    Success --> Idle : 弹窗关闭
    Error --> Idle : 确认/关闭
```

## 涉及文件

| 文件 | 职责 |
|:-----|:-----|
| `presentation/home/HomeViewModel.kt` | 五步协程编排、进度管理、异常处理 |
| `domain/usecase/lock/*.kt` | 5 个 UseCase（每步一个） |
| `data/nfc/NfcRepositoryImpl.kt` | IsoDep APDU 通信 |
| `data/local/LocalCryptoRepository.kt` | 本地 AES 加密 |
| `data/local/security/LocalKeyManager.kt` | 调试密钥管理 |

## 设计理由

1. **每步一个 UseCase**：单一职责，便于 Phase 2 单独替换步骤 3（LocalEncrypt → RequestCipher）。
2. **协程 + withTimeout**：NFC 操作天然串行，协程完美匹配；5 秒超时防止永久等待。
3. **进度状态机**：OperationState 密封类覆盖全部状态，when 表达式强制穷举。
4. **异常分层处理**：DeviceMismatch、IOException、Timeout、Cancellation 各有明确处理策略。

## Phase 2 演进

- 步骤 3 替换为 `RequestCipherUseCase`（POST /crypto/encrypt）
- 步骤 3 新增网络超时/403/断网异常分支
- 操作结束后新增 `ReportOperationResultUseCase`（异步上报）
- Phase 3：步骤 3+ 中断时写入 pending_reports
