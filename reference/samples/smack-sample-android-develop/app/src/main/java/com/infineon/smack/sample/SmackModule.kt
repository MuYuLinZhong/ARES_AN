package com.infineon.smack.sample

import com.infineon.smack.sdk.SmackSdk
import com.infineon.smack.sdk.android.AndroidNfcAdapterWrapper
import com.infineon.smack.sdk.log.AndroidSmackLogger
import com.infineon.smack.sdk.smack.DefaultSmackClient
import dagger.Module
import dagger.Provides
import dagger.hilt.InstallIn
import dagger.hilt.components.SingletonComponent
import kotlinx.coroutines.Dispatchers
import javax.inject.Singleton

@Module
@InstallIn(SingletonComponent::class)
object SmackModule {

    @Singleton
    @Provides
    @Suppress("InjectDispatcher")
    fun provideSmackSdk(): SmackSdk {
        return SmackSdk.Builder()
            .setSmackClient(DefaultSmackClient(AndroidSmackLogger()))
            .setNfcAdapterWrapper(AndroidNfcAdapterWrapper())
            .setCoroutineDispatcher(Dispatchers.IO)
            .build()
    }
}
