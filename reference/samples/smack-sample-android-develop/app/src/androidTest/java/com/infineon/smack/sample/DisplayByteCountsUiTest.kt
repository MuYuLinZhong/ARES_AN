package com.infineon.smack.sample

import com.adevinta.android.barista.assertion.BaristaVisibilityAssertions.assertDisplayed
import com.adevinta.android.barista.interaction.BaristaClickInteractions.clickOn
import com.infineon.smack.sample.overview.MainActivity
import com.infineon.smack.sample.smack.ByteCount
import com.infineon.smack.sample.smack.SampleSmackApi
import com.infineon.smack.sample.smack.SampleSmackState
import dagger.hilt.android.testing.BindValue
import dagger.hilt.android.testing.HiltAndroidRule
import dagger.hilt.android.testing.HiltAndroidTest
import de.xroot.android.TestDispatcher
import de.xroot.android.lazyActivityScenarioRule
import io.mockk.every
import io.mockk.mockk
import io.mockk.verify
import kotlinx.coroutines.flow.flowOf
import kotlinx.coroutines.test.runCurrent
import kotlinx.coroutines.test.runTest
import org.junit.Before
import org.junit.Rule
import org.junit.Test

/**
 * Test for https://www.x-root.info/jira/browse/INFINEON-142
 */
@HiltAndroidTest
internal class DisplayByteCountsUiTest {

    @get:Rule(order = 0)
    var hiltRule = HiltAndroidRule(this)

    @get:Rule
    var activityRule = lazyActivityScenarioRule<MainActivity>(launchActivity = false)

    @BindValue
    val pockSmackApi: SampleSmackApi = mockk(relaxed = true)

    @Before
    fun clear() {
        hiltRule.inject()
    }

    @Test
    fun app_should_show_byte_counts() = runTest(TestDispatcher.DISPATCHER) {
        // Given a view state with byte counts
        val state = SampleSmackState(true, ByteCount(3, 2, 1))
        every { pockSmackApi.getStateAndKeepAlive() } returns flowOf(state)
        activityRule.launch()
        runCurrent()

        // Then the byte counts should be displayed
        assertDisplayed(R.id.sentValueTextView, "3")
        assertDisplayed(R.id.receivedValueTextView, "2")
        assertDisplayed(R.id.divergentValueTextView, "1")
    }

    @Test
    fun click_on_reset_should_call_view_model_reset() = runTest(TestDispatcher.DISPATCHER) {
        // Given a running activity
        activityRule.launch()
        runCurrent()

        // When the reset button is clicked
        clickOn(R.id.resetButton)

        // Then the counter should reset
        verify(exactly = 1) { pockSmackApi.resetCount() }
    }
}
