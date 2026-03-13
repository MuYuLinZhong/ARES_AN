package com.infineon.smack.sample

import com.infineon.smack.sdk.SmackSdk
import com.infineon.smack.sdk.fake.smack.FakeSmackClient
import dagger.Module
import dagger.Provides
import dagger.hilt.components.SingletonComponent
import dagger.hilt.testing.TestInstallIn
import de.xroot.android.TestDispatcher
import io.mockk.mockk
import javax.inject.Singleton

@TestInstallIn(
    components = [SingletonComponent::class],
    replaces = [SmackModule::class]
)
@Module
object TestNfcModule {

    @Singleton
    @Provides
    fun provideSmackSdk(): SmackSdk {
        return SmackSdk.Builder()
            .setSmackClient(FakeSmackClient)
            .setNfcAdapterWrapper(mockk(relaxed = true))
            .setCoroutineDispatcher(TestDispatcher.DISPATCHER)
            .build()
    }
}
