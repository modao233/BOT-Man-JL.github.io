# 再谈资源管理（1）

> 2018/11/15
>
> 用一个具体的 C++ 例子，一步步阐述 [资源管理小记](Resource-Management.md) 的抽象讨论

## [no-toc] [no-number] TOC

[TOC]

## 需求描述

设计一个浏览器书签树形结构，节点分为两种：

- **链接节点** (url node)：包含 **节点标题**、**跳转链接**
- **文件夹节点** (folder node)：包含 **节点标题**、**子节点**

[align-center]

[img=max-width:70%]

![Chrome Bookmark](Resource-Management/chrome-bookmark.png)

对于上图所示书签结构，可以用 JSON 表示为：

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

> [代码链接](Resource-Management/structural.cpp)

从直观来看，我们可以用一个结构体存储这两类节点：

- 包含 **节点标签**、**跳转链接**、**子节点** 三个字段
- 另外附加一个字段 `is_url` 判断其类型

``` cpp
struct BookmarkNode {
  // common properties
  bool is_url;
  string title;

  // url properties
  string url;

  // folder properties
  using Children = vector<BookmarkNode>;
  Children children;
};
```

可以通过嵌套 `struct BookmarkNode` 构造存储 [sec|需求描述] 描述的书签数据：

``` cpp
auto root = BookmarkNode{false, "Bookmark Root", "",
  {
    BookmarkNode{true, "Github", "https://github.com/", {}},
    BookmarkNode{true, "BOT Man", "https://bot-man-jl.github.io/", {}},
    BookmarkNode{false, "Search Engines", "",
      {
        BookmarkNode{true, "Bing", "https://cn.bing.com/", {}},
        BookmarkNode{true, "Baidu", "https://www.baidu.com/", {}},
      },
    },
  },
};
```

从构造数据的代码可以看出，`struct BookmarkNode` 存在 **冗余** 字段：

- 对于 **链接节点**：**子节点** `children` 字段总是空的
- 对于 **文件夹节点**：**跳转链接** `url` 字段也总是空的

不同编译器下，结构体 `struct BookmarkNode` 大小：

| 编译器 | `sizeof(BookmarkNode)` |
| -- | -- |
| MSVC | 76 |
| clang++/g++ | 96 |

## 使用 `new/delete` 手动管理资源

> [代码链接](Resource-Management/virtual-pointer.cpp)

