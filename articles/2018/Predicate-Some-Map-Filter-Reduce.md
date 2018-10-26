# 谓词逻辑/高阶函数：让代码更简洁

> 2018/10/25
>
> 如何设计使用谓词逻辑/高阶函数，让代码更简洁、更清晰。

## TOC [no-toc]

[TOC]

## 谓词逻辑：从实现到语义的抽象

**谓词** (predicate) 是一类返回 true/false 的函数。

有一段代码，实现复制 chrome 地址栏文字后，自动修正复制内容的功能：

``` cpp
// fix clipboard text in omnibox after copy
if (0 != display_url_.compare(clipboard_text)) {
  if (0 == display_url_.find(clipboard_text) &&
      std::wstring::npos == clipboard_text.find(STR_HTTP)) {
    // add 'http://' to clipboard text
  } else {
    // not fix
  }
} else {
  // use page url if copied full display url
}
```

上边的字符串比较表达式可以使用谓词简化：

谓词 | 对应的代码
-----|-----
`Equal(str1, str2)`      | `0 == str1.compare(str2)`
`StartsWith(str1, str2)` | `0 == str1.find(str2)`
`Contains(str1, str2)`   | `std::wstring::npos != str1.find(str2)`

简化后的代码：

``` cpp
// fix clipboard text in omnibox after copy
if (!Equal(display_url_, clipboard_text)) {
  if (StartsWith(display_url_, clipboard_text) &&
      !Contains(clipboard_text, STR_HTTP)) {
    // add 'http://' to clipboard text
  } else {
    // not fix
  }
} else {
  // use page url if copied full display url
}
```

## 高阶函数：消除循环和临时变量

### 例子：使用 `some` 合并谓词

很多时候，我们需要对一个序列的每个元素检查同一个谓词。例如：

- 检查序列中 **每个元素** 是否 **都** 满足某个条件；
- 检查序列中是否 **存在某些元素** 满足这个条件。

设元素 `x` 是否满足条件 `f` 的表述为：`f(x)`，上述两个组合可以表述为：

- `f(x1) && f(x2) && f(x3) ...`
- `f(x1) || f(x2) || f(x3) ...`

实际上对应了我们常用的 `every`/`some` 逻辑。

输入的 `config`：

``` json
{
  "avaliable_days": [ 1, 3, 5, 7 ]
}
```

检查的目标：

- 检查一天是否为 周一、周三、周五、周日 中的某一天
- 注意：周日既可以表示为 `0`，也可以表示为 `7`

使用 `for` 实现的代码：

``` js
function isDayAvaliable(config, day) {
  for (const d of config.avaliable_days) {
    if (d % 7 === day)
      return true;
  }
  return false;
}
```

使用高阶函数 `some` 化简的代码：

``` js
function isDayAvaliable(config, day) {
  return config.avaliable_days.some(d => d % 7 === day);
}
```

### 例子：使用 `map` 映射结果

[cheerio.js](https://cheerio.js.org/) 提供了类似 jQuery 的解析 DOM 树接口，可以用于抓取页面元素。对于每个 DOM 节点的解析结果，都可以用两种方式获取：

- `.children()` 返回当前节点的子节点
- `.map(fn)` 遍历当前节点的子节点，执行函数 `fn`，返回执行结果并构造为数组

抓取的页面格式：

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

抓取的目标结果：

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

使用 `map` 抓取的代码：

``` js
const result = $('.myclass ul').map(
  (i, e_ul) => { anchors: $('li a', e_ul).map(
    (i, e_a) => $(e_a).text()
  )}
);
```

### 高阶函数 `map`/`reduce`/`filter` 的几个应用

数学上的描述：

- map: (E → F) × Seq<‍E> → Seq<‍F>
- filter: (E → boolean) × Seq<‍E> → Seq'<‍E>
- reduce: (F × E → F) × Seq<‍E> × F → F

图形上描述：

- map:

[align-center]

[img=max-width:50%]

![map](https://atendesigngroup.com/sites/default/files/array-map.png)

- filter:

[align-center]

[img=max-width:50%]

![filter](https://atendesigngroup.com/sites/default/files/array-filter.png)

- reduce:

[align-center]

[img=max-width:50%]

![reduce](https://atendesigngroup.com/sites/default/files/array-reduce.png)

#### 稳定排序：使用 `map` 映射下标进行复合排序

> 参考：[Fast stable sorting algorithm implementation in javascript (@stackoverflow)](https://stackoverflow.com/questions/1427608/fast-stable-sorting-algorithm-implementation-in-javascript/48660568#48660568)

``` js
// [ y1, y2, x, y3 ] => [ x, y1, y2, y3 ]
function stableSort(array, compare) {
  return array
    .map((item, index) => ({ item, index }))
    .sort((a, b) => compare(a.item, b.item) || a.index - b.index)
    .map(({ item }) => item);
}
```

#### 数组去重：使用 `filter` 过滤出首次出现的元素

``` js
// [ x, y, y ] => [ x, y ]
function uniqueArray(array) {
  return array.filter((elem, i) => array.indexOf(elem) === i);
}
```

#### 数组扁平化：使用 `reduce` 规约拼接数组元素

``` js
// [ [x, y], [z] ] => [ x, y, z ]
function flatArray(array) {
  return array.reduce((acc, elem) => acc.concat(elem), []);
}
```

## 写在最后

> 延伸阅读：
>
> - [Simplify your JavaScript – Use .map(), .reduce(), and .filter()](https://medium.com/poka-techblog/simplify-your-javascript-use-map-reduce-and-filter-bd02c593cc2d)
> - [Array Map, Filter and Reduce in JS](https://atendesigngroup.com/blog/array-map-filter-and-reduce-js)
> - [js map, reduce, forEach, filter的一般实现](https://blog.csdn.net/u011700203/article/details/47191893)
> - [Reading 25: Map, Filter, Reduce | MIT 6.005](http://web.mit.edu/6.005/www/fa15/classes/25-map-filter-reduce/)

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2018, BOT Man
