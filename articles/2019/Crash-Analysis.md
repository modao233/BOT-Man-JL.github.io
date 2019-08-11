# 崩溃分析笔记

> 2019/8/4
> 
> TODO

[heading-numbering]

## TOC [no-toc] [no-number]

[TOC]

## 知识储备

### 操作系统/汇编指令

- CPU/内存/GPU/磁盘/网卡
- 进程 _(Process)_/线程 _(Thread)_
  - 不同线程 有独立的 调用栈 + 寄存器状态
- 进程地址空间 _(Process Address Space)_
  - TEXT 代码
  - DATA 常量数据
  - BSS 静态变量
  - HEAP 堆
  - MEM_MAPPED 内存映射
  - STACK 栈
- 调用栈 _(Call Stack)_
  - 参数传递，参考【调用约定】
  - 返回数值/指针，通过 `eax` 返回（包括堆上分配的对象）
  - 返回对象，通过第一个参数传入为对象预留的栈上空间地址
- 栈帧 _(Stack Frame)_
  - `eip` 当前指令，自动修改
  - `ebp` 当前栈底，需要手动修改
  - `esp` 当前栈顶，自动修改
- 调用约定 _(Calling Convention)_
  - `__cdecl` 右到左压栈，调用者清栈（默认，支持变长参数)
  - `__stdcall` 右到左压栈，被调用者清栈（代码更小）
  - `__fastcall` 前两个参数放入寄存器，其余的右到左压栈，被调用者清栈
  - _this call_ `this` 放入 `ecx`，右到左压栈，调用者清栈
  - _nacked call_ 不修改栈内存 `push/pop ebp`？
- 调用优化 _(Optimization)_
  - 优先使用寄存器（存储局部变量、传递参数）
  - 如果寄存器不够用，再使用栈上空间

### C++ 编译

- 对象内存布局
- 虚函数调用
- 堆内存管理
  - 调用 `operator new/delete` 申请内存
- 构造函数调用
- 析构函数调用

### 上层知识

- 基础概念
  - 智能指针/设计模式/回调/单例/弱引用/消息循环
  - 日志/字符串操作/时间/文件/系统/i18n/调试辅助
- 代码组织结构
- 软件生命周期模型
  - 客户端模型 = 启动时初始化 -> 处理窗口消息 + 处理队列任务 -> 退出时反初始化
- 进程/线程模型
  - [Chromium 进程](https://developers.google.cn/web/updates/2018/09/inside-browser-part1) = 1 Browser + n Renderer + 1 GPU + x Util
  - [Chromium 线程](https://github.com/chromium/chromium/blob/master/docs/threading_and_tasks.md#threads) = 1 UI (Browser)/Main (Renderer) + 1 IO (for IPC) + x Spec + n Worker (Pool)
- [对象生命周期模型](../2018/Resource-Management.md)
  - 互斥所有权 `std::unique_ptr`
  - 共享所有权 `std::shared_ptr`
  - 弱引用 `std::weak_ptr/base::WeakPtr`
  - 通过外部消息管理 `Class::OnDestroy() { delete this; }`

## 崩溃类型

> Windows 用户态 C++ 程序的常见崩溃

### 野指针崩溃

- 调用栈下层传入的某个指针损坏
- 调用栈上层收到的参数出现错误
- 现象可能是内存访问越界崩溃，也可能是其他崩溃
- Mini Dump 数据不全，只能用 Full Dump 排查

### 空指针崩溃

- 回溯调用栈，可以发现空指针首次出现的位置，并推断出现的路径
- 经常出现于调用前置条件改变，对条件的假设失效

### 指针强转崩溃

- 父类指针 `static_cast<>()` 转成子类时，类型不匹配导致数据错误
- 可能不会立即崩溃，也可能破坏内存导致其他崩溃

### 栈溢出崩溃

- 逻辑错误导致无穷递归调用
- 可能是外部输入导致（例如通过发送 `WM_CLOSE`，导致无限弹出关闭确认对话框）

### 除零崩溃

- 包括 除法 `/` 和 取模 `%`
- `idiv eax, ecx` -> `eax` 商 & `ecx` 余数

### 内存申请崩溃

- 输入大量字符串
- 配置文件过大

### 外部调用崩溃

- `DLLMain`/`WinProc` 回调位置错误
- `QueryPerformanceFrequency`/`localtime(&-1)`/`wcsftime(..., nullptr)` 函数调用崩溃

### 硬件错误崩溃

- 指令寄存器 `eip` 指令对齐错误 `MISALIGNED_CODE`（可能导致汇编指令错误）
- 内存加载错误 `MEMORY_CORRUPTION memory_corruption!MyDll`（可能导致汇编指令错误）
- 内存映射文件 `MEM_MAPPED` 磁盘错误 `IN_PAGE_ERROR hardware_disk!Unknown`

## Windbg 常用命令

- `dv` 列举当前栈上所有变量的值
- `dt ADDR TYPE` 查看 TYPE 类型对象在 ADDR 上的内存布局
- `dx ((TYPE *)ADDR)` 还原 ADDR 上 TYPE 实际的派生类
- `dps __VFN_table` 查看虚函数表指针对应的虚函数地址
- `x MOD!SYM*` 匹配符号（包括虚函数表指针）
- `s -d 0 L80000000 VAL` 搜索内存中是否存在 VAL 值
- `!address ADDR` 查看地址属性（堆/栈，可读写性）

## 分析技巧

> 利用符号、代码，分析 C++ 程序崩溃 Dump

### 判断对象是否有效

- 如果 `__VFN_table` 有效，表示对象头部完整
- 如果 [`base::WeakPtrFactory::ptr_ == this`](https://cs.chromium.org/chromium/src/base/memory/weak_ptr.h?q=base::WeakPtrFactory) 表示对象尾部完整
- 通过 `ref_count_/size_` 等变量推测对象数据成员是否有效
- 判断是否执行过析构函数（例如 `ptr_ = nullptr;`）；但如果内存被回收再利用，新数据可能覆盖写入

### 查找内存中的对象

- `x` 列出类的（所有）虚函数表指针
- `s -d` 搜索内存中的虚函数表指针（多重继承取最小指针）
- `dt` 还原对象内存布局
- 通过再次搜索（成员指针）或 减去偏移量（成员对象），找到对象的持有者

### 排查异步回调任务

- 通过 [`base::Location base::PendingTask::posted_from;`](https://cs.chromium.org/chromium/src/base/pending_task.h?q=base::PendingTask) 定位任务抛出的来源

## 参考 [no-number]

- [Windbg 调试命令详解 - 张佩](http://yiiyee.cn/blog/2013/08/23/windbg/)
- [利用 C++ 类对象的虚拟函数表指针在内存中搜索对象 - 张佩](https://blog.csdn.net/blog_index/article/details/7016696)
- [从堆里寻找丢失的数据 - 张银奎](http://advdbg.org/blogs/advdbg_system/articles/3413.aspx)
- [使用 Windbg 分析 C++ 的虚函数表原理 - bingoli](https://bingoli.github.io/2019/03/27/windbg-multi-inherit/)
- [使用 Windbg 分析 C++ 的多重继承原理 - bingoli](https://bingoli.github.io/2019/03/21/virtual-table-by-windbg/)

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2019, BOT Man
