package com.infineon.smack.sample.overview

import androidx.annotation.StringRes
import androidx.annotation.VisibleForTesting
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.infineon.smack.sample.R
import com.infineon.smack.sample.smack.SampleSmackApi
import com.infineon.smack.sample.smack.SampleSmackState
import com.infineon.smack.sdk.SmackSdk
import com.infineon.smack.sdk.mailbox.exception.WrongKeyException
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.CancellationException
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.SharingStarted
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.catch
import kotlinx.coroutines.flow.flowOn
import kotlinx.coroutines.flow.map
import kotlinx.coroutines.flow.retry
import kotlinx.coroutines.flow.stateIn
import kotlinx.coroutines.plus
import javax.inject.Inject

@HiltViewModel
class MainViewModel @Inject constructor(
    smackSdk: SmackSdk,
    private val smackApi: SampleSmackApi,
) : ViewModel() {

    @VisibleForTesting
    val viewStateFlow: Flow<MainViewState> = smackApi
        .getStateAndKeepAlive()
        .flowOn(smackSdk.coroutineDispatcher)
        .map { pocSmackState -> pocSmackState.toViewState() }
        .catch { exception ->
            emit(
                MainViewState.forError(
                    getErrorTitleResId(exception),
                    getErrorMessageResId(exception),
                    smackApi.firmwareToggleFlow.value
                )
            )
            throw exception
        }
        .retry { exception ->
            exception !is CancellationException
        }

    val viewState: StateFlow<MainViewState> = viewStateFlow.stateIn(
        scope = viewModelScope + smackSdk.coroutineDispatcher,
        started = SharingStarted.WhileSubscribed(0, 0),
        initialValue = MainViewState(false, 0, 0, 0)
    )

    private fun SampleSmackState.toViewState(): MainViewState {
        return MainViewState(
            isConnected,
            byteCount.sentByteCount,
            byteCount.receivedByteCount,
            byteCount.divergentByteCount,
            showMissingFirmwareText = shouldShowMissingFirmware
        )
    }

    @StringRes
    private fun getErrorTitleResId(exception: Throwable): Int =
        when (isWrongKeyException(exception)) {
            true -> R.string.error_title_invalid_key
            else -> R.string.error_title_try_again
        }

    @StringRes
    private fun getErrorMessageResId(exception: Throwable): Int =
        when (isWrongKeyException(exception)) {
            true -> R.string.error_message_invalid_key
            else -> R.string.error_message_try_again
        }

    private fun isWrongKeyException(exception: Throwable) = exception is WrongKeyException

    fun resetCount() {
        smackApi.resetCount()
    }

    fun setFirmwareToggle(enabled: Boolean) {
        smackApi.setFirmwareToggle(enabled)
    }
}
