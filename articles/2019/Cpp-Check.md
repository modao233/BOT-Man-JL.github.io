# 漫谈 C++ 的各种检查

> 2019/9/20
> 
> What you don't use you don't pay for. (zero-overhead principle) —— Bjarne Stroustrup

在学习了 Chromium/base 库（[笔记](Chromium-Base-Notes.md)）后，我体会到了优秀工程师和其他人的差距 —— 拥有较高的个人素质固然重要，但更重要的是能 **降低开发门槛**，让其他人更快的融入团队，一起协作（尤其像 Chromium **开源项目** 由社区维护，开发者水平参差不齐）。

项目中，降低开发门槛的方法有很多 ~~（没吃过猪肉，但见过猪跑）~~：除了 制定 **代码规范**、划分 **功能模块**、完善 **单元测试**、推行 **代码审查**、整理 **相关文档** 之外，针对强类型的编译语言 C++，Chromium/base 库加入了大量的 **静态/动态检查**。

本文主要分享 我对 Chromium/base 库中 C++ 编译时/运行时检查的一些理解：

[TOC]

> 由于 Chromium/base 改动频繁，本文某些细节可能会过期。如果有什么新发现，**欢迎补充**~ 😉

## 静态检查

单元测试 no-compile test `*_unittest.nc`

### 语法支持

- `DISALLOW_COPY_AND_ASSIGN`
- `DISALLOW_IMPLICIT_CONSTRUCTORS`

### 静态断言

主要基于 [C++ 元编程](../2017/Cpp-Metaprogramming.md) 技术。

例如，[深入 C++ 回调](Inside-Cpp-Callback.md)

## 动态检查

主要基于 `CHECK`/`DCHECK` 基础设施。

单元测试 `EXPECT_DCHECK_DEATH`

### 数值溢出

> [ES.106: Don’t try to avoid negative values by using `unsigned`](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Res-nonnegative)

### 观察者模式

参考：[令人抓狂的 观察者模式](Insane-Observer-Pattern.md)

### 线程相关

- `base::ThreadRestrictions` 检查调用 对线程的影响
- `base::ThreadChecker/SequenceChecker` 检查对象 是否线程安全
- `base::internal::CheckedLock` 检查死锁

## 工具检查

TODO

## 写在最后 [no-toc]

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2019, BOT Man
