package com.infineon.smack.sample.overview

import com.adevinta.android.barista.assertion.BaristaVisibilityAssertions.assertDisplayed
import com.adevinta.android.barista.assertion.BaristaVisibilityAssertions.assertNotDisplayed
import com.adevinta.android.barista.interaction.BaristaSleepInteractions
import com.infineon.smack.sample.R
import dagger.hilt.android.testing.BindValue
import dagger.hilt.android.testing.HiltAndroidRule
import dagger.hilt.android.testing.HiltAndroidTest
import de.xroot.android.TestDispatcher
import de.xroot.android.lazyActivityScenarioRule
import io.mockk.every
import io.mockk.mockk
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.test.runTest
import org.junit.Before
import org.junit.Ignore
import org.junit.Rule
import org.junit.Test

@HiltAndroidTest
internal class MainActivityTest {

    @get:Rule
    var hiltRule = HiltAndroidRule(this)

    @get:Rule
    var activityRule = lazyActivityScenarioRule<MainActivity>(launchActivity = false)

    @BindValue
    val viewModel: MainViewModel = mockk(relaxed = true)

    @Before
    fun clear() {
        hiltRule.inject()
    }

    // Acceptance Test for https://www.x-root.info/jira/browse/INFINEON-201
    @Ignore("Ignored because of high flakiness")
    @Test
    fun when_the_view_state_contains_alert_information_a_dialog_should_appear() =
        runTest(TestDispatcher.DISPATCHER) {
            // Given a view state that contains alert information
            val alertTitleRes = R.string.error_title_try_again
            val alertMessageRes = R.string.error_message_try_again
            every { viewModel.viewState } returns MutableStateFlow(
                MainViewState.forError(alertTitleRes, alertMessageRes, true)
            )
            activityRule.launch()
            BaristaSleepInteractions.sleep(1000)
            // Then a dialog should be displayed
            assertDisplayed(alertTitleRes)
            assertDisplayed(alertMessageRes)
            assertDisplayed(R.string.error_message_no_nvm_firmware)
        }

    @Ignore("Doesn't run on CI")
    @Test
    fun app_should_start() = runTest(TestDispatcher.DISPATCHER) {
        // Given a view state
        every { viewModel.viewState } returns MutableStateFlow(
            MainViewState(true, 1, 2, 3)
        )
        activityRule.launch()
        // Then the app should be displayed
        assertDisplayed(R.id.connectionHeadline)
        assertNotDisplayed(R.string.error_message_no_nvm_firmware)
    }
}
