# 两个提高 可扩展性 的途径

> 2017/8/24
>
> 鹅厂 7 - 8 月 实习笔记

[heading-numbering]

## [no-toc] [no-number] TOC

[TOC]

## 热编译代码

### 背景

上个月，需要上线一个新的服务：根据用户的属性（用户画像、机器属性等），进行个性化的内容分发。服务使用 _Node_ 开发，并希望能利用 _JSON_ 编写配置文件，同时支持 **热更新规则**。例如：根据用户的年龄（`age`）搜索不同的页面。

``` json
[
  {
    "age": [0, 10],
    "search": "https://google.com/search?q=kid"
  },
  {
    "age": [10, 20],
    "search": "https://google.com/search?q=teenager"
  },
  {
    "age": [30, 200],
    "search": "https://google.com/search?q=old%20people"
  }
]
```

但是问题来了：**更新配置文件** 只能设定代码里已经写好的规则类型（例如 `age`）。而随着业务的不断升级，需要经常加入新的规则字段；而新加入的规则字段是开放性的 _(open-ended)_，即不能预先定义一个有限集合保存可能出现的规则。

例如：需要加入对性别（`gender`）进行判断。

``` json
[
  {
    "age": [0, 10],
    "gender": "male",
    "search": "https://google.com/search?q=boy"
  },
  {
    "age": [0, 10],
    "gender": "female",
    "search": "https://google.com/search?q=girl"
  },
  ...
]
```

再如：需要加入对是否周日（`is_sunday`）进行判断。

``` json
[
  {
    "age": [0, 10],
    "is_sunday": false,
    "search": "https://google.com/search?q=kid"
  },
  {
    "age": [0, 10],
    "is_sunday": true,
    "search": "https://google.com/search?q=kids%20party"
  },
  ...
]
```

### 实现

这时候需要有 **动态配置规则类型** 的能力。由于服务使用 Node 开发：

- 利用脚本的动态特性，可以从配置文件加载 **规则描述文本**
- 再将描述文本编译为 **规则匹配函数**
- 最后在规则匹配的过程中，利用这些编译好的函数直接运行判断

同时使用 **热更新配置** 以及 **热编译代码**，可以大大化简了服务升级的过程，使其更加平滑。

> 由于 **规则配置** 和 **规则描述配置** 都在服务器管理后台上完成，可以避免脚本注入的问题。

#### 规则描述配置

由于 _JSON_ 不能直接写函数，所有我们用文本对规则描述进行配置。

``` json
{
  "age": "(request, rule) => rule[0] <= request.age && request.age < rule[1]",
  "gender": "(request, rule) => rule == request.gender",
  "is_sunday": "(request, rule) => rule == (new Date().getDay() == 0)",
  ...
}
```

#### 编译规则描述

首先，和动态加载配置文件一样：

- 需要一个 `configLoader` 监听 `configUpdated` 事件
- 并通过 `loadConfig` 函数重新加载配置

``` javascript
configLoader.on('configUpdated', loadConfig);
```

然后，在 `loadConfig` 中：

- 提取每条 **规则描述文本**（`rawText`）
- 使用 `eval` 在当前执行的上下文，将 `rawText` 编译为函数对象
- 并给将不同 `key` 对应的 `testFunction` 保存起来

``` javascript
try {
    eval('testFunction[key] = ' + rawText);
} catch (e) {
    throw new Error('invalid function: "' + rawText + '"');
}
```

最后，在规则匹配阶段，只需要调用编译好的函数进行判断，从而不会带来太多额外的开销。（编译时间对请求规模是常数时间）

``` javascript
testFunction[key](request, rule);
```

## 倒用装饰器

### 背景

由于新上线的服务需要接入已有的一个规则过滤服务，完成一些更复杂的规则匹配。（例如，调用其他服务获取更多用于匹配的数据）

但是旧的规则过滤服务要求用户配置的规则遵循他的一套规范。例如：旧服务可以匹配用户的饮食兴趣。

