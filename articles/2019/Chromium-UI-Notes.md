# Chromium/ui 开发笔记

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
- Accessibility 无障碍：支持朗读/选择，可用于 Direct-UI 自动化测试

## 进程

- [Chromium 多进程架构](https://developers.google.cn/web/updates/2018/09/inside-browser-part1)
  - 划分：1 Browser + n Renderer + n Extension (Renderer) + 1 GPU + (1 Plugin) + x Util
  - 模型：`process-per-site-instance`（内存最大）/`process-per-site`（内存相对小）/`process-per-tab`（默认）/`single-process`（内存最小，易于调试，Android WebView 使用该模型）
- 进程服务化
  - Browser 进程作为中心节点，能隔离/恢复其他进程的崩溃
  - Renderer/Plugin 进程在沙箱内运行，不能直接访问系统资源
  - Util 进程功能拆分为 网络服务/渲染服务/加密服务/打印服务...
- 多窗口架构
  - n 主窗口 (Browser) + n 渲染 Host 窗口 (Browser) + 1 立即 D3D 窗口 (GPU)
  - Browser 进程统一管理窗口，Renderer 进程通过 IPC 传输渲染数据
- 跨进程通信
  - 上层概念：RPC (Proxy + Stub)，底层实现：管道 / COM 组件对象模型
  - 路由问题：操作注册/映射
  - 类型问题：输入合法性检查
  - 安全问题：对端合法性校验
  - 顺序问题：相对于原始的管道，RPC 不保证消息顺序
- 渲染模型
  - 软件渲染：共享内存/存储（多数情况下比 GPU 加速快）
  - 硬件加速：指令缓存（Render 进程向 GPU 进程提交）+ 分层渲染（`//cc`）

## 线程

- [Chromium 多线程架构](https://github.com/chromium/chromium/blob/master/docs/threading_and_tasks.md#threads)
  - Browser 进程：1 UI + 1 IO (IPC/network) + x Spec + Worker-Pool
  - Renderer 进程：1 Main + 1 IO (IPC/network) + 1 Compositor + 1 Raster + n Worker + x Spec
- 无锁架构：可以在线程间抛（定时）任务（并设置结果回调）
  - 200ms 原则：不能在 UI 线程进行耗时操作（例如  I/O 操作、大量数据 CPU 密集计算、部分系统调用）
  - UI 响应：从 UI 线程将 I/O 任务抛到 Worker 线程池执行，任务完成后再将结果抛回 UI 线程
  - IPC 消息：在 IO 线程收到消息后，把任务抛到 UI 线程执行
- 消息泵：能同时处理 消息（窗口消息/libevent 回调/IO 完成消息）和 任务（普通/定时）
  - 基于超时机制：在规定的时间内 等待/处理 消息，超时后 处理 任务
  - 发送特殊消息：在等待消息时，发送 1 字节到管道，结束消息的等待，转到处理任务
- 数据竞争：不能在 非 UI 线程访问/操作 UI 数据（例如 缓存字体相关布局信息）

## 项目

- [浏览器](V8-Hippy-Share.md#Chromium-架构) + 插件（独立升级）+ 云控（灰度/策略/运营/捞取）+ 攻防（驱动/服务）
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
