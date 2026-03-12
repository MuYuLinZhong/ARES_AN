package com.infineon.smack.sample.smack

import android.nfc.TagLostException
import com.infineon.smack.sdk.SmackSdk
import com.infineon.smack.sdk.common.Milliseconds
import com.infineon.smack.sdk.common.RandomByteArrayFactory
import com.infineon.smack.sdk.mailbox.SmackMailbox
import com.infineon.smack.sdk.mailbox.datapoint.MailboxDataPoint.SCRATCH8
import kotlinx.coroutines.CancellationException
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.FlowCollector
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.combine
import kotlinx.coroutines.flow.transformLatest
import javax.inject.Inject

class SampleSmackApi @Inject constructor(private val smackSdk: SmackSdk) {

    private val byteCountStateFlow = MutableStateFlow(ByteCount())
    val firmwareToggleFlow = MutableStateFlow(false)

    private var lastState: SampleSmackState = SampleSmackState(isConnected = false, ByteCount())

    private suspend fun FlowCollector<SampleSmackState>.emit(
        isConnected: Boolean? = null,
        newByteCount: ByteCount? = null,
        showError: Boolean? = null
    ) {
        isConnected?.let { lastState = lastState.copy(isConnected = isConnected) }
        newByteCount?.let { lastState = lastState.copy(byteCount = newByteCount) }
        showError?.let { lastState = lastState.copy(shouldShowMissingFirmware = showError) }
        emit(lastState)
    }

    @Suppress("TooGenericExceptionCaught")
    fun getStateAndKeepAlive(): Flow<SampleSmackState> = combine(
        smackSdk.mailboxApi.mailbox, byteCountStateFlow, firmwareToggleFlow, ::Triple
    ).transformLatest { (smackMailbox, byteCount, useDatapoint) ->
        log("Mailbox and byte count flow is updated ($byteCount, $smackMailbox, useDatapoint=$useDatapoint")
        smackMailbox?.let { mailbox ->
            emit(isConnected = true, newByteCount = byteCount)
            startKeepAliveCircle(mailbox, byteCount, useDatapoint)
        } ?: emit(isConnected = false)
    }

    private suspend fun FlowCollector<SampleSmackState>.startKeepAliveCircle(
        mailbox: SmackMailbox,
        currentByteCount: ByteCount,
        useDatapoint: Boolean,
    ) {
        log("Writing and reading keep alive packages...")
        val newByteCount = currentByteCount + sendRequestAndAccumulateState(mailbox, useDatapoint)
        emit(newByteCount = newByteCount, showError = false)
        repeatRequests(currentByteCount = newByteCount)
    }

    private suspend fun repeatRequests(currentByteCount: ByteCount) {
        delay(KEEP_ALIVE_INTERVAL)
        byteCountStateFlow.value = currentByteCount
    }

    private suspend fun FlowCollector<SampleSmackState>.sendRequestAndAccumulateState(
        mailbox: SmackMailbox,
        useDatapoint: Boolean
    ): ByteCount {
        val bytesToSend: ByteArray = getRandomBytesToSend(useDatapoint)
        val readBytes: ByteArray = writeAndReadByteArray(useDatapoint, mailbox, bytesToSend)
        val divergentCount = bytesToSend.getDivergentByteCount(readBytes).toLong()
        return ByteCount(
            sentByteCount = bytesToSend.size.toLong(),
            receivedByteCount = readBytes.size.toLong(),
            divergentByteCount = divergentCount
        )
    }

    private fun getRandomBytesToSend(useDatapoint: Boolean) = if (useDatapoint) {
        RandomByteArrayFactory.create(1)
    } else {
        RandomByteArrayFactory.createWord()
    }

    private suspend fun FlowCollector<SampleSmackState>.writeAndReadByteArray(
        useDatapoint: Boolean,
        mailbox: SmackMailbox,
        bytesToSend: ByteArray
    ) = if (useDatapoint) {
        try {
            smackSdk.mailboxApi.writeDataPoint(mailbox, SCRATCH8, bytesToSend, null)
            smackSdk.mailboxApi.readDataPoint(mailbox, SCRATCH8)
        } catch (exception: TagLostException) {
            log("Exception while using data point. Maybe firmware is missing? Exception: ${exception.message}")
            emit(showError = true)
            throw CancellationException()
        }
    } else {
        smackSdk.mailboxApi.writeWord(mailbox, bytesToSend, 0)
        smackSdk.mailboxApi.readWord(mailbox, 0)
    }

    fun resetCount() {
        log("Resetting byte count")
        lastState = lastState.copy(byteCount = ByteCount())
        byteCountStateFlow.value = ByteCount()
    }

    fun setFirmwareToggle(enabled: Boolean) {
        log("Setting firmware toggle to $enabled")
        lastState = lastState.copy(shouldShowMissingFirmware = false)
        firmwareToggleFlow.value = enabled
    }

    private fun log(message: String) {
        smackSdk.smackClient.nfcLogger.logVerbose(message)
    }

    companion object {

        private const val KEEP_ALIVE_INTERVAL: Milliseconds = 100L
    }
}
