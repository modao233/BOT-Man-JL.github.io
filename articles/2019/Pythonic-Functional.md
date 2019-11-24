# Python 风格的函数式

> 2019/11/23
> 
> Readability counts. —— The Zen of Python, by Tim Peters (`import this`)

写给 **非脚本语言用户**（比如学习 Python 之前的我）。

- https://docs.python.org/3/tutorial/datastructures.html
- https://docs.python.org/3/howto/functional.html
- https://developer.ibm.com/articles/l-prog/
- https://developer.ibm.com/tutorials/l-prog2/
- https://developer.ibm.com/tutorials/l-prog3/

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

## iterable/range/enumerate

``` python
with open(__file__) as file:
    for line_num, line in enumerate(filter(len, map(str.rstrip, file))):
        print('%d: %s' % (line_num, line))
```

- with 类似 RAII
- for-iterable
- map-filter-print
- enumerate 代替 range

## generator/comprehension

- Python2 vs Python3: list vs generator

``` python
list(map(str.upper, ['aaa', 'bbb']))
[s.upper() for s in ['aaa', 'bbb']]

{s: s.upper() for s in ['aaa', 'bbb']}
```

``` python
list(filter(lambda x: x % 2, range(10)))
[x for x in range(10) if x % 2]
```

- [移除 `reduce` 函数](https://docs.python.org/3.0/whatsnew/3.0.html#builtins)
- 用于 `partial` 构造绑定参数
- nested

## times

- https://docs.python.org/3/library/stdtypes.html#common-sequence-operations

``` python
nums = [10, 20, 30]
fmt = '[' + ', '.join(['{}'] * len(nums)) + ']'
fmt.format(*nums)
```

## unpacking

- https://docs.python.org/3/tutorial/controlflow.html#tut-unpacking-arguments

``` python
matrix = [[1, 2], [3, 4], [5, 6]]
zip(*matrix)
```

## 写在最后

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2019, BOT Man
