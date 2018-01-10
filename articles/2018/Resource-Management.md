# 程序里的资源管理小记

> 2018/1/10
>
> TODO

[heading-numbering]

## [no-toc] [no-number] TOC

[TOC]

## 背景

**资源** _(resource)_ 是可以被获取，使用结束后需要被释放的东西。

- 从计算机的角度来看，常见的资源有：内存、锁、socket、文件、线程等
- 从人类的角度来看，资源可以是真实世界里 **实体** _(entity)_ 在计算机中的表示

如果一个长期运行的程序，获取了资源后没有释放，就会导致资源 **泄露** _(leak)_。资源如果长期泄露，就会导致其他申请的程序得不到资源，从而导致资源不足、程序崩溃的问题。

### 资源和对象生命周期

> Item 13: Use objects to manage resources. —— Meyer Scott, _Effective C++ 3rd Edition_

在面向对象程序设计中，资源往往通过以对象为载体 —— 对象创建时，获取资源；对象销毁时，释放资源。所以很多时候，资源的管理通过对象的 **生命周期** _(lifetime)_ 管理实现。

### 资源和可变、不变对象

> Item 14: Think carefully about copying behavior in resource-managing classes. —— Meyer Scott, _Effective C++ 3rd Edition_



## 系统边界内的资源管理

- 一个实体对应一个对象
- 分类
  - 自动（scoped/temporary）
  - 静态（static/global）
  - 自由
    - 垃圾回收
    - 错误引用导致资源泄露
    - 悬垂引用/观察者模式

[![GC Roots With Memory Leak](Resource-Management/gc-roots-with-memory-leak.png)](https://www.dynatrace.com/resources/ebooks/javabook/how-garbage-collection-works/)

## 系统边界外的资源管理

- 一个实体对应不同对象
- ID 同步
- 事务处理

## 写在最后 [no-number]

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2018, BOT Man
