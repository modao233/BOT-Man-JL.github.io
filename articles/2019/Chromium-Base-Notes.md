# Chromium/base 库笔记

> 2019/9/17
> 
> 读书破万卷，下笔如有神。—— 杜甫

[TOC]

## 常用宏

- `DISALLOW_COPY_AND_ASSIGN` 禁用拷贝构造/赋值
- `DISALLOW_IMPLICIT_CONSTRUCTORS` 禁用默认构造
- `ignore_result` 忽略 WARN_UNUSED_RESULT 检查

## STL 工具

- `base::Contains` 支持 字符串 `::npos` / 线性容器 `std::find() != end()` / 关联容器 `.find() != end()`
- `base::Erase[If]` 适配各类容器

## 容器

- `base::flat_map`/`base::flat_set` 基于线性存储的关联容器
- `base::small_map` 小容量使用 `base::flat_map`，大容量使用 `std::map/unordered_map`
- `base::RingBuffer`/`base::circular_deque` 环形缓冲（固定大小）/环形队列（自动扩容）
- `base::LinkedList` 侵入式双向链表，快速删除/插入节点

## 数值

- `base::CheckedNumeric` 检查溢出/截断
- `base::ClampedNumeric` 截断数值运算
- `base::StrictNumeric` 安全类型转换

## 时间

- `base::Time` 时间点
- `base::TimeDelta` 时间间隔
- `base::TimeTicks/ThreadTicks` 进程/线程 时钟 tick

## 线程同步

- `base::Lock` 锁
- `base::ConditionVariable` 条件变量
- `base::WaitableEvent` 事件

## 原子操作

- `base::AtomicFlag` 原子标识
- `base::AtomicRefCount` 原子计数器
- `base::AtomicSequenceNumber` 原子自增器

## 任务模型

- `base::Bind/Callback` 生命周期严格的函数闭包，支持 弱引用检查/调用次数限制（参考：[深入 C++ 回调](Inside-Cpp-Callback.md)）
- `base::PendingTask` 将异步任务封装为统一的 `void()` 闭包（调试：记录抛出来源 + 跟踪当前任务列表）
- `base::CancelableTaskTracker` 支持 线程安全 取消已经抛出的任务（实现：存储 `base::RefCountedThreadSafe` 包装的 `base::AtomicFlag` 标识，记录是否被取消）

## 线程模型

- 底层线程模型
  - `base::PlatformThread` 系统线程
  - `base::Thread`/`base::SimpleThread` 带有/不带 消息循环的线程
  - `base::ThreadPoolInstance` 线程池
  - `base::RunLoop` -> `base::MessageLoop` -> `base::MessagePump` -> `base::TaskRunner` 接口
