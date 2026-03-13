package com.infineon.smack.sample.smack

import org.junit.jupiter.api.Assertions.*
import org.junit.jupiter.params.ParameterizedTest
import org.junit.jupiter.params.provider.Arguments
import org.junit.jupiter.params.provider.MethodSource
import java.util.stream.Stream

internal class SampleByteArrayExtensionsTest {

    @ParameterizedTest
    @MethodSource("getDivergentByteCountParams")
    fun `getDivergentByteCount should calculate number divergent bytes`(
        first: ByteArray,
        other: ByteArray,
        expectedResult: Int
    ) {
        assertEquals(expectedResult, first.getDivergentByteCount(other))
    }

    companion object {

        @Suppress("unused")
        @JvmStatic
        fun getDivergentByteCountParams(): Stream<Arguments> {
            // first array, other array, expected result int
            return Stream.of(
                // both empty:
                Arguments.arguments(byteArrayOf(), byteArrayOf(), 0),
                // first empty:
                Arguments.arguments(byteArrayOf(0x00), byteArrayOf(), 1),
                // other empty:
                Arguments.arguments(byteArrayOf(), byteArrayOf(0x00), 1),
                // both equal:
                Arguments.arguments(
                    byteArrayOf(0x00, 0xfe.toByte()),
                    byteArrayOf(0x00, 0xfe.toByte()),
                    0
                ),
                // switched positions:
                Arguments.arguments(
                    byteArrayOf(0x00, 0xfe.toByte()),
                    byteArrayOf(0xfe.toByte(), 0x00.toByte()),
                    2
                ),
                // first longer:
                Arguments.arguments(
                    byteArrayOf(0x00, 0xfe.toByte(), 0xfe.toByte()),
                    byteArrayOf(0x00, 0xfe.toByte()),
                    3
                ),
                // other longer:
                Arguments.arguments(
                    byteArrayOf(0x00, 0xfe.toByte()),
                    byteArrayOf(0x00, 0xfe.toByte(), 0xfe.toByte()),
                    3
                ),
                // 1 differs:
                Arguments.arguments(
                    byteArrayOf(0x01, 0x02, 0x03, 0x04),
                    byteArrayOf(0x01, 0x02, 0x03, 0x66),
                    1
                ),
                // 2 differ:
                Arguments.arguments(
                    byteArrayOf(0x01, 0x02, 0x03, 0x04),
                    byteArrayOf(0x66, 0x02, 0x03, 0x66),
                    2
                ),
                // 3 differ:
                Arguments.arguments(
                    byteArrayOf(0x01, 0x02, 0x03, 0x04),
                    byteArrayOf(0x66, 0x66, 0x03, 0x66),
                    3
                ),
                // 4 differ:
                Arguments.arguments(
                    byteArrayOf(0x01, 0x02, 0x03, 0x04),
                    byteArrayOf(0x66, 0x66, 0x66, 0x66),
                    4
                ),
                // connection broke down in between:
                Arguments.arguments(
                    byteArrayOf(0x01, 0x02, 0x03, 0x04),
                    byteArrayOf(),
                    4
                ),
            )
        }
    }
}
