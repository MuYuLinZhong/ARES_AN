你是一个经验丰富的Android架构师，现在帮我重构一个现有的Android项目。项目当前情况：有静态页面（UI）已经能运行和跳转，但全是假数据和写死的内容，没有真正业务逻辑。现在需要用MVVM模式 + Clean Architecture风格来分离组件，让代码更易维护、测试和扩展。

用大白话一步步解释整个想法，然后生成代码：

1. **重构的核心想法**：重构不是推倒重来，而是逐步翻新旧代码。目标是从“全塞一起的乱代码”变成“分层清晰的积木”。好处：改UI不影响逻辑，换数据来源只改一处，bug少，AI容易帮写新功能。

2. **组件分层的想法**（像公司分工）：
   - **UI层（页面/客厅装修）**：只管显示和用户交互，比如Composable或Activity。只盯着“状态照片”刷新显示，不能自己去上网或算逻辑。例子：显示加载圈圈、列表、按钮点击喊别人干活。
   - **Presentation层（ViewModel/遥控器大脑）**：页面的经理，持有“状态照片”（uiState，包括加载中、数据、错误）。接到页面事件后，喊下面工人干活，然后更新照片让页面刷新。使用StateFlow或LiveData让数据单向流。
   - **Domain层（UseCase和Model/工人和数据形状）**：核心业务规则。每个UseCase是一个工人，只干一件事（如登录UseCase：检查输入、喊仓库验证）。Model是纯数据类（如User类：名字、年龄）。
   - **Data层（Repository/仓库总管）**：数据来源统一入口。先用假数据（FakeRepository，返回硬写内容）测试全流程，后来换真网络（Retrofit API）或数据库（Room）。抽象成接口，易切换。

   分层原则：数据单向流（仓库→工人→大脑→页面），每个层只依赖下面层，不反着来。使用Hilt或Koin自动注入对象。

3. **应用到我的项目**：假设我挑登录页面（有输入框、按钮、显示欢迎或错误）。基于现有假代码，重构它：
   - 创建文件夹：ui/screen/LoginScreen.kt, presentation/viewmodel/LoginViewModel.kt, domain/usecase/LoginUseCase.kt, domain/model/User.kt, data/repository/AuthRepository.kt。
   - 生成代码：用Kotlin + Jetpack Compose（如果不是Compose，用XML View类似）。先用假仓库跑通，然后加真API处理。
   - 加错误/加载状态：uiState用data class，页面用when处理不同情况。
   - 生成单元测试：测试工人和大脑。

输出格式：先大白话总结想法，然后每个层代码片段，最后完整示例文件。