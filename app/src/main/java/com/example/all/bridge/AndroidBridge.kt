package com.example.all.bridge

import android.os.Handler
import android.os.Looper
import android.util.Log
import android.webkit.JavascriptInterface
import android.webkit.WebView
import com.example.all.domain.model.OperationState
import com.example.all.domain.model.OperationType
import com.example.all.presentation.adddevice.AddDeviceViewModel
import com.example.all.presentation.adddevice.NfcScanState
import com.example.all.presentation.devices.DeviceListViewModel
import com.example.all.presentation.home.HomeViewModel
import com.example.all.presentation.settings.SettingsViewModel
import org.json.JSONArray
import org.json.JSONObject

/**
 * WebView JS↔Native 桥接器 —— 连接前端 React 页面与 Android 原生业务逻辑
 *
 * 职责：
 * 1. 接收前端 JS 调用（@JavascriptInterface 方法）并分发给对应 ViewModel
 * 2. 将 ViewModel 状态变化推送回前端（evaluateJavascript 调用 window.onXxx 回调）
 *
 * Phase 1 实现 10 个接口（开/关锁、设备管理、偏好设置相关）
 * Phase 2 stub 10 个接口（登录/登出、权限管理等，只打日志不执行）
 *
 * 注册方式：MainActivity 中调用 webView.addJavascriptInterface(bridge, "AndroidBridge")
 * 前端调用：window.AndroidBridge.onUnlockClicked(JSON.stringify({deviceId, deviceName}))
 */
