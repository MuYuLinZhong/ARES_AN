# NAC1080 Lock Firmware

ARES 项目自定义门锁固件，基于 Infineon NAC1080 SmAcK C SDK 开发。

## 目录结构

```
nac1080_lock/
  src/
    lock_main.c          - 固件入口，初始化并进入主循环
    lock_datapoints.c    - 数据点定义，ID 对齐 Android SDK LockDataPoint
    lock_auth.c          - AES 密钥管理 + 会话认证
    lock_motor.c         - H-Bridge 电机控制
    lock_aparam.c        - APARAM 配置（NVM 参数表）
    startup_smack.c      - 启动代码（从 SDK smack_sl 复制）
  inc/
    lock_datapoints.h    - 数据点头文件
    lock_auth.h          - 认证模块头文件
    lock_motor.h         - 电机控制头文件
    lock_config.h        - 全局配置常量
  doc/
    firmware_protocol.md - 固件协议文档
```

## 构建

需要 Infineon NAC1080 工具链（Keil MDK 或 GNU Arm Embedded Toolchain）。

```bash
# 配置工具链路径
export ARM_GCC_PATH=/path/to/arm-none-eabi

# 构建
make all

# 烧录（通过 SWD 调试器）
make flash
```

## 兼容性

固件数据点 ID 与 SmAcK Android SDK `LockDataPoint.kt` 完全对齐，
因此 Android 端可以继续使用 `SmackLockApi` 无需修改。
