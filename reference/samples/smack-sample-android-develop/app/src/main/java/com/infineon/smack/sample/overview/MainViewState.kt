package com.infineon.smack.sample.overview

import androidx.annotation.StringRes

data class MainViewState(
    val isConnected: Boolean = false,
    val sentBytesCount: Long = 0,
    val receivedBytesCount: Long = 0,
    val divergentBytesCount: Long = 0,
    @StringRes val alertTitle: Int? = null,
    @StringRes val alertMessage: Int? = null,
    val showMissingFirmwareText: Boolean = false,
) {

    companion object {

        fun forError(
            @StringRes alertTitleRes: Int,
            @StringRes alertMessageRes: Int,
            firmwareToggleEnabled: Boolean
        ) = MainViewState(
            alertTitle = alertTitleRes,
            alertMessage = alertMessageRes,
            showMissingFirmwareText = firmwareToggleEnabled,
        )
    }
}
