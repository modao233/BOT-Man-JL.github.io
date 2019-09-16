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
  - Browser 进程：1 UI + 1 IO (IPC) + x Spec + Worker-Pool
  - Renderer 进程：1 Main + 1 IO (IPC) + 1 Compositor + 1 Raster + n Worker + x Spec
- 无锁架构：可以通过 [`base::Post[Delayed]Task[AndReply[WithResult]]`](https://github.com/chromium/chromium/blob/master/docs/threading_and_tasks.md#keeping-the-browser-responsive) 在线程间抛（定时）任务（并设置结果回调）
  - UI 响应：从 UI 线程将 I/O 任务抛到 Worker 线程池执行，任务完成后再将结果抛回 UI 线程
  - IPC 消息：在 IO 线程收到消息后，把任务抛到 UI 线程执行
  - 200ms 原则：不能在 UI 线程进行耗时操作（例如  I/O 操作、大量数据 CPU 密集计算、部分系统调用），可以利用 `base::ThreadRestrictions` 检查
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

## 模式

- MVC
  - Model 管理数据，通知数据变化
  - View 展示数据，监听数据变化
  - Controller 处理操作，修改数据
- Host/Manager
  - 管理器对象
  - 管理其他对象的生命周期
- Host/Proxy
  - 代理对象
  - 不直接处理实际逻辑
- Impl
  - 实现对象
  - 处理实际逻辑，但一般不被直接调用（中间有 Host/Proxy）
- Provider
  - 抽象工厂
  - 生成不同抽象类的实现
- Delegate
  - 数据依赖注入
  - 将具体参数的计算委派到使用者
- Client
  - 行为依赖注入
  - 将回调行为的实现委派到使用者
- Listener/Handler
  - 行为依赖注入
  - 一个触发者，一个关注者，若干个事件（例如 点击/加载）
- Observer
  - 行为依赖注入
  - 一个触发者，多个关注者，若干个事件（例如 状态变化）
- Traits
  - 模板元编程，类型对应的特征参数
  - 在编译时根据特征，选择模板的逻辑（例如 析构时是否检查观察者全部移除，析构时是否需要释放单例）
  - 编译时逻辑注入，重载特定的逻辑（例如 自定义内存管理方式，可重载创建/销毁的静态函数）

## 基础库

- 常用宏
  - `DISALLOW_COPY_AND_ASSIGN`/`DISALLOW_IMPLICIT_CONSTRUCTORS`
  - `ignore_result` 绕过 WARN_UNUSED_RESULT 检查
- STL 工具
  - `base::Contains` 支持 字符串 `::npos` / 线性容器 `std::find() != end()` / 关联容器 `.find() != end()`
  - `base::Erase[If]` 适配各类容器
- 容器
  - `base::flat_map`/`base::flat_set` 基于线性存储的关联容器
  - `base::small_map` 小容量使用 `base::flat_map`，大容量使用 `std::map/unordered_map`
  - `base::RingBuffer`/`base::circular_deque` 环形缓冲（固定大小）/环形队列（自动扩容）
  - `base::LinkedList` 侵入式链表，同时标记 prev/next
- 数值
  - `base::CheckedNumeric` 检查溢出/截断
  - `base::ClampedNumeric` 截断数值运算
  - `base::StrictNumeric` 安全类型转换
- 时间
  - `base::Time` 时间点
  - `base::TimeDelta` 时间间隔
  - `base::TimeTicks/ThreadTicks` 进程/线程 时钟 tick
- 计时器
  - `base::OneShotTimer` 一次
  - `base::RepeatingTimer` 重复
- 同步
  - `base::Lock` 锁
  - `base::ConditionVariable` 条件变量
  - `base::WaitableEvent` 事件（锁 + 条件变量）
- 原子操作
  - `base::AtomicFlag` 原子标识
  - `base::AtomicRefCount` 原子计数器
  - `base::AtomicSequenceNumber` 原子自增器
- 自动还原
  - `base::AutoReset` 自动还原 bool
  - `base::AutoLock` 自动还原锁
- 调试
  - `base::debug::WaitForDebugger`/`base::debug::BreakDebugger` 等待调试器/调试器中断
  - `base::debug::StackTrace` 打印调用栈
- `base::PendingTask`
  - 将异步任务封装为统一的 `void()` 闭包
  - 调试：记录抛出来源 + 跟踪当前任务列表
- `base::AtExitManager`
  - 析构时（和注册顺序相反）执行任务
  - 一般在 `main()` 入口处创建，并支持通过链表嵌套
- `base::RefCounted`/`scoped_refptr`
  - 侵入式引用计数管理
  - 线程安全：`base::RefCountedThreadSafe` 通过 原子操作 增减引用计数
- `base::WeakPtr`（`base::SupportsWeakPtr`/`base::WeakPtrFactory`）
  - 侵入式弱引用管理
  - 实现：存储 `base::RefCountedThreadSafe` 包装的 `base::AtomicFlag` 标识，记录对象的有效性
  - 标识线程安全：其他线程可能会 持有、销毁 标识
  - 对象线程不安全：当对象析构时，其他线程检查 标识 可能仍有效（可以利用 `base::SequenceChecker` 检查）
- `base::NoDestructor`
  - 实现 `Leakey` 特征
  - 存储：`alignas(T) char storage_[sizeof(T)]`
  - 创建：`new (storage_) T(...)`
- `base::Singleton`
  - 通过 原子操作 标识状态（未构造/正在构造/已构造），保证构造过程不会重入
  - 通过 自旋锁 尝试置换，让重入线程 原地等待 直到构造完成
  - 如果没有 `Leakey` 特征，会在 `AtExitManager` 里 `delete` 析构
  - 作用域：避免使用全局单例，可以改用函数局部的静态对象

## 测试

- 运行时断言 `CHECK`/`DCHECK`
- 编译时断言 no-compile test (`static_assert`)
- 单元测试 unittest
- 模糊测试 fuzzer
