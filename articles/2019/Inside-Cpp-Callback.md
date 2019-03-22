# 深入 C++ 回调

> 2019/3/21
> 
> 本文主要分析 [Chromium 的 Bind/Callback 机制](https://github.com/chromium/chromium/blob/master/docs/callback.md)，并讨论设计 C++ 回调时 ~~你想知道的~~（你可能不知道的）一些问题

- 如果你还不知道什么是 **回调** _(callback)_，欢迎阅读 [如何浅显的解释回调函数](../2017/Callback-Explained.md)
- 如果你还不清楚 **可调用对象** _(callable object)_ 和 **回调接口** _(callback interface)_ 的区别，欢迎阅读 [回调 vs 接口](../2017/Callback-vs-Interface.md)（本问主要讨论类似 `std::function` 的 **可调用对象**，而不是基于接口的回调）

回调是一个大家所熟知的概念：

- **图形界面客户端** 常用 [事件循环 _(event loop)_](https://en.wikipedia.org/wiki/Event_loop) 有条不紊的处理 用户输入/计时器/系统处理/跨进程通信 等事件，一般采用回调响应事件
- **I/O 密集型程序** 常用 [异步 I/O _(asynchronous I/O)_](https://en.wikipedia.org/wiki/Asynchronous_I/O) 提高吞吐率，进一步引申出 设计上的 [Reactor](https://en.wikipedia.org/wiki/Reactor_pattern)、系统上的 [纤程 _(fiber)_](https://en.wikipedia.org/wiki/Fiber_%28computer_science%29)、语言上的 [协程 _(coroutine)_](https://en.wikipedia.org/wiki/Coroutine) 等概念，一般采用回调处理 I/O 完成的返回结果

如果你已经熟悉了 [`std::bind`](https://en.cppreference.com/w/cpp/utility/functional/bind)/[lambda 表达式](https://en.cppreference.com/w/cpp/language/lambda) + [`std::function`](https://en.cppreference.com/w/cpp/utility/functional/function)，那么你在设计 C++ 回调时，**是否考虑过这几个问题**：

[TOC]

## 回调是同步还是异步的

**同步** _(sync)_ 回调

**异步** _(async)_ 回调

### 如何保证回调的有效性

- [F.52: Prefer capturing by reference in lambdas that will be used locally, including passed to algorithms](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-reference-capture)
- [F.53: Avoid capturing by reference in lambdas that will be used nonlocally, including returned, stored on the heap, or passed to another thread](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-value-capture)

### 如何处理可取消的回调

- 不能保证异步回调总是有效
- 如果捕获的 **上下文失效**，应该及时停止
- 引入 **可取消** _(cancelable)_ 回调

## 回调只能执行一次还是可以多次

**一次** _(once)_

**多次** _(repeating)_

### 何时销毁捕获的上下文

- 多次回调何时销毁
- 一次回调何时销毁
- 如果没有被执行，上下文如何销毁（是否有副作用）
- Go 相关：
  - [Goroutine Leaks - The Forgotten Sender](https://www.ardanlabs.com/blog/2018/11/goroutine-leaks-the-forgotten-sender.html)，不开始
  - [Goroutine Leaks - The Abandoned Receivers](https://www.ardanlabs.com/blog/2018/12/goroutine-leaks-the-abandoned-receivers.html)，不结束

## 回调是否可拷贝

**可拷贝** _(copyable)_

### 回调对象是否可拷贝

ref_count

- 在构造回调时，通过拷贝还是移动捕获上下文
- 在执行回调时，通过拷贝还是移动传递上下文

## 写在最后 [no-toc]

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2019, BOT Man
