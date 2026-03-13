# Infineon NFC Tag Side Controller (Smack) Sample App

This is a small sample application to test the connection to a SmAcK NAC1080.

It uses the Infineon NFC Tag Side Controller (Smack) SDK for Android (local AAR file) to connect to the chip and write and read words or
data points to the Smack Mailbox.

To write and read words you do not need any firmware on your chip. To write and read the Scratch8
data point you need to create a firmware with the SmAcK Firmware SDK.

## Usage

The following snippets may be useful for Android App developers:

### Using MailboxApi to query UID, Firmware, ChargeLevel or to call an AppFunction:

```kotlin
suspend fun testUid(mailbox: SmackMailbox) {
    log("Reading UID...")
    val uid = smackSdk.mailboxApi.getUid(mailbox)
    log("UID: $uid")
}

suspend fun testReadFirmware(mailbox: SmackMailbox) {
    log("Reading firmware name...")
    val firmwareName = smackSdk.mailboxApi.getFirmwareName(mailbox, testLockKey)
    log("Reading firmware version...")
    val firmwareVersion = smackSdk.mailboxApi.getFirmwareVersion(mailbox, testLockKey)
    log("Firmware: $firmwareName $firmwareVersion")
}

suspend fun testGetChargeLevel(mailbox: SmackMailbox) {
    log("Reading charge level...")
    val chargeLevel = smackSdk.mailboxApi.getChargeLevel(mailbox, testLockKey)
    log("Charge level: ${chargeLevel.percentage}")
}

suspend fun testCallAppFunction(mailbox: SmackMailbox) {
    log("Calling app function 15 with random word...")
    val responseBytes = smackSdk.mailboxApi.callAppFunction(
        mailbox,
        CallAppFunctionIndexValidator.LAST_INDEX.toByte(),
        RandomByteArrayFactory.createWord()
    ).joined()
    log("Response to call app function: ${responseBytes.toHexString()}")
}
```

### Querying Measurement Api:

Caution: MeasurementApi needs a SmAcK NAC 1080 with Firmware from Infineon.

```kotlin
suspend fun testMeasurements(mailbox: SmackMailbox) {
    log("Reading temperature...")
    val temperature = smackSdk.measurementApi.getTemperature(mailbox)
    log("Temperature: $temperature °C")
    log("Reading humidity...")
    val humidity = smackSdk.measurementApi.getHumidity(mailbox)
    log("Humidity: $humidity %")
    log("Reading pressure...")
    val pressure = smackSdk.measurementApi.getPressure(mailbox)
    log("Pressure: $pressure bar")
    log("Reading value reserved for future use...")
    val reservedValue = smackSdk.measurementApi.getReserved(mailbox)
    log("Value reserved for future use: $reservedValue")
}
```

## License

    Copyright 2022 Infineon Technologies AG
