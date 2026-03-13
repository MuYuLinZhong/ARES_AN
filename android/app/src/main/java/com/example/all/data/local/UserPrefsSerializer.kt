package com.example.all.data.local

import androidx.datastore.core.CorruptionException
import androidx.datastore.core.Serializer
import com.example.all.UserPrefs
import com.google.protobuf.InvalidProtocolBufferException
import java.io.InputStream
import java.io.OutputStream

/**
 * UserPrefs Proto 序列化器 —— DataStore 读写时使用
 *
 * 负责将 UserPrefs protobuf 消息序列化/反序列化为字节流。
 * DataStore 内部在文件 I/O 时自动调用此序列化器。
 *
 * 默认值：震动开启、NFC 灵敏度 Medium、无本地密钥（使用 BuildConfig 默认值）
 */
object UserPrefsSerializer : Serializer<UserPrefs> {

    override val defaultValue: UserPrefs = UserPrefs.newBuilder()
        .setVibrationEnabled(true)
        .setNfcSensitivity("Medium")
        .setLocalDeviceKeyHex("")
        .build()

    /** 从输入流反序列化 protobuf 消息 */
    override suspend fun readFrom(input: InputStream): UserPrefs {
        try {
            return UserPrefs.parseFrom(input)
        } catch (e: InvalidProtocolBufferException) {
            throw CorruptionException("无法解析 UserPrefs proto 数据", e)
        }
    }

    /** 将 protobuf 消息序列化写入输出流 */
    override suspend fun writeTo(t: UserPrefs, output: OutputStream) {
        t.writeTo(output)
    }
}
