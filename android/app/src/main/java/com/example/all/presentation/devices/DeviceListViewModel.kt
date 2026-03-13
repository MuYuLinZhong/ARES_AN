package com.example.all.presentation.devices

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.example.all.domain.model.Device
import com.example.all.domain.usecase.device.GetDeviceListUseCase
import com.example.all.domain.usecase.device.SearchDevicesUseCase
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch
import javax.inject.Inject

/**
 * 设备列表 ViewModel —— 管理设备列表展示和搜索过滤
 *
 * Phase 1 功能：
 * 1. 从 Room 读取设备列表（通过 Flow 自动刷新）
 * 2. 按昵称关键词本地搜索过滤
 *
 * Phase 2 演进：新增 Cache-Then-Network（先展示缓存，后台拉云端更新）。
 *
 * 数据流：Room Flow → GetDeviceListUseCase → DeviceListViewModel → AndroidBridge → WebView
 */
@HiltViewModel
class DeviceListViewModel @Inject constructor(
    private val getDeviceListUseCase: GetDeviceListUseCase,
    private val searchDevicesUseCase: SearchDevicesUseCase
) : ViewModel() {

    private val _uiState = MutableStateFlow(DeviceListUiState())
    val uiState: StateFlow<DeviceListUiState> = _uiState.asStateFlow()

    init {
        // 订阅 Room 设备列表 Flow，数据变化时自动更新 UI 状态
        viewModelScope.launch {
            getDeviceListUseCase().collect { devices ->
                _uiState.update { state ->
                    state.copy(
                        allDevices = devices,
                        filteredDevices = searchDevicesUseCase(devices, state.searchKeyword)
                    )
                }
            }
        }
        loadDevices()
    }

    /**
     * 加载设备列表
     * Phase 1：只读 Room（Flow 已在 init 中订阅，此处仅控制 loading 状态）
     * Phase 2：发起网络请求拉取云端设备列表
     */
    fun loadDevices() {
        viewModelScope.launch {
            _uiState.update { it.copy(isLoading = true) }
            // Phase 1：Room Flow 已自动推送数据，无需额外操作
            // TODO Phase 2: deviceRepository.fetchAndCacheDevices()
            _uiState.update { it.copy(isLoading = false) }
        }
    }

    /**
     * 搜索关键词变化 —— 实时过滤设备列表
     * @param keyword 用户输入的搜索文本
     */
    fun onSearchKeywordChanged(keyword: String) {
        _uiState.update { state ->
            state.copy(
                searchKeyword = keyword,
                filteredDevices = searchDevicesUseCase(state.allDevices, keyword)
            )
        }
    }
}

/**
 * 设备列表 UI 状态
 * @property allDevices      全量设备列表（Room 数据源）
 * @property filteredDevices  搜索过滤后的设备列表（展示给 UI）
 * @property searchKeyword    当前搜索关键词
 * @property isLoading        是否正在加载（Phase 2 网络请求时使用）
 * @property isOffline        是否离线（Phase 2 断网时使用）
 */
data class DeviceListUiState(
    val allDevices: List<Device> = emptyList(),
    val filteredDevices: List<Device> = emptyList(),
    val searchKeyword: String = "",
    val isLoading: Boolean = false,
    val isOffline: Boolean = false
)
