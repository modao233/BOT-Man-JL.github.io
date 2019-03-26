# 深入 C++ 回调

> 2019/3/21
> 
> 本文主要分析 [Chromium 的 Bind/Callback 机制](https://github.com/chromium/chromium/blob/master/docs/callback.md)，并讨论设计 C++ 回调时 ~~你想知道的~~（**你可能不知道的**）一些问题

背景阅读：

- 如果你还不知道什么是 **回调** _(callback)_，欢迎阅读 [如何浅显的解释回调函数](../2017/Callback-Explained.md)
- 如果你还不知道什么是 **回调上下文** _(callback context)_ 和 **闭包** _(closure)_，欢迎阅读 [对编程范式的简单思考](Thinking-Programming-Paradigms.md)
- 如果你还不清楚 **可调用对象** _(callable object)_ 和 **回调接口** _(callback interface)_ 的区别，欢迎阅读 [回调 vs 接口](../2017/Callback-vs-Interface.md)（本问主要讨论类似 `std::function` 的 **可调用对象**，而不是基于接口的回调）

回调是被广泛应用的概念：

- **图形界面客户端** 常用 [事件循环 _(event loop)_](https://en.wikipedia.org/wiki/Event_loop) 有条不紊的处理 用户输入/计时器/系统处理/跨进程通信 等事件，一般采用回调响应事件
- **I/O 密集型程序** 常用 [异步 I/O _(asynchronous I/O)_](https://en.wikipedia.org/wiki/Asynchronous_I/O) 协调各模块处理速率，提高吞吐率，进一步引申出 设计上的 [Reactor](https://en.wikipedia.org/wiki/Reactor_pattern)、系统上的 [纤程 _(fiber)_](https://en.wikipedia.org/wiki/Fiber_%28computer_science%29)、语言上的 [协程 _(coroutine)_](https://en.wikipedia.org/wiki/Coroutine) 等概念，一般采用回调处理 I/O 完成的返回结果

如果你已经熟悉了 [`std::bind`](https://en.cppreference.com/w/cpp/utility/functional/bind)/[`lambda`](https://en.cppreference.com/w/cpp/language/lambda) + [`std::function`](https://en.cppreference.com/w/cpp/utility/functional/function)，那么你在设计 C++ 回调时，**是否考虑过这几个问题**：

[TOC]

本文分析 Chromium 的 [`base::Bind`](https://github.com/chromium/chromium/blob/master/base/bind.h) + [`base::Callback`](https://github.com/chromium/chromium/blob/master/base/callback.h) 回调机制。（参考：[Callback<> and Bind() | Chromium Docs](https://github.com/chromium/chromium/blob/master/docs/callback.md)）很多人会有这样的疑惑：既然 STL 已经提供了 `std::bind` + `std::function`，**为什么 Chromium 项目还要“造轮子”呢**？读完这篇文章，带你领略 Chromium 回调机制的强大之处。

## 回调是同步还是异步的

**同步** _(sync)_ 回调是立即被执行的调用，**异步** _(async)_ 回调是之后可能被执行的调用。

两者的本质区别在于：

- 同步回调会在 **构造回调** 的 **调用栈** _(call stack)_ 里 **局部执行**
- 异步回调一般会在构造后存储起来，在 **未来某个时刻**（不同的调用栈里）**非局部执行**

### 回调时上下文会不会失效

本质上，回调是一个调用函数的过程，所以这个过程里总会包括两个角色：计算和数据。一般的，回调的计算是一个函数，而数据来源于两部分：

- **绑定** _(bound)_ 的数据，即回调的 **上下文**（在回调构造时捕获）
- **未绑定** _(unbound)_ 的数据，即执行回调时需要额外传入的数据

> 带有绑定数据的回调，又叫做 **闭包**。（参考：[对编程范式的简单思考](Thinking-Programming-Paradigms.md)）

对于 **同步回调** 执行时的调用栈和构造时一致，只要程序员思路清晰，一般不会出现上下文失效的情况。例如，累加一组得分（使用 `lambda` 表达式捕获上下文 `total`）：

``` cpp
int total = 0;
std::for_each(std::begin(scores), std::end(scores),
              [&total](auto score) { total += score; });
//             ^ context variable |total| is always valid
```

而 **异步回调** 执行时的调用栈往往和构造时的不一致；所以，在回调的 **调用时刻**，构造时捕获的上下文 **可能失效**。例如，用户界面为了不阻塞 **UI 线程** 响应用户输入，在 **后台线程** 从文件里异步加载背景图片，加载完成后再从 **UI 线程** 显示到界面上：

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

> 使用 C++ 11 lambda 表达式描述：
> 
> ``` cpp
> void View::FetchImageAsync(const std::string& filename) {
>   base::PostTaskAndReplyWithResult(
>       FROM_HERE, base::Bind(&LoadImageFromFile, filename),
>       base::Bind([this](const Image& image) {
>         // WARNING: |this| may be invalid now!
>         if (background_image_view_)
>           background_image_view_->SetImage(image);
>       }));
> }
> ```

在 `LoadImageFromFile` 返回时：

- 如果 `View` 对象仍然有效（界面还在显示），则执行 `background_image_view_->SetImage(image)`
- 否则，`background_image_view_` 变成 [野指针 _(wild pointer)_](https://en.wikipedia.org/wiki/Dangling_pointer)，从而导致 **崩溃**

上述代码第二个 `base::Bind` 的参数传递都是 **不安全的**（无法通过编译）：

- 传递普通对象的裸指针（容易导致悬垂引用）
- 传递捕获了上下文的 lambda 表达式（很难限制 lambda 表达式捕获了什么）

> 注：
> 
> - `base::PostTaskAndReplyWithResult` 属于 Chromium 的多线程任务模型（参考：[Keeping the Browser Responsive | Threading and Tasks in Chrome](https://github.com/chromium/chromium/blob/master/docs/threading_and_tasks.md#keeping-the-browser-responsive)）
> - 在 **C++ 核心指南** _(C++ Core Guidelines)_ 里也有类似的讨论：
>   - [F.52: Prefer capturing by reference in lambdas that will be used locally, including passed to algorithms](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-reference-capture)
>   - [F.53: Avoid capturing by reference in lambdas that will be used nonlocally, including returned, stored on the heap, or passed to another thread](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-value-capture)

### 如何处理失效的回调上下文

如果捕获的上下文失效，回调应该 **及时取消**。对于上边的例子，可以给 `base::Bind` 传递 `View` 对象的 **弱引用** _(weak reference)_ `base::WeakPtr<View>`：

``` cpp
void View::FetchImageAsync(const std::string& filename) {
  base::PostTaskAndReplyWithResult(
      FROM_HERE, base::Bind(&LoadImageFromFile, filename),
      base::Bind(&View::LoadImageCallback,
                 weak_factory_.GetWeakPtr()));
  //             ^ use |WeakPtr| rather than raw |this|
}
```

> 使用 C++ 11 lambda 表达式描述：
> 
> ``` cpp
> void View::FetchImageAsync(const std::string& filename) {
>   base::PostTaskAndReplyWithResult(
>       FROM_HERE, base::Bind(&LoadImageFromFile, filename),
>       base::Bind([](const base::WeakPtr<View>& weak_ptr,
>                     const Image& image) {
>         // check if |this| is valid
>         if (weak_ptr && background_image_view_)
>           background_image_view_->SetImage(image);
>       }, weak_factory_.GetWeakPtr()));
> }
> ```

在 `LoadImageFromFile` 返回时：

- 如果 `View` 对象仍然有效（界面还在显示），则执行 `background_image_view_->SetImage(image)`
- 否则，弱引用失效，**不执行回调**（因为界面已经退出，**没必要** 在设置图片了）

> 注：
> 
> - [`base::WeakPtr`](https://github.com/chromium/chromium/blob/master/base/memory/weak_ptr.h) 属于 **侵入式** _(intrusive)_ 智能指针，不支持跨线程使用
> - 基于弱引用指针，Chromium 封装了 **可取消** _(cancelable)_ 回调 [`base::CancelableCallback`](https://github.com/chromium/chromium/blob/master/base/cancelable_callback.h)，提供 `Cancel`/`IsCancelled` 接口。（参考：[Cancelling a Task | Threading and Tasks in Chrome](https://github.com/chromium/chromium/blob/master/docs/threading_and_tasks.md#cancelling-a-task)）

## 回调只能执行一次还是可以多次

软件设计里，只有三个数 —— [`0`，`1`，`∞`（无穷）](https://en.wikipedia.org/wiki/Zero_one_infinity_rule)。类似的，不管是同步回调还是异步回调，我们只关心它被执行 `0` 次，`1` 次，还是若干次。所以，Chromium 从设计上，把回调分为两种：

- **一次** _(once)_ 回调
  - 使用 `base::BindOnce` 构造为 `base::OnceCallback`
  - 只提供 `R Run(Args... args) &&` 接口，只能通过 `std::move(callback).Run(...)` 一次性调用
- **多次** _(repeating)_ 回调
  - 使用 `base::BindRepeating` 构造为 `base::RepeatingCallback`
  - 还提供 `R Run(Args... args) const &` 接口，可以通过 `callback.Run(...)` 多次调用

Chromium 区分回调的可调用次数：一方面，在语义上更明确；另一方面，更易于管理 [对象的生命周期 _(object lifetime)_](https://en.wikipedia.org/wiki/Object_lifetime)。（参考：[资源管理小记](../2018/Resource-Management.md)）

### 如何管理上下文生命周期

从生命周期的角度看，回调上下文分为两种：**需要回调对象管理生命周期的** 和 **不由回调对象管理的**。目前，Chromium 支持的上下文绑定方式有：

| | 回调参数类型（目的）| 绑定数据类型（源）| 回调对象是否管理上下文生命周期 |
|-|---|---|---|
| `std::ref/cref()` | `T&` / `const T&` | `T&` / `const T&` | 否，自己保证上下文有效性 |
| `base::Unretained()` | `T*` | `T*` | 否，自己保证上下文有效性 |
| `base::WeakPtr<>` | `T*` | `base::WeakPtr<>` | 否，弱引用保证上下文有效性 |
| `base::Owned()` | `T*` | `T*` | 是，`delete` |
| `std::unique_ptr<>` | `T*` / `std::unique_ptr<>` | `std::unique_ptr<>` | 是，`~std::unique_ptr()` |
| `base::RetainedRef()` | `T*` | `scoped_refptr<>` | 是，`~scoped_refptr()` |
| `scoped_refptr<>` | `scoped_refptr<>` | `scoped_refptr<>` | 是，`~scoped_refptr()` |

- 在构造回调时，使用 `std::forward` 构造上下文变量
  - 提供辅助函数
- 在执行回调时，通过拷贝还是移动传递上下文

> Chromium 实现细节：
> 
> - 对于 `base::OnceCallback` 使用 `base::internal::Invoker::RunOnce`，使用 `std::move` 移动上下文（右值引用）
> - 对于 `base::RepeatingCallback` 使用 `base::internal::Invoker::Run`，直接传递上下文（左值引用）

**可拷贝** _(copyable)_

- `std::bind` 由上下文决定是否可拷贝
- `std::function` 必须可拷贝的
- `base::OnceCallback` 不可拷贝
- `base::RepeatingCallback` 使用 `scoped_refptr<BindStateBase>`

### 何时销毁捕获的上下文

对于 C 语言里的回调 **没有闭包的概念**，如果需要在异步回调里传递上下文，一般需要由 **发送者申请、填充内存**，由 **接收者销毁、释放内存**。例如，使用 libevent 监听 socket 可写事件，实现异步/非阻塞发送数据（[例子来源](../2017/Callback-vs-Interface.md#C-语言中的回调)）：

``` c
// callback code
void do_send(evutil_socket_t fd, short events, void* context) {
  char* buffer = (char*)context;
  // ... send |buffer| via |fd|
  free(buffer);
}

// client code
char* buffer = malloc(buffer_size);
// ... fill |buffer|
event_new(event_base, fd, EV_WRITE, do_send, buffer);
```

- client 代码 **申请** 发送缓冲区 `buffer` 资源，并作为 `context` 传入注册函数
- callback 代码从 `context` 中取出 `buffer`，发送数据后 **释放** `buffer` 资源
- 如果 `do_send` 没有被执行，申请的 `buffer` 就不会被释放，从而导致 **内存泄漏** _(memory leak)_

问题来了：**如果回调没有被执行**（`0` 次回调），**上下文又该如何销毁**？

Chromium 的回调支持闭包的概念，将 **上下文**（绑定的数据）存储在回调对象里，**生命周期跟随** 对应的 **回调对象**。

| | 一次回调 | 多次回调 |
|-|---------|---------|
| 何时销毁回调对象 | 执行（一次）后销毁 | 所有调用结束后销毁，从而保证每次调用时有效 |
| 调用时是否销毁上下文 | 是，直接把上下文移动给函数 | 总不，每次传递上下文的引用 |
| 回调对象销毁时是否销毁上下文 | 如果没被调用，则此时销毁 | 总是 |
| 回调对象/上下文 生命周期 | 回调对象 ≥ 上下文 | 回调对象 = 上下文 |

Chromium 的回调机制，明确了回调对象/上下文的生命周期，能有效避免内存泄漏。

> 补充：如果使用 **协程**，也有类似的资源管理问题
> 
> - [Goroutine Leaks - The Forgotten Sender](https://www.ardanlabs.com/blog/2018/11/goroutine-leaks-the-forgotten-sender.html)（回调准备好了，但发送方不开始 —— 回调不被执行，也不被释放）
> - [Goroutine Leaks - The Abandoned Receivers](https://www.ardanlabs.com/blog/2018/12/goroutine-leaks-the-abandoned-receivers.html)（回调执行后，发送方不结束 —— 回调没有结束，也不被释放）

这里又引入了另一个 **不易察觉问题**：由于 **一次调用** 的上下文 **销毁时机不确定**，回调不一定会执行。如果其上下文中包含了 **复杂析构函数** 的对象（例如，析构时做数据上报），那么析构时需要检查依赖条件的有效性（例如，检查数据上报环境是否有效），否则导致崩溃。

## 写在最后 [no-toc]

Chromium 的 Bind/Callback 实现基于现代 C++ 元编程技术。

之所以 C++ 的回调关心同步异步/调用次数，是因为 C++ 要求程序员 **管理对象的声明周期**。

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2019, BOT Man
