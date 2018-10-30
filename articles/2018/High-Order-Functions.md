# 高阶函数：消除循环和临时变量

> 2018/10/27
>
> 如何设计使用高阶函数，让代码更简洁、更清晰。

## 目录 [no-toc]

[TOC]

## 使用 `every/some` (`all/any`) 合并谓词

> 数学描述：
>
> $$some/every: (E \rightarrow boolean) \times [E] \rightarrow boolean$$

很多时候，我们需要对多个元素检查同一个 **谓词** (predicate)。例如：

- 检查 **每个元素** 是否 **都** 满足一个条件；
- 检查是否 **存在某些元素** 满足一个条件。

设这组元素序列为 $S$，谓词为 $P$，用离散数学可以表述为：

- $\forall{x \in S}, P(x)$
- $\exists{x \in S}, P(x)$

> 编程语言实现：
>
> - JavaScript：[`Array.prototype.some`](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/some)/[`Array.prototype.every`](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/every)
> - C++：[`std::any_of`/`std::all_of`](https://en.cppreference.com/w/cpp/algorithm/all_any_none_of)

### 例子：检查日期

检查的目标：

- 检查一天是否为 周一、周三、周五、周日 中的某一天
- 由于不同人配置方法不一样，周日既可以表示为 `0`，也可以表示为 `7`

对应输入的配置：

``` json
{
  "avaliable_days": [ 1, 3, 5, 7 ]
}
```

使用 `for` 实现的 JavaScript 代码：

``` js
function isDayAvaliable(avaliable_days, day_now) {
  for (const day of avaliable_days) {
    if (day % 7 === day_now)
      return true;
  }
  return false;
}
```

使用高阶函数 `some` 化简的 JavaScript 代码：

``` js
function isDayAvaliable(avaliable_days, day_now) {
  return avaliable_days.some(day => day % 7 === day_now);
}
```

相比循环实现的代码，使用高阶函数一方面缩短代码的长度，另一方面可读性更好 —— 从代码上可以直观的看出其语义：检查 `avaliable_days` 中是否存在 `some` `day`，满足和 `day_now` 表示同一天的 。

## 使用 `map` 映射结果

> 数学描述：
>
> $$map: (E \rightarrow F) \times [E] \rightarrow [F]$$

图形描述：

[align-center]

[img=max-width:50%]

![Map](High-Order-Functions/map.png)

[align-center]

map - [source](https://atendesigngroup.com/sites/default/files/array-map.png)

有时候，我们需要对多个元素进行同一个 **转换** (transform)。例如：上边图片里，给定 3 个水果，把每个水果都进行切片转换，得到 3 个水果切片。操作的特点是：

- 对每个元素都进行相同的转换
- 转换后结果的个数和转换前相同

> 编程语言实现：
>
> - JavaScript：[`Array.prototype.map`](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/map)
> - C++：[`std::transform`](https://en.cppreference.com/w/cpp/algorithm/transform)

### 例子：稳定排序

JavaScript 里，[`Array.prototype.sort`](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/sort) 函数不保证 [排序的稳定性](https://en.wikipedia.org/wiki/Sorting_algorithm#Stability)；而在很多业务场景下，要求排序是稳定的。例如：有一个比赛名单，上面记录了每个人的获胜次数；现在需要选出获胜次数最少的若干个人，获胜次数相同则优先选择名单顺序靠前的人。

假设给定输入：

``` json
[
  {
    "id": "john",
    "win": 2
  },
  {
    "id": "jill",
    "win": 1
  },
  {
    "id": "jack",
    "win": 3
  },
  {
    "id": "jess",
    "win": 2
  }
]
```

根据上述规则，我们应该选择 `john` 和 `jill`，而不会选择 `jill` 和 `jess`。

我们可以利用（不稳定排序 + 下标排序）构造一个复合排序，实现稳定排序。

> 代码参考：[Fast stable sorting algorithm implementation in javascript (@stackoverflow)](https://stackoverflow.com/questions/1427608/fast-stable-sorting-algorithm-implementation-in-javascript/48660568#48660568)

``` js
// [ y1, y2, x, y3 ] => [ x, y1, y2, y3 ]
function stableSort(array, compare) {
  return array
    .map((item, index) => ({ item, index }))
    .sort((a, b) => compare(a.item, b.item) || a.index - b.index)
    .map(({ item }) => item);
}
```

首先，将每个元素连同其下标转换成（元素，下标）序对；然后，根据元素的比较函数作为排序依据；如果比较相等，那么就优先排列下标靠前的元素；最后，从排好序的（元素，下标）序对中取出元素，从而得到结果。

### 例子：抓取页面

[cheerio.js](https://cheerio.js.org/) 提供了类似 jQuery 的解析 DOM 树接口，可以用于抓取页面元素。对于每个 DOM 节点的解析结果，都可以用两种方式获取：

- `.children()` 返回当前节点的子节点
- `.map(fn)` 遍历当前节点的子节点，执行函数 `fn`，返回执行结果并构造为数组

假设抓取的页面格式：

``` html
<div class="myclass">
  <p>section1</p>
  <ul>
    <li>Please visit <a href="about:blank">website1</a>!</li>
    <li>Please visit <a href="about:blank">website2</a>!</li>
  </ul>
  <p>section2</p>
  <ul>
    <li>Please visit <a href="about:blank">website3</a>!</li>
    <li>Please visit <a href="about:blank">website4</a>!</li>
  </ul>
</div>
```

对应抓取的目标结果：

``` json
[
  { "anchors": [ "website1", "website2" ] },
  { "anchors": [ "website3", "website4" ] }
]
```

使用 `for` 实现的代码：

``` js
const result = [];
for (cosnt e_ul of $('.myclass ul').children()) {
  const anchors = [];
  for (const e_a of $('li a', e_ul).children()) {
    const text = $(e_a).text();
    anchors.push(text);
  }
  result.push({ anchors });
}
```

- 需要使用两层 for 循环
- 需要使用 `anchors` 变量保存中间结果

使用 `map` 抓取的代码：

``` js
const result = $('.myclass ul').map(
  (i, e_ul) => { anchors: $('li a', e_ul).map(
    (i, e_a) => $(e_a).text()
  )}
);
```

## 使用 `filter` 过滤结果

> 数学描述：
>
> $$filter: (E \rightarrow boolean) \times [E] \rightarrow [E]'$$

图形描述：

[align-center]

[img=max-width:50%]

![Filter](High-Order-Functions/filter.png)

[align-center]

filter - [source](https://atendesigngroup.com/sites/default/files/array-filter.png)

有时候，我们需要对多个元素按照一个规则进行 **筛选** (filter)。例如：上边图片里，给定 3 个水果，我们筛选球形的水果，得到 1 个水果（橙子）。操作的特点是：

- 对每个元素都检查相同的谓词
- 筛选后结果的个数 ≤ 元素的总数

> 编程语言实现：
>
> - JavaScript：[`Array.prototype.filter`](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/filter)
> - C++：[`std::copy_if`](https://en.cppreference.com/w/cpp/algorithm/copy)

### 例子：数组去重：使用 `filter` 过滤出首次出现的元素

``` js
// [ x, y, y ] => [ x, y ]
function uniqueArray(array) {
  return array.filter((elem, i) => array.indexOf(elem) === i);
}
```

## 使用 `reduce` 规约结果

> 数学描述：
>
> $$reduce: (E \times F \rightarrow F) \times [E] \times F \rightarrow F'$$

图形描述：

[align-center]

[img=max-width:50%]

![Reduce](High-Order-Functions/reduce.png)

[align-center]

reduce - [source](https://atendesigngroup.com/sites/default/files/array-reduce.png)

有时候，我们需要对多个元素按照一个规则先进行 **加工** (process)，然后 **累加** (accumulate) 起来。例如：上边图片里，给定 3 个水果，我们先提供一个碗，每一步把水果切成小块并放入碗里，最后得到一碗水果块。操作的特点是：

- 给定一个 **初始值** 作为累加结果
- **每次** 把 **一个** 元素通过规约方法累加到结果上
- 规约完成后，最终得到 **一个结果**

> 编程语言实现：
>
> - JavaScript：[`Array.prototype.reduce`](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/reduce)
> - C++：[`std::accumulate`](https://en.cppreference.com/w/cpp/algorithm/accumulate)

### 例子：数组扁平化：使用 `reduce` 规约拼接数组元素

``` js
// [ [x, y], [z] ] => [ x, y, z ]
function flatArray(array) {
  return array.reduce((acc, elem) => acc.concat(elem), []);
}
```

## 写在最后

综上，（尤其针对多个循环嵌套的情况，[sec|例子：抓取页面]），可以通过高阶函数嵌套的方法，消除循环和临时变量。

> 延伸阅读：
>
> - [Simplify your JavaScript – Use .map(), .reduce(), and .filter()](https://medium.com/poka-techblog/simplify-your-javascript-use-map-reduce-and-filter-bd02c593cc2d)
> - [Array Map, Filter and Reduce in JS](https://atendesigngroup.com/blog/array-map-filter-and-reduce-js)
> - [js map, reduce, forEach, filter的一般实现](https://blog.csdn.net/u011700203/article/details/47191893)
> - [Reading 25: Map, Filter, Reduce | MIT 6.005](http://web.mit.edu/6.005/www/fa15/classes/25-map-filter-reduce/)

相关 demo 代码：

- [js 版本](High-Order-Functions/High-Order-Functions.js)
- [C++ 版本](High-Order-Functions/High-Order-Functions.cpp)

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2018, BOT Man
