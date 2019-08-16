# 崩溃分析笔记

> 2019/8/4
> 
> 我们的工作价值并不在于如何调试问题，而是在于调试问题的速度和准确度。—— Mark Russinovich

[heading-numbering]

如果说通过调试发现问题，可以比作一场 “模拟人生” 游戏；那么通过崩溃现场（Dump）排查问题，更像一次 “侦探解密” 游戏。

对于前者，你可以通过不断尝试，验证猜想；对于后者，你只能看到崩溃瞬间的状态，而不能还原当时的场景。

## 目录 [no-toc] [no-number]

[TOC]

## 崩溃类型

> Windows 用户态 C++ 程序的常见崩溃

### 野指针崩溃

- 调用栈下层传入的某个指针损坏（或为空）
- 调用栈上层收到的参数出现错误
- 现象可能是内存访问越界崩溃，也可能是其他崩溃
- 排查：Mini Dump 数据不全，只能用 Full Dump 分析
- 可能恰好无效地址附近数据为空，访问数据成员时，变为空指针崩溃（标识：[sec|判断对象是否有效] 判断对象是否有效）

### 空指针崩溃

- 回溯调用栈，可以发现空指针首次出现的位置，并推断出现的路径
- 常见于 函数的前置条件改变，调用者对条件的假设失效
- 可能加上 offset 再访问数据成员，变为野指针崩溃（标识：指针值小于对象大小）

### 指针强转崩溃

- 基类指针 `static_cast<>()` 转成派生类时，类型不匹配导致数据错误
- 可能不会立即崩溃，也可能破坏内存导致其他崩溃

### 栈溢出崩溃

- 逻辑错误导致无穷递归调用
- 可能是外部输入导致（例如 发送 `WM_CLOSE`，导致无限弹出关闭确认对话框）

### 除零崩溃

- 包括 除法 `/` 和 取模 `%`
- `cdq; idiv eax, ?` -> `eax` 商 & `edx` 余数

### 内存申请崩溃（业务相关）

- 输入大量字符串
- 配置文件过大

### 外部调用崩溃（不可控）

- `QueryPerformanceFrequency`/`localtime(&-1)`/`wcsftime(..., nullptr)` 函数调用崩溃
- `DLLMain`/`WinProc` 回调位置错误

### 硬件/指令错误崩溃（不可控）

- 指令对齐/解析错误 `MISALIGNED_CODE`/`ILLEGAL_INSTRUCTION`（可能是注入导致）
- 内存加载错误 `MEMORY_CORRUPTION memory_corruption!MyDll`，导致汇编指令错误
- 内存映射文件 `MEM_MAPPED` 磁盘错误 `IN_PAGE_ERROR hardware_disk!Unknown`

## Windbg 常用命令

> 工欲善其事，必先利其器。——《论语·卫灵公》

- `!analyze -v; .ecxr` 定位崩溃位置
- `|`/`~`/`.frame` 进程/线程/栈帧
- `kPL` 查看调用栈和参数，并隐藏代码位置
- `kvL` 查看调用栈和调用约定，并隐藏代码位置
- `dv` 列举当前栈帧上 参数、局部变量 的值
- `dt ADDR TYPE` 查看 TYPE 类型对象在 ADDR 上的内存布局
- `dx ((TYPE *)ADDR)`
  - 使用 [NatVis](https://docs.microsoft.com/en-us/visualstudio/debugger/create-custom-views-of-native-objects) 查看 TYPE 可视化数据（内置 STL 支持）
  - 根据 对象内存布局 + 虚函数表指针，还原 ADDR 上 TYPE 实际的派生类
- `dps __VFN_table` 查看虚函数表指针对应的虚函数地址
- `u [ADDR]`/`ub [ADDR]`/`uf [ADDR/SYM]` 反汇编/往前反汇编/反汇编函数
- `x MOD!SYM*` 匹配函数符号（包括虚函数表指针）
- `s -d 0 L80000000 VAL` 搜索内存中是否存在 VAL 值
- `!address ADDR` 查看地址属性（堆/栈/代码区，可读/可写/可执行）

## 分析技巧

> 利用符号、代码，分析 C++ 程序崩溃 Dump

### 判断代码是否有效-1

- `uf eip` 查看崩溃代码的指令
- `lm a eip` 查看崩溃代码对应模块的地址范围，记录 `START1`
- `?eip-START1` 计算崩溃代码在模块中的偏移量，记录 `OFFSET`
- 本地运行相同版本的模块 `MODULE`
- `lm m MODULE` 查看正常代码对应模块的地址范围，记录 `START2`
- `uf START2+OFFSET` 查看正常代码的指令，和崩溃代码比较

### 判断代码是否有效-2

- `uf eip` 查看崩溃函数的指令
- 检查是否有明显逻辑错误（例如，`push ebp; mov ebp, esp` 被篡改）

### 判断对象是否有效

- 如果 `__VFN_table` 有效，表示对象头部完整
- 如果 [`base::WeakPtrFactory::ptr_ == this`](https://cs.chromium.org/chromium/src/base/memory/weak_ptr.h?q=base::WeakPtrFactory) 表示对象尾部完整
- 通过 `ref_count_/size_` 等变量推测对象数据成员是否有效
- 如果对象内存没被覆写：判断是否执行过析构函数（例如 `ptr_ = nullptr;`）
- 如果对象内存已被覆写：通过 `db/da/du` 查看是否被写成字符串

### 查找内存中的对象

- `x` 列出类的（所有）虚函数表指针
- `s -d` 搜索内存中的虚函数表指针（多重继承取最小指针）
- `dt` 还原对象内存布局
- 通过再次搜索（成员指针）或 减去偏移量（成员对象），找到对象的持有者

### 排查异步回调任务

- 通过 [`base::Location base::PendingTask::posted_from;`](https://cs.chromium.org/chromium/src/base/pending_task.h?q=base::PendingTask) 定位任务抛出的来源

### 考虑外部原因

- 发布线上运营任务，导致触达某些不常用路径
- 修改了某些崩溃，可能引发后续执行的代码崩溃；如果比较分散，不易于聚合统计

## 参考 [no-number]

- [C++ 反汇编笔记](Cpp-Disassembly-Notes.md)
- [Windbg 调试命令详解 - 张佩](http://yiiyee.cn/blog/2013/08/23/windbg/)
- [利用 C++ 类对象的虚拟函数表指针在内存中搜索对象 - 张佩](https://blog.csdn.net/blog_index/article/details/7016696)
- [从堆里寻找丢失的数据 - 张银奎](http://advdbg.org/blogs/advdbg_system/articles/3413.aspx)

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2019, BOT Man
