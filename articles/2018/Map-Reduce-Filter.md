# 谓词逻辑/高阶函数

> 2018/10/25
>
> 如何设计使用谓词逻辑/高阶函数，让代码更清晰

## 谓词逻辑：从实现到语义的抽象

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

上边的代码可以使用 **谓词** (predicate) 简化：

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

``` js
function cmpPeople(a, b) {
  return (a.records || []).length - (b.records || []).length;
}

// => [ [ 'id', 'id', ...], [ 'id', 'id', ...], ... ]
function selectPeople(config) {
  const groups = [];
  for (const { key, count } of config.rules) {
    const original_group = Object.assign({}, config[key]);
    original_group.sort(cmpPeople);

    const group = [];
    for (const { id } of original_group) {
      group.push(id);
    }
    groups.push(group.slice(0, count));
  }
  return groups;
}

// => [ [ 'id', 'id', ...], [ 'id', 'id', ...], ... ]
function selectPeople(config) {
  return config.rules.map(({ key, count }) =>
    Object.assign({}, config[key])
      .sort(cmpPeople)
      .map(({ id }) => id)
      .slice(0, count)
  );
}
```

``` js
function shouldNotify(config) {
  const day_now = new Date().getDay();
  return config.days_should_notify.some(day => day % 7 === day_now);
}
```

### cheerio.js 抓取页面元素

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

使用 [cheerio.js](https://cheerio.js.org/) 抓取的代码：

``` js
$('.myclass ul').map(
  (i, e) => { anchors: $('li a', e).map(
    (i, e) => $(e).text()
  )}
);
```

### 使用高阶函数实现一些常用功能

``` js
// [ y1, y2, x ] => [ x, y1, y2 ]
function stableSort(array, compare) {
  return array
    .map((item, index) => ({ item, index }))
    .sort((a, b) => compare(a.item, b.item) || a.index - b.index)
    .map(({ item }) => item);
}
```

> ref: [stableSort@stackoverflow](https://stackoverflow.com/questions/1427608/fast-stable-sorting-algorithm-implementation-in-javascript/48660568#48660568).

``` js
// [ x, y, y ] => [ x, y ]
function uniqueArray(array) {
  return array.filter((elem, index) => array.indexOf(elem) === index);
}
```

``` js
// [ [x, y], [z] ] => [ x, y, z ]
function flatArray(array) {
  return array.reduce((acc, elem) => acc.concat(elem), []);
}
```

## 写在最后

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2018, BOT Man
