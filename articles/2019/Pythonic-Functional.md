# Python 风格的函数式

> 2019/11/23
> 
> Readability counts. —— The Zen of Python, by Tim Peters (`import this`)

由于 现代编程语言 常常相互借鉴（例如 几乎所有语言都支持了 lambda 表达式/匿名函数/闭包），许多人会说：

> 学什么编程语言都一样，会一种就行。

但我 **不赞同** 这个观点 —— 我认为：用不同的语言写代码，就应该 “入乡随俗”，多领会各种语言的 **设计的艺术**。

> 三人行，必有我师焉；择其善者而从之，其不善者而改之。——《论语‧述而》

Python 为了提高 **可读性** _(readability)_，提供了很多 **语法糖** _(syntactic sugar)_，开创了别具一格的 **Python 风格** _(Pythonic)_ 的 **函数式编程**。

[TOC]

## 什么是 Pythonic

举个例子，实现一个简单的需求：

- 按行打印 **当前脚本** 内容
- 去掉每行 **右侧空字符**
- 过滤所有 **空行**（去掉右侧空字符后）
- 打印时加上 **行号**（不包括空行）

**学习 Python 前**，凭感觉会这么写：

``` python
file = open(__file__)
try:
    index = 1
    while True:
        line = file.readline()
        if not line:
            break

        strip_line = line.rstrip()
        if len(strip_line) != 0:
            print('{:2}: {}'.format(index, strip_line))
            index += 1
finally:
    file.close()
```

**学习 Python 后**，只需要 3 行代码：

``` python
with open(__file__) as file:
    for index, line in enumerate(filter(len, map(str.rstrip, file)), 1):
        print(f'{index}: {line}')
```

