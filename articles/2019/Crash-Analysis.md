# 崩溃分析笔记

> 2019/8/4
> 
> Windows 崩溃分析笔记

## 常见的崩溃类型

### 空指针崩溃

- 检查空指针位置
- 排查空指针出现的逻辑

### 野指针崩溃（Mini Dump 数据可能缺失，最好使用 Full Dump）

- 可能因为调用栈下边的某个指针损坏
- 上层调用传入的参数都是错误的

### 内存申请失败崩溃

- 例如 用户输入大量字符串
- 再如 用户配置文件过大

### 系统调用失败崩溃

- 例如 DLLMain/WinProc 回调位置错误
- 再如 QueryPerformanceFrequency 函数调用崩溃

### 硬盘数据错误

- FAILURE_PROBLEM_CLASS: `IN_PAGE_ERROR`
- FAILURE_SYMBOL_NAME: `hardware_disk!Unknown`

## 程序的崩溃时机

- 初始化
- 处理窗口消息
- 处理队列任务

## Windbg 常用命令

- `dv` 查看当前栈上变量的值
- `dt ADDR [TYPE]` 查看 TYPE 类型对象在 ADDR 上的内存布局
- `dx ((TYPE *)ADDR)` 还原 ADDR 上基类 TYPE 对应的派生类
- `dps __VFN_table` 查看虚函数表对应的成员函数
- `x MOD!SYM*` 匹配符号（包括虚函数表指针）
- `s -d 0 L?80000000 VAL` 搜索内存中是否存在 VAL 值
- `!address ADDR` 查看地址属性（堆/栈，可读写性）

## 判断对象的指针是否合法

- 如果 `__VFN_table` 有效，表示对象头部完整
- 如果 `base::WeakPtrFactory::ptr_ == this` 表示对象尾部完整
- 通过 `ref_count_/size_` 等变量推测对象数据成员是否合法

## 查找内存中的对象

- `x` 列出类的（所有）虚函数表指针
- `s -d` 搜索内存中的虚函数表指针（多重继承取最小指针）
- `dt` 还原对象内存布局
- 通过再次搜索（成员指针）或 减去偏移量（成员对象），找到对象的持有者

## 排查异步回调任务

- 查看 `base::PendingTask::posted_from` 定位任务抛出的来源

## 参考

- [Windbg 调试命令详解 - 张佩](http://yiiyee.cn/blog/2013/08/23/windbg/)
- [利用 C++ 类对象的虚拟函数表指针在内存中搜索对象 - 张佩](https://blog.csdn.net/blog_index/article/details/7016696)
- [从堆里寻找丢失的数据 - 张银奎](http://advdbg.org/blogs/advdbg_system/articles/3413.aspx)
- [使用 Windbg 分析 C++ 的虚函数表原理 - bingoli](https://bingoli.github.io/2019/03/27/windbg-multi-inherit/)
- [使用 Windbg 分析 C++ 的多重继承原理 - bingoli](https://bingoli.github.io/2019/03/21/virtual-table-by-windbg/)

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2019, BOT Man
