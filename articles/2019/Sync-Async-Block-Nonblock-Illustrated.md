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

- 可以使用 libevent 提供的 [`evutil_make_socket_nonblocking()`](https://github.com/libevent/libevent/blob/master/include/event2/util.h) 将 fd 设置为非阻塞
- 如果无法发送数据，[`EVUTIL_SOCKET_ERROR()`](https://github.com/libevent/libevent/blob/master/include/event2/util.h) 返回 `EAGAIN` / `EWOULDBLOCK`

如果需要 **提高 I/O 吞吐率**，可以采用 [I/O 复用（例如 libevent）](http://libevent.org/) 的形式（**用户态** 的 **异步、非阻塞**）。

## 异步、阻塞

只在 **支持协程** 的系统里存在，一般采用 async/await 接口：

``` javascript
async function send(fd, buffer, flags) { ... }

let ret = await send(fd, buffer, flags);
```

- UNIX 系统 **原生不支持** 异步、阻塞模型（参考：[Asynchronous I/O | Wikipedia](https://en.wikipedia.org/wiki/Asynchronous_I/O#Forms)）

由于系统使用异步 I/O，有很好的吞吐率。

## 异步、非阻塞

一般采用 **回调** 的接口。例如，Node.js 通过 [`fs.writeFile()`](https://nodejs.org/api/fs.html#fs_fs_writefile_file_data_options_callback) 读取文件：

``` javascript
fs.writeFile(file, data, (err) => {
  if (err) {
    throw err;
  }
  // done here
});
```

由于系统使用异步 I/O，有很好的吞吐率。

## 写在最后

关于 **阻塞/非阻塞**：

| | 阻塞 | 非阻塞 |
|-|---|---|
| 函数调用返回 | I/O 完成后 | 提交 I/O 请求后 |
| 获取结果 | 函数返回时 | 某次函数返回时 或 回调函数参数 |
| 使用结果 | 函数的后续代码 可以使用 I/O 的结果 | 调用关系复杂 |
| 对应的例子 | 普通 `fd` 的 `send()` 和 `async function send()` | 非阻塞 `fd` 的 `send()` 和 `fs.writeFile()` |

关于 **同步/异步**：

| | 同步 | 异步 |
|-|---|---|
| 谁来控制 I/O 完成 | 调用者 | 系统 |
| I/O 完成后的调用栈 | 和调用者一致 | 和调用者不在同一个栈里 |
| 调用者的上下文 | 不会失效 | 可能失效 |
| 对应的例子 | `send()` | `async function send()` 和 `fs.writeFile()` |

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2019, BOT Man
