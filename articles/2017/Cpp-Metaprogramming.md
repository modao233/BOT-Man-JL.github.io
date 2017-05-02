# C++ 元编程 —— 让编译器帮你写程序

> 2017/5/2
>
> 本文是对 C++ 元编程 相关内容的理解，为 2017 程序设计语言结课论文。

[heading-numbering]

## 目录

[TOC]

## C++ 中的元编程

### 什么是元编程

**元编程** _(metaprogramming)_ 是一种操作 **程序实体** _(program entity)_ （例如，类 或者 函数）的程序设计方法，用于在编译时计算出 运行时需要的类型和函数。

一般的编程是通过直接编写 **程序** _(program)_，通过编译器 **编译** _(compile)_，产生目标代码，并用于 **运行时** _(run time)_ 执行。与普通的编程不同，元编程则是借助语言提供的 **模板** _(template)_ 机制，通过编译器 **推导** (deduce)，在 **编译时** _(compile time)_ 生成程序。元编程经过编译器推导得到的程序，再进一步通过编译器编译，产生最终的目标代码。

因此，元编程又被成为 **两级编程** _(two-level programming)_，**生成式编程** _(generative programming)_ 或 **模板元编程** (template metaprogramming)。[cpp-pl]

### 元编程在 C++ 中的位置

C++ 是作为一个高级程序设计语言，除了有 C 语言的相关功能外，还有两大 **抽象机制** _(abstraction mechanisms)_：**面向对象编程** _(object-oriented programming)_ 和 **模板编程** _(generic programming)_。[cpp-pl]

为了实现面向对象编程，C++ 提供了 **类** _(class)_ —— 一种利用基本类型建立新类型的工具。通过类，我们可以构造出利用抽象的基本数据类型，构造出真实世界中的数据类型。而在模板编程方面，在 C++ 中通过提供 **模板** 的支持，以一种直观的方式，来表示 **通用概念** _(general concept)_。

模板编程在 C++ 中分为两种：**泛型编程** _(generic programming)_ 和 **元编程** _(meta-programming)_。前者注重于编写通用的 **类型** _(type)_ 或 **算法** _(algorithm)_，以及设计他们的接口，不需要关注编译器如何推导生成的代码；而后者注重于设计模板推导过程中的 **选择** _(selection)_ 和 **迭代** _(iteration)_，更接近于普通的编程。[cpp-pl]

### C++ 中的模板

模板的概念最早由 David R. Musser 和 Alexander A. Stepanov 提出 [generic-programming]，并最早应用于 C++ 程序设计语言。其设计初衷是用于泛型编程，进行代码生成。Alexander A. Stepanov 在 C++ 之父的邀请下，参与了 C++ **标准模板库** _(STL)_ （简称 标准库） 的设计。[cpp-evo]

假设给定 $M$ 个已有的 **数据类型** _(data type)_，我们需要为每个数据类型实现 $N$ 个具有相似功能的 类 或 函数。如果不使用模板，就像 C 语言那样，我们需要编写 $M \times N$ 份代码，即分别为每种数据类型编写 $N$ 个具有相似功能的 类 或 函数；如果使用了模板，我们仅需要编写 $N$ 份代码，即编写 $N$ 个独立于具体数据类型的模板。

而到了现代 C++ 模板也被用于元编程中。例如，K. Czarnecki 和 U. Eisenecker 提出了利用纯模板实现 Lisp 解释器的方法。[gererative-programming] 利用元编程，我们可以很方便的编写 **类型安全** _(type-safe)_、**运行时高效** _(runtime high)_ 的代码。

目前最新的 C++ 标准（C++ 17）将模板分成了 4 类 [cppref-template]：

- 类模板 _(class template)_
- 函数模板 _(function template)_
- 别名模板 _(alias template)_
- 变量模板 _(variable template)_

其中，类模板 和 函数模板 在 C++ 的早期版本已经引入，而 别名模板 和 变量模板 则分别在 C++ 11 和 C++ 14 引入。

#### 类模板

**类模板** 用于定义一类具有相似功能的类，属于是泛型中对 **类型** 的抽象，在标准库中的 **容器** _(container)_ 里广泛使用。例如，我们经常使用的 `std::vector` 就是一个类模板。

当我们构造 `std::vector<int>` 时，编译器为我们生成了一个针对于 `int` 类型的具有 `vector` 操作（例如，尾部插入、下标访问）的类，命名为 `vector_int`；当我们构造 `std::vector<double>` 时，类似的，编译器为我们生成了一个针对于 `double` 类型的具有 `vector` 操作的类，命名为 `vector_double`。（命名因编译器而异）这就相当于我们写了一份具有 `vector` 操作的代码（模板），在使用的时候，编译器为我们生成了不同类型的多份代码。

#### 函数模板

**函数模板** 用于定义一类具有相似功能的函数，属于是泛型中对 **算法** 的抽象，在标准库中的 **函数** _(function)_ 里广泛使用。例如，我们可以通过使用 `std::max` 得到两个元素的最大值，它的一个重载可以通过以下代码实现：