为了避免存储冗余数据（浪费空间/容易出错），可以通过 [**多态** _(Polymorphism)_](Object-Oriented-Programming.md#多态-Polymorphism) 的机制，为书签节点定义抽象接口：

- 包含获取 **节点标签**、**跳转链接**、**子节点** 的三个接口
- 另外附加一个 `is_url()` 接口，判断节点类型

``` cpp
class BookmarkNode {
 public:
  virtual ~BookmarkNode() = default;

  // common getters
  virtual bool is_url() const = 0;
  virtual string& title() = 0;

  // url getters
  virtual string& url() = 0;

  // folder getters
  using Children = vector<BookmarkNode*>;
  virtual Children& children() = 0;
};
```

> 对于 C++ 的接口，需要定义 `virtual public` 的析构函数，或 protected 析构函数。（参考：[C.35: A base class destructor should be either public and virtual, or protected and nonvirtual](http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#c35-a-base-class-destructor-should-be-either-public-and-virtual-or-protected-and-nonvirtual)）

针对 **链接节点**，我们可以重载 `BookmarkNode` 接口：

- 重载 `is_url()` 接口，总是返回 `true`
- 重载 `title()/url()` 接口，返回 **节点标签**、**跳转链接**
- 重载 `children()` 接口，返回空内容

``` cpp
class UrlNode : public BookmarkNode {
 public:
  UrlNode(string title, string url)
      : title_(title), url_(url) {}

  bool is_url() const override { return true; }
  string& title() override { return title_; }
  string& url() override { return url_; }

  Children& children() override {
    static Children dummy;
    return dummy;
  }

 private:
  string title_;
  string url_;
};
```

针对 **文件夹节点**，我们同样重载 `BookmarkNode` 接口：

- 重载 `is_url()` 接口，总是返回 `false`
- 重载 `title()/children()` 接口，返回 **节点标签**、**子节点**
- 重载 `url()` 接口，返回空内容

``` cpp
class FolderNode : public BookmarkNode {
 public:
  FolderNode(string title, Children children)
      : title_(title), children_(children) {}

  bool is_url() const override { return false; }
  string& title() override { return title_; }
  Children& children() override { return children_; }

  string& url() override {
    static string dummy;
    return dummy;
  }

 private:
  string title_;
  Children children_;
};
```

很明显，`class UrlNode/FolderNode` 编译出来的大小比 `struct BookmarkNode` 小：

| 编译器 | `sizeof(UrlNode)` | `sizeof(FolderNode)` |
| -- | -- | -- |
| MSVC | 60 | 48 |
| clang++/g++ | 72 | 64 |

可以通过嵌套 `class UrlNode/FolderNode` 构造存储 [sec|需求描述] 描述的书签数据：

``` cpp
auto root = new FolderNode{"Bookmark Root",
  {
    new UrlNode{"Github", "https://github.com/"},
    new UrlNode{"BOT Man", "https://bot-man-jl.github.io/"},
    new FolderNode{"Search Engines",
      {
        new UrlNode{"Bing", "https://cn.bing.com/"},
        new UrlNode{"Baidu", "https://www.baidu.com/"},
      },
    },
  },
};
```

相比于 [sec|结构化语言实现] 的 `struct BookmarkNode` 构造代码，看起来简洁很多。

但使用 `new` 申请的内存，需要我们 **手动** 使用 `delete` **释放**。可以使用一个递归函数实现释放所有节点：

``` cpp
void DestroyBookmarkTree(BookmarkNode* root) {
  if (!root || root->is_url())
    return;

  for (BookmarkNode* child : root->children())
    DestroyBookmarkTree(child);

  delete root;
}

// client
DestroyBookmarkTree(root);
```

## 使用 `unique_ptr` 自动回收资源

> [代码链接](Resource-Management/virtual-unique-ptr.cpp)

使用指针管理对象生命周期，是一个很头疼的问题。常常会因为对象所有权不明确，而遇到：

- 资源泄露：忘记释放申请过的资源
- 访问越界：使用已经释放过的资源

所以，现代 C++ 提倡使用智能指针管理对象所有权。（参考：[R.20: Use unique_ptr or shared_ptr to represent ownership](http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rr-owner)）

我们只需要在 [sec|使用 `new/delete` 手动管理资源] 代码的基础上，将

``` cpp
class BookmarkNode ...
  using Children = vector<BookmarkNode*>;
```

改为

``` cpp
class BookmarkNode ...
  using Children = vector<unique_ptr<BookmarkNode>>;
```

就可以了 —— 让 [`unique_ptr`](https://en.cppreference.com/w/cpp/memory/unique_ptr) 帮助我们管理对象生命周期，不再需要 `DestroyBookmarkTree` 函数了。

由于 `unique_ptr` 对象是不可复制的，这里会遇到一个微妙的问题，[不能使用初始化列表构造 `vector<unique_ptr<BookmarkNode>>`](https://stackoverflow.com/questions/25827435/any-way-to-initialize-a-vector-of-unique-ptr)：

``` cpp
auto folder = make_unique<FolderNode>("Search Engines",
  BookmarkNode::Children{
    make_unique<UrlNode>("Bing", "https://cn.bing.com/"),
    make_unique<UrlNode>("Baidu", "https://www.baidu.com/"),
  }
);
```

这里会出现 **编译错误**：`unique_ptr(const unique_ptr&) = delete;`！

原因是：上边的代码调用了 `vector` 的构造函数 [`vector(initializer_list<T> init)`](https://en.cppreference.com/w/cpp/container/vector/vector)，而这个函数需要拷贝初始化列表 `init`，恰好 `unique_ptr` 对象不可复制，所以不能通过编译。

为了解决这个问题，我们可以引入辅助函数：

- 使用 `emplace_back()` 将各个节点放入 `BookmarkNode::Children`
- 再用初始化后的 `BookmarkNode::Children` 构造 `FolderNode`

``` cpp
unique_ptr<BookmarkNode> NewUrlNode(string title, string url) {
  return std::make_unique<UrlNode>(title, url);
}

template <typename... Nodes>
unique_ptr<BookmarkNode> NewFolderNode(string title, Nodes&&... nodes) {
  BookmarkNode::Children children;
  using Expander = int[];
  (void)Expander{
      0, ((void)children.emplace_back(std::forward<Nodes>(nodes)), 0)...};

  return std::make_unique<FolderNode>(title, std::move(children));
}
```

> C++ 14 中，我们可以使用 Expander 展开变长模板参数，实现 C++ 17 的 [变长模板折叠（规约）功能](https://en.cppreference.com/w/cpp/language/fold)：
>
> - [Parameter pack > Expansion loci > Brace-enclosed initializers](https://en.cppreference.com/w/cpp/language/parameter_pack#Brace-enclosed_initializers)
> - [Variadic template pack expansion](https://stackoverflow.com/questions/25680461/variadic-template-pack-expansion/25683817#25683817)

利用这两个辅助函数，可以方便的嵌套构造出存储 [sec|需求描述] 描述的书签数据：

``` cpp
auto root = NewFolderNode("Bookmark Root",
    NewUrlNode("Github", "https://github.com/"),
    NewUrlNode("BOT Man", "https://bot-man-jl.github.io/"),
    NewFolderNode("Search Engines",
        NewUrlNode("Bing", "https://cn.bing.com/"),
        NewUrlNode("Baidu", "https://www.baidu.com/")));
```

## 写在最后 [no-number]

使用 `unique_ptr` 管理资源，可以更明确对象的所有权语义，更明确对象的生命周期。

这篇文章只是一个简单的铺垫，欢迎阅读下一篇文章：[再谈资源管理（2）](Resource-Management-Explained-2.md)。

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2018, BOT Man
