package com.example.all.ui

import android.app.PendingIntent
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.nfc.NfcAdapter
import android.nfc.Tag
import android.os.Build
import android.os.Bundle
import android.view.ViewGroup
import android.webkit.WebResourceRequest
import android.webkit.WebResourceResponse
import android.webkit.WebSettings
import android.webkit.WebView
import androidx.activity.viewModels
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.lifecycleScope
import androidx.webkit.WebViewAssetLoader
import androidx.webkit.WebViewClientCompat
import com.example.all.bridge.AndroidBridge
import com.example.all.domain.model.OperationState
import com.example.all.presentation.adddevice.AddDeviceViewModel
import com.example.all.presentation.devices.DeviceListViewModel
import com.example.all.presentation.home.HomeViewModel
import com.example.all.presentation.settings.SettingsViewModel
import dagger.hilt.android.AndroidEntryPoint
import kotlinx.coroutines.launch
import java.io.IOException
import javax.inject.Inject

/**
 * 主 Activity —— 承载 WebView 的应用壳
 *
 * 核心职责：
 * 1. 加载本地 React/TSX Web 应用（assets/web/index.html）
 * 2. 注册 AndroidBridge 连接 JS 与 Native
 * 3. 管理 NFC ForegroundDispatch（前台优先接收 NFC Tag）
 * 4. 监听 NFC 状态广播（开/关变化通知前端）
 * 5. 收集 ViewModel 状态变化并推送给 WebView
 *
 * Phase 1：完整实现以上所有职责
 * Phase 2：增加 ForceLogout 监听、NetworkMonitor 等
 */
@AndroidEntryPoint
class MainActivity : AppCompatActivity() {

    // ====== Hilt 注入的 ViewModel ======
    private val homeViewModel: HomeViewModel by viewModels()
    private val deviceListViewModel: DeviceListViewModel by viewModels()
    private val addDeviceViewModel: AddDeviceViewModel by viewModels()
    private val settingsViewModel: SettingsViewModel by viewModels()

    /** NFC 适配器 */
    @Inject lateinit var nfcAdapterProvider: dagger.Lazy<NfcAdapter?>
    private val nfcAdapter: NfcAdapter? get() = nfcAdapterProvider.get()

    /** JS↔Native 桥接器 */
    private lateinit var bridge: AndroidBridge

    /** WebView 实例 */
    private lateinit var webView: WebView

    /** NFC ForegroundDispatch 的 PendingIntent */
    private var nfcPendingIntent: PendingIntent? = null

