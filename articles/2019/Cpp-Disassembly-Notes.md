# C++ 反汇编笔记

> 2019/8/11
> 
> 博观而约取，厚积而薄发。—— 苏轼

[heading-numbering]

## 目录 [no-toc] [no-number]

[TOC]

## 操作系统

### 计算机结构 _(Computer Architecture)_

- CPU/内存/磁盘/网卡/GPU

### 进程 _(Process)_/线程 _(Thread)_

- 进程（正在执行的程序的一个实例）= 代码 + 运行状态（内存/线程/资源...）
- 线程（系统最小可调度的指令序列）= 调用栈 + 寄存器状态

### 进程地址空间 _(Process Address Space)_

- TEXT 代码
- READONLY_DATA 常量数据
- DATA 静态数据（已初始化）
- BSS 静态数据（未初始化）
- HEAP 堆（低到高）
- MEM_MAPPED 内存映射（高到低）
- STACK 栈（高到低）

## 函数调用

### 调用栈 _(Call Stack)_

- 函数签名 = 函数名 + 各个参数类型
- 参数（小于等于 4-byte）传递：优先使用寄存器，再使用栈内存
- 参数（大于 4-byte）传递：TODO
- 返回值（小于等于 4-byte）传递：通过 `eax` 返回
- 返回值（大于 4-byte）传递：第一个参数传入为对象预留的栈上内存地址

### 栈帧 _(Stack Frame)_

- `eip` 当前指令
  - 执行下一条指令 自动修改
  - `jxx [ADDR]` 手动修改
  - `call [ADDR]` 手动修改（相当于 `push eip; mov eip, [ADDR]`）
  - `ret` 手动修改（相当于 `pop eip`）
- `ebp` 当前栈底
  - 函数入口 `push ebp; mov ebp, esp` 手动修改
  - 函数出口 `pop ebp` 手动还原
- `esp` 当前栈顶
  - `push/pop REG` 自动修改
  - `sub/add esp n` 手动修改（申请/释放 局部变量的空间）
  - 函数出口 `ret n`（用于 被调用者退栈）
    - 先弹出 `[n/4]` 个 4-byte 大小的参数（相当于 `add esp n`）
    - 再弹出 4-byte 大小的 `RetAddr`（相当于 `pop eip`）
- 内存布局（高到低）
  - 传入参数（访问方式：`[ebp+n], n >= 8`）
  - `RetAddr` 上一个函数的 返回地址，即下一个指令的 `eip`（访问方式：`[ebp+4]`）
  - `ChildEBP` 上一个函数的 基地址 `ebp`（访问方式：`[ebp]`）
  - 局部变量（访问方式：`[ebp-n], n >= 4`）

### 调用约定 _(Calling Convention)_

- `__cdecl` 右到左压栈，调用者退栈（默认，支持变长参数)
- `__stdcall` 右到左压栈，被调用者退栈
- `__fastcall` 前两个参数放入寄存器，其余的右到左压栈，被调用者退栈
- `this call` 把 this 指针放入 `ecx`，右到左压栈，调用者退栈
- `nacked call` 不修改栈帧，不执行 `push/pop ebp`

## C++ 语言

### 引用 _(Reference)_

- 编译器转换为 指针 _(pointer)_ 实现

### 条件/循环控制流 _(Conditional/Loop Control Flow)_

- 通过 `jxx [ADDR]` 跳转实现

### 异常控制流 _(Exception Control Flow)_