- **异常安全** _(exception safe)_ 的打开/关闭文件
  - 前：将 `close()` 写在 [`finally` 语句](https://docs.python.org/3/reference/compound_stmts.html#the-try-statement) 内，避免异常时泄露
  - 后：使用 [`with` 语句](https://docs.python.org/3/reference/compound_stmts.html#the-with-statement)（类似 C++ 的 RAII/scoped 思想）
- **迭代** _(iterate)_ 读取脚本 文件的每一行
  - 前：使用 `while` 循环调用 `readline()` 函数，直到读到 `None` 时结束
  - 后：使用 `for` 循环遍历 [**迭代器** _(iterator)_](https://docs.python.org/3/howto/functional.html#iterators) 的结果
- 去掉空字符、过滤空行
  - 前：使用临时变量存储每行 `rstrip()` 的结果，使用 `if` 判断 `len()` 是否为空
  - 后：使用高阶函数 [`map()`](https://docs.python.org/3/library/functions.html#map)/[`filter()`](https://docs.python.org/3/library/functions.html#filter) 消除循环和临时变量（[参考](../2018/Higher-Order-Functions.md)）
- 记录行号
  - 前：使用自增的临时变量存储（如果少一个缩进，就会导致结果错误）
  - 后：使用 [`enumerate()` 函数](https://docs.python.org/3/library/functions.html#enumerate) 从 `1` 开始生成下标
- 格式化输出
  - 前：使用常规的 [`format()` 函数](https://docs.python.org/3/library/stdtypes.html#str.format)
  - 后：使用 [f-string _(formatted string literal)_](https://docs.python.org/3/reference/lexical_analysis.html#f-strings) 化简

所以，什么是 Pythonic —— 用代码描述 **做什么** _(what-to-do)_，而不是 **怎么做** _(how-to-do)_ —— 提升可读性。

## 迭代器 生成器

先聊聊为什么需要 **迭代器**：

- lazy if possible: `return file.readlines()` vs `yield file.readline()`

Python 3 [返回 `generator`，而不是 `list`](https://docs.python.org/3.0/whatsnew/3.0.html#views-and-iterators-instead-of-lists)：

- `range(sys.maxsize)`

定义：

- https://docs.python.org/3/howto/functional.html#iterators
- https://docs.python.org/3/howto/functional.html#generator-expressions-and-list-comprehensions

**惰性求值** _(lazy evaluation)_ 是一种 **使用时按需求值** 的求值策略（[参考](https://en.wikipedia.org/wiki/Lazy_evaluation)）：

- 一方面，**减少** 无用计算带来的 **额外开销**，并对 **使用者** 完全 **透明**
- 另一方面，支持了表示 **无穷** _(infinite)_ 的算法和数据结构（例如 无穷迭代器 [`itertools.count()`](https://docs.python.org/3/library/itertools.html#itertools.count)/[`itertools.cycle()`](https://docs.python.org/3/library/itertools.html#itertools.cycle)/[`itertools.repeat()`](https://docs.python.org/3/library/itertools.html#itertools.repeat)）

### 高阶函数

``` python
list(map(str.upper, ['aaa', 'bbb']))
list(filter(lambda x: x % 2, range(10)))
reduce(lambda d, s: dict(d, **{s: s.upper()}), ['aaa', 'bbb'], {})
```

- functor
  - function/method 例如 `len`/`str.upper`
  - lambda 例如 `lambda x: x % 2`
  - `partial` 绑定左边参数，比较奇怪
  - `operator` 运算符
- [移除 `reduce` 函数](https://docs.python.org/3.0/whatsnew/3.0.html#builtins)

### 推导式

``` python
[s.upper() for s in ['aaa', 'bbb']]
[x for x in range(10) if x % 2]
{s: s.upper() for s in ['aaa', 'bbb']}

{s.upper() for s in ['aaa', 'bbb']}
```

- https://treyhunner.com/2015/12/python-list-comprehensions-now-in-color/
- https://docs.python.org/3/tutorial/datastructures.html#list-comprehensions

### 高阶函数 vs 推导式

- https://docs.python.org/3/library/itertools.html#itertools.product

``` python
product = itertools.product
list(filter(lambda t: t[0] < t[1] and t[0] ** 2 + t[1] ** 2 == t[2] ** 2,
            product(range(1, 100), repeat=3)))
```

- https://docs.python.org/3/library/itertools.html#itertools.chain

``` python
flatten = itertools.chain.from_iterable
list(filter(
    lambda t: t[0] ** 2 + t[1] ** 2 == t[2] ** 2,
    flatten(map(lambda z:
                flatten(map(lambda x:
                            map(lambda y: (x, y, z),
                                range(x, z + 1)),
                            range(1, z + 1))),
                range(1, 100)))))
```

- https://docs.python.org/3/tutorial/datastructures.html#nested-list-comprehensions

``` python
[(x, y, z) for z in range(1, 100)
           for x in range(1, z + 1)
           for y in range(x, z + 1)
           if x ** 2 + y ** 2 == z ** 2]
```

不要滥用：

- https://treyhunner.com/2019/03/abusing-and-overusing-list-comprehensions-in-python/

## 其他技巧

- https://docs.python.org/3/library/stdtypes.html#common-sequence-operations

``` python
nums = [10, 20, 30]
('[' + ', '.join(['0x{:02X}'] * len(nums)) + ']').format(*nums)
```

- https://docs.python.org/3/tutorial/datastructures.html#nested-list-comprehensions

``` python
matrix = [[1, 2], [3, 4], [5, 6]]
list(zip(*matrix))
```

- https://docs.python.org/3/tutorial/datastructures.html#looping-techniques

TODO

- https://docs.python.org/3/tutorial/datastructures.html#more-on-conditions

TODO

## 最后聊聊 Python 这个语言

虽然 Python 的可读性不错，但可写性并不好（对于 **其他语言用户** 不太友好；仁者见仁，智者见智）：

- `len(LIST)` 而不是 `LIST.length()`（参考：[Why does Python use methods for some functionality (e.g. list.index()) but functions for other (e.g. len(list))? | Design and History FAQ](https://docs.python.org/3/faq/design.html#why-does-python-use-methods-for-some-functionality-e-g-list-index-but-functions-for-other-e-g-len-list)）
- `STR.join(LIST)` 而不是 `LIST.join(STR)`（但 `LIST.split(STR)` 却是有的，参考：[Why is join() a string method instead of a list or tuple method? | Design and History FAQ](https://docs.python.org/3/faq/design.html#why-is-join-a-string-method-instead-of-a-list-or-tuple-method)）
- `COND ? EXPR1 : EXPR2` 运算符 写为 `EXPR1 if COND else EXPR`（参考：[Is there an equivalent of C’s “?:” ternary operator? | Programming FAQ](https://docs.python.org/3/faq/programming.html#is-there-an-equivalent-of-c-s-ternary-operator)）

作为一个 **非脚本语言用户**，离开了 **编译器的检查** 和 IDE 强大的 **智能提示**，感觉自己不会写代码了：

- 由于 Python 是 **运行时强类型** 语言（参考：[Strong versus Weak Typing
_(A Conversation with Guido van Rossum)_](https://www.artima.com/intv/strongweak.html#part2)）
- 只有在 **运行时**，才能发现函数调用的参数（个数/类型）错误
- 由于使用的是 Python 2，函数不能指定 参数/返回值 的类型，VSCode **智能提示** 经常失效

尽管如此，Python 的 **核心语言** _(core language)_ 还算比较简单，很多概念都是 **良好定义** _(well-defined)_ 的；所以只要理解了基本原理，还是能很快排查各种错误的。

由于写本文时我的 Python 代码量不超过 1k 行，所以 如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2019, BOT Man