- 上层线程模型
  - `base::TaskRunner` 线程池中的 并行调度 任务管理器
  - `base::SequencedTaskRunner` 线程池中的 顺序调度 任务管理器
  - `base::SingleThreadTaskRunner` 独立线程的（顺序调度）任务管理器
  - `base::Post[Delayed]Task[AndReply[WithResult]]` 抛出（定时）任务（并通过回调返回结果）
  - Chromium 推荐使用 逻辑序列 而不是 物理线程，参考：[Prefer Sequences to Physical Threads | Threading and Tasks in Chrome](https://github.com/chromium/chromium/blob/master/docs/threading_and_tasks.md#prefer-sequences-to-physical-threads)
- 其他
  - `base::ThreadLocalStorage` 线程本地存储 TLS
  - `base::SequenceBound` 管理 其他序列上的 非序列安全的 对象生命周期（创建/使用/销毁）
  - `base::WatchDog` 启动后台监控线程，定期检查是否 Arm，否则 Alarm 报警
- 线程检查
  - `base::ThreadRestrictions` 检查当前线程是否允许特殊操作（标识存放在 线程本地存储，调用相关函数时检查）
    - 阻塞调用：例如同步 I/O 操作，建议异步处理
    - 同步原语：可能导致卡顿/死锁
    - CPU 密集任务：可能导致 CPU 繁忙，建议异步处理
    - 非 Leakey 单例支持：如果主线程先退出并销毁单例，导致 non-joinable 线程访问野指针，出现退出崩溃
  - `base::ThreadChecker/SequenceChecker` 检查线程/序列安全（对象构造时 关联当前线程/序列，使用时/析构时 检查是否在同一线程/序列）

## 定时器

- `base::OneShotTimer` 执行一次
- `base::RepeatingTimer` 重复执行
- 允许指定运行的 逻辑序列，默认为 当前序列

## ScopeGuard

- `base::AutoReset` 自动还原 bool
- `base::AutoLock` 自动还原锁（析构时调用 `Release`）
- `ScopedObserver` 自动移除观察者（析构时调用 `RemoveObserver`）

## 调试

- `base::debug::WaitForDebugger`/`base::debug::BreakDebugger` 等待调试器/调试器中断
- `base::debug::StackTrace` 打印调用栈

## 其他

- `base::AtExitManager`
  - 析构时（和注册顺序相反）执行任务
  - 一般在 `main()` 入口处创建，并支持通过链表嵌套
- `base::RefCounted`
  - 侵入式引用计数管理
  - 线程安全：`base::RefCountedThreadSafe` 通过 原子操作 增减引用计数
  - 使用：多线程下 `scoped_refptr` 所有权/销毁顺序/销毁时机 难以确定，尽量使用 `std::unique_ptr`
- `base::WeakPtr`（`base::SupportsWeakPtr`/`base::WeakPtrFactory`）
  - 侵入式弱引用管理
  - 实现：存储 `base::RefCountedThreadSafe` 包装的 `base::AtomicFlag` 标识，记录对象的有效性
  - 标识线程安全：其他线程可能会 持有、销毁 标识
  - 对象线程不安全：当对象析构时，其他线程检查 标识 可能仍有效
- `base::NoDestructor`
  - 实现 `Leakey` 特征
  - 存储：`alignas(T) char storage_[sizeof(T)]`
  - 创建：`new (storage_) T(...)`
- `base::Singleton`
  - 通过 AcquireLoad/ReleaseStore 原子操作标识状态（未构造/正在构造/已构造），避免构造过程不会重入
  - 通过 AcquireLoad 原子操作检查状态，让重入线程 原地等待（`PlatformThread::YieldCurrentThread/Sleep`）直到构造完成
  - 如果没有 `Leakey` 特征，会在 `base::AtExitManager` 里 `delete` 析构
  - 如果没有 `Leakey` 特征 或 非静态存储，不允许在 non-joinable 的线程上访问单例（`base::ThreadRestrictions` 会检查）
  - 使用：避免使用全局单例，可以改用函数局部的静态对象
  - 补充：双检查锁方案会有内存顺序问题，重入的线程可能会读取到正在构造的单例对象（[C++ and the Perils of Double-Checked Locking](https://www.aristeia.com/Papers/DDJ_Jul_Aug_2004_revised.pdf)）
- `base::ObserverList`
  - 支持 在被观察者析构时，检查所有观察者是否都被移除（[参考：被观察者先销毁问题](Insane-Observer-Pattern.md#问题-被观察者先销毁)）
  - 支持 在迭代过程中，检查是否有迭代重入（排查潜在的逻辑错误，[参考：死循环问题](Insane-Observer-Pattern.md#问题-死循环)）
  - 支持 在迭代过程中，移除观察者（实现：标记为“待移除”，然后等迭代结束后移除，[参考：循环内删除迭代器](Insane-Observer-Pattern.md#问题-被观察者先销毁)）
  - 支持 `base::CheckedObserver` 在通知前检查观察者的有效性，避免因为通知无效观察者导致崩溃（[参考：观察者先销毁问题](Insane-Observer-Pattern.md#问题-观察者先销毁)）
  - 线程安全：`base::ObserverListThreadSafe` 通过 `base::Lock` 实现异步通知/回调
- `base::Value`
  - JSON 数据类型（None/Boolean/Integer/Double/String/Blob/Dictionary/List）
  - 派生类：`base::DictionaryValue`/`base::ListValue`

## 模式

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
- pImpl
  - 前置声明实现对象，只保存实现对象的 `std::unique_ptr`
  - 要求：不能在头文件中 inline 析构函数，因为在析构函数定义时实现对象没有定义
- Traits
  - 模板元编程，类型对应的特征参数
  - 在编译时根据特征，选择模板的逻辑（例如 析构时是否检查观察者全部移除，析构时是否需要释放单例）
  - 编译时逻辑注入，重载特定的逻辑（例如 自定义内存管理方式，可重载创建/销毁的静态函数）

## 测试

- 运行时断言 `CHECK`/`DCHECK`
- 编译时断言 no-compile test (`static_assert`)
- 单元测试 unittest
- 模糊测试 fuzzer

## 参考

- [Chromium 基础库使用说明 - rogeryi](https://www.zybuluo.com/rogeryi/note/56894)
