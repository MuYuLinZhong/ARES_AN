package com.example.all.di

import android.content.Context
import android.nfc.NfcAdapter
import dagger.Module
import dagger.Provides
import dagger.hilt.InstallIn
import dagger.hilt.android.qualifiers.ApplicationContext
import dagger.hilt.components.SingletonComponent
import javax.inject.Singleton

/**
 * NFC 模块 —— 提供系统 NfcAdapter 实例
 *
 * NfcAdapter 是 Android 系统单例，通过 NfcAdapter.getDefaultAdapter() 获取。
 * 返回 null 表示设备不支持 NFC。
 *
 * 注意：NfcAdapter 本身可以为 null（设备无 NFC 芯片），
 * 因此提供可空类型，由 SplashViewModel 检查并处理。
 */
@Module
@InstallIn(SingletonComponent::class)
object NfcModule {

    /**
     * 获取系统 NFC 适配器
     * @return NfcAdapter 实例；设备不支持 NFC 时返回 null
     */
    @Provides
    @Singleton
    fun provideNfcAdapter(@ApplicationContext context: Context): NfcAdapter? =
        NfcAdapter.getDefaultAdapter(context)
}
