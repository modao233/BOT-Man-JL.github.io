# 资源管理小记

> 2018/1/10
>
> TODO

[heading-numbering]

## [no-toc] [no-number] TOC

[TOC]

## 背景

**资源** _(resource)_ 是 _使用前需要被获取、使用后需要被释放_ 的东西。

- 从计算机的角度来看，常见的资源有：内存、锁、socket、文件、线程等
- 从人类的角度来看，资源是真实世界的业务逻辑上的 **实体** _(entity)_ 在计算机中的表示

资源管理是程序设计中的一个重要问题：

- 资源是有限的
  - 如果程序获取了资源后不释放，就会导致资源 **泄露** _(leak)_
  - 由于资源的有限性，其他程序会申请不到这些资源
- 所以资源需要被释放
  - 如果程序访问了已经被释放的资源，可能出现 **异常** _(exception)_
  - 如果程序没有做相应的检查，就会崩溃

## 资源在面向对象程序设计中的管理

这里主要讨论面向对象程序设计中的资源管理。

### 资源和对象生命周期

> Item 13: Use objects to manage resources. —— Meyer Scott, _Effective C++ 3rd Edition_

在面向对象程序设计中，资源往往通过以对象为载体。所以很多时候，资源的管理 **可以（但不一定）** 通过对象的 **生命周期** _(lifetime)_ 管理实现 —— 对象创建时，获取资源；对象销毁时，释放资源。

而对象的销毁主要有两种方式：

- [手动管理](https://en.wikipedia.org/wiki/Manual_memory_management) —— 需要在代码中显式销毁对象
- 自动管理 —— 一般通过 [垃圾回收](https://en.wikipedia.org/wiki/Garbage_collection_%28computer_science%29) 的方式进行

对于以上两种方式，没有优劣，但不同应用场景下各有利弊。例如：

- 手动管理
  - 不释放不再使用的资源，会导致泄露问题（可以使用 [RAII 范式](https://en.wikipedia.org/wiki/Resource_acquisition_is_initialization) 避免）
  - 使用已经被释放的资源，会导致悬垂引用问题（可以通过观察者模式让引用失效，[sec|一对多关系]）
  - 需要注意资源释放的时机，如果在资源正在使用时（常见的是对象回调栈上）释放，可能导致崩溃问题（可以通过异步释放的方式改进）
- 自动管理
  - （基于 [引用计数](https://en.wikipedia.org/wiki/Reference_counting#Variants_of_reference_counting) 的自动管理）对象之间循环引用，会导致资源泄露的问题（可以使用 [弱引用](https://en.wikipedia.org/wiki/Weak_reference) 解决）
  - （基于可达性的自动管理）忘记断开对不再使用的对象的引用，导致资源泄露的问题（只能需要程序员注意）

[align-center]

![GC Roots With Memory Leak](Resource-Management/gc-roots-with-memory-leak.png)

[align-center]

图：被遗忘的引用 导致内存泄露（[来源](https://www.dynatrace.com/resources/ebooks/javabook/how-garbage-collection-works/)）

### 资源和对象的映射关系

> Item 14: Think carefully about copying behavior in resource-managing classes. —— Meyer Scott, _Effective C++ 3rd Edition_

用于资源管理的对象，往往需要注意它的 **复制语义**：

- 如果资源是不可复制的，那么这个对象也是不可复制的
- 如果资源可以复制，那么对象的复制会导致相应资源的复制

这里，就涉及到了资源和对象的 **映射关系**。

#### 一对一关系

同一个资源，在程序中由一个（mutable）对象来管理。

- 一般这个对象是不可以复制的
- 这个对象创建时申请资源，销毁时释放资源
- 对这个（mutable）对象的读取和修改，直接映射到对资源的读取和修改上
- C++ 中可以使用 `unique_ptr` 管理

#### 一对多关系

同一个资源，在程序中有多个（mutable/immutable）对象来表示。

- 一般就涉及到对象共享资源的情况
- 申请资源往往在第一个对象创建时，而释放资源的时机不确定
  - 某一个（或一些）对象销毁后释放，并让其他对象失效
  - 所有对象都销毁时释放
- 读取任意一个对象，都直接映射到对资源的读取上
- 修改其中一个对象
  - （mutable）不仅会修改资源，还会影响其他对象的状态
  - （immutable）只是相当于操作了资源的一个副本，不影响其他对象
- C++ 中可以使用 `shared_ptr/weak_ptr` 管理
  - `shared_ptr` 可以实现 _所有对象都销毁时释放_
  - `weak_ptr` 可以实现 _某一个（或一些）对象销毁后释放，并让其他对象失效_

> 关于 mutable/immutable 对象的讨论参考：
>
> - https://stackoverflow.com/questions/214714/mutable-vs-immutable-objects
> - https://stackoverflow.com/questions/8056130/immutable-vs-mutable-types
>
> 关于 `unique_ptr/shared_ptr/weak_ptr` 参考：
>
> - http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rr-owner

## 超出系统边界的资源管理

- 在一个系统内，一个资源往往可以利用 **一个实体**（业务上）表示，同一个实体也可以用 **多个对象**（代码上）表示
- 而当一个资源被多个系统共享时，就需要用 **多个实体** 来表示这个资源（一个系统存在一个实体），资源对象的管理也变得复杂

下面只是一些遇到的问题和解决的思路：

- 资源的 **释放**
  - 在一个系统内，资源可以由一个（或多个）对象（协同）管理
  - 而多个系统间，资源往往需要一个中心进行协调管理（master-slave 模式）
  - 例如，一个操作系统下运行多个应用程序，每个应用程序是一个系统；操作系统作为中心，负责申请和释放资源；应用程序只是提交申请和释放的请求，操作系统响应这些请求
- 资源的 **同步**
  - 在一个系统内，一个资源对应一个实体；操作实体对应的对象，就实现了对资源的操作
  - 而多个系统间，一个资源对应了多个实体；任意一个系统内，对实体的操作需要影响其他系统里的实体；一般让不同的实体使用同一个标识（ID, identifier）表示同一个资源
  - 例如，不同设备上同步访问的历史记录，每条历史记录需要用一个唯一的 ID 表示；因为，用指针标识对象、用对象标识实体的方法，都不能跨越系统边界（类似于同一组对象/实体生存在多个平行的世界里）
- **事务** 的处理
  - 不管在一个系统内，还是不同系统间，都可能出现同时操作同一个资源的问题
  - 很多时候每一组操作都希望是原子性的，即是一个 **事务** _(transaction)_ —— 同一组操作要么全部完成，要么全都不生效
  - 为了解决这个问题，常常需要引入 [事务处理](https://en.wikipedia.org/wiki/Transaction_processing) 的方法

## 写在最后 [no-number]

之后会继续阅读 **系统设计** 相关的文献，不断完善这篇文章。😊

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2018, BOT Man
