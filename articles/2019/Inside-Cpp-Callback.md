# 深入 C++ 回调

> 2019/3/21
> 
> 本文主要分析 [Chromium 的 Bind/Callback 机制](https://github.com/chromium/chromium/blob/master/docs/callback.md)，并讨论设计 C++ 回调时 ~~你想知道的~~（你可能不知道的）一些问题

- 如果你还不知道什么是 **回调** _(callback)_，欢迎阅读 [如何浅显的解释回调函数](../2017/Callback-Explained.md)
- 如果你还不清楚 **可调用对象** _(callable object)_ 和 **回调接口** _(callback interface)_ 的区别，欢迎阅读 [回调 vs 接口](../2017/Callback-vs-Interface.md)（本问主要讨论类似 `std::function` 的 **可调用对象**，而不是基于接口的回调）

## 写在前面

回调是一个大家所熟知，而且日常频繁使用的概念：

- **图形界面客户端** 常用 [事件循环 _(event loop)_](https://en.wikipedia.org/wiki/Event_loop) 有条不紊的处理 用户输入/计时器/系统处理/跨进程通信 等事件
- **I/O 密集型程序** 常用 [异步 I/O _(asynchronous I/O)_](https://en.wikipedia.org/wiki/Asynchronous_I/O) 提高吞吐率，进一步引申出 [Reactor 模式](https://en.wikipedia.org/wiki/Reactor_pattern)、[纤程 _(fiber)_](https://en.wikipedia.org/wiki/Fiber_%28computer_science%29)（系统上）/ [协程 _(coroutine)_](https://en.wikipedia.org/wiki/Coroutine)（语言上）等概念

如果你已经熟悉了 [`std::bind`](https://en.cppreference.com/w/cpp/utility/functional/bind)/[lambda 表达式](https://en.cppreference.com/w/cpp/language/lambda) + [`std::function`](https://en.cppreference.com/w/cpp/utility/functional/function)，那么你在设计 C++ 回调时，**是否考虑过这几个问题**：

- 回调是 **同步** _(sync)_ 的还是 **异步** _(async)_ 的
  - 如果异步，回调时上下文是否仍然有效
  - [F.52: Prefer capturing by reference in lambdas that will be used locally, including passed to algorithms](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-reference-capture)
  - [F.53: Avoid capturing by reference in lambdas that will be used nonlocally, including returned, stored on the heap, or passed to another thread](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-value-capture)
  - 异步回调时捕获的 **上下文失效**，应如何处理
  - 异步回调是否 **可取消** (cancelable)
- 回调对象和上下文是否 **可拷贝** _(copyable)_
  - 在构造回调上下文时，通过拷贝还是移动捕获
  - 在拷贝回调对象时，捕获的上下文对象执行拷贝还是移动（ref_count）
  - 在执行回调时，如何传递捕获的上下文
- 回调只执行 **一次** _(once)_ 还是执行 **多次** _(repeating)_
  - 何时销毁捕获的上下文
  - 如果没有被执行，上下文如何销毁（是否有副作用）
  - 如果上下文失效，如何取消执行
  - [Goroutine Leaks - The Forgotten Sender](https://www.ardanlabs.com/blog/2018/11/goroutine-leaks-the-forgotten-sender.html)，不开始
  - [Goroutine Leaks - The Abandoned Receivers](https://www.ardanlabs.com/blog/2018/12/goroutine-leaks-the-abandoned-receivers.html)，不结束

## 写在最后

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2019, BOT Man
