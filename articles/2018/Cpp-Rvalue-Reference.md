# C++ 11 右值引用总结

> 2018/7/15
>
> 你想知道的：右值引用/移动语义/拷贝省略/通用引用/完美转发

[heading-numbering]

## [no-toc] [no-number] TOC

[TOC]

## 写在前面

> 如果你还不知道 C++ 11 引入的右值引用怎么实现的，可以读读这篇文章，看看有什么 **启发**；如果你已经对右值引用了如指掌，也可以读读这篇文章，看看有什么 **补充**。欢迎交流~ 😉

尽管 **C++ 17 标准** 在去年底已经正式发布了，但由于 C++ 语言变得 **越来越复杂**，让许多人对很多新特性 **望而却步**。

就连 2011 年发布的 C++ 11 标准，很多人虽然对 **右值引用/通用引用/移动语义/完美转发/拷贝省略** 之类的概念都 **有所耳闻**，却没有机会详细了解其 **设计初衷/实现原理**，甚至对一些细节 **有所误解**（包括我；如果对文章有任何问题，欢迎指出）。

我刚开始学习 C++ 的时候，也常常混淆这几个概念。但随着深入了解、与人探讨，才逐步理清楚这几个概念的来龙去脉。先分享几个我曾经的犯下的错误。

### 错误：移动临时值

``` cpp
void fn() {
  //...

  std::string base_url  = tag->GetBaseUrl();
  std::string user_name = user_mgr->GetName();
  if (!base_url.empty())
    UpdateQueryUrl(std::move(base_url) + "&q=" +
                   std::move(user_name));
  else
    UpdateQueryUrl(default_base_url_ + "&q=" +
                   std::move(user_name));

  //...
}
```

上述代码的问题在于：对临时值使用 `std::move`，会导致后续代码不能使用 `base_url` 和 `user_name`；如果使用，会出现 **未定义行为** _(undefined behavior)_。

正确做法是封装成一个函数，只在返回值使用 `std::move`（这也符合函数式思想，尽可能减少临时值）：

``` cpp
std::string GetQueryUrlString(const Tag* tag,
                              const UserMgr* user_mgr) {
  ASSERT(tag && user_mgr);

  std::string base_url  = tag->GetBaseUrl();
  std::string user_name = user_mgr->GetName();
  if (!base_url.empty())
    return std::move(base_url) + "&q=" +
           std::move(user_name);
  else
    return default_base_url_ + "&q=" +
           std::move(user_name);
}

void fn() {
  //...
  UpdateQueryUrl(GetQueryUrlString(tag, user_mgr));
  //...
}
```

### 错误：移动返回值

``` cpp
std::unique_ptr<int> fn() {
  auto ret = std::make_unique<int>(64);
  return std::move(ret);  // -> return ret;
}
```

上述代码的问题在于：对返回值使用 **冗余的** `std::move`。因为 C++ 会把返回值当成右值，直接进行 [sec|移动语义] 移动构造（语言标准）；如果编译器允许 [sec|拷贝省略] 拷贝省略，还可以省略这一步的构造，直接移动内存（编译器优化）。

## 右值引用

> 如果已经了解了左右值基本概念，可以直接跳过这个部分。

### 概念

C++ 11 强化了左值/右值的概念：**左值** (lvalue, left value) 指的是可以通过变量名/指针/引用使用的值，**右值** (rvalue, right value) 指的是函数返回的临时值。

区分左右值的一个不完全正确的方法是：左值可以取到有效地址，而右值不行。另一个从概念上的区分方法是：可以给左值赋值，但右值不行。

### 示例

``` cpp
class Foo {
public:
  void Process(Data& data);   // data is lvalue
  void Process(Data&& data);  // data is rvalue
};
```

使用时，会根据参数/对象的左右值属性重载不同的函数：

``` cpp
Foo foo;
Data data;

foo.Process(data);    // data is lvalue
foo.Process(Data{});  // data is rvalue
```

另外，我们还可以加入 **常引用** (const reference) 作为参数的成员函数，同时接受左值、右值参数：

``` cpp
// class Foo
  void Handle(const Data& data);  // data is const-ref

foo.Handle(data);    // ok, data is lvalue
foo.Handle(Data{});  // ok, data is rvalue
```

## 移动语义

### 概念

在 C++ 11 强化左值/右值概念后，我们可以针对右值进行优化。于是，C++ 11 中就提出了 **移动语义** (move semantic)：右值对象一般是临时对象；转移该对象时，内部包含的资源 **不需要先复制再删除**，只需要直接 **从旧对象移动到新对象**。