- [Google 风格](https://google.github.io/styleguide/cppguide.html#Exceptions) **已禁用**
- Windows 通过 [Structured Exception Handling (SEH)](https://docs.microsoft.com/en-us/cpp/cpp/structured-exception-handling-c-cpp) 实现
- TODO

### 作用域 _(Scope)_

- 全局（静态）对象：存放在 BSS，进程退出时销毁
- 局部（栈/参数/返回值）对象：存放在 STACK，超出局部作用域时销毁
- 堆对象：存放在 HEAP，代码逻辑控制销毁

### 堆内存管理

- 堆段 _(segment/bin)_：
  - 固定大小，由操作系统管理，给程序使用；满了继续申请，空了还给系统
  - Windows 上：申请 `HeapCreate`，释放 `HeapDestroy`
- 堆块 _(block/entry/chunk)_：
  - 在堆段上分配，包括 本块大小、前一堆块大小、对齐信息、数据部分；对于 C++ 代码只能访问数据部分
  - Windows 上：申请 `HeapAlloc`，释放 `HeapFree`
  - C++ 层面：申请 `malloc`/`operator new`，释放 `free`/`operator delete`

## C++ 面向对象

### 虚函数表 _(Virtual Function Table)_

- 基类如果有虚函数，就有一个虚表；派生类针对每个基类，有不同的虚表
- 基类布局：`[类型信息指针 (opt)]-[虚函数1指针-虚函数2指针-...]`
- 派生类布局：`[类型信息指针 (opt)]-[基类虚函数1指针-基类虚函数2指针-...]-[派生类虚函数1指针-派生类虚函数2指针-... (opt)]`
- 如果禁用 [RTII](https://en.cppreference.com/w/cpp/types)，则没有类型信息指针
- 对于纯虚函数，虚函数指针指向 [`_purecall`](../2017/Cpp-Windows-Crash.md)

### 对象内存布局 _(Object Memory Layout)_

- 连续内存，32-bit 程序按照 4-byte 对齐
- 基类布局：`[虚函数指针 (opt)]-[数据成员1-数据成员2-...]`
- 派生类布局：`[基类1布局-基类2布局-...]-[数据成员1-数据成员2-...]`
- 访问数据成员：`[this+-n]`
- 对于继承 `class Derived : private Base {};` 的内存布局，和组合 `class Super { private: Sub member_; };` 一样

### 多重继承 _(Multiple Inheritance)_

- 派生类指针调用基类虚函数
  - 需要把 this 指针偏移到基类的地址上（向后 `+n` 找到基类地址）
  - 派生类重载的虚函数里，this 指针指向对应基类的偏移位置（向前 `-n` 还原派生类地址）
  - 对于第一个基类，`n == 0` 地址相同
- 派生类的虚函数指针，只追加到对应第一个基类的虚表后

### 虚继承 _(Virtual Inheritance)_

- TODO

### 虚函数调用 _(Virtual Function Call)_

- `lea ecx, [OBJ+n]` 存入对象 `OBJ` 在偏移 `n` 后的基类 this 指针
- `mov eax, [ecx]` 取出基类 this 指针对应的虚表指针
- `call dword ptr [eax+n]` 调用虚表的偏移 `n` 对应的虚函数

### 构造函数 _(Constructor)_/析构函数 _(Destructor)_

- 调用顺序（两者严格相反）：
  - 构造：基类1构造函数->基类2构造函数->...->成员1构造函数->成员2构造函数->...
  - 析构：成员2析构函数->成员1析构函数->...->基类2析构函数->基类1析构函数->...
- 构造的本质：
  - 先根据不同作用域，分配 `sizeof(Obj)` 的内存
  - 再把内存起始地址作为 this 指针，通过 `this call` 方式调用构造函数
- 析构的本质：
  - 先通过 `this call` 调用析构函数
  - 再根据不同作用域，释放对象的内存
- 避免在构造/析构时调用虚函数：
  - 对于直接调用，编译器已知虚函数指针，不需要查虚表
  - 对于间接调用（调用的非虚函数调用了虚函数），[如果调用了纯虚函数，就会出现崩溃](../2017/Cpp-Windows-Crash.md)
- 每个构造/析构函数入口，写入当前类的虚表指针：
  - 属于 [C++ 的保护措施](https://isocpp.org/wiki/faq/strange-inheritance#calling-virtuals-from-ctors)，构造/析构过程中分别写入，而不是一次性写入
  - 否则如果使用了派生类的虚表指针，就会（间接）调用派生类的虚函数，但派生类的数据成员未构造或已析构，导致崩溃）
- 基类的析构函数必须声明为虚函数：否则可能直接用派生类 this 指针调用基类析构函数

## 上层知识

### 基础概念

- 智能指针/设计模式/回调/单例/弱引用/消息循环
- 日志/字符串操作/时间/文件/系统/i18n/调试辅助

### C++ 标准库

- 理解 STL 中隐含的概念，和 STL 的基本实现原理（也可以借助 [NatVis](https://docs.microsoft.com/en-us/visualstudio/debugger/create-custom-views-of-native-objects) 可视化查看对象）
- 从 `std::string` 内存中定位 `c_str_/size_/capacity_` 数据
- 从 智能指针 内存中定位 `ptr_/ref_count_` 等数据

### [对象生命周期模型](../2018/Resource-Management.md)

- 互斥所有权 `std::unique_ptr`
- 共享所有权 `std::shared_ptr`
- 弱引用关系 `std::weak_ptr`/[`base::WeakPtr`](https://cs.chromium.org/chromium/src/base/memory/weak_ptr.h?q=base::WeakPtr)
- 外部调用管理 `Class::OnDestroy() { delete this; }`

### 应用进程/线程模型

- [Chromium 进程](https://developers.google.cn/web/updates/2018/09/inside-browser-part1) = 1 Browser + n Renderer + 1 GPU + x Util
- [Chromium 线程](https://github.com/chromium/chromium/blob/master/docs/threading_and_tasks.md#threads) = 1 UI (Browser)/Main (Renderer) + 1 IO (for IPC) + x Spec + n Worker (Pool)

### 应用生命周期模型

- 桌面客户端模型 = 启动时初始化 -> 处理窗口消息 + 处理队列任务 -> 退出时反初始化
- 移动客户端模型/后台服务模型...

## 参考 [no-number]

- 《C++ 反汇编与逆向分析技术揭秘》钱林松 赵海旭
- [图说 C++ 对象模型：对象内存布局详解 - melonstreet](https://www.cnblogs.com/QG-whz/p/4909359.html)
- [使用 Windbg 分析 C++ 的虚函数表原理 - bingoli](https://bingoli.github.io/2019/03/27/windbg-multi-inherit/)
- [使用 Windbg 分析 C++ 的多重继承原理 - bingoli](https://bingoli.github.io/2019/03/21/virtual-table-by-windbg/)
- [Dance In Heap（一）：浅析堆的申请释放及相应保护机制 - hellowuzekai](https://www.freebuf.com/articles/system/151372.html)
- [浅析 Windows 下堆的结构 - hellowuzekai](https://www.freebuf.com/articles/system/156174.html)

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2019, BOT Man
