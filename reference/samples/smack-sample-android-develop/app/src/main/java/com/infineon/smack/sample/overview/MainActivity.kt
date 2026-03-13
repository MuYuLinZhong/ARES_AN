package com.infineon.smack.sample.overview

import android.annotation.SuppressLint
import android.content.Intent
import android.os.Bundle
import androidx.activity.viewModels
import androidx.annotation.StringRes
import androidx.annotation.VisibleForTesting
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.isVisible
import androidx.lifecycle.Lifecycle
import androidx.lifecycle.lifecycleScope
import androidx.lifecycle.repeatOnLifecycle
import com.infineon.smack.sample.R
import com.infineon.smack.sample.databinding.MainActivityBinding
import com.infineon.smack.sdk.SmackSdk
import dagger.hilt.android.AndroidEntryPoint
import kotlinx.coroutines.launch
import javax.inject.Inject

@AndroidEntryPoint
class MainActivity : AppCompatActivity() {

    private val viewModel: MainViewModel by viewModels()
    private lateinit var binding: MainActivityBinding

    @VisibleForTesting
    @Inject
    lateinit var smackSdk: SmackSdk

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = MainActivityBinding.inflate(layoutInflater)
        setContentView(binding.root)
        setTitle(R.string.app_title)
        binding.resetButton.setOnClickListener { viewModel.resetCount() }
        binding.firmwareSwitch.setOnCheckedChangeListener { _, checked ->
            viewModel.setFirmwareToggle(checked)
        }
        lifecycleScope.launch {
            lifecycle.repeatOnLifecycle(Lifecycle.State.STARTED) {
                viewModel.viewState.collect(::updateViewState)
            }
        }
        smackSdk.onCreate(this)
    }

    private fun showAlert(@StringRes titleRes: Int, @StringRes messageRes: Int) {
        smackSdk.isEnabled = false
        runOnUiThread {
            AlertDialog.Builder(this)
                .setTitle(titleRes)
                .setMessage(messageRes)
                .setNeutralButton(R.string.ok) { dialogInterface, _ ->
                    dialogInterface.dismiss()
                }
                .setOnDismissListener {
                    smackSdk.isEnabled = true
                }
                .show()
        }
    }

    @SuppressLint("MissingSuperCall")
    public override fun onNewIntent(intent: Intent?) {
        super.onNewIntent(intent)
        smackSdk.onNewIntent(intent)
    }

    private fun updateViewState(state: MainViewState) = with(binding) {
        showConnection(state.isConnected)
        sentValueTextView.text = state.sentBytesCount.toString()
        receivedValueTextView.text = state.receivedBytesCount.toString()
        divergentValueTextView.text = state.divergentBytesCount.toString()
        firmwareText.isVisible = state.showMissingFirmwareText
        if (state.alertTitle != null && state.alertMessage != null) {
            showAlert(state.alertTitle, state.alertMessage)
        }
    }

    private fun MainActivityBinding.showConnection(isConnected: Boolean) {
        connectedImageView.isVisible = isConnected
        connectedTextView.isVisible = isConnected
        disconnectedImageView.isVisible = !isConnected
        disconnectedTextView.isVisible = !isConnected
    }
}
