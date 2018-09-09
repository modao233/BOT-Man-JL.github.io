# 不推荐开发微信小程序

> 2018/9/19
>
> 微信 ~~开放~~ **封闭** 平台小程序的一些坑，seriously 🙃

## 写在前面

最近和 _Team 56_ 的小伙伴们体验了一下微信平台的小程序开发，开发了一个 [本地新闻小程序 —— 嗅嗅新鲜事](https://github.com/BOT-Man-JL/xiuxiu)（Demo）。

经过这次开发，我体会到了 2 年前一次黑客马拉松上听到的一句话：

> 微信平台的最大不足，就是过于 **封闭** ...

我们那次使用 [Slack 开放平台](https://api.slack.com/) 实现课题。由于那是第一次接触 Web 前端，就随口问了一下微信小程序生态的情况，没想到听到的全是吐槽。当时想，线上那么多小程序都蛮好用的，怎么开发者们都在吐槽呢。。。现在是切身体会到了 —— **不推荐开发微信小程序**。

本文简单的吐槽一下这次开发中，遇到的一些坑。

## 并不简单的 Web 开发 —— WXML 不是 HTML

首次创建小程序例子项目的时候，乍一看，这不就是 [vue](https://cn.vuejs.org/) 风格的前端框架吗？写了一段时间之后，才发现：小程序的 [WXML](https://developers.weixin.qq.com/miniprogram/dev/quickstart/basic/file.html) 并不是想象中的 HTML！小程序开发并不是简单的 Web 前端开发！

### 不能操作 DOM 元素

在 JavaScript 里，经常能看到类似下面的语句：

``` js
document.querySelector(".log").innerHTML += msg + "<br/>";
```

- 首先，选中 DOM 元素
- 然后，修改选中元素的子元素
- 最后，实现修改页面的内容和样式

而小程序的设计思路和上边的代码大相径庭 —— 不能动态修改 DOM 元素，只能通过修改 `class` 或使用 [`wx:if` 条件渲染](https://developers.weixin.qq.com/miniprogram/dev/framework/view/wxml/conditional.html) 动态修改样式。。。

由于不能动态修改页面节点结构，小程序更适合显示

> [封闭式的、逻辑简单的](http://www.cnblogs.com/yingwo/p/5912797.html) 页面。

尽管小程序也提供了一些 [节点信息获取方法](https://developers.weixin.qq.com/miniprogram/dev/api/wxml-nodes-info.html)，却仍然无法实现丰富的功能。

所以，小程序很难渲染出像很多网页一样丰富、动态的内容。（其实是可以的，只是代价特别大 😐）

### 原生组件

https://developers.weixin.qq.com/miniprogram/dev/component/native-component.html

### 其他限制

- [代码大小不能超过 2M，分开打包不能超过 8M](https://developers.weixin.qq.com/miniprogram/dev/framework/subpackages.html)
- [本地存储只允许 10M](https://developers.weixin.qq.com/miniprogram/dev/api/data.html)
- [网络请求也有诸多苛刻的限制...](https://developers.weixin.qq.com/miniprogram/dev/api/api-network.html)

## 敏捷的敌人 —— XX的审核

- 音视频/交流板块
- 流畅慢
- WebView 权限限制（公众号外链）

https://developers.weixin.qq.com/miniprogram/dev/component/web-view.html

## 写在最后

最后，我在文档里发现了一些小问题；正准备反馈，但是 **没找到反馈的入口**，于是放弃了。😔

想起 2 年前第一次接触 Web 前端，第一次使用 JavaScript，就有幸体验到了什么叫做 **开放平台**。。。

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2018, BOT Man
