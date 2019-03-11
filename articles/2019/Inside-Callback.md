# 深入回调机制

> 2019/1/12
> 
> 本文主要分析 [Chromium 的 Bind/Callback 机制](https://github.com/chromium/chromium/blob/master/docs/callback.md)，并讨论设计 C++ 回调时 ~~你想知道的~~（你可能不知道的）一些问题

## 写在前面

- 如果你还不知道什么是 **回调** _(callback)_，欢迎阅读 [如何浅显的解释回调函数](../2017/Callback-Explained.md)
- 如果你还不清楚 **可调用对象** _(callable object)_ 和 **回调接口** _(callback interface)_ 的区别，欢迎阅读 [回调 vs 接口](../2017/Callback-vs-Interface.md)
- 消息队列/事件循环 -> 纤程/协程

如果你已经熟悉了 [`std::bind`](https://en.cppreference.com/w/cpp/utility/functional/bind)/[lambda 表达式](https://en.cppreference.com/w/cpp/language/lambda) + [`std::function`](https://en.cppreference.com/w/cpp/utility/functional/function)，那么在设计 C++ 回调时，你是否考虑过这几个问题：

- 回调是 **同步** _(sync)_ 的还是 **异步** _(async)_ 的
  - 如果异步，回调时上下文是否仍然有效
  - [F.52: Prefer capturing by reference in lambdas that will be used locally, including passed to algorithms](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-reference-capture)
  - [F.53: Avoid capturing by reference in lambdas that will be used nonlocally, including returned, stored on the heap, or passed to another thread](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-value-capture)
- 回调对象是否 **可拷贝** _(copyable)_
  - 如果可以，在回调对象拷贝时，捕获的上下文对象执行拷贝还是移动
- 回调只执行 **一次** _(once)_ 还是执行 **多次** _(repeating)_
  - 如果多次，何时销毁捕获的上下文
  - 如果多次，每次执行如何传递不可拷贝的上下文
  - 如果没有被执行，上下文如何销毁
- 回调时如果部分 **上下文失效**，应如何处理

Go 语言也有类似的问题：

- [Goroutine Leaks - The Forgotten Sender](https://www.ardanlabs.com/blog/2018/11/goroutine-leaks-the-forgotten-sender.html)
- [Goroutine Leaks - The Abandoned Receivers](https://www.ardanlabs.com/blog/2018/12/goroutine-leaks-the-abandoned-receivers.html)

## 写在最后

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2019, BOT Man
