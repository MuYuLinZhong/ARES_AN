package com.infineon.smack.sample.smack

fun ByteArray.getDivergentByteCount(other: ByteArray): Int {
    return if (size != other.size) {
        kotlin.math.max(size, other.size)
    } else {
        filterIndexed { index, byte -> byte != other[index] }.count()
    }
}
