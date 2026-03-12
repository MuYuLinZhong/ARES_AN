package com.infineon.smack.sample.overview

import com.infineon.smack.sample.smack.SampleSmackApi
import com.infineon.smack.sample.smack.TestPocSmackState
import com.infineon.smack.sdk.SmackSdk
import com.infineon.smack.sdk.smack.SmackClient
import com.infineon.smack.sdk.smack.exception.ProtocolException
import de.xroot.android.CoroutinesTestExtension
import de.xroot.android.InstantExecutorExtension
import io.mockk.coEvery
import io.mockk.every
import io.mockk.mockk
import kotlinx.coroutines.flow.firstOrNull
import kotlinx.coroutines.flow.flowOf
import kotlinx.coroutines.test.StandardTestDispatcher
import kotlinx.coroutines.test.runTest
import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertNotNull
import org.junit.jupiter.api.Test
import org.junit.jupiter.api.extension.ExtendWith

@ExtendWith(InstantExecutorExtension::class, CoroutinesTestExtension::class)
class MainViewModelTest {

    @Test
    fun `initial connection state should be not connected`() = runTest {
        // Given a view model with no connection
        val viewModel = MainViewModel(
            createSmackSdk(mockk(relaxed = true)),
            mockk(relaxed = true) {
                every { getStateAndKeepAlive() } returns
                    flowOf(TestPocSmackState.disconnected())
            }
        )

        // Then the connected field should be false
        assertEquals(
            false,
            viewModel.viewStateFlow.firstOrNull()?.isConnected
        )
    }

    @Test
    fun `view state should show established connection`() = runTest {
        // Given a view model with a connection
        val viewModel = MainViewModel(
            createSmackSdk(mockk(relaxed = true)),
            mockk(relaxed = true) {
                coEvery { getStateAndKeepAlive() } returns flowOf(TestPocSmackState.connected())
            }
        )

        // Then the connected field should be false
        assertEquals(
            true,
            viewModel.viewStateFlow.firstOrNull()?.isConnected
        )
    }

    @Test
    fun `when an error occurs, the view state should contain alert info`() = runTest {
        // Given a view model with a connected but throwing PocSmackApi
        val smackSdk = createConnectedButThrowingSmackSdk()
        val viewModel = MainViewModel(
            smackSdk,
            SampleSmackApi(smackSdk)
        )

        // When an exception is thrown
        // (implicitly with the connected and throwing PocSmackApi)
        val viewState = viewModel.viewStateFlow.firstOrNull()

        // Then the view state should contain alert information
        assertNotNull(viewState?.alertTitle)
        assertNotNull(viewState?.alertMessage)
    }

    private fun createSmackSdk(smackClient: SmackClient) = SmackSdk.Builder()
        .setSmackClient(smackClient)
        .setCoroutineDispatcher(StandardTestDispatcher())
        .setNfcAdapterWrapper(mockk(relaxed = true))
        .build()

    private fun createConnectedButThrowingSmackSdk(): SmackSdk {
        return createSmackSdk(
            mockk(relaxed = true) {
                every { isConnected } returnsMany listOf(flowOf(true), flowOf(false))
                coEvery { sendByteArray(any()) } throws ProtocolException("Forced exception")
            }
        )
    }
}