在 C++ 提出移动语义之前，编译器往往可以针对需要移动的对象进行 [sec|拷贝省略] 拷贝省略的优化。由于拷贝省略只是编译器可选的优化项，没有纳入标准，C++ 11 语言标准就引入了移动语义。

### 用途

#### 避免先复制再释放资源

针对一些包含了资源的对象，我们可以通过移动对象的资源进行优化。例如，我们常用的 STL 类模板都有：

- 以左值作为参数的 **拷贝构造函数** (copy constructor)
- 以右值作为参数的 **移动构造函数** (move constructor)

``` cpp
template<typename T> class vector {
 public:
  vector(const vector& rhs); // copy data
  vector(vector&& rhs);      // move data
 private:
  T* data_;
  size_t size_;
};

vector::vector(const vector& rhs) {
  auto &lhs = *this;
  lhs.size_ = rhs.size_;
  lhs.data_ = new T[lhs.size_];
  memcpy(lhs.data_, rhs.data, rhs.size_);  // copy data
}

vector::vector(vector&& rhs) {
  auto &lhs = *this;
  lhs.size_ = rhs.size_;
  lhs.data_ = rhs.data_;  // move data
  rhs.size_ = 0;
  rhs.data_ = nullptr;    // set data of rhs to null
}
```

上述代码中，`std::vector` 根据参数的左右值判断：

- 参数为左值时，拷贝构造，使用 `memcpy` 拷贝原对象的内存
- 参数为右值时，移动构造，把指向原对象内存的指针 `data_`、内存大小 `size_` 复制到新对象，并把原对象这两个成员置 `0`

除了判断参数是否为左右值，我们还可以判断对象本身是否为左右值。例如，对成员函数加入 **引用修饰符** (reference qualifier)，针对对象本身的左右值属性进行优化。

``` cpp
class Foo {
public:
  Data& data() & { return data_; }        // lvalue, ref
  Data data() && { return move(data_); }  // rvalue, move
};

auto ret1 = foo.data();    // foo is lvalue, ref and copy
auto ret2 = Foo{}.data();  // foo is rvalue, move directly
```

#### 转移不可复制的资源

