package com.infineon.smack.sample.smack

import com.infineon.smack.sdk.SmackSdk
import io.mockk.coEvery
import io.mockk.every
import io.mockk.mockk
import kotlinx.coroutines.flow.collect
import kotlinx.coroutines.flow.flowOf
import kotlinx.coroutines.test.StandardTestDispatcher
import kotlinx.coroutines.test.runTest
import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Test
import org.junit.jupiter.api.assertThrows

internal class SampleSmackApiTest {

    @Test
    fun `when the sdk throws an exception, the PocSmackApi should rethrow it`() = runTest {
        // Given a throwing PocSmackApi
        val smackSdk = SmackSdk.Builder()
            .setSmackClient(
                mockk(relaxed = true) {
                    every { isConnected } returns flowOf(true)
                    coEvery { sendByteArray(any()) } throws Exception("Forced exception")
                }
            )
            .setCoroutineDispatcher(StandardTestDispatcher())
            .setNfcAdapterWrapper(mockk(relaxed = true))
            .build()
        val sampleSmackApi = SampleSmackApi(smackSdk)

        // When the connection is established, then the exception should be rethrown
        val exception = assertThrows<Exception> {
            sampleSmackApi.getStateAndKeepAlive().collect()
        }
        assertEquals("Forced exception", exception.message)
    }
}
