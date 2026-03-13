package com.example.all.di

import com.example.all.data.fake.FakeAuthRepository
import com.example.all.data.fake.FakePermissionRepository
import com.example.all.data.local.DataStorePreferencesRepository
import com.example.all.data.local.LocalCryptoRepository
import com.example.all.data.local.LocalDeviceRepository
import com.example.all.data.nfc.SmackLockRepository
import com.example.all.data.repository.AuthRepository
import com.example.all.data.repository.CryptoRepository
import com.example.all.data.repository.DeviceRepository
import com.example.all.data.repository.NfcRepository
import com.example.all.data.repository.PermissionRepository
import com.example.all.data.repository.PreferencesRepository
import dagger.Binds
import dagger.Module
import dagger.hilt.InstallIn
import dagger.hilt.components.SingletonComponent
import javax.inject.Singleton

/**
 * 仓库绑定模块 —— 将 Repository 接口与具体实现类绑定
 *
 * Phase 1 绑定策略：
 *   - CryptoRepository     → LocalCryptoRepository（本地 AES 加密）
 *   - DeviceRepository     → LocalDeviceRepository（Room 直接读写）
 *   - NfcRepository        → SmackLockRepository（mailboxApi + 挑战应答）
 *   - AuthRepository       → FakeAuthRepository（stub，不参与）
 *   - PermissionRepository → FakePermissionRepository（stub，不参与）
 *   - PreferencesRepository → DataStorePreferencesRepository（偏好 + 本地密钥）
 *
 * Phase 2 演进：只需修改此文件中的绑定关系即可切换到云端实现，
 *               ViewModel 和 UseCase 层完全零改动。
 */
@Module
@InstallIn(SingletonComponent::class)
abstract class RepositoryModule {

    /** 加密仓库：Phase 1 用本地密钥加密 */
    @Binds @Singleton
    abstract fun bindCryptoRepository(
        impl: LocalCryptoRepository
    ): CryptoRepository

    /** 设备仓库：Phase 1 仅 Room 本地读写 */
    @Binds @Singleton
    abstract fun bindDeviceRepository(
        impl: LocalDeviceRepository
    ): DeviceRepository

    /** NFC 仓库：SmAcK mailboxApi + 挑战应答实现 */
    @Binds @Singleton
    abstract fun bindNfcRepository(
        impl: SmackLockRepository
    ): NfcRepository

    /** 认证仓库：Phase 1 stub */
    @Binds @Singleton
    abstract fun bindAuthRepository(
        impl: FakeAuthRepository
    ): AuthRepository

    /** 权限仓库：Phase 1 stub */
    @Binds @Singleton
    abstract fun bindPermissionRepository(
        impl: FakePermissionRepository
    ): PermissionRepository

    /** 偏好仓库：Phase 1 精简版（无 Token） */
    @Binds @Singleton
    abstract fun bindPreferencesRepository(
        impl: DataStorePreferencesRepository
    ): PreferencesRepository
}