在之前写的 [资源管理小记](Resource-Management.md#资源和对象的映射关系) 提到：如果资源是不可复制的，那么装载资源的对象也应该是不可复制的。

如果资源对象不可复制，那么它就只能通过移动创建新对象。例如，智能指针 `std::unique_ptr` 只允许移动构造，不允许拷贝构造。

``` cpp
template<typename T> class unique_ptr {
 public:
  unique_ptr(const unique_ptr& rhs) = delete;
  unique_ptr(unique_ptr&& rhs);     // move only
 private:
  T* data_;
};

unique_ptr::unique_ptr(unique_ptr&& rhs) {
  auto &lhs = *this;
  rhs.data_ = lhs.data_;
  lhs.data_ = nullptr;
}
```

上述代码中，`std::unique_ptr` 的移动构造过程和 `std::vector` 类似：

- 把指向原对象内存的指针 `data_` 复制到新对象
- 把原对象的指针 `data_` 置为空

### 反例：不遵守移动语义

移动语义只是语言上的一个 **概念**，具体是否移动对象的资源、如何移动对象的资源，都需要通过编写代码 **实现**。而移动语义常常被误认为移动对象本身。

例如，`vector::vector(vector&& rhs)` 也可以不遵守移动语义，执行拷贝语义：

``` cpp
vector::vector(vector&& rhs) {
  auto &lhs = *this;
  lhs.size_ = rhs.size_;
  lhs.data_ = new T[lhs.size_];
  memcpy(lhs.data_, rhs.data, rhs.size_);  // copy data
}
```

那么，一个 `vector` 对象在被 `move` 移动后仍然可用：

```
std::vector<int> v1 { 0, 1, 2, 3 };
auto v2 = std::move(v1);

v1[0] = v2[3];           // ok, but a bit odd
ASSERT(v1[0] != v1[0]);
ASSERT(v1[0] == v1[3]);
```

虽然代码可以那么写，但是在语义上有问题，还违背了移动语义的初衷。

## 拷贝省略

### 概念

尽管有了移动语义，仍然有进一步优化的空间。编译器实现认为，与其调用一次没有太多意义的移动构造函数，不如直接让编译器把内存拷贝过去。于是就有了 [拷贝省略](https://en.cppreference.com/w/cpp/language/copy_elision) (copy elision) 的优化。

很多人会把移动语义和拷贝省略 **混淆**：

- 移动语义 是 **语言标准** 提出的概念，通过编写遵守移动语义的移动构造函数、右值限定成员函数，实现逻辑上优化 **对象内资源** 的转移流程
- 拷贝省略 是非标准的 **编译器优化**，不能通过代码本身控制，由编译器直接将 **整个对象** 在内存上转移

### 示例

在 C++ 14 标准中没有强制编译器支持这个优化，而 C++ 17 标准针对这项优化有了进一步的强制性。C++ 17 提出了 **纯右值** (prvalue, pure rvalue) 的概念，并要求编译器对纯右值进行拷贝省略优化。

``` cpp
Type f() {
  // ...
  throw t;
  // ...
  return t;

  // NRVO from lvalue to ret (not guaranteed)
  // if NRVO is disabled, move ctor is called
}
 
void g(Type arg);

Type v = f();     // copy elision from prvalue (C++ 17)
g(f());           // copy elision from prvalue (C++ 17)
```

构造左值、构造参数时，传入的纯右值可以确保被优化；而返回值不保证被优化。

## 通用引用

> 如果对模板编程不感兴趣，可以直接跳过这个部分。

### 概念

> Item 24: Distinguish universal references from rvalue references. —— Meyer Scott, _Effective Modern C++_

[Meyer Scott 指出](https://isocpp.org/blog/2012/11/universal-references-in-c11-scott-meyers)：有时候符号 `&&` 并不一定代表右值引用，它也可能是左值引用 —— 如果一个被引用符号需要通过左右值属性推导（模板推导或 `auto` 推导），那么这个符号可能是左值或右值 —— 这叫做 **通用引用** (universal reference)。

``` cpp
// rvalue ref: no type deduction
void f1(Widget&& param1);
Widget&& var1 = Widget();
template<typename T> void f2(vector<T>&& param2);

// universal ref: type deduction
auto&& var2 = var1;       
template<typename T> void f3(T&& param);
```

上述代码中，前三个 `&&` 符号不涉及被引用符号的左右值属性推导，是右值引用；而后两个 `&&` 符号会根据实际的 `var1` 类型或 `param` 类型推导左右值。

对于 `var2`：因为 `var1` 是右值引用，所以 `var2` 也是右值引用。对于 `T`，`param` 如果传入右值引用，`T` 是右值引用；如果 `param` 是左值引用，`T` 也是左值引用。

### 用途

C++ 11 引入了变长模板的概念，允许向模板参数里传入不定长的不同的类型。由于每个类型可能是左值或右值，针对所有类型特化不同的模板是不现实的。

假设没有通用引用，即所有的 `&&` 均表示右值引用，模板 `std::make_unique` 至少需要两个重载：

- 对于传入的左值引用 `const Args& ...args`，只要展开 `args...` 就可以转发这一组左值引用了
- 对于传入的右值引用 `Args&& ...args`，需要通过 `std::move` 转发出去，即 `std::move<Args>(args)...`

``` cpp
template<typename T, typename... Args>
unique_ptr<T> make_unique(const Args& ...args) {
  return unique_ptr<T> {
    new T { args... }
  };
}

template<typename T, typename... Args>
unique_ptr<T> make_unique(Args&& ...args) {
  return unique_ptr<T> {
    new T { std::move<Args>(args)... }
  };
}
```

为什么要用 `std::move` 转发：

- 对于右值引用参数，传入函数体后，再继续传入另一个函数时，会变成左值。（在函数体内，这个值可以被取地址、被赋值）
- 如果需要保持原来的右值属性，需要通过 `std::move` 转变回到右值属性，再转发出去

上述代码存在一个问题：如果传入的 `...args` 既有右值又有左值，那么这两个模板都不能正确匹配。为了解决这个问题，C++ 11 同时引入了通用引用的概念，用于推导不确定引用的左右值属性。

有了通用引用的概念，模板 `std::make_unique` 只需要一个重载：

``` cpp
template<typename T, typename... Args>
unique_ptr<T> make_unique(Args&& ...args) {
  return unique_ptr<T> {
    new T { std::forward<Args>(args)... }
  };
}
```

上述代码使用了 `std::forward` 实现参数的 **完美转发** (perfect forwarding)：

- 如果参数传入前是左值，我们要以左值的形式转发到下一个函数
- 如果参数传入前是右值，我们要先还原为右值的形式（使用 `std::move`），再转发到下一个函数

而 `std::forward` 的实现原理比较简单：还原 `&&` 传入前的类型，对左值直接返回左值引用，对右值通过 `std::move` 变为右值引用。

## 写在最后 [no-number]

虽然这些东西你不知道，也不会伤害你；但如果你知道了，就可以合理利用，从而提升编码效率，避免不必要的问题。

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2018, BOT Man
