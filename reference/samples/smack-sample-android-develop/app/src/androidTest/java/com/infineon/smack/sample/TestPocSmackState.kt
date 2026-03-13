package com.infineon.smack.sample

import com.infineon.smack.sample.smack.ByteCount
import com.infineon.smack.sample.smack.SampleSmackState

object TestPocSmackState {

    val connectedState = SampleSmackState(true, ByteCount())
    val disconnectedState = SampleSmackState(false, ByteCount())
}
