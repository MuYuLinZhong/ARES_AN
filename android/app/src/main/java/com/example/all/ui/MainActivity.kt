package com.example.all.ui

import android.content.Intent
import android.os.Bundle
import android.view.ViewGroup
import android.webkit.WebResourceRequest
import android.webkit.WebResourceResponse
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
import com.infineon.smack.sdk.SmackSdk
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
 * 3. 委托 SmackSdk 管理 NFC 生命周期（ForegroundDispatch 由 SmackLifecycleObserver 自动处理）
 * 4. 收集 ViewModel 状态变化并推送给 WebView
 */
@AndroidEntryPoint
class MainActivity : AppCompatActivity() {

    private val homeViewModel: HomeViewModel by viewModels()
    private val deviceListViewModel: DeviceListViewModel by viewModels()
    private val addDeviceViewModel: AddDeviceViewModel by viewModels()
    private val settingsViewModel: SettingsViewModel by viewModels()

    @Inject lateinit var smackSdk: SmackSdk

    private lateinit var bridge: AndroidBridge
    private lateinit var webView: WebView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // SmackSdk 注册生命周期观察者，自动管理 NFC ForegroundDispatch
        smackSdk.onCreate(this)

        webView = WebView(this).apply {
            layoutParams = ViewGroup.LayoutParams(
                ViewGroup.LayoutParams.MATCH_PARENT,
                ViewGroup.LayoutParams.MATCH_PARENT
            )
        }
        setContentView(webView)

        webView.settings.apply {
            javaScriptEnabled = true
            domStorageEnabled = true
            allowFileAccess = false
            allowContentAccess = false
        }

        bridge = AndroidBridge(
            homeViewModel = homeViewModel,
            deviceListViewModel = deviceListViewModel,
            addDeviceViewModel = addDeviceViewModel,
            settingsViewModel = settingsViewModel
        )
        bridge.attachWebView(webView)
        webView.addJavascriptInterface(bridge, "AndroidBridge")

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

        if (hasBundledWebApp()) {
            webView.loadUrl("https://appassets.androidplatform.net/assets/web/index.html")
        } else {
            showFallback("Web 应用未打包。请在 app 目录执行 npm install && npm run build:android 后重新运行。")
        }

        observeViewModels()
    }

    /**
     * NFC Intent 转发给 SmackSdk
     * SmackLifecycleObserver 会解析 NfcA Tag 并通过 SmackClient 建立连接
     */
    override fun onNewIntent(intent: Intent) {
        super.onNewIntent(intent)
        smackSdk.onNewIntent(intent)
    }

    private fun observeViewModels() {
        lifecycleScope.launch {
            homeViewModel.uiState.collect { state ->
                when (val op = state.operationState) {
                    is OperationState.InProgress -> bridge.pushLockProgress(op)
                    is OperationState.Success -> bridge.pushLockResult(op)
                    is OperationState.Error -> bridge.pushLockResult(op)
                    is OperationState.Idle -> { }
                }
            }
        }

        lifecycleScope.launch {
            deviceListViewModel.uiState.collect { state ->
                bridge.pushDeviceList(state.filteredDevices, state.isOffline)
            }
        }

        lifecycleScope.launch {
            addDeviceViewModel.uiState.collect { state ->
                bridge.pushNfcScanState(state.nfcScanState)
            }
        }

        lifecycleScope.launch {
            settingsViewModel.uiState.collect { _ ->
                bridge.pushPreferencesSaved(true)
            }
        }
    }

    private fun hasBundledWebApp(): Boolean {
        return try {
            assets.open("web/index.html").close()
            true
        } catch (e: IOException) {
            false
        }
    }

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
