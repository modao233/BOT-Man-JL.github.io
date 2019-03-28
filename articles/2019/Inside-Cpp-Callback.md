# 深入 C++ 回调

> 2019/3/21
> 
> 本文深入分析 [Chromium 的 Bind/Callback 机制](https://github.com/chromium/chromium/blob/master/docs/callback.md)，并讨论设计 C++ 回调时 ~~你想知道的~~（**你可能不知道的**）一些问题。

背景阅读：

- 如果你还不知道什么是 **回调** _(callback)_，欢迎阅读 [如何浅显的解释回调函数](../2017/Callback-Explained.md)
- 如果你还不知道什么是 **回调上下文** _(callback context)_ 和 **闭包** _(closure)_，欢迎阅读 [对编程范式的简单思考](Thinking-Programming-Paradigms.md)
- 如果你还不清楚 **可调用对象** _(callable object)_ 和 **回调接口** _(callback interface)_ 的区别，欢迎阅读 [回调 vs 接口](../2017/Callback-vs-Interface.md)（本问主要讨论类似 `std::function` 的 **可调用对象**，而不是基于接口的回调）

回调是被广泛应用的概念：

- **图形界面客户端** 常用 [事件循环 _(event loop)_](https://en.wikipedia.org/wiki/Event_loop) 有条不紊的处理 用户输入/计时器/系统处理/跨进程通信 等事件，一般采用回调响应事件
- **I/O 密集型程序** 常用 [异步 I/O _(asynchronous I/O)_](https://en.wikipedia.org/wiki/Asynchronous_I/O) 协调各模块处理速率，提高吞吐率，进一步引申出 设计上的 [Reactor](https://en.wikipedia.org/wiki/Reactor_pattern)、系统上的 [纤程 _(fiber)_](https://en.wikipedia.org/wiki/Fiber_%28computer_science%29)、语言上的 [协程 _(coroutine)_](https://en.wikipedia.org/wiki/Coroutine) 等概念，一般采用回调处理 I/O 完成的返回结果

从语言上看，回调是一个调用函数的过程，涉及两个角色：计算和数据。一般的，回调的计算是一个函数，而数据来源于两部分：

- **绑定** _(bound)_ 的数据，即回调的 **上下文**（在回调构造时捕获）
- **未绑定** _(unbound)_ 的数据，即执行回调时需要额外传入的数据

不带绑定数据的回调，只是单纯的函数（例如 C 语言的函数指针）；带有绑定数据的回调，又叫做 **闭包**。（参考：[对编程范式的简单思考](Thinking-Programming-Paradigms.md)）

从实现上看，在面向对象语言中，闭包表示为一个对象（例如 `std::function`），闭包和上下文之间可能是 [关联/组合/聚合关系](https://en.wikipedia.org/wiki/Class_diagram#Instance-level_relationships)。这要求在设计时，明确上下文的 **所有权** _(ownership)_，实现对象的 **生命周期管理** _(lifetime management)_。（参考：[资源管理小记](../2018/Resource-Management.md#资源和对象的映射关系)）

从所有权/生命周期的角度看，上下文又分为两种：

- **弱引用** _(weak reference)_ 上下文
  - 闭包 **不拥有** 上下文，所以回调执行时 **上下文可能失效**
  - 如果没有检查，可能会导致 **崩溃**
- **强引用** _(strong reference)_ 上下文
  - 闭包 **拥有** 上下文，能保证回调执行时 **上下文一直有效**
  - 如果忘记释放，可能会导致 **泄漏**

如果你已经熟悉了 [`std::bind`](https://en.cppreference.com/w/cpp/utility/functional/bind)/[`lambda`](https://en.cppreference.com/w/cpp/language/lambda) + [`std::function`](https://en.cppreference.com/w/cpp/utility/functional/function)，那么你在设计 C++ 回调时，**是否考虑过这几个问题**：

[TOC]

本文分析 Chromium 的 [`base::Bind`](https://github.com/chromium/chromium/blob/master/base/bind.h) + [`base::Callback`](https://github.com/chromium/chromium/blob/master/base/callback.h) 回调机制，带你领略回调设计的精妙之处。（参考：[Callback<> and Bind() | Chromium Docs](https://github.com/chromium/chromium/blob/master/docs/callback.md)）

## 回调是同步还是异步的

**同步回调** _(sync callback)_ 是立即被执行的调用，会在 **构造回调** 的 **调用栈** _(call stack)_ 里 **局部执行**。例如，累加一组得分（使用 `lambda` 表达式捕获上下文 `total`）：

``` cpp
int total = 0;
std::for_each(std::begin(scores), std::end(scores),
              [&total](auto score) { total += score; });
//             ^ context variable |total| is always valid
```

- **绑定的数据**：`total`，局部变量的上下文（弱引用）
- **未绑定的数据**：`score`，每次迭代传递的值

<!--
# https://sequencediagram.org/
-->

<!--
title accumulate-sync
participant Current Thread

[->>Current Thread: Call Accumulate
activate Current Thread
Current Thread->>Current Thread: Call lambda
Current Thread->>Current Thread: Call lambda
Current Thread->>Current Thread: Call lambda
[<<-Current Thread: Return

space
deactivate Current Thread
-->

[img=max-width:40%]

[align-center]

![Accumulate Sync](Inside-Cpp-Callback/accumulate-sync.svg)

**异步回调** _(async callback)_ 是之后可能被执行的调用，一般在构造后存储起来，在 **未来某个时刻**（不同的调用栈里）**非局部执行**。例如，用户界面为了不阻塞 **UI 线程** 响应用户输入，在 **后台线程** 从文件里异步加载背景图片，加载完成后再从 **UI 线程** 显示到界面上：

``` cpp
Image LoadImageFromFile(const std::string& filename);

void View::LoadImageCallback(const Image& image) {
  // WARNING: |this| may be invalid now!
  if (background_image_view_)
    background_image_view_->SetImage(image);
}

void View::FetchImageAsync(const std::string& filename) {
  base::PostTaskAndReplyWithResult(
      FROM_HERE, base::Bind(&LoadImageFromFile, filename),
      base::Bind(&View::LoadImageCallback, this));
  //                                       ^ use raw |this|
}
```

- **绑定的数据**：代码第二个 `base::Bind` 捕获了 `View` 对象的 `this` 指针（弱引用）
- **未绑定的数据**：`View::LoadImageCallback`/`lambda` 表达式 的参数 `const Image& image`

<!--
title fetch-image-async
participant UI thread
participant Background Thread

[->>UI thread: Call View::FetchImageAsync
activate UI thread
UI thread->Background Thread: Post LoadImageFromFile

space
deactivate UI thread

space
activate UI thread
note over UI thread: Run other tasks

space
deactivate UI thread

Background Thread->>Background Thread: Call LoadImageFromFile
activate Background Thread
Background Thread->UI thread: Post View::LoadImageCallback

space
deactivate Background Thread

space
UI thread->>UI thread: Call View::LoadImageCallback
activate UI thread
UI thread->>[: Call ImageView::SetImage

space
deactivate UI thread
-->

[img=max-width:90%]

[align-center]

![Fetch Image Async](Inside-Cpp-Callback/fetch-image-async.svg)

> 注：
> 
> - `base::PostTaskAndReplyWithResult` 属于 Chromium 的多线程任务模型（参考：[Keeping the Browser Responsive | Threading and Tasks in Chrome](https://github.com/chromium/chromium/blob/master/docs/threading_and_tasks.md#keeping-the-browser-responsive)）
> - 第二个 `base::Bind` 使用 C++ 11 lambda 表达式可以等效为：
> 
> ``` cpp
> base::Bind([this](const Image& image) {
>   // WARNING: |this| may be invalid now!
>   if (background_image_view_)
>     background_image_view_->SetImage(image);
> })
> ```

### 回调时（弱引用）上下文会不会失效

对于 **同步回调** 执行时的调用栈和构造时一致，只要设计思路清晰，一般不会出现上下文失效的情况。而 **异步回调** 执行时的调用栈往往和构造时的不一致，在回调的 **调用时刻**，构造时捕获的 **弱引用上下文 可能失效**。

例如 `View::FetchImageAsync` 的代码里，在 `LoadImageFromFile` 返回时，执行 `View::LoadImageCallback` 回调：

- 如果界面还在显示，`View` 对象仍然有效，则执行 `background_image_view_->SetImage(image)`
- 如果界面已经退出，`background_image_view_` 变成 [野指针 _(wild pointer)_](https://en.wikipedia.org/wiki/Dangling_pointer)，调用导致 **崩溃**

其实，上述两段代码都无法编译（Chromium 做了对应的 **静态断言** _(static assert)_）—— 因为第二个 `base::Bind` 的参数传递都是 **不安全的**：

- 传递普通对象的 **裸指针**，容易导致悬垂引用
- 传递捕获了上下文的 lambda 表达式，**无法检查** lambda 表达式捕获的弱引用的有效性

在 **C++ 核心指南** _(C++ Core Guidelines)_ 里也有类似的讨论：

- [F.52: Prefer capturing by reference in lambdas that will be used locally, including passed to algorithms](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-reference-capture)
- [F.53: Avoid capturing by reference in lambdas that will be used nonlocally, including returned, stored on the heap, or passed to another thread](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-value-capture)

### 如何处理失效的（弱引用）上下文

如果捕获的弱引用上下文失效，回调应该 **及时取消**。对于异步加载图片的例子，可以给第二个 `base::Bind` 传递 `View` 对象的 **弱引用指针**，即 `base::WeakPtr<View>`：

``` cpp
void View::FetchImageAsync(const std::string& filename) {
  base::PostTaskAndReplyWithResult(
      FROM_HERE, base::Bind(&LoadImageFromFile, filename),
      base::Bind(&View::LoadImageCallback,
                 weak_factory_.GetWeakPtr()));
  //             ^ use |WeakPtr| rather than raw |this|
}
```

在 `LoadImageFromFile` 返回时，执行 `View::LoadImageCallback` 回调：

- 如果界面还在显示，`View` 对象仍然有效，则执行 `background_image_view_->SetImage(image)`
- 否则，弱引用失效，**不执行回调**（因为界面已经退出，**没必要** 在设置图片了）

> 注：
> 
> - [`base::WeakPtr`](https://github.com/chromium/chromium/blob/master/base/memory/weak_ptr.h) 属于 Chromium 提供的 **侵入式** _(intrusive)_ 智能指针，非 **线程安全** _(thread-safe)_
> - `base::Bind` 针对 `base::WeakPtr` 扩展了 `base::IsWeakReceiver` 检查，调用时增加 `if (!weak_ptr) return;` 的弱引用有效性检查（参考：[Customizing the behavior | Callback<> and Bind()](https://github.com/chromium/chromium/blob/master/docs/callback.md#customizing-the-behavior)）

基于弱引用指针，Chromium 封装了 **可取消** _(cancelable)_ 回调 [`base::CancelableCallback`](https://github.com/chromium/chromium/blob/master/base/cancelable_callback.h)，提供 `Cancel`/`IsCancelled` 接口。（参考：[Cancelling a Task | Threading and Tasks in Chrome](https://github.com/chromium/chromium/blob/master/docs/threading_and_tasks.md#cancelling-a-task)）

## 回调只能执行一次还是可以多次

软件设计里，只有三个数 —— [`0`，`1`，`∞`（无穷）](https://en.wikipedia.org/wiki/Zero_one_infinity_rule)。类似的，不管是同步回调还是异步回调，我们只关心它被执行 `0` 次，`1` 次，还是若干次。

### 为什么要区分一次和多次回调

我们先举个 **反例**。对于 C 语言基于函数指针的回调：

- 由于 **没有闭包**，需要自己申请/释放上下文
- 由于 **资源所有权不明确**，难以从指针 `T*` 中判断是否需要释放（强引用）上下文

例如，使用 libevent 监听 socket 可写事件，实现异步/非阻塞发送数据（[例子来源](../2017/Callback-vs-Interface.md#C-语言中的回调)）：

``` c
// callback code
void do_send(evutil_socket_t fd, short events, void* context) {
  char* buffer = (char*)context;
  // ... send |buffer| via |fd|
  free(buffer);  // free |buffer| here!
}

// client code
char* buffer = malloc(buffer_size);  // alloc |buffer| here!
// ... fill |buffer|
event_new(event_base, fd, EV_WRITE, do_send, buffer);
```

- 正确情况：`do_send` **只执行一次**
  - client 代码 **申请** 发送缓冲区 `buffer` 资源，并作为 `context` 传入注册函数
  - callback 代码从 `context` 中取出 `buffer`，发送数据后 **释放** `buffer` 资源
- 错误情况：`do_send` **没有被执行**
  - client 代码申请的 `buffer` 就不会被释放，从而导致 **泄漏**
- 错误情况：`do_sent` **被执行多次**
  - callback 代码取出的 `buffer` 可能已经被释放，从而导致 **崩溃**

### 何时销毁（强引用）上下文

根据回调次数，Chromium 把回调分为两种：

| | `base::OnceCallback` | `base::RepeatingCallback` |
|-|----------------------|---------------------------|
| 最多可调用次数 | 一次 | 多次 |
| 构造方法 | `base::BindOnce` | `base::BindRepeating` |
| 是否可拷贝 | 否 | 是 |
| 调用方法 | `R Run(Args... args) &&`，只能通过 `std::move(callback).Run(...)` 一次性调用 | `R Run(Args... args) const &`，可以通过 `callback.Run(...)` 多次调用 |
| 调用后状态 | 进入 **失效状态**，无法再调用 | 一直处于 **有效状态** |

> 注：
> 
> - 写在成员函数后的 **引用限定符** _(reference qualifier)_ `&&` / `const &`，分别表示 在对象处于 右值/非右值 状态时调用

解决了一次和多次的回调问题，还剩下一个问题：如果回调 **没被执行**（`0` 次回调）**上下文又该如何销毁**？

为了确保资源能有效的管理，在执行 `base::Bind` 时，传递的每个上下文参数 **存储在闭包里**，对象的 **所有权归属于闭包**。例如，改写上边 异步/非阻塞发送数据 的代码：

> 假设 `using Event::Callback = base::OnceCallback<void()>;`

``` cpp
// callback code
void DoSend(std::unique_ptr<Buffer> buffer) {
  // ...
}  // free |buffer| via |~unique_ptr()|

// client code
std::unique_ptr<Buffer> buffer = ...;
event->SetCallback(
    base::BindOnce(&DoSend, std::move(buffer)));
```

- 构造闭包时：`buffer` 移动到 `base::OnceCallback` 内
- 回调执行时：`buffer` 从 `base::OnceCallback` 的上下文移动到 `DoSend` 的参数里，并在 `DoSend` 结束时销毁（**所有权转移**）
- 闭包销毁时：如果回调没有执行，`buffer` 未被销毁，则此时销毁（**保证销毁且只销毁一次**）

> 假设 `using Event::Callback = base::RepeatingCallback<void()>;`

``` cpp
// callback code
void DoSend(const Buffer* buffer) {
  // ...
}  // DON'T free reusable |buffer|

// client code
std::unique_ptr<Buffer> buffer = ...;
event->SetCallback(
    base::BindRepeating(&DoSend, std::move(buffer)));
```

- 构造闭包时：`buffer` 移动到 `base::RepeatingCallback` 内
- 回调执行时：每次传递 `buffer.get()` 的指针，只使用 `buffer` 的数据
- 闭包销毁时：总是销毁 `buffer`（**统一在此时销毁**）

这里又引入了另一个微妙的问题：由于 **一次回调** 的上下文 **销毁时机不确定**，上下文 **析构函数** 的调用时机 **也不确定**！

如果上下文中包含了 **复杂析构函数** 的对象（例如，析构时做数据上报），那么析构时需要检查依赖条件的有效性（例如，检查数据上报环境是否有效），否则导致 **崩溃**。（类似于 **异步回调的弱引用上下文失效问题**）

### 如何传递（强引用）上下文

STL 原生的 `std::bind`/`lambda` + `std::function` 并没有明确 **可调用次数**，对（强引用）上下文生命周期管理也不够明确：

``` cpp
// OK, pass |std::unique_ptr| by move construction
auto unique_lambda = [p = std::unique_ptr<int>{new int}]() {};
// OK, pass |std::unique_ptr| by ref
unique_lambda();
// Bad, require |unique_lambda| copyable
std::function<void()>{std::move(unique_lambda)};

// OK, pass |std::unique_ptr| by move
auto unique_bind =
    std::bind([](std::unique_ptr<int>) {}, std::unique_ptr<int>{});
// Bad, failed to copy construct |std::unique_ptr|
unique_bind();
// Bad, require |unique_bind| copyable
std::function<void()>{std::move(unique_bind)};

auto shared_lambda = [p = std::shared_ptr<int>{}]() {};
std::function<void()>{shared_lambda};  // OK, copyable

auto shared_func = [](std::shared_ptr<int> ptr) {
  assert(ptr.use_count() == 6);                       // (ref6)
};
auto p = std::shared_ptr<int>{new int};               // (ref1)
auto shared_bind = std::bind(shared_func, p);         // (ref2)
auto copy_bind = shared_bind;                         // (ref3)
auto shared_fn = std::function<void()>{shared_bind};  // (ref4)
auto copy_fn = shared_fn;                             // (ref5)
assert(p.use_count() == 5);
shared_fn();
```

- `unique_lambda`/`unique_bind`
  - 闭包只能移动，不能拷贝，不能用于构造 `std::function`
  - `unique_lambda` 可以执行，上下文在 `lambda` 函数体内作为引用
  - `unique_bind` 不能执行，因为函数的接收参数要求拷贝 `std::unique_ptr`
- `shared_lambda`/`shared_bind`/`std::function`
  - 闭包可以拷贝，对其拷贝也会拷贝闭包拥有的上下文
  - `shared_lambda` 和对应到 `std::function` 可以执行，上下文在 `lambda` 函数体内作为引用
  - `shared_bind` 和对应到 `std::function` 可以执行，上下文会拷贝成新的 `std::shared_ptr`

> 注：
> 
> - 最新提案 [`std::unique_function`](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p0228r3.html) 为 STL 添加类似 `base::OnceCallback` 的支持

Chromium 的 `base::Callback` 优化了 STL 回调机制遇到的问题：

| | `lambda` | `bind` | `function` | `Repeating Callback` | `Once Callback` |
|-|----------|--------|------------|---------------------|----------------|
| 构造闭包 | 传值或传引用 <td colspan=4> 使用 `std::forward` **完美转发** _(perfect forwarding)_ |
| 执行回调 <td colspan=4> 把上下文作为左值，直接传递到函数的接收参数（**左值引用**）| 使用 `std::move` 移动上下文，并结束其生命周期（**右值引用**）|
| 拷贝闭包 <td colspan=2> 由 **上下文决定** 是否可拷贝（如果可以，拷贝上下文）| **可拷贝**，拷贝上下文 | **浅拷贝** _(shallow copy)_，通过 `scoped_refptr` 共享所有权 | **不可拷贝** |

> 注：
> 
> - [`scoped_refptr`](https://github.com/chromium/chromium/blob/master/base/memory/scoped_refptr.h) 也属于 Chromium 提供的 **侵入式** _(intrusive)_ 智能指针，通过对象内部引用计数，实现类似 `std::shared_ptr` 的功能

目前，Chromium 支持的上下文绑定方式有：

| Chromium 支持 | 回调参数类型（目的）| 绑定数据类型（源）| 闭包是否拥有上下文 |
|---|---|---|---|
| `std::ref/cref()` | `T&` / `const T&` | `T&` / `const T&` | 否，自己保证有效性 |
| `base::Unretained()` | `T*` | `T*` | 否，自己保证有效性 |
| `base::WeakPtr` | `T*` | `base::WeakPtr` | 否，弱引用指针保证有效性 |
| `base::Owned()` | `T*` | `T*` | 是，`delete` 销毁 |
| `std::unique_ptr` | `T*` / `std::unique_ptr` | `std::unique_ptr` | 是，`~std::unique_ptr()` 销毁 |
| `base::RetainedRef()` | `T*` | `scoped_refptr` | 是，`~scoped_refptr()` 销毁 |
| `scoped_refptr` | `scoped_refptr` | `scoped_refptr` | 是，`~scoped_refptr()` 销毁 |

> 注：
> 
> - 主要参考 [Quick reference for advanced binding | Callback<> and Bind()](https://github.com/chromium/chromium/blob/master/docs/callback.md#quick-reference-for-advanced-binding)
> - `base::Unretained/Owned/RetainedRef()` 类似于 `std::ref/cref()`，构造特殊类型数据的封装（参考：[Customizing the behavior | Callback<> and Bind()](https://github.com/chromium/chromium/blob/master/docs/callback.md#customizing-the-behavior)）

## 写在最后 [no-toc]

从这篇文章可以看出：C++ 是很复杂的。。。

- C++ 要求程序员手动管理对象生命周期，对象 **从出生到死亡** 的各个环节都要想清楚
- Chromium 的 Bind/Callback 实现基于 **现代 C++ 元编程**，实现起来很复杂（参考：[浅谈 C++ 元编程](../2017/Cpp-Metaprogramming.md)）

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2019, BOT Man
