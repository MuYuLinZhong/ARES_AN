# 11 · 安全加固：证书固定 · Token 加密存储

> **模块边界**：所有安全相关的基础设施，为网络层和存储层提供加密能力。  
> **依赖模块**：无（基础设施层）  
> **被依赖**：`09-network`（证书固定）、`08-storage`（Token 加密）、`02-auth`（Token 存取）

---

## 1. 证书固定（Certificate Pinning）

### 1.1 目的

防止中间人攻击（MITM）：即使用户在设备上安装了恶意 CA 证书，攻击者也无法用伪造证书截获 API 通信（尤其是 NFC 密文传输）。

### 1.2 实现方式

在 OkHttp 客户端中配置 `CertificatePinner`，绑定云端 API 域名的证书公钥指纹（SHA-256）：

```
OkHttpClient.Builder()
    .certificatePinner(
        CertificatePinner.Builder()
            .add("api.your-domain.com", "sha256/AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=")  // 主证书指纹
            .add("api.your-domain.com", "sha256/BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB=")  // 备用证书指纹（证书轮换预留）
            .build()
    )
```

### 1.3 指纹获取方式

部署时通过以下命令获取服务器证书的 SHA-256 公钥指纹：
```bash
openssl s_client -connect api.your-domain.com:443 | openssl x509 -pubkey -noout | \
  openssl pkey -pubin -outform der | openssl dgst -sha256 -binary | base64
```

### 1.4 证书轮换策略

- 同时配置**主证书**和**备用证书**两个指纹（OkHttp 支持多个，满足其一即通过）
- 证书到期前至少 **30 天**发布新版本 App，将备用证书升级为主证书，并添加新的备用证书
- 紧急情况（证书泄漏）：发布强制更新，所有旧版本拒绝使用（配合服务端下发响应头 `X-Force-Update: true`）

### 1.5 证书固定失败的处理

OkHttp 证书校验失败时抛出 `SSLPeerUnverifiedException`，在 Repository 层捕获：
- 提示用户"网络连接不安全，请检查是否处于可信网络环境"
- **不降级**（不跳过证书验证），安全不可妥协

---

## 2. Token 加密存储（Android Keystore + DataStore）

### 2.1 目的

Token 存储在 DataStore 中，若不加密，Root 设备上可通过 `/data/data/` 目录直接提取。使用 Android Keystore 确保密钥在硬件安全模块（TEE/StrongBox）中，数据在其他设备上无法解密。

### 2.2 密钥生成

**文件**：`data/local/security/KeystoreManager.kt`

在首次需要存储 Token 时，通过 `KeyGenerator` 在 Android Keystore 中创建一个 AES-256-GCM 密钥：

```
密钥别名：  "nac1080_token_key"
算法：      AES-256-GCM
密钥存储：  AndroidKeyStore（硬件安全模块）
用途：      ENCRYPT | DECRYPT
不可导出：  setIsStrongBoxBacked(true)（如果设备支持 StrongBox）
```

密钥在 Keystore 中持久化，App 卸载时自动删除，重装后重新生成。

### 2.3 加密流程

**每次写入 Token**：
```
1. 获取 Keystore 中的 AES-GCM 密钥
2. 生成随机 IV（12 字节，SecureRandom）
3. 用该密钥 + IV 对 Token 字符串做 AES-GCM 加密
4. 将加密后的字节和 IV 一起写入 DataStore
   （IV 明文存储，密钥在 Keystore 中不导出）
```

**每次读取 Token**：
```
1. 从 DataStore 读取密文字节和 IV
2. 获取 Keystore 中的 AES-GCM 密钥
3. 用密钥 + IV 解密，得到 Token 字符串
4. 解密失败（密钥不匹配/数据损坏）→ catch UserNotAuthenticatedException
   → 清除 DataStore，跳转登录页
```

### 2.4 密钥保护特性

| 特性 | 说明 |
| :--- | :--- |
| 硬件绑定 | 密钥存储在 TEE（Trusted Execution Environment）中，无法通过软件提取 |
| 设备绑定 | 密钥无法导出到其他设备，即使备份了 DataStore 文件也无法在其他手机解密 |
| App 绑定 | 密钥与 App 的 keystore 别名绑定，其他 App 无法访问 |
| 卸载清理 | App 卸载时密钥自动销毁，无残留 |

### 2.5 适配旧机型

部分老旧 Android 机型（Android 6.0 以下）不支持 Android Keystore 的 AES-GCM。

- **最低支持**：API 23（Android 6.0）
- API 23 以下：不做加密，明文存储（不在支持范围内）
- API 23~27：使用软件 Keystore（安全级别略低于硬件 TEE，但仍有应用隔离保护）
- API 28+：尝试使用 StrongBox，失败则降级到软件 TEE

---

## 3. 安全检查清单

| 检查项 | 实现位置 | 状态 |
| :--- | :--- | :--- |
| HTTPS 全站 | OkHttp 默认行为 | ✅ 必须 |
| 证书固定 | `NetworkModule` CertificatePinner | ✅ 必须 |
| Token 加密存储 | `KeystoreManager` + DataStore | ✅ 必须 |
| NFC Challenge 不落盘 | HomeViewModel 协程局部变量 | ✅ 必须 |
| NFC 密文不落盘 | 同上 | ✅ 必须 |
| adb backup 防护 | `AndroidManifest.xml` 中 `android:allowBackup="false"` | ✅ 必须 |
| 手机号脱敏展示 | Room / UI 仅存储/展示后四位 | ✅ 必须 |
| 混淆（ProGuard/R8） | `build.gradle` release 配置中启用 minify | ✅ 建议 |
