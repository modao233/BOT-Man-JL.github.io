# UI 开发笔记

> 2019/9/3
> 
> Windows Chromium Direct-UI 开发相关笔记

## 架构

- 组件通过 Direct-UI View 实现，而不是 MFC/WTL 的子窗口形式
- View 通过树形结构管理

## 布局

- 父容器在 `Layout` 时，调用 `SetBounds` 导致子元素重新 `Layout`
- 内联/分块：Inline/Block
- 栈式/网格：StackPanel/Grid
- 边距/边框：Content -> Padding/Inset -> Border -> Margin
- XAML 语法布局器
  - 动态加载布局信息，支持原生渲染
  - 通过适配器模式，底层支持 MFC/WTL/Chromium UI 元素创建/绑定
  - 支持 WebView 和 嵌套布局

## 层次

- 创建 View/窗口时，考虑 Z-Index
- 支持 平移/旋转/缩放 变换

## 绘制

- 双缓冲：
  - FrontBuffer 用于渲染，BackBuffer 用于当前绘制
  - 用户看到的每一帧都是正确的，从而避免闪屏
- 脏区域：
  - 每次只重绘 无效区域，不在区域内不需要绘制

## 动画

- 周期：`时间间隔 == 1/帧率`
- 进度：不同效果对应不同的 `时间-进度` 曲线 `f(t % Interval)`
- 重绘：
  - 定时器触发后，根据当前时间对应的进度，修改布局/元素样式（变成脏区域）
  - 需要注意动画的变化范围，脏区域过大可能有性能问题

## 焦点

- 创建新的窗口时，考虑是否 Inactive
- 创建新的 View 时，考虑是否 Focusable

## 拖拽

- 封装系统 DragDrop 接口

## 滚动

- ListView 复用：
  - 只创建可见的 ListView（包括不完整显示的）
  - 针对不同滚动位置，填充不同 ListItem 数据

## 事件

- 创建
  - WM_CREATE
  - WM_PAINT
- 销毁
  - WM_CLOSE
  - WM_DESTROY
  - WM_NCDESTROY
- 交互
  - Key/Mouse/Gesture
  - Accelerator
  - 系统事件

## 其他

- Tooltip 提示：鼠标 hover 时弹出气泡
- ContextMenu 上下文菜单：右键位置弹出菜单
- Accessibility 无障碍：支持朗读/选择

## 线程

- [Chromium 多线程架构](https://github.com/chromium/chromium/blob/master/docs/threading_and_tasks.md#threads)：
  - Browser 进程：1 UI + 1 IO (IPC) + x Spec + Worker-Pool
  - Renderer 进程：1 Main + 1 IO (IPC) + 1 Compositor + 1 Raster + n Worker + x Spec
- 无锁架构：可以通过 [`base::PostTaskAndReply`](https://github.com/chromium/chromium/blob/master/docs/threading_and_tasks.md#keeping-the-browser-responsive) 将 I/O 任务抛到 Worker 线程池执行，保证 UI 响应
- 200ms 原则：不能在 UI 线程进行耗时操作（例如  I/O 操作、大量数据 CPU 密集计算、部分系统调用），可以利用 [`base::ThreadRestrictions`](https://github.com/chromium/chromium/blob/master/base/threading/thread_restrictions.h) 检查
- 数据竞争：不能在 非 UI 线程访问/操作 UI 数据（例如 缓存字体相关布局信息）

## 模式

- MVC
  - Model 负责数据和通知变化
  - View 负责显示和监听数据变化
  - Controller 负责修改数据
- Delegate
  - 数据依赖注入
  - 将具体参数的填充委派到使用者
- Host
  - TODO
- Client
  - TODO
- Listener/Handler
  - 行为依赖注入
  - 一个触发者，一个关注者，若干个事件（例如 点击/加载）
- Observer
  - 行为依赖注入
  - 一个触发者，多个关注者，若干个事件（例如 状态变化）
- Task
  - 行为依赖注入
  - 将异步任务闭包封装为统一的 Task，并记录抛出来源
- Singleton
  - 通过 原子操作 置换，保证构造过程不会重入
  - 通过 自旋锁 尝试置换，让重入线程 原地等待
  - 如果不是 Leakey，会在 AtExitManager 里析构
- Traits
  - 模板元编程，类型对应的特征参数

## 项目

- [浏览器](V8-Hippy-Share.md#Chromium-架构) + 插件（独立升级）+ 云控（灰度/策略/运营/捞取）+ 攻防（驱动/服务）
- [多进程架构](https://developers.google.cn/web/updates/2018/09/inside-browser-part1)
  - 1 Browser + n Renderer + n Extension (Renderer) + 1 GPU + (1 Plugin) + x Util
  - 服务化：Browser 进程作为中心节点，Renderer 进程在沙盒内运行，Util 进程功能拆分为 网络服务/渲染服务/加密服务...（崩溃/卡顿 隔离）
  - 优缺点：Browser 进程可以快速恢复其他进程的崩溃，但占用更多内存（内存不足时可以合并）
- 多窗口架构
  - n 主窗口 (Browser) + n 渲染 Host 窗口 (Browser) + 1 立即 D3D 窗口 (GPU)
  - Browser 进程统一管理窗口，Renderer 进程通过 IPC 传输渲染数据
- 跨进程通信
  - 上层概念：RPC (Proxy + Stub)，底层实现：管道 / COM 组件对象模型
  - 路由问题：操作注册/映射
  - 类型问题：输入合法性检查
  - 安全问题：对端合法性校验
  - 顺序问题：相对于原始的管道，RPC 不保证消息顺序
- 拆分 Browser/UI
  - 开发时，工程独立：快速编译/快速升核/支持多核
  - 运行时，进程独立：快速启动/崩溃隔离与恢复
- 修改大项目
  - 统一代码规范
  - 尽量只增不改
  - 新增的代码 存放在独立目录
  - 修改的代码 使用宏区分

## 优化

- 创建时，按需加载（不可见部分）-> 延迟加载（不影响首屏绘制）
- 布局时，缓存字体相关尺寸计算结果
- 绘制时，减小脏区域尺寸
- 避免阻塞 UI 调用（例如 `SendMessage` -> `PostMessage`）
- 缓存系统调用结果（例如 显示器/DPI）