class AndroidBridge(
    private val homeViewModel: HomeViewModel,
    private val deviceListViewModel: DeviceListViewModel,
    private val addDeviceViewModel: AddDeviceViewModel,
    private val settingsViewModel: SettingsViewModel
) {
    companion object {
        private const val TAG = "AndroidBridge"
    }

    /** WebView 实例引用，用于 evaluateJavascript 推送数据到前端 */
    private lateinit var webView: WebView

    /** 绑定 WebView 实例 —— 在 MainActivity 注册 Bridge 后调用 */
    fun attachWebView(wv: WebView) {
        webView = wv
    }

    // ===================== Phase 1 实现：开/关锁 =====================

    /**
     * 开锁 —— 前端点击 Unlock 按钮时调用
     * JSON 参数: { "deviceId": "NAC1080-A1B2C3", "deviceName": "办公室门锁" }
     */
    @JavascriptInterface
    fun onUnlockClicked(json: String) {
        val obj = JSONObject(json)
        val deviceId = obj.getString("deviceId")
        val deviceName = obj.optString("deviceName", "")
        mainThread {
            // 先记录操作意图，等待 NFC Tag 到达后执行
            homeViewModel.requestOperation(deviceId, OperationType.Unlock)
        }
    }

    /**
     * 关锁 —— 前端点击 Lock 按钮时调用
     * JSON 参数: { "deviceId": "NAC1080-A1B2C3", "deviceName": "办公室门锁" }
     */
    @JavascriptInterface
    fun onLockClicked(json: String) {
        val obj = JSONObject(json)
        val deviceId = obj.getString("deviceId")
        mainThread {
            homeViewModel.requestOperation(deviceId, OperationType.Lock)
        }
    }

    /** 取消操作 —— 前端点击 Cancel 按钮时调用 */
    @JavascriptInterface
    fun onOperationCancelled() {
        mainThread { homeViewModel.onOperationCancelled() }
    }

    // ===================== Phase 1 实现：设备管理 =====================

    /** 刷新设备列表 —— 前端下拉刷新或进入设备页时调用 */
    @JavascriptInterface
    fun onRefreshDeviceList() {
        mainThread { deviceListViewModel.loadDevices() }
    }

    /**
     * 搜索关键词变化 —— 前端搜索框输入时实时调用
     * JSON 参数: { "keyword": "办公室" }
     */
    @JavascriptInterface
    fun onSearchKeywordChanged(json: String) {
        val obj = JSONObject(json)
        val keyword = obj.getString("keyword")
        mainThread { deviceListViewModel.onSearchKeywordChanged(keyword) }
    }

    /**
     * 从设备列表快捷开锁
     * JSON 参数: { "deviceId": "NAC1080-A1B2C3", "deviceName": "办公室门锁" }
     */
    @JavascriptInterface
    fun onUnlockFromList(json: String) {
        val obj = JSONObject(json)
        val deviceId = obj.getString("deviceId")
        mainThread {
            homeViewModel.requestOperation(deviceId, OperationType.Unlock)
        }
    }

    /**
     * 开始 NFC 扫描添加设备
     * JSON 参数: { "nickname": "办公室门锁" }
     */
    @JavascriptInterface
    fun onStartNfcScan(json: String) {
        val obj = JSONObject(json)
        val nickname = obj.getString("nickname")
        mainThread { addDeviceViewModel.startNfcScan(nickname) }
    }

    /** 取消 NFC 扫描 */
    @JavascriptInterface
    fun onCancelNfcScan() {
        mainThread { addDeviceViewModel.cancelScan() }
    }

    // ===================== Phase 1 实现：偏好设置 =====================

    /**
     * 切换震动开关
     * JSON 参数: { "enabled": true }
     */
    @JavascriptInterface
    fun onToggleVibration(json: String) {
        val obj = JSONObject(json)
        val enabled = obj.getBoolean("enabled")
        mainThread { settingsViewModel.onToggleVibration(enabled) }
    }

    /**
     * 修改 NFC 灵敏度
     * JSON 参数: { "level": "High" }
     */
    @JavascriptInterface
    fun onNfcSensitivityChanged(json: String) {
        val obj = JSONObject(json)
        val level = obj.getString("level")
        mainThread { settingsViewModel.onNfcSensitivityChanged(level) }
    }

    // ===================== Phase 2 Stub =====================

    @JavascriptInterface
    fun onLoginClicked(json: String) {
        Log.d(TAG, "onLoginClicked: Phase 2 未实现, json=$json")
    }

    @JavascriptInterface
    fun onLogout() {
        Log.d(TAG, "onLogout: Phase 2 未实现")
    }

    @JavascriptInterface
    fun onUpdatePasswordClicked(json: String) {
        Log.d(TAG, "onUpdatePasswordClicked: Phase 2 未实现, json=$json")
    }

    @JavascriptInterface
    fun onDeleteAccountClicked() {
        Log.d(TAG, "onDeleteAccountClicked: Phase 2 未实现")
    }

    @JavascriptInterface
    fun onLoadDeviceDetail(json: String) {
        Log.d(TAG, "onLoadDeviceDetail: Phase 2 未实现, json=$json")
    }

    @JavascriptInterface
    fun onInviteUser(json: String) {
        Log.d(TAG, "onInviteUser: Phase 2 未实现, json=$json")
    }

    @JavascriptInterface
    fun onRevokeUser(json: String) {
        Log.d(TAG, "onRevokeUser: Phase 2 未实现, json=$json")
    }

    @JavascriptInterface
    fun confirmRevokeUser() {
        Log.d(TAG, "confirmRevokeUser: Phase 2 未实现")
    }

    @JavascriptInterface
    fun onRemoveDevice(json: String) {
        Log.d(TAG, "onRemoveDevice: Phase 2 未实现, json=$json")
    }

    @JavascriptInterface
    fun onToggleOnlineMode(json: String) {
        Log.d(TAG, "onToggleOnlineMode: Phase 2 未实现, json=$json")
    }

    // ===================== Native → JS 推送方法 =====================

    /**
     * 推送开/关锁进度到前端
     * 前端回调: window.onLockProgress({ progress: 65, stepText: "正在加密验证..." })
     */
    fun pushLockProgress(state: OperationState.InProgress) {
        val json = JSONObject().apply {
            put("progress", state.progress)
            put("stepText", state.stepText)
        }
        pushToJs("onLockProgress", json.toString())
    }

    /**
     * 推送开/关锁结果到前端
     * 前端回调: window.onLockOperationResult({ success: true, message: "开锁成功" })
     */
    fun pushLockResult(state: OperationState) {
        val json = JSONObject()
        when (state) {
            is OperationState.Success -> {
                json.put("success", true)
                json.put("message", state.message)
            }
            is OperationState.Error -> {
                json.put("success", false)
                json.put("message", state.message)
                json.put("isRetryable", state.isRetryable)
            }
            else -> return
        }
        pushToJs("onLockOperationResult", json.toString())
    }

    /**
     * 推送 NFC 状态变化到前端
     * 前端回调: window.onNfcStatusChanged({ enabled: false })
     */
    fun pushNfcStatus(enabled: Boolean) {
        val json = JSONObject().put("enabled", enabled)
        pushToJs("onNfcStatusChanged", json.toString())
    }

    /**
     * 推送设备列表到前端
     * 前端回调: window.onDevicesLoaded({ devices: [...], isOffline: false })
     */
    fun pushDeviceList(
        devices: List<com.example.all.domain.model.Device>,
        isOffline: Boolean = false
    ) {
        val devicesArray = JSONArray()
        devices.forEach { device ->
            devicesArray.put(JSONObject().apply {
                put("deviceId", device.deviceId)
                put("nickname", device.nickname)
                put("serialNo", device.serialNo)
                put("isValid", device.isValid)
                put("lastSyncAt", device.lastSyncAt)
            })
        }
        val json = JSONObject().apply {
            put("devices", devicesArray)
            put("isOffline", isOffline)
        }
        pushToJs("onDevicesLoaded", json.toString())
    }

    /**
     * 推送 NFC 扫描状态到前端
     * 前端回调: window.onNfcScanStateChanged({ state: "scanning" })
     */
    fun pushNfcScanState(state: NfcScanState) {
        val json = JSONObject()
        when (state) {
            is NfcScanState.Idle -> json.put("state", "idle")
            is NfcScanState.Scanning -> json.put("state", "scanning")
            is NfcScanState.Connecting -> json.put("state", "connecting")
            is NfcScanState.Success -> {
                json.put("state", "success")
                json.put("deviceId", state.deviceId)
            }
            is NfcScanState.Error -> {
                json.put("state", "error")
                json.put("message", state.message)
            }
        }
        pushToJs("onNfcScanStateChanged", json.toString())
    }

    /**
     * 推送偏好保存结果到前端
     * 前端回调: window.onPreferencesSaved({ success: true })
     */
    fun pushPreferencesSaved(success: Boolean) {
        val json = JSONObject().put("success", success)
        pushToJs("onPreferencesSaved", json.toString())
    }

    // ===================== 通用工具 =====================

    /**
     * 向 WebView 推送 JS 回调
     * 在主线程执行 evaluateJavascript，调用 window.{callbackName}({jsonPayload})
     */
    fun pushToJs(callbackName: String, jsonPayload: String) {
        mainThread {
            if (::webView.isInitialized) {
                webView.evaluateJavascript(
                    "window.$callbackName($jsonPayload)", null
                )
            }
        }
    }

    /** 切换到主线程执行（WebView 操作必须在主线程） */
    private fun mainThread(block: () -> Unit) {
        Handler(Looper.getMainLooper()).post(block)
    }
}
