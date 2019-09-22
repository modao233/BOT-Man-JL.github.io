# 漫谈 C++ 的各种检查

> 2019/9/20
> 
> What you don't use you don't pay for. (zero-overhead principle) —— Bjarne Stroustrup

在学习了 [Chromium/base 库](https://github.com/chromium/chromium/tree/master/base)（[笔记](Chromium-Base-Notes.md)）后，我体会到了一般人和 **优秀工程师** 的差距 —— 拥有较高的个人素质固然重要，但更重要的是能 **降低开发门槛**，让其他人更快的融入团队，一起协作（尤其像 Chromium **开源项目** 由社区维护，开发者水平参差不齐）。

> 没吃过猪肉，但见过猪跑。🙃

项目中，降低开发门槛的方法有很多：除了 制定 **代码规范**、划分 **功能模块**、完善 **单元测试** _(unit test)_、推行 **代码审查** _(code review)_、整理 **相关文档** 之外，针对强类型的编译语言 C++，Chromium/base 库加入了大量的 **检查** _(check)_。

为什么代码中需要各种检查？在 C++ 中调用一个函数、使用一个类、实例化一个模板时，对传入的参数、使用的时机，往往会有很多 **限制** _(constraint/restriction)_（例如，数值参数不能传入负数、对象的访问不是线程安全的、函数调用不能重入）；而处理限制问题的方法 大致分为三种：

1. 口口相传：在 **代码审查** 时，有经验的开发者 向 新手开发者 传授经验（很容易失传）
2. 文档说明：在 **相关文档** 中，提示使用者 功能模块的各种隐含限制（很容易被忽略）
3. 检查限制：在合理划分 **功能模块** 的前提下，对模块的隐含限制 进行检查，并加入针对检查的 **单元测试**（最安全的保障，单元测试即文档）

本文主要分享 我对 Chromium/base 库中使用的限制检查的一些理解：

[TOC]

## 编译时检查

编译时静态检查，主要依靠 C++ 语言提供的 **语法支持**/**静态断言** 和 **编译器扩展** 实现 —— 在检查失败的情况下，编译失败。

### 测试设施

如何确保代码中添加的检查有效呢？最高效的方法是：为 “检查” 添加单元测试。但对于 编译时检查 遇到了一个 **难点** —— 如果检查失败，那么编译就无法通过。

Chromium 为此支持了 [编译失败测试 _(no-compile test)_](https://dev.chromium.org/developers/testing/no-compile-tests)：

- 单元测试文件中，每个用例通过 `#ifdef` 切割
- 每个用例中，标明 编译失败后出现的 报错细节
- 通过 `#define` 运行各个用例
- 在编译失败后，检查 报错细节 是否完全一致

对应的单元测试文件后缀为 `*_unittest.nc`，通过 [`nocompile.gni`](https://github.com/chromium/chromium/blob/master/build/nocompile.gni) 加入单元测试工程。

### 可拷贝性检查

C++ 语言本身加入了很多编译时检查（例如 类的成员访问控制 _(member access control)_、`const` 关键字 在编译成汇编语言后，不能反编译还原），但 C++ 对象默认是可拷贝的，从而带来了许多不必要的麻烦（参考 [资源管理小记](../2018/Resource-Management.md#资源和对象的映射关系)）。

> 尤其是 **多态** _(polymorphic)_ 类的默认拷贝行为，一般都不符合预期：
> 
> - [C.67: A polymorphic class should suppress copying](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rc-copy-virtual)
> - [C.130: For making deep copies of polymorphic classes prefer a virtual clone function instead of copy construction/assignment](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-copy)

为此，Chromium 提供了两个常用的宏：

- `DISALLOW_COPY_AND_ASSIGN` 用于禁用类的 拷贝构造函数 和 拷贝赋值函数
- `DISALLOW_IMPLICIT_CONSTRUCTORS` 用于禁用类的 默认构造函数

由于 Chromium 大量使用了 C++ 的多态特性，这些宏随处可见。

### 参数类型检查

Chromium 还基于 [现代 C++ 元编程](../2017/Cpp-Metaprogramming.md) 技术，通过 `static_assert` 进行静态断言。

在之前写的 [深入 C++ 回调](Inside-Cpp-Callback.md) 中，分析了 Chromium 的 [`base::Callback<>` + `base::Bind()`](https://github.com/chromium/chromium/blob/master/docs/callback.md) 回调机制，提到了 `base::Bind` 的静态断言检查。

`base::Bind` 为了 [处理失效的（弱引用）上下文](Inside-Cpp-Callback.md#如何处理失效的（弱引用）上下文)，针对弱引用指针 [`base::WeakPtr`](https://github.com/chromium/chromium/blob/master/base/memory/weak_ptr.h) 扩展了 `base::IsWeakReceiver` 检查，判断弱引用的上下文是否有效；并通过 静态断言检查传入参数，强制要求使用者 遵循弱引用检查的规范：

- `base::Bind` 不允许直接将 **`this` 指针** 绑定到 类的成员函数 上，因为 `this` 裸指针可能失效 变成野指针
- `base::Bind` 不允许绑定 **lambda 表达式**，因为 `base::Bind` 无法检查 lambda 表达式捕获的 弱引用 的 有效性
- `base::Bind` 只允许将 `base::WeakPtr` 指针绑定到 **没有返回值的**（返回 `void`）类的成员函数 上，因为 当弱引用失效时不调用回调，也不会生成返回值

`base::Callback` 区分 [回调只能执行一次还是可以多次](Inside-Cpp-Callback.md#回调只能执行一次还是可以多次)，通过引用限定符 `&&` / `const &`，区分 在对象处于 非 const 右值 / 其他 状态时的 `Run` 成员函数；并通过 静态断言检查对象状态，只允许一次回调 `base::OnceCallback` 在 非 const 右值 状态下调用 `Run` 函数，从而实现了更严谨的 [资源管理语义](../2018/Resource-Management.md#资源和对象的映射关系)：

- 只有 `base::OnceClosure cb; std::move(cb).Run();` 可以编译
  - `base::OnceClosure cb; cb.Run();` 编译错误
  - `const base::OnceClosure cb; cb.Run();` 编译错误
  - `const base::OnceClosure cb; std::move(cb).Run();` 编译错误
- 此外，`base::OnceCallback` 支持绑定 `base::RepeatingCallback` 回调，反之不行

另外，静态断言检查还广泛应用在 **容器**、**智能指针** 的模板实现中，用于生成 [可读性更好的](../2017/Cpp-Metaprogramming.md#实例化错误) **实例化错误信息**。

### 线程标记检查

最新的 Chromium 使用了 Clang 编译代码，通过扩展 **线程标记** _(thread annotation)_，[分析线程安全问题](https://clang.llvm.org/docs/ThreadSafetyAnalysis.html)。（参考 [Thread Safety Annotations for Clang - DeLesley Hutchins](https://llvm.org/devmtg/2011-11/Hutchins_ThreadSafety.pdf)）

其中，单元测试文件 [`thread_annotations_unittest.nc`](https://github.com/chromium/chromium/blob/master/base/thread_annotations_unittest.nc) 描述了一些 锁的错误使用场景 —— 假设数据 data 标记为  `GUARDED_BY` 锁 lock：

- 访问 data 之前，忘记获取 lock
- 获取 lock 之后，忘记释放 lock

## 运行时检查

运行时动态检查，主要基于 Chromium/base 库提供的 **断言 `DCHECK`/`CHECK`** 实现 —— 如果断言失败，运行着的程序会立即终止。

其中，`DCHECK` 只对 调试版 _(debug)_ 有效，而 `CHECK` 也可用于 发布版 _(release)_ —— 从而避免在发布版进行 无用的检查。

### 测试设施

检查的方法很直观 —— 构造一个检查失败的场景，期望断言失败。Chromium/base 基础设施中的 `EXPECT_DCHECK_DEATH` 提供了这个功能。

对应的单元测试文件后缀为 `*_unittest.cc`。

### 数值溢出检查

在 C++ 里，数值类型 都是固定大小的 [标量类型](https://en.cppreference.com/w/cpp/language/type) —— 如果存储超出范围的数据，会导致 **溢出** _(overflow)_。

很多时候，溢出是一个非常令人头疼的问题。例如，尝试通过 **使用无符号数 避免出现负数**，往往是一个典型的徒劳之举。（比如 `unsigned(0) - unsigned(1) == UINT_MAX`，参考 [ES.106: Don’t try to avoid negative values by using `unsigned`](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Res-nonnegative)）

为此，Chromium 的 [base/numerics](https://github.com/chromium/chromium/tree/master/base/numerics) 提供了一个 无依赖 _(dependency-free)_、仅头文件 _(header-only)_ 的模板库，应对数值溢出问题：

- `base::StrictNumeric`/`base::strict_cast<>()` **编译时 阻止溢出** —— 如果 类型转换 有溢出的可能性，通过静态断言报错
- `base::CheckedNumeric`/`base::checked_cast<>()` **运行时 检查溢出** —— 如果 数值运算/类型转换 出现溢出，立即终止程序
- `base::ClampedNumeric`/`base::saturated_cast<>()` **运行时 截断运算** —— 如果 数值运算/类型转换 出现溢出，对计算结果 **截断** _(non-sticky saturating)_ 处理

### 观察者模式检查

在之前写的 [令人抓狂的 观察者模式](Insane-Observer-Pattern.md) 中，介绍了如何通过 Chromium/base 提供的 [`base::ObserverList`](https://github.com/chromium/chromium/blob/master/base/observer_list.h)，检查 观察者模式 中一些潜在的问题。

#### 生命周期检查

由于观察者和被观察者的生命周期往往是独立的，所以总会发生一些阴差阳错的问题：

- [观察者先销毁](Insane-Observer-Pattern.md#问题-观察者先销毁)
  - 问题：若 `base::ObserverList` 通知时不检查 观察者是否有效，可能导致 野指针崩溃
  - 解决：观察者继承于 [`base::CheckedObserver`](https://github.com/chromium/chromium/blob/master/base/observer_list_types.h)，在通知前 `base::ObserverList` 检查观察者弱引用 `base::WeakPtr` 的有效性
- [被观察者先销毁](Insane-Observer-Pattern.md#问题-被观察者先销毁)
  - 问题：若 `base::ObserverList` 销毁时不检查 观察者列表是否为空，可能导致 被观察者销毁后，观察者不能再移除（野指针崩溃）
  - 解决：模板参数 `check_empty` 若为 `true`，在析构时断言 “观察者已被全部移除”

#### 通知迭代检查

观察者可能在 `base::ObserverList` 通知时访问 `base::ObserverList` 对象：

- 添加观察者
  - 问题：是否需要在本次迭代中，继续通知新加入的观察者
  - 解决：被观察者参数 `base::ObserverListPolicy` 决定迭代过程中，是否通知新加入的观察者
- 移除观察者
  - 问题：循环内（间接）删除迭代器，导致迭代器失效（崩溃）`for(auto it = c.begin(); it != c.end(); ++it) c.erase(it);`
  - 解决：迭代器对象成员 `MarkForRemoval()` 标记为 “待移除”，然后等迭代结束后移除
- 通知迭代重入
  - 问题：许多情况下，没有对重入进行特殊处理，可能会导致 [死循环问题](Insane-Observer-Pattern.md#问题-死循环)
  - 解决：模板参数 `allow_reentrancy` 若为 `false`，在迭代时断言 “正在通知迭代时 不允许重入”
- 线程安全问题
  - 问题：由于 `base::ObserverList` 不是线程安全的，在通知迭代中，需要保证其他操作在同一线程/序列
  - 解决：被观察者成员 `iteration_sequence_checker_` 线程/序列检查器 在迭代开始时关联线程/序列，在结束时取消关联（参考 [sec|线程相关检查]）

和 [`base::Singleton`](https://github.com/chromium/chromium/blob/master/base/memory/singleton.h) 一样，Chromium/base 的设计模式实现 堪称 C++ 里的典范 —— 无论是功能上，还是性能上，均为 “人无我有，人有我优”。

### 线程相关检查

多线程环境下，我们往往需要关注以下几个问题：

- 数据竞争 _(data race)_：保证某个对象在同一个 线程/序列 中 创建/访问/销毁，而不使用 锁 或 原子操作
- 线程响应性 _(responsive)_：对于某些 线程/序列，不能进行 阻塞 / CPU 密集 / 涉及 `AtExit`/ 锁相关 操作
- 死锁问题 _(deadlock)_：能在调试模式下，尽可能的发现死锁问题

#### 线程安全检查

- `base::ThreadChecker/SequenceChecker` 检查对象 是否线程安全
- 例如，`base::ObserverList` 在迭代时检查

#### 线程限制检查

- `base::ThreadRestrictions` 检查调用 对线程的影响
- `base::WatchDog` 启动后台线程，监控其他线程心跳情况

#### 死锁检查

- `base::internal::CheckedLock` 检查死锁

## 写在最后 [no-toc]

由于 Chromium/base 改动频繁，本文某些细节 **可能会过期**。如果有什么新发现，**欢迎补充**~ 😉

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2019, BOT Man
