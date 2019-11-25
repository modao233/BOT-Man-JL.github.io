# Python 风格的函数式

> 2019/11/23
> 
> Readability counts. —— The Zen of Python, by Tim Peters (`import this`)

写给 **非脚本语言用户**（比如学习 Python 之前的我）。

运行时才发现参数个数/参数类型问题。。

## Intro

一个最经典的例子：如何打印文件中每一个非空行（去掉每行右侧的空字符），并在打印时加上行号。对于 C/C++/Java 等静态语言用户，凭借第一感觉，会这么写：

``` python
file = open(__file__)
try:
    line_num = 1
    while True:
        line = file.readline()
        if not line:
            break

        strip_line = line.rstrip()
        if len(strip_line):
            print('%2d: %s' % (line_num, strip_line))
            line_num += 1
finally:
    file.close()
```

- open/close 容易忘记关闭（如果不写在 finally，则可能泄露）
- while-readline
- rstrip-check-print
- increment 位置问题（如果少一个缩进，则打印原始行号）

## iterable

- https://docs.python.org/3/howto/functional.html#iterators

``` python
with open(__file__) as file:
    for line_num, line in enumerate(filter(len, map(str.rstrip, file)), 1):
        print('%d: %s' % (line_num, line))
```

- with 类似 RAII
- for-iterable
- map-filter-print
- enumerate 代替 increment

## generator/comprehensions

- https://docs.python.org/3/howto/functional.html#generator-expressions-and-list-comprehensions

### map/filter/reduce

- [返回 `generator`，而不是 `list`](https://docs.python.org/3.0/whatsnew/3.0.html#views-and-iterators-instead-of-lists)
  - lazy if possible: `return file.readlines()` vs `yield file.readline()`
  - `range(sys.maxsize)`

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

### list/set/dict comprehensions

``` python
[s.upper() for s in ['aaa', 'bbb']]
[x for x in range(10) if x % 2]
{s: s.upper() for s in ['aaa', 'bbb']}

{s.upper() for s in ['aaa', 'bbb']}
```

- https://docs.python.org/3/tutorial/datastructures.html#list-comprehensions

``` python
product = itertools.product
list(filter(lambda t: t[0] < t[1] and t[0] ** 2 + t[1] ** 2 == t[2] ** 2,
            product(range(1, 100), repeat=3)))

flatten = itertools.chain.from_iterable
list(filter(
    lambda t: t[0] ** 2 + t[1] ** 2 == t[2] ** 2,
    flatten(map(lambda z:
                flatten(map(lambda x:
                            map(lambda y: (x, y, z),
                                range(x, z + 1)),
                            range(1, z + 1))),
                range(1, 100)))))

[(x, y, z) for z in range(1, 100)
           for x in range(1, z + 1)
           for y in range(x, z + 1)
           if x ** 2 + y ** 2 == z ** 2]
```

- https://docs.python.org/3/tutorial/datastructures.html#nested-list-comprehensions
- https://docs.python.org/3/library/itertools.html#itertools.chain

## others

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

## 写在最后

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2019, BOT Man
