package com.infineon.smack.sample.smack

data class ByteCount(
    val sentByteCount: Long = 0,
    val receivedByteCount: Long = 0,
    val divergentByteCount: Long = 0
) {

    operator fun plus(other: ByteCount): ByteCount {
        return ByteCount(
            sentByteCount = sentByteCount + other.sentByteCount,
            receivedByteCount = receivedByteCount + other.receivedByteCount,
            divergentByteCount = divergentByteCount + other.divergentByteCount
        )
    }
}