``` cpp
template <typename T>
const T &max (const T &a, const T &b)
{
    return (a < b) ? b : a;
}
```

其中，`typename T` 是函数模板的 **模板参数**，属于是 **类型参数** _(type parameter)_。在函数定义的部分，使用了类型 `T`：函数的参数和返回值的类型都是 `const T &`。在编写代码时，我们不需要知道这个类型 `T` 是什么，只需要直到它可以重载运算符 `<` 就足够了。

当我们调用 `max (1, 0)` 时，参数是 `int` 类型，它就会推导出函数返回值为 `const int &`，并生成代码：

``` cpp
const int &max (const int &a, const int &b)
{
    return (a < b) ? b : a;
}
```

当我们调用 `max (1.0, 0.0)` 时，参数是 `double` 类型，它就会推导出函数返回值为 `const double &`，并生成代码：

``` cpp
const double &max (const double &a, const double &b)
{
    return (a < b) ? b : a;
}
```

同样的，我们就可以通过编写一份代码，得到了不同的类型数据的实现。

#### 别名模板

**别名模板** 提供了一种支持参数传递的设置别名的方法。实际的应用中，它能提供复杂模板类或模板函数的简化版本。例如，标准库里的 `std::string` 表示的是类型为 `char` 的串；而如果我们需要不同类型的串时，可以通过把类型 `T` 和 `T` 对应的 `char_traits` 传入 `std::basic_string<T, TRAITS>` 得到。如果我们不想每次都传入 `TRAITS`，则可以将它设为默认（标准库已实现）：

``` cpp
template <typename T>
using mystring = std::basic_string<T, std::char_traits<T>>;
```

在使用的时候，`mystring<wchar_t>` 就等价于 `std::basic_string<wchar_t, std::char_traits<wchar_t>>` 了。

#### 变量模板

**变量模板** 是 C++ 14 标准中新加入的特性，它相当于是一种具有模板特性的常量定义方法。例如，下面的代码可以在编译时获得不同类型的 $\pi$ 的值，即 `pi<int>` 对应 `3`，`pi<double>` 对应 `3.14159`（因编译器而异）。

``` cpp
template <class T>
constexpr T pi = T(3.1415926535897932385);
```

## 如何元编程

元编程已被广泛的应用于现代 C++ 的程序设计中。BOT Man 利用元编程设计了一个 **对象关系映射** _(object-relation mapping, ORM)_。[naive-orm] [better-orm]

和普通编程不一样，C++ 的模板机制仅仅提供了 **纯函数** _(pure functional)_ 的方法，即不支持变量，而且所有的模板推导必须能在编译时完成。这些限制使得元编程的难度大大增加，但是我们可以使用 **编译时测试** _(compile-time test)_ 和 **编译时迭代** _(compile-time iteration)_ 的方法进行模板推导。

**编译时测试** 相当于面向过程编程中的 **选择语句** _(selection statement)_（`if-else`, `switch`），而 **编译时迭代** 相当于面向过程编程中的 **迭代语句** _(iteration statement)_（`for`, `while`, `do`）。由于 C++ 中的模板被证明是图灵完备的 [template-turing-complete]，我们可以通过编写模板演算规则，实现元编程。

### 编译时测试

### 编译时迭代

## C++ 元编程的不足

### 调试代码 (debugging)

- 实例化
- 动态调试

### 代码膨胀 (code bloat)

- 死代码 (dead code) 链接时优化
- 冗余运算 (redundant calculations) 设计时优化

### 缺少编程时检查 (programming-time checking)

- 概念 (concept)
- 其他语言的接口 (interface)

## 总结

## 参考文献

- [cpp-pl]: Bjarne Stroustrup. The C++ Programming Language (Fourth Edition) [M] _Addison-Wesley_, 2013.
- [generic-programming]: David R. Musser, Alexander A. Stepanov. Generic Programming [C] // P. Gianni. In _Symbolic and Algebraic Computation: International symposium ISSAC_, 1988: 13–25.
- [cpp-evo]: Bjarne Stroustrup: The Design and Evolution of C++ [M] _Addison-Wesley_, 1994.
- [gererative-programming]: K. Czarnecki, U. Eisenecker. Generative Programming: Methods, Tools,
and Applications [M] _Addison-Wesley_, 2000.
- [cppref-template]: cppreference.com. Templates. http://en.cppreference.com/w/cpp/language/templates.
- [naive-orm]: BOT Man JL. 如何设计一个简单的 C++ ORM. https://bot-man-jl.github.io/articles/?post=2016/How-to-Design-a-Naive-Cpp-ORM
- [better-orm]: BOT Man JL. 如何设计一个更好的 C++ ORM. https://bot-man-jl.github.io/articles/?post=2016/How-to-Design-a-Better-Cpp-ORM
- [template-turing-complete]: Todd L. Veldhuizen. C++ Templates are Turing Complete [R] Indiana University Computer Science Technical Report. 2003.