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

### 错误：返回前，移动临时值

> [ES.56: Write `std::move()` only when you need to explicitly move an object to another scope](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Res-move)

``` cpp
std::string base_url  = tag->GetBaseUrl();
if (!base_url.empty())
  UpdateQueryUrl(std::move(base_url) + "&q=" + word_);
else
  UpdateQueryUrl(default_base_url_ + "&q=" + word_);
```

上述代码的问题在于：对临时值使用 `std::move`，会导致后续代码不能使用 `base_url`；如果使用，会出现 **未定义行为** _(undefined behavior)_。（参考：[`basic_string(basic_string&&)`](https://en.cppreference.com/w/cpp/string/basic_string/basic_string)）

正确做法是封装成一个函数，只在返回值使用 `std::move`（这也符合函数式思想，尽可能减少临时值）：

``` cpp
std::string GetQueryUrlString(const Tag* tag) {
  std::string base_url  = tag->GetBaseUrl();
  if (!base_url.empty())
    return std::move(base_url) + "&q=" + word_;
  else
    return default_base_url_ + "&q=" + word_;
}

// ...
UpdateQueryUrl(GetQueryUrlString(tag));
```

### 误解：被移动的对象不能再使用

> [C.64: A move operation should move and leave its source in a valid state](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rc-move-semantic)

``` cpp
auto p = std::make_unique<int>(1);
auto q = std::move(p);
assert(p == nullptr);  // OK: use after move

p.reset(new int{2});   // or p = std::make_unique<int>(2);
assert(*p == 1);       // OK: realive now
```

很多人会认为：被移动后的对象会进入一个 **非法状态** _(invalid state)_，从而不能再使用。实际上，C++ 标准要求对象被移动后，进入一个 **合法但未指定状态** _(“valid but unspecified” state)_ —— 即调用该对象的方法不会出现异常。处于这个状态的对象：

- （基本要求）能正确析构（不会释放被移动的资源，例如 `unique_ptr::~unique_ptr` 检查指针的有效性）
- （一般要求）重新赋值后，和新的对象没有差别（C++ 标准库基于这个假设）
- （更高要求）恢复为默认值（例如 `unique_ptr` 恢复为 `unique_ptr{}`）

关于移动语义的实现原理，参考 [sec|移动语义] 代码。

### 误解：返回时，不移动右值引用参数

> [F.18: For “will-move-from” parameters, pass by `X&&` and `std::move` the parameter](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-consume)

``` cpp
std::unique_ptr<int> foo(std::unique_ptr<int>&& val) {
  //...
  return val;    // not compiled, -> return std::move(val);
}
```

上述代码的问题在于：没有对返回值使用 `std::move`，提示 `unique_ptr(const unique_ptr&) = delete` 不能编译。

因为不论 **左值引用** 还是 **右值引用** 的参数，传入函数体后，都会 **退化为左值引用**（在函数体内，这个引用的值可以被取地址、被赋值）；而在函数体内使用 **传入的右值引用** 时，如果不把引用 **强制还原**，就只能退化为左值引用继续使用。所以，返回右值引用时，需要使用 `std::move` 显式移动。

在 [sec|完美转发] 完美转发部分，详细描述了如何合理的保持引用的左右值属性。

### 误解：返回时，移动临时值

> [F.48: Don’t `return std::move(local)`](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-return-move-local)

``` cpp
std::unique_ptr<int> bar() {
  auto ret = std::make_unique<int>(64);
  //...
  return std::move(ret);  // -> return ret;
}
```

上述代码的问题在于：没必要使用 `std::move` 移动返回的临时值。

和传入的引用参数不一样，C++ 会把 **临时值** 当成 **右值** 返回，通过右值引用进行 [sec|移动语义] 移动构造（语言标准）；如果编译器允许 [sec|拷贝省略] 拷贝省略，还可以省略这一步的构造，直接移动内存（编译器优化）。

## 右值引用

> 如果已经了解了左右值引用基本概念，可以直接跳过这个部分。

C++ 11 强化了左右值的概念：**左值** (lvalue, left value) 指的是可以通过变量名/指针/引用使用的值，**右值** (rvalue, right value) 指的是表达式返回的临时值。区分左右值的一个不完全正确的方法是：左值可以 **取地址**，而右值不行；另一个从概念上的区分方法是：可以给左值 **赋值**，但右值不行。

C++ 11 还引入了右值引用的概念：**左值引用** (l-ref, lvalue reference) 是指通过 `&` 符号引用左值对象；**右值引用** (r-ref, rvalue reference) 是指通过 `&&` 符号引用右值对象。而一旦 **右值引用被初始化** 后，就会立即 **退化成左值引用**（这个引用可以被取地址、被赋值；参考 [sec|误解：返回时，不移动右值引用参数] _误解：返回时，不移动右值引用参数_）。

``` cpp
void f(Data&  data);  // 1, data is l-ref
void f(Data&& data);  // 2, data is r-ref

Data   data;
Data&  data1 = data;
Data&& data2 = data;

f(data);    // 1, data is lvalue
f(Data{});  // 2, data is rvalue
f(data1);   // 1, data1 is l-ref
f(data2);   // 1, data2 degenerates to l-ref
```

另外，C++ 还允许 **常引用** (c-ref, const reference) 作为参数的重载，同时接受左右值参数：

``` cpp
void f(const Data& data);  // data is c-ref

f(data);    // ok, data is lvalue
f(Data{});  // ok, data is rvalue
```

## 移动语义

在 C++ 11 强化左右值概念后，我们可以针对右值进行优化。于是，C++ 11 中就提出了 **移动语义** (move semantic)：右值对象一般是临时对象；转移该对象时，内部包含的资源 **不需要先复制再删除**，只需要直接 **从旧对象移动到新对象**。

在 C++ 提出移动语义之前，编译器往往可以针对需要移动的对象进行 [sec|拷贝省略] 拷贝省略的优化。由于拷贝省略只是编译器可选的优化项，没有纳入标准，C++ 11 语言标准就引入了移动语义。

### 避免先复制再释放资源

针对一些包含了资源的对象，我们可以通过移动对象的资源进行优化。例如，我们常用的 STL 类模板都有：

- 以常引用作为参数的 **拷贝构造函数** (copy constructor)
- 以右值引用作为参数的 **移动构造函数** (move constructor)

``` cpp
template<typename T> class vector {
 public:
  vector(const vector& rhs); // copy data
  vector(vector&& rhs);      // move data
  ~vector();                 // dtor
 private:
  T* data_ = nullptr;
  size_t size_ = 0;
};

vector::vector(const vector& rhs) {
  auto &lhs = *this;
  lhs.size_ = rhs.size_;
  lhs.data_ = new T[rhs.size_];
  memcpy(lhs.data_, rhs.data, rhs.size_);  // copy data
}

vector::vector(vector&& rhs) {
  auto &lhs = *this;
  lhs.size_ = rhs.size_;
  lhs.data_ = rhs.data_;  // move data
  rhs.size_ = 0;
  rhs.data_ = nullptr;    // set data of rhs to null
}

vector::~vector() {
  if (data_)              // release only if owned
    delete[] data_;
}
```

上述代码中，构造函数 `vector::vector` 根据参数的左右值属性判断：

- 参数为左值时，拷贝构造，使用 `memcpy` 拷贝原对象的内存
- 参数为右值时，移动构造，把指向原对象内存的指针 `data_`、内存大小 `size_` 复制到新对象，并把原对象这两个成员置 `0`

析构函数 `vector::~vector` 检查 data_ 是否有效，决定是否需要释放资源。

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

### 转移不可复制的资源

在之前写的 [资源管理小记](Resource-Management.md#资源和对象的映射关系) 提到：如果资源是不可复制的，那么装载资源的对象也应该是不可复制的。

如果资源对象不可复制，那么它就只能通过移动创建新对象。例如，智能指针 `std::unique_ptr` 只允许移动构造，不允许拷贝构造。

``` cpp
template<typename T> class unique_ptr {
 public:
  unique_ptr(const unique_ptr& rhs) = delete;
  unique_ptr(unique_ptr&& rhs);  // move only
 private:
  T* data_ = nullptr;
};

unique_ptr::unique_ptr(unique_ptr&& rhs) {
  auto &lhs = *this;
  lhs.data_ = rhs.data_;
  rhs.data_ = nullptr;
}
```

上述代码中，`unique_ptr` 的移动构造过程和 `vector` 类似：

- 把指向原对象内存的指针 `data_` 复制到新对象
- 把原对象的指针 `data_` 置为空

### 反例：不遵守移动语义

移动语义只是语言上的一个 **概念**，具体是否移动对象的资源、如何移动对象的资源，都需要通过编写代码 **实现**。而移动语义常常被误认为，编译器自动生成移动对象本身的代码（[sec|拷贝省略] 拷贝省略）。

为了证明这一点，我们可以实现不遵守移动语义的 `bad_vector::bad_vector(bad_vector&& rhs)`，执行拷贝语义：

``` cpp
bad_vector::bad_vector(bad_vector&& rhs) {
  auto &lhs = *this;
  lhs.size_ = rhs.size_;
  lhs.data_ = new T[rhs.size_];
  memcpy(lhs.data_, rhs.data, rhs.size_);  // copy data
}
```

那么，一个 `bad_vector` 对象在被 `move` 移动后仍然可用：

``` cpp
bad_vector<int> v1 { 0, 1, 2, 3 };
auto v2 = std::move(v1);

v1[0] = v2[3];           // ok, but a bit odd
assert(v1[0] != v1[0]);
assert(v1[0] == v1[3]);
```

虽然代码可以那么写，但是在语义上有问题，还违背了移动语义的初衷。

### 拷贝省略

尽管有了移动语义，仍然有进一步优化的空间。编译器实现认为，与其调用一次没有太多意义的移动构造函数，不如直接让编译器把对象本身的内存拷贝过去。于是就有了 [拷贝省略](https://en.cppreference.com/w/cpp/language/copy_elision) (copy elision) 的优化。

很多人会把移动语义和拷贝省略 **混淆**：

- 移动语义是 **语言标准** 提出的概念，通过编写遵守移动语义的移动构造函数、右值限定成员函数，**逻辑上** 实现优化 **对象内资源** 的转移流程
- 拷贝省略是非标准的 **编译器优化**，跳过移动/拷贝构造函数，让编译器直接移动 **整个对象** 的内存

在 C++ 14 标准中没有强制编译器支持这个优化，而 C++ 17 标准针对这项优化有了进一步的强制性。C++ 17 提出了 **纯右值** (prvalue, pure rvalue) 的概念，并要求编译器对纯右值进行拷贝省略优化。（[参考](https://jonasdevlieghere.com/guaranteed-copy-elision/)）

``` cpp
Data f() {
  Data val;
  // ...
  throw val;
  // ...
  return val;

  // NRVO from lvalue to ret (not guaranteed)
  // if NRVO is disabled, move ctor is called
}
 
void g(Date arg);

Data v = f();     // copy elision from prvalue (C++ 17)
g(f());           // copy elision from prvalue (C++ 17)
```

构造左值、构造参数时，传入的纯右值可以确保被优化；而返回值不保证被优化。

## 通用引用

> 如果对模板编程不感兴趣，可以直接跳过这个部分。

### 概念

> Item 24: Distinguish universal references from rvalue references. —— Meyer Scott, _Effective Modern C++_

[Meyer Scott 指出](https://isocpp.org/blog/2012/11/universal-references-in-c11-scott-meyers)：有时候符号 `&&` 并不一定代表右值引用，它也可能是左值引用 —— 如果一个引用符号需要通过左右值属性推导（模板推导或 `auto` 推导），那么这个符号可能是左值引用或右值引用 —— 这叫做 **通用引用** (universal reference)。

``` cpp
// rvalue ref: no type deduction
void f1(Widget&& param1);
Widget&& var1 = Widget();
template<typename T> void f2(vector<T>&& param2);

// universal ref: type deduction
auto&& var2 = var1;       
template<typename T> void f3(T&& param);
```

上述代码中，前三个 `&&` 符号不涉及引用符号的左右值属性推导，是右值引用；而后两个 `&&` 符号会根据初始值推导左右值属性：

- 对于 `var2`：因为 `var1` 是右值引用，所以 `var2` 也是右值引用
- 对于 `T&&`，如果 `param` 传入右值引用，`T&&` 是右值引用 `&&`；如果传入左值引用，`T&&` 也是左值引用 `&`。

### 变长引用参数模板

C++ 11 引入了变长模板的概念，允许向模板参数里传入不同类型的不定长引用。由于每个类型可能是左值引用或右值引用；针对所有可能的左右值引用组合，特化所有模板是不现实的。

假设没有通用引用的概念，即所有的 `&&` 均表示右值引用，模板 `std::make_unique` 至少需要两个重载：

- 对于传入的左值引用 `const Args& ...args`，只要展开 `args...` 就可以转发这一组左值引用
- 对于传入的右值引用 `Args&& ...args`，需要通过 `std::move` 转发出去，即 `std::move<Args>(args)...`

> 为什么要用 `std::move` 转发：见 [sec|误解：返回时，不移动右值引用参数] _误解：返回时，不移动右值引用参数_。

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

上述代码存在一个问题：如果传入的 `...args` 既有左值引用又有右值引用，那么这两个模板都不匹配。为了解决这个问题，C++ 11 同时引入了通用引用的概念，用于推导引用的不确定左右值属性。

引入了通用引用的概念，模板 `std::make_unique` 只需要一个重载：

``` cpp
template<typename T, typename... Args>
unique_ptr<T> make_unique(Args&& ...args) {
  return unique_ptr<T> {
    new T { std::forward<Args>(args)... }
  };
}
```

### 完美转发

上述代码使用了 `std::forward` 实现参数的 **完美转发** (perfect forwarding)：

- 如果参数传入前是 **左值引用**，我们要以 **左值引用** 的形式转发到下一个函数
- 如果参数传入前是 **右值引用**，我们要先还原为 **右值引用** 的形式（使用 `std::move`），再转发到下一个函数

而 `std::forward` 的实现原理比较简单：利用模板重载的方法，构造不涉及左右值属性推导的两个重载：

- 对 **左值引用** 可以根据需要返回 **左值引用** 或 **右值引用**
- 对 **右值引用** 仅返回 **右值引用**

``` cpp
template <typename T>
T&& forward(std::remove_reference_t<T>& val) {
  // forward lvalue as either lvalue or rvalue
  return (static_cast<T&&>(val));
}

template <typename T>
T&& forward(std::remove_reference_t<T>&& val) {
  // forward rvalue as rvalue
  static_assert(!std::is_lvalue_reference_v<T>, "bad forward");
  return (static_cast<T&&>(val));
}
```

## 写在最后 [no-number]

虽然这些东西你不知道，也不会伤害你；但如果你知道了，就可以合理利用，从而提升 C++ 的使用效率，避免不必要的问题。

感谢 [@flythief](https://github.com/thiefuniverse) 提出的修改意见~

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2018, BOT Man
