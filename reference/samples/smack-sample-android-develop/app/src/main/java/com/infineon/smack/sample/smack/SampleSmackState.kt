package com.infineon.smack.sample.smack

data class SampleSmackState(
    val isConnected: Boolean,
    val byteCount: ByteCount,
    val shouldShowMissingFirmware: Boolean = false,
)
