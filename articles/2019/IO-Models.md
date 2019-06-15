# 从时间/空间看 I/O 模型

> 2019/5/6
> 
> 从时间/空间的维度，总结几种 I/O 模型的异同

在学习了 [V8 JavaScript 引擎](https://v8.dev/) 后，对 I/O 模型有了进一步的理解，这里用几个简单的例子阐述 **同步异步**、**阻塞非阻塞** 的区别。

## 同步、阻塞

例如，调用 UNIX 系统的 `send()` 通过 **普通的** `fd` 发送数据：

``` cpp
ssize_t ret = send(fd, buffer, len, flags);
```

- 当前线程的函数调用 **阻塞到发送完成时**
- 如果需要 **提高 I/O 吞吐率**，可以采用 **多进程/多线程** 的形式

## 同步、非阻塞

例如，调用 UNIX 系统的 `send()` 通过 **非阻塞的** `fd` 发送数据：

``` cpp
evutil_make_socket_nonblocking(fd);

size_t len_to_send = len;
while (!len_to_send) {
  ssize_t ret = send(fd, buffer, len, flags);
  // case1: ready to send
  if (ret >= 0) {
    len_to_send -= ret;
    continue;
  }
  // case2: not ready
  if (EVUTIL_SOCKET_ERROR() == EAGAIN) {
    continue;
  }
  // case3: error
  throw err;
}
```

- 可以使用 [libevent](http://libevent.org/) 提供的 [`evutil_make_socket_nonblocking()`](https://github.com/libevent/libevent/blob/master/include/event2/util.h) 将 fd 设置为非阻塞
- 函数调用 **立即返回**：
  - 如果调用时可以发送数据，会立即发送数据，并返回已发送数据大小
  - 如果调用时暂时无法发送数据，[`EVUTIL_SOCKET_ERROR()`](https://github.com/libevent/libevent/blob/master/include/event2/util.h) 返回 `EAGAIN` / `EWOULDBLOCK`
- 如果需要 **提高 I/O 吞吐率**，可以采用 **I/O 复用** 的形式（参考：[《epoll 的本质是什么》](https://my.oschina.net/editorial-story/blog/3052308)）

## 异步、非阻塞

例如，Node.js 通过 [`fs.readFile()`](https://nodejs.org/api/fs.html#fs_fs_readfile_path_options_callback) 读取文件：

``` javascript
fs.readFile(filename, (err, data) => {
  if (err) {
    // handle |err|
  }
  // use |data|
});
```

- 需要系统/语言支持，一般提供基于 [回调](https://en.wikipedia.org/wiki/Callback_%28computer_programming%29) 的接口
- 如果系统/语言不支持，可以基于 **I/O 复用** 在 **用户态** 实现（例如 [libevent](http://libevent.org/) 提供了一组基于回调的异步接口）
- 由于系统使用异步 I/O，有很好的吞吐率

## 异步、阻塞

例如，Node.js 用 [`util.promisify`](https://nodejs.org/api/util.html#util_util_promisify_original) 封装 `fs.readFile()` 接口：

``` javascript
const readFileAsync = util.promisify(fs.readFile);

try {
  const data = await readFileAsync(filename);
  // use |data|
} catch (err) {
  // handle |err|
}
```

- 需要系统/语言支持 [协程](https://en.wikipedia.org/wiki/Coroutine)（例如 [ECMAScript6](https://en.wikipedia.org/wiki/ECMAScript)），一般采用基于协程原语的 `async/await` 接口
- 如果系统/语言不支持，则无法实现（例如，**UNIX 系统/C 语言 原生不支持** 协程，参考：[Asynchronous I/O | Wikipedia](https://en.wikipedia.org/wiki/Asynchronous_I/O#Forms)）
- 如果调用多次 `await ...()`，只需要外边有一层 try-catch 处理错误；而基于回调的 **异步、非阻塞**，只能通过嵌套一层层 [回调地狱 _(callback hell)_](http://callbackhell.com/) 描述流程
- 由于系统使用异步 I/O，有很好的吞吐率

## 写在最后

**阻塞/非阻塞** 像是 **空间** 维度的对比 —— “发起 I/O 请求” 代码是否 **紧跟着** “I/O 完成处理逻辑”：

- **阻塞模型** 在 **I/O 完成后** 通过函数调用的返回值返回结果；由于阻塞了当前代码段的执行，**可以保证** 下一行执行的代码能获取到正确的 **I/O 结果**
- **非阻塞模型** 在 **提交 I/O 请求后** 立即返回，**不能保证** 往下执行的代码能得到 **I/O 结果**
  - 对于 **同步模型**，需要调用者 **不断检查** I/O 是否完成
  - 对于 **异步模型**，系统会在 I/O 结束时，**通过回调 传递 I/O 结果**

**同步/异步** 像是 **时间** 维度的对比 —— 同一个线程里的 I/O 代码是否有 **明确调用的顺序**：

- 对于 **异步模型**，一个线程可以 **同时处理多个 I/O**，调用顺序不连贯
  - 对于 **非阻塞模型**，通过 **回调** 给各个调用者 **传递** 它发起的 I/O 的 **结果**，**回调顺序不确定**
  - 对于 **阻塞模型**，通过协程的 **挂起/恢复** 机制阻塞当前代码的执行，同时切换到其他地方执行，导致 I/O 操作调用 **完成时机不确定**
- 而 **同步模型**，一个线程只能 **同时进行一个 I/O**，调用顺序是连贯的
  - 对于 **阻塞模型**，调用者会 **一直等到** 当前 I/O 完成后，再往下执行
  - 对于 **非阻塞模型**，调用者会 **不断检查** 当前 I/O 的执行情况，直到完成后再往下执行

总结起来：

- **阻塞模型** 能让代码更连贯，写起来逻辑更简单
- **异步模型** 能避免等待 I/O 的 **时间浪费**，**吞吐率更高**；但由于调用顺序不明确，需要注意 **上下文失效** 的情况（参考：[深入 C++ 回调](Inside-Cpp-Callback.md#回调是同步还是异步的)）
- 最复杂的是 **异步、非阻塞** 模型：
  - 空间上不连贯，要求调用者在 “发起 I/O 请求” 代码和 “I/O 完成处理逻辑” 之间，**传递上下文**
  - 时间上不连贯，不知道 “发起 I/O 请求” 到 “I/O 完成” 之间，**其他的代码** 干了什么

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2019, BOT Man
