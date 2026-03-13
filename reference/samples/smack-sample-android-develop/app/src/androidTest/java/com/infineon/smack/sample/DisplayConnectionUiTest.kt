package com.infineon.smack.sample

import com.adevinta.android.barista.assertion.BaristaVisibilityAssertions.assertDisplayed
import com.adevinta.android.barista.assertion.BaristaVisibilityAssertions.assertNotDisplayed
import com.infineon.smack.sample.TestPocSmackState.connectedState
import com.infineon.smack.sample.TestPocSmackState.disconnectedState
import com.infineon.smack.sample.overview.MainActivity
import com.infineon.smack.sample.smack.SampleSmackApi
import dagger.hilt.android.testing.BindValue
import dagger.hilt.android.testing.HiltAndroidRule
import dagger.hilt.android.testing.HiltAndroidTest
import de.xroot.android.TestDispatcher
import de.xroot.android.lazyActivityScenarioRule
import io.mockk.every
import io.mockk.mockk
import kotlinx.coroutines.flow.flowOf
import kotlinx.coroutines.test.runCurrent
import kotlinx.coroutines.test.runTest
import org.junit.Before
import org.junit.Rule
import org.junit.Test

/**
 * Test for https://www.x-root.info/jira/browse/INFINEON-137
 */
@HiltAndroidTest
class DisplayConnectionUiTest {

    @get:Rule
    var hiltRule = HiltAndroidRule(this)

    @get:Rule
    var activityRule = lazyActivityScenarioRule<MainActivity>(launchActivity = false)

    @BindValue
    val pockSmackApi: SampleSmackApi = mockk()

    @Before
    fun clear() {
        hiltRule.inject()
    }

    @Test
    fun app_should_start_with_no_nfc_connection_displayed() {
        // Given no connection
        every { pockSmackApi.getStateAndKeepAlive() } returns flowOf()

        // When the activity is displayed
        activityRule.launch()

        // Then no connection should be displayed
        assertNoConnectionDisplayed()
    }

    @Test
    fun nfc_connection_should_be_displayed() = runTest(TestDispatcher.DISPATCHER) {
        // Given a connection
        every { pockSmackApi.getStateAndKeepAlive() } returns flowOf(connectedState)

        // When the activity is displayed
        activityRule.launch()
        runCurrent()

        // Then a connection should be visible
        assertConnectionDisplayed()
    }

    @Test
    fun no_nfc_connection_should_be_displayed_after_disconnection() {
        // Given a disconnect
        every { pockSmackApi.getStateAndKeepAlive() } returns flowOf(disconnectedState)

        // When the activity is displayed
        activityRule.launch()

        // Then no connection should be displayed
        assertNoConnectionDisplayed()
    }

    private fun assertNoConnectionDisplayed() {
        assertDisplayed(R.string.nfc_disconnected)
        assertNotDisplayed(R.string.nfc_connected)
        assertDisplayed(R.id.disconnectedImageView)
        assertNotDisplayed(R.id.connectedImageView)
    }

    private fun assertConnectionDisplayed() {
        assertNotDisplayed(R.string.nfc_disconnected)
        assertDisplayed(R.string.nfc_connected)
        assertNotDisplayed(R.id.disconnectedImageView)
        assertDisplayed(R.id.connectedImageView)
    }
}
