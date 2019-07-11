# C++ 20 协程

> 2019/7/11
> 
> C++ 20 协程分享

[slide-mode]

---

### 背景

- [从 时空维度 看 I/O 模型](IO-Models.md)
  - 阻塞/非阻塞：空间维度，代码编写是否连贯
  - 同步/异步：时间维度，等待时是否执行其他代码
- 同步 -> 异步，阻塞 -> 非阻塞 -> 阻塞
- 多进程/多线程 -> I/O 复用（轮询）-> 回调 -> **协程**
- 有栈协程（自己切换调用栈）
  - Unix `getcontext/setcontext/swapcontext`
  - Windows `CreateFiber/SwitchToFiber`（纤程）
- 无栈协程（需要语言支持，例如 C#/JavaScript...）

---

### 示例 - 异步下载

``` cpp
Future<std::string> DownloadAsync() {
  auto res = co_await Fetch("https://xxx");
  if (res.succ)
    co_return res.content;
  co_return co_await Fetch("https://yyy");
}

std::cout << DownloadAsync.get();
```

---

### 示例 - 流式读取

``` cpp
Generator<char> ReadFile() {
  for (auto ch : Read("file.txt")) {
    if (ch == EOF)
      co_return;
    co_yield ch;
  }
}

for (char ch : ReadFile())
  std::cout << ch;
```

---

### 概念

- C++ 协程：
  - 含有 `co_` 关键字的函数
  - 函数返回值符合 Coroutine 概念
- `co_return`：
  - 返回 Coroutine 函数
  - 返回 Future 概念的结果
- `co_yield`：返回 Generator 概念的结果
- `co_await`：等待 Awaitable 概念的返回结果

---

### 实现 - 协程展开

``` cpp
Coroutine __FN__(...) {
  Coroutine::promise_type promise{...};
  __return__ = promise.get_return_object();
  co_await promise.initial_suspend();
  try { __fn__ } catch (...) {
    promise.unhandled_exception();
  }
final_suspend:
  co_await promise.final_suspend();
}
```

---

### 实现 - Coroutine 概念

``` cpp
concept Coroutine {
  struct promise_type {
    Coroutine get_return_object();
    Awaitable initial_suspend();
    Awaitable final_suspend();
    void unhandled_exception();
  };
};
```

---

### 实现 - `co_return/co_yield` 展开

``` cpp
// co_return;
promise.return_void();
goto final_suspend;

// co_return val;
promise.return_value(val);
goto final_suspend;

// co_yield val;
co_await promise.yield_value(val);
```

---

### 实现 - Future 概念

``` cpp
concept Future : Coroutine {
  struct promise_type {
    void return_value(const T& value);

    // or (exclusive)
    void return_void();
  };
};
```

---

### 实现 - Generator 概念

``` cpp
concept Generator : Coroutine {
  struct promise_type {
    Awaitable yield_value(const T& value);

    // may have
    void return_void();
  };
};
```

---

### 实现 - `co_await` 展开

``` cpp
// /* ret = */ co_await awaiter;
if (!awaiter.await_ready())
  awaiter.await_suspend();
/* ret = */ awaiter.await_resume();
```

---

### 实现 - Awaitable 概念

``` cpp
concept Awaitable {
  bool await_ready();
  void await_suspend(std::coroutine_handle<>);
  auto await_resume();
};

bool await_ready(Awaitable&);
void await_suspend(Awaitable&, std::coroutine_handle<>);
auto await_resume(Awaitable&);
```

---

### 初体验

- 编译器生成大量代码，看不到调用者
- 执行 `co_await` 后，之前的上下文可能失效

---

### Q & A

<br/>
<br/>
<br/>
<br/>

[align-right]

# 谢谢 🙂

---