``` json
[
  {
    "food": {
      "value": "hot pot|cantonese food|chinese food",
      "not": false,
      "splitor": "|",
      "and": false
    },
    "search": "https://google.com/search?q=chinese"
  },
  {
    "food": {
      "value": "seafood",
      "not": false,
      "splitor": "|",
      "and": false
    },
    "search": "https://google.com/search?q=seafood"
  }
]
```

为了改良规则编写的体验、提升规则过滤引擎可扩展性，新服务决定重构旧服务，并 **兼容** 旧服务使用的 **规则配置**。

### 实现

#### 旧服务的实现

旧服务要求所有的规则配置要有 4 个字段：

- `value` 表示规则的值
- `not` 表示是否对结果取反
- `splitor` 表示对 `value` 分割时的分隔符
- `and` 表示对分割的结果取 与/或

而在旧服务的规则判断流程中，所有的规则都需要经过 分割、比较、与/或 短路、取反 的过程：

``` javascript
function testOldKey(request, rule, key) {
    let pass = true;
    const targets = rule.value.split(rule.splitor);
    for (const target of targets) {
        pass = testFunction[key](request, target);  // *

        // Short circuit
        if (pass && !rule.and) break;
        if (!pass && rule.and) break;
    }
    return rule.not ? !pass : pass;
}
```

#### 新服务的需求

新服务为了简化配置，允许使用简单的记法，同时又必须让旧配置能够继续使用，甚至允许两者可以同时使用。例如：允许同时判断 `food` 和 `age` 字段。

``` json
[
  {
    "food": {
      "value": "hot pot|cantonese food|chinese food",
      "not": false,
      "splitor": "|",
      "and": false
    },
    "age": [0, 10],
    "search": "https://google.com/search?q=kids%20chinese%20food"
  },
  ...
]
```

如果直接使用旧服务的框架，可以通过 `isOldKey` 判断是否为旧服务的规则，再分别调用。

``` javascript
if (isOldKey(key))
    return testOldKey(request, rule, key);
else
    return testFunction[key](request, rule);
```

#### 新服务的实现

为了简化客户端的调用、并兼容未来不可预测的扩展，我们可以利用 **装饰器** _(decorator)_ 模式对基本的 `testFunction` 进行 **装饰** _(embellish)_，从而实现 **开闭原则** _(open/closed principle)_。

``` javascript
function testWrapper(testFunction) {
    return function (request, rule) {
        let pass = true;
        const targets = rule.value.split(rule.splitor);
        for (const target of targets) {
            pass = testFunction(request, target);  // *

            // Short circuit
            if (pass && !rule.and) break;
            if (!pass && rule.and) break;
        }
        return rule.not ? !pass : pass;
    };
}
```

**装饰器** `testWrapper` 返回一个函数 **闭包** _(closure)_，闭包内部上执行 [sec|旧服务的实现] 描述的流程，并通过调用传入的 `testFunction` 执行实际的判断。旧服务规则字段（例如 `food`）对应的函数，可以通过 `testWrapper` 生成。

``` json
{
  "food": "testWrapper( (request, target) => getFood(request.id) == target )",
  ...
}
```

最后，和 [sec|编译规则描述] 一样，客户端代码通过执行 `testFunction` 就可以完成对一个 key 的匹配：

- 倒用 **装饰器** 模式，暴露一个 **更通用的接口**，让实现处理 **自己的细节**
- 针对配置格式的专门处理方法从 **客户端层** 降到 **配置处理层**（消除 `isOldKey`）
- 旧服务、新服务（甚至未来更多服务）的规则对 **客户端** 完全 **透明**

![Glass Wall](Improve-Flexibility/glass-wall.jpg)

## 声明

所有代码均与实际代码不同，仅用于描述问题。

本文主要内容是我的 **个人理解**。对本文有什么问题，**欢迎斧正**。😉

This article is published under MIT License &copy; 2017, BOT Man
