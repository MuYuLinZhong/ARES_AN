package com.example.all

import android.app.Application
import dagger.hilt.android.HiltAndroidApp

/**
 * 应用入口类 —— Hilt 依赖注入的根容器
 *
 * @HiltAndroidApp 触发 Hilt 代码生成：
 * 1. 创建应用级依赖容器（SingletonComponent）
 * 2. 自动发现所有 @Module 和 @InstallIn 标注的模块
 * 3. 为所有 @HiltViewModel、@AndroidEntryPoint 提供依赖
 *
 * 需要在 AndroidManifest.xml 中注册：android:name=".AresApplication"
 */
@HiltAndroidApp
class AresApplication : Application()
