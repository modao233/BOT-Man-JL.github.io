# 探讨 C++ 并发模型

> 2019/12/7
> 
> TODO

## 多线程

- 为什么要并发
- 并发 vs 并行
- 问题
  - 资源管理
  - 线程安全
- Join vs Detach
- Thread pool
- Interruptable

## Lock based

- Problems
  - Hiding - Deadlock/Livelock
  - Leaking
- Practice
  - Granularity
  - Proctect data, not process
  - With timeout
  - Atomic double buffer
- Approach
  - `recursive` - reentrance
  - `shared_lock` - reader/writter lock
  - `conditional_variable` - repeating event
  - `future`/`promise` - once event

## Lock free

- Memory model
- Busy waiting

## Active Object

## Actor

- `base::ThreadChecker/SequenceChecker`
  - Item 16: Make const member functions thread safe.
  - unless you’re certain they’ll never be used in a concurrent context.

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2019, BOT Man
