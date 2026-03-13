package com.infineon.smack.sample.smack

object TestPocSmackState {

    fun connected() = any().copy(isConnected = true)
    fun disconnected() = any().copy(isConnected = false)

    fun any() = SampleSmackState(isConnected = false, ByteCount())
}
