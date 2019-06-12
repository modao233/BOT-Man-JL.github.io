# 同步异步、阻塞非阻塞

> 2019/5/6
> 
> 总结几种 I/O 模型

之前学习 I/O 模型的时候，感觉理解不是很透彻，现在用最几个简单的例子阐述他们直接的区别。

## 同步、阻塞

例如，调用系统的 `send()` 通过 **普通的** `fd` 发送数据：

``` cpp
ssize_t ret = send(fd, buffer, len, flags);
```

如果需要 **提高 I/O 吞吐率**，可以采用 **多进程/多线程** 的形式。

## 同步、非阻塞

例如，调用系统的 `send()` 通过 **非阻塞的** `fd` 发送数据：

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
- 如果无法发送数据，[`EVUTIL_SOCKET_ERROR()`](https://github.com/libevent/libevent/blob/master/include/event2/util.h) 返回 `EAGAIN` / `EWOULDBLOCK`

如果需要 **提高 I/O 吞吐率**，可以采用 [I/O 复用](https://en.wikipedia.org/wiki/Multiplexing) 的形式。（参考：[《epoll 的本质是什么》](https://my.oschina.net/editorial-story/blog/3052308)）

## 异步、非阻塞

很多的语言和系统都支持，一般采用 [回调](https://en.wikipedia.org/wiki/Callback_%28computer_programming%29) 的接口。例如，Node.js 通过 [`fs.writeFile()`](https://nodejs.org/api/fs.html#fs_fs_writefile_file_data_options_callback) 读取文件：

``` javascript
fs.writeFile(file, data, (err) => {
  if (err) {
    throw err;
  }
  // done here
});
```

由于系统使用异步 I/O，有很好的吞吐率。一般基于 I/O 复用，实现 **用户态** 的 **异步、非阻塞**（例如 libevent）。

## 异步、阻塞

需要系统支持 [协程](https://en.wikipedia.org/wiki/Coroutine)（例如 JavaScript ES6），一般采用 async/await 接口：

``` javascript
async function send(fd, buffer, flags) { ... }

let ret = await send(fd, buffer, flags);
```

- UNIX 系统（C 语言）**原生不支持** 该模型（参考：[Asynchronous I/O | Wikipedia](https://en.wikipedia.org/wiki/Asynchronous_I/O#Forms)）

由于系统使用异步 I/O，有很好的吞吐率。

## 写在最后

关于 **阻塞/非阻塞**：

- **阻塞模型** 在 **I/O 完成后** 返回函数调用结果，卡住当前代码段的执行，**可以保证** 往下执行的代码能获取到正确的 **I/O 结果**
- **非阻塞模型** 在 **提交 I/O 请求后** 立即返回，**不能保证** 往下执行的代码能获取到 **I/O 的结果**
  - 对于 **同步模型**，可以通过查询当前 I/O 状态，**不断检查** I/O 的执行情况
  - 对于 **异步模型**，系统会在 I/O 结束时，**回调** 注册的函数
- 相比较：
  - **阻塞模型** 的代码 **顺序描述** I/O 完成后的处理逻辑，看起来更连贯
  - **非阻塞模型** 的 “发起 I/O 请求” 和 “I/O 完成处理逻辑” 代码，执行 **不顺序、连贯**

关于 **同步/异步**：

- 对于 **异步模型**，由于系统知道所有 I/O 的进行状态，所以它可以同时处理多个 I/O，充分的利用 CPU 资源
  - 对于 **非阻塞模型**，等到 I/O 完成时，系统通过 **回调** 的方式，给调用者传递执行结果（基于回调）
  - 对于 **阻塞模型**，系统在调用者进入 I/O 时把他 **挂起**，再把他 **恢复**（基于协程）
- 而 **同步模型**，调用者只知道自己的 I/O 状态，不知道其他 I/O 的状态
  - 对于 **阻塞模型**，调用者会一直 **等到 I/O 完成** 再往下执行
  - 对于 **非阻塞模型**，调用者需要 **不断检查** I/O 的执行情况，直到完成时才往下执行
- 相对于同步模型，**异步模型** 能避免 I/O 未完成的调用者浪费 CPU 资源，**吞吐率更高**
- 不过，由于 **异步模型** 是系统调度的，执行后续逻辑时，需要注意上下文失效的情况（参考：[深入 C++ 回调](Inside-Cpp-Callback.md#回调是同步还是异步的)）

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2019, BOT Man
