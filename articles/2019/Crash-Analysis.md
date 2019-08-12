# 崩溃分析笔记

> 2019/8/4
> 
> 我们的工作价值并不在于如何调试问题，而是在于调试问题的速度和准确度。—— Mark Russinovich

[heading-numbering]

## 目录 [no-toc] [no-number]

[TOC]

## 知识储备

> 工欲善其事，必先利其器。——《论语·卫灵公》

### 底层知识

> 推荐阅读：《C++ 反汇编与逆向分析技术揭秘》钱林松 赵海旭

- 计算机结构 _(Computer Architecture)_
  - CPU/内存/磁盘/网卡/GPU
- 进程 _(Process)_/线程 _(Thread)_
  - 进程（正在执行的程序的一个实例）= 代码 + 运行状态（内存/线程/资源...）
  - 线程（系统最小可调度的指令序列）= 调用栈 + 寄存器状态
- 进程地址空间 _(Process Address Space)_
  - TEXT 代码
  - DATA 常量数据
  - BSS 静态变量
  - HEAP 堆
  - MEM_MAPPED 内存映射
  - STACK 栈
- 调用栈 _(Call Stack)_
  - 参数传递：优先使用寄存器，再使用栈内存
  - 返回值（小于等于 4-byte）传递：通过 `eax` 返回
  - 返回值（大于 4-byte）传递：第一个参数传入为对象预留的栈上空间地址
- 栈帧 _(Stack Frame)_
  - `eip` 当前指令，自动修改
  - `ebp` 当前栈底，需要手动修改
  - `esp` 当前栈顶，自动修改（自平衡）或 手动修改（手动平衡）
- 调用约定 _(Calling Convention)_
  - `__cdecl` 右到左压栈，调用者退栈（默认，支持变长参数)
  - `__stdcall` 右到左压栈，被调用者退栈（代码更小）
  - `__fastcall` 前两个参数放入寄存器，其余的右到左压栈，被调用者退栈
  - `this call` 把 this 指针放入 `ecx`，右到左压栈，调用者退栈
  - `nacked call` 不修改栈内存，不执行 `push/pop ebp`
- 作用域 _(Scope)_
  - 全局（静态）对象：存放在 BSS，进程退出时销毁
  - 局部（栈/参数/返回值）对象：存放在 STACK，超出局部作用域时销毁
  - 堆对象：存放在 HEAP，代码逻辑控制销毁
- 引用 _(Reference)_
  - 编译器转换为 指针 _(pointer)_ 实现
- 条件/循环控制流 _(Conditional/Loop Control Flow)_
  - 通过 `jxx [ADDR]` 跳转实现
- 异常控制流 _(Exception Control Flow)_
  - [Google 风格](https://google.github.io/styleguide/cppguide.html#Exceptions) **已禁用**
- 虚函数表 _(Virtual Function Table)_
  - 有虚函数的基类，有一个虚函数表；派生类针对每个基类，有多个虚函数表
  - 基类布局：`[类型信息指针 (opt)]-[虚函数1指针-虚函数2指针-...]`
  - 派生类布局：`[类型信息指针 (opt)]-[基类虚函数1指针-基类虚函数2指针-...]-[派生类虚函数1指针-派生类虚函数2指针-... (opt)]`
  - 如果禁用 [RTII](https://en.cppreference.com/w/cpp/types)，则没有类型信息指针
  - 对于多重继承，派生类虚函数指针只追加到第一个基类的虚函数表后
  - 对于纯虚函数，虚函数指针指向 [`_purecall`](../2017/Cpp-Windows-Crash.md)
- 对象内存布局 _(Object Memory Layout)_
  - 连续内存，32-bit 程序按照 4-byte 对齐
  - 基类布局：`[虚函数指针 (opt)]-[数据成员1-数据成员2-...]`
  - 派生类布局：`[基类1布局-基类2布局-...]-[数据成员1-数据成员2-...]`
  - 通过 `[this+-n]` 访问对象的数据成员
  - 对于多重继承，调用成员函数时，需要先把 this 指针移动到对应类的偏移上
  - 对于多重继承，基类的析构函数必须声明为虚函数，否则直接用派生类 this 指针调用基类析构函数
  - 对于私有继承 `class Derived : private Base {};`，内存布局和 组合对象 `class Super { private: Sub member_; };` 一致
- 菱形继承 _(Diamond Inheritance)_
  - TODO
- 虚函数调用 _(Virtual Function Call)_
  - `lea ecx, [OBJ]` 存入对象 `OBJ` 的 this 指针
  - `mov eax, [ecx+n]` 取出对象 `OBJ` 的偏移 `n` 对应的虚函数表指针
  - `call dword ptr [eax+n]` 调用虚函数表的偏移 `n` 对应的虚函数
- 堆内存管理
  - 调用 `operator new/delete` 申请内存
  - TODO
- 构造函数调用
  - 顺序：基类1构造函数->基类2构造函数->...->成员1构造函数->成员2构造函数->...
- 析构函数调用
  - 顺序：成员2析构函数->成员1析构函数->...->基类2析构函数->基类1析构函数->...（和构造相反）
  - 析构派生类时，需要还原基类的虚函数表指针，否则找不到基类的析构函数的虚函数 TODO

### 上层知识

- 基础概念
  - 智能指针/设计模式/回调/单例/弱引用/消息循环
  - 日志/字符串操作/时间/文件/系统/i18n/调试辅助
- 业务代码组织结构
- 应用生命周期模型
  - 桌面客户端模型 = 启动时初始化 -> 处理窗口消息 + 处理队列任务 -> 退出时反初始化
  - 移动客户端模型/后台服务模型...
- [对象生命周期模型](../2018/Resource-Management.md)
  - 互斥所有权 `std::unique_ptr`
  - 共享所有权 `std::shared_ptr`
  - 弱引用关系 `std::weak_ptr`/[`base::WeakPtr`](https://cs.chromium.org/chromium/src/base/memory/weak_ptr.h?q=base::WeakPtr)
  - 外部调用管理 `Class::OnDestroy() { delete this; }`
- 进程/线程模型
  - [Chromium 进程](https://developers.google.cn/web/updates/2018/09/inside-browser-part1) = 1 Browser + n Renderer + 1 GPU + x Util
  - [Chromium 线程](https://github.com/chromium/chromium/blob/master/docs/threading_and_tasks.md#threads) = 1 UI (Browser)/Main (Renderer) + 1 IO (for IPC) + x Spec + n Worker (Pool)

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

- 基类指针 `static_cast<>()` 转成派生类时，类型不匹配导致数据错误
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

- [图说 C++ 对象模型：对象内存布局详解 - melonstreet](https://www.cnblogs.com/QG-whz/p/4909359.html)
- [使用 Windbg 分析 C++ 的虚函数表原理 - bingoli](https://bingoli.github.io/2019/03/27/windbg-multi-inherit/)
- [使用 Windbg 分析 C++ 的多重继承原理 - bingoli](https://bingoli.github.io/2019/03/21/virtual-table-by-windbg/)
- [Windbg 调试命令详解 - 张佩](http://yiiyee.cn/blog/2013/08/23/windbg/)
- [利用 C++ 类对象的虚拟函数表指针在内存中搜索对象 - 张佩](https://blog.csdn.net/blog_index/article/details/7016696)
- [从堆里寻找丢失的数据 - 张银奎](http://advdbg.org/blogs/advdbg_system/articles/3413.aspx)

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2019, BOT Man
