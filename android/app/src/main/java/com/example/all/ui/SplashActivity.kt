package com.example.all.ui

import android.content.Intent
import android.os.Bundle
import android.provider.Settings
import androidx.activity.viewModels
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.lifecycleScope
import com.example.all.presentation.splash.SplashViewModel
import com.example.all.presentation.splash.StartupUiState
import dagger.hilt.android.AndroidEntryPoint
import kotlinx.coroutines.launch

/**
 * 启动页 Activity —— App 冷启动时的第一个页面
 *
 * 职责：
 * 1. 收集 SplashViewModel 的 UI 状态
 * 2. 根据状态执行跳转或显示弹窗：
 *    - GoToHome → 跳转 MainActivity（首页）
 *    - NfcNotAvailable → 弹窗"此设备不支持 NFC"→ 退出
 *    - NfcDisabled → 弹窗"请先开启 NFC"→ 跳转系统 NFC 设置
 *
 * Phase 2 演进：增加 GoToLogin 状态 → 跳转登录页。
 */
@AndroidEntryPoint
class SplashActivity : AppCompatActivity() {

    /** 通过 Hilt 注入的 ViewModel */
    private val viewModel: SplashViewModel by viewModels()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // 收集 ViewModel 状态流，根据状态做出响应
        lifecycleScope.launch {
            viewModel.uiState.collect { state ->
                when (state) {
                    is StartupUiState.Checking -> {
                        // 检测中，等待
                    }
                    is StartupUiState.GoToHome -> {
                        // NFC 可用，跳转首页
                        startActivity(Intent(this@SplashActivity, MainActivity::class.java))
                        finish()
                    }
                    is StartupUiState.GoToLogin -> {
                        // TODO Phase 2：跳转登录页
                        // startActivity(Intent(this@SplashActivity, LoginActivity::class.java))
                        // finish()
                    }
                    is StartupUiState.NfcNotAvailable -> {
                        showNfcNotAvailableDialog()
                    }
                    is StartupUiState.NfcDisabled -> {
                        showNfcDisabledDialog()
                    }
                }
            }
        }
    }

    /** 用户从系统 NFC 设置返回后重新检测 */
    override fun onResume() {
        super.onResume()
        viewModel.recheckNfc()
    }

    /**
     * 弹窗：此设备不支持 NFC
     * 点击"确定"后退出应用
     */
    private fun showNfcNotAvailableDialog() {
        AlertDialog.Builder(this)
            .setTitle("不支持 NFC")
            .setMessage("此设备没有 NFC 功能，无法使用本应用。")
            .setCancelable(false)
            .setPositiveButton("确定") { _, _ -> finish() }
            .show()
    }

    /**
     * 弹窗：NFC 未开启
     * "去设置"→ 跳转系统 NFC 设置页
     * "取消"→ 退出应用
     */
    private fun showNfcDisabledDialog() {
        AlertDialog.Builder(this)
            .setTitle("NFC 未开启")
            .setMessage("请先在系统设置中开启 NFC 功能。")
            .setCancelable(false)
            .setPositiveButton("去设置") { _, _ ->
                // 跳转到系统 NFC 设置页面
                startActivity(Intent(Settings.ACTION_NFC_SETTINGS))
            }
            .setNegativeButton("取消") { _, _ -> finish() }
            .show()
    }
}