    /**
     * NFC 状态变化广播接收器
     * 监听 NFC 开/关事件，通知前端更新按钮状态
     */
    private val nfcStateReceiver = object : BroadcastReceiver() {
        override fun onReceive(context: Context, intent: Intent) {
            if (intent.action == NfcAdapter.ACTION_ADAPTER_STATE_CHANGED) {
                val state = intent.getIntExtra(NfcAdapter.EXTRA_ADAPTER_STATE, NfcAdapter.STATE_OFF)
                val enabled = state == NfcAdapter.STATE_ON
                bridge.pushNfcStatus(enabled)
            }
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // 初始化 WebView
        webView = WebView(this).apply {
            layoutParams = ViewGroup.LayoutParams(
                ViewGroup.LayoutParams.MATCH_PARENT,
                ViewGroup.LayoutParams.MATCH_PARENT
            )
        }
        setContentView(webView)

        // 配置 WebView
        webView.settings.apply {
            javaScriptEnabled = true
            domStorageEnabled = true
            // 允许使用 WebViewAssetLoader 加载本地资源
            allowFileAccess = false
            allowContentAccess = false
        }

        // 初始化 Bridge 并注册到 WebView
        bridge = AndroidBridge(
            homeViewModel = homeViewModel,
            deviceListViewModel = deviceListViewModel,
            addDeviceViewModel = addDeviceViewModel,
            settingsViewModel = settingsViewModel
        )
        bridge.attachWebView(webView)
        webView.addJavascriptInterface(bridge, "AndroidBridge")

        // 设置 WebViewAssetLoader 加载本地 assets/web/ 目录
        val assetLoader = WebViewAssetLoader.Builder()
            .addPathHandler("/assets/", WebViewAssetLoader.AssetsPathHandler(this))
            .build()

        webView.webViewClient = object : WebViewClientCompat() {
            override fun shouldInterceptRequest(
                view: WebView,
                request: WebResourceRequest
            ): WebResourceResponse? {
                return assetLoader.shouldInterceptRequest(request.url)
            }
        }

        // 加载本地 Web 应用
        if (hasBundledWebApp()) {
            webView.loadUrl("https://appassets.androidplatform.net/assets/web/index.html")
        } else {
            showFallback("Web 应用未打包。请在 app 目录执行 npm install && npm run build:android 后重新运行。")
        }

        // 初始化 NFC ForegroundDispatch
        setupNfcForegroundDispatch()

        // 注册 NFC 状态广播监听
        registerNfcStateReceiver()

        // 收集 ViewModel 状态变化并推送到 WebView
        observeViewModels()
    }

    /**
     * 设置 NFC ForegroundDispatch
     *
     * 让当前 Activity 在前台时优先接收 NFC Tag（而非让系统弹出选择器）。
     * 这是 NFC 应用的标准模式，确保用户靠近门锁时 App 能立即响应。
     */
    private fun setupNfcForegroundDispatch() {
        val intent = Intent(this, javaClass).addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP)
        val flags = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_MUTABLE
        } else {
            PendingIntent.FLAG_UPDATE_CURRENT
        }
        nfcPendingIntent = PendingIntent.getActivity(this, 0, intent, flags)
    }

    /**
     * 注册 NFC 状态变化广播
     * 监听 ACTION_ADAPTER_STATE_CHANGED 以感知用户在系统设置中开/关 NFC
     */
    private fun registerNfcStateReceiver() {
        val filter = IntentFilter(NfcAdapter.ACTION_ADAPTER_STATE_CHANGED)
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            registerReceiver(nfcStateReceiver, filter, RECEIVER_NOT_EXPORTED)
        } else {
            registerReceiver(nfcStateReceiver, filter)
        }
    }

    /**
     * 收集所有 ViewModel 的状态变化，通过 Bridge 推送到前端
     *
     * 使用 lifecycleScope.launch 确保只在 Activity 存活时收集，
     * Activity 销毁时自动取消（避免内存泄漏）。
     */
    private fun observeViewModels() {
        // 开/关锁状态 → 前端进度条/结果
        lifecycleScope.launch {
            homeViewModel.uiState.collect { state ->
                when (val op = state.operationState) {
                    is OperationState.InProgress -> bridge.pushLockProgress(op)
                    is OperationState.Success -> bridge.pushLockResult(op)
                    is OperationState.Error -> bridge.pushLockResult(op)
                    is OperationState.Idle -> { /* 空闲状态不推送 */ }
                }
            }
        }

        // 设备列表 → 前端设备页
        lifecycleScope.launch {
            deviceListViewModel.uiState.collect { state ->
                bridge.pushDeviceList(state.filteredDevices, state.isOffline)
            }
        }

        // NFC 扫描状态 → 前端添加设备页
        lifecycleScope.launch {
            addDeviceViewModel.uiState.collect { state ->
                bridge.pushNfcScanState(state.nfcScanState)
            }
        }

        // 偏好变化 → 前端设置页
        lifecycleScope.launch {
            settingsViewModel.uiState.collect { _ ->
                bridge.pushPreferencesSaved(true)
            }
        }
    }

    /**
     * NFC Tag 到达 —— ForegroundDispatch 检测到 NFC Tag 时系统调用
     *
     * 将 Tag 分发给：
     * 1. HomeViewModel（如有待处理的开/关锁操作）
     * 2. AddDeviceViewModel（如正在扫描添加设备）
     */
    override fun onNewIntent(intent: Intent) {
        super.onNewIntent(intent)
        if (NfcAdapter.ACTION_TECH_DISCOVERED == intent.action ||
            NfcAdapter.ACTION_TAG_DISCOVERED == intent.action ||
            NfcAdapter.ACTION_NDEF_DISCOVERED == intent.action
        ) {
            val tag = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                intent.getParcelableExtra(NfcAdapter.EXTRA_TAG, Tag::class.java)
            } else {
                @Suppress("DEPRECATION")
                intent.getParcelableExtra(NfcAdapter.EXTRA_TAG)
            }

            tag?.let {
                // 分发给 HomeViewModel（开/关锁）
                homeViewModel.onNfcTagDiscovered(it)
                // 分发给 AddDeviceViewModel（添加设备）
                addDeviceViewModel.onNfcTagDiscovered(it)
            }
        }
    }

    /** 前台时启用 NFC ForegroundDispatch */
    override fun onResume() {
        super.onResume()
        nfcAdapter?.enableForegroundDispatch(
            this,
            nfcPendingIntent,
            null, // IntentFilter 为空 = 接收所有类型的 NFC Tag
            null  // techLists 为空 = 不限制技术类型
        )
    }

    /** 后台时禁用 NFC ForegroundDispatch */
    override fun onPause() {
        super.onPause()
        nfcAdapter?.disableForegroundDispatch(this)
    }

    override fun onDestroy() {
        super.onDestroy()
        unregisterReceiver(nfcStateReceiver)
    }

    /** 检查 assets/web/index.html 是否存在 */
    private fun hasBundledWebApp(): Boolean {
        return try {
            assets.open("web/index.html").close()
            true
        } catch (e: IOException) {
            false
        }
    }

    /** 显示错误占位页 */
    private fun showFallback(message: String) {
        webView.loadDataWithBaseURL(
            null,
            """
            <html><body style='font-family:sans-serif;padding:24px;'>
            <h2>无法加载应用</h2>
            <p>${escapeHtml(message)}</p>
            </body></html>
            """.trimIndent(),
            "text/html",
            "UTF-8",
            null
        )
    }

    private fun escapeHtml(text: String): String =
        text.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")
}
