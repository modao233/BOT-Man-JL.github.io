# 再谈资源管理（1）

> 2018/11/10
>
> 用一个具体的 C++ 例子，一步步阐述 [资源管理小记](Resource-Management.md) 的抽象讨论

## [no-toc] [no-number] TOC

[TOC]

## 需求描述

设计一个浏览器书签树形结构，节点分为两种：

- 链接节点：包含 节点标题、跳转链接
- 文件夹节点：包含 节点标题、所有子节点

[align-center]

[img=max-width:70%]

![Chrome Bookmark](Resource-Management/chrome-bookmark.png)

对于上图所示书签结构，用 JSON 可以表示为：

``` json
[
  {
    "name": "GitHub",
    "type": "url",
    "url": "https://github.com/"
  },
  {
    "name": "BOT Man",
    "type": "url",
    "url": "https://bot-man-jl.github.io/"
  },
  {
    "children": [
      {
        "name": "Bing",
        "type": "url",
        "url": "https://cn.bing.com/"
      },
      {
        "name": "Baidu",
        "type": "url",
        "url": "https://www.baidu.com/"
      }
    ],
    "name": "Search Engines",
    "type": "folder"
  }
]
```

## 结构化语言实现

``` cpp
struct BookmarkNode {
  // common properties
  string title;

  // url properties
  string url;

  // folder properties
  using Children = vector<BookmarkNode>;
  Children children;
};
```

``` cpp
auto root = BookmarkNode{
  "Bookmark Root",
  "",
  {
    BookmarkNode{"Github", "https://github.com/", {}},
    BookmarkNode{"BOT Man", "https://bot-man-jl.github.io/", {}},
    BookmarkNode{
      "Search Engines",
      "",
      {
        BookmarkNode{"Bing", "https://cn.bing.com/", {}},
        BookmarkNode{"Baidu", "https://www.baidu.com/", {}},
      },
    },
  },
};
```

sizeof(BookmarkNode) == 72 88

## 使用 `new/delete` 手动管理资源

sizeof(BookmarkNode) == 4 8
sizeof(UrlNode) == 60 72
sizeof(FolderNode) == 48 64

## 使用 `unique_ptr` 自动回收资源

## 写在最后 [no-number]

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2018, BOT Man
