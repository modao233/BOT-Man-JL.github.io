# C++ 元编程 —— 让编译器帮你写程序

> 2017/5/2
>
> 本文是对 C++ 元编程 相关内容的理解，为 2017 程序设计语言结课论文。

[heading-numbering]

## 目录

[TOC]

## C++ 中的元编程

### 什么是元编程

**元编程** _(metaprogramming)_ 通过操作 **程序实体** _(program entity)_，在 **编译时** _(compile time)_ 计算出 **运行时** _(run time)_ 需要的类型和函数。

一般的编程是通过直接编写 **程序** _(program)_，通过编译器 **编译** _(compile)_，产生目标代码，并用于 **运行时** 执行。与普通的编程不同，元编程则是借助语言提供的 **模板** _(template)_ 机制，通过编译器 **推导** (deduce)，在 **编译时** 生成程序。元编程经过编译器推导得到的程序，再进一步通过编译器编译，产生最终的目标代码。在 [sec|测试类型] 中，我们用了一个例子说明了两者的区别。

因此，元编程又被成为 **两级编程** _(two-level programming)_，**生成式编程** _(generative programming)_ 或 **模板元编程** (template metaprogramming)。[cpp-pl]

### 元编程在 C++ 中的位置

> C++ 语言 = C 语言的超集 + 抽象机制 + 标准库

目前 C++ 的 **抽象机制** _(abstraction mechanisms)_ 主要有两种：**面向对象编程** _(object-oriented programming)_ 和 **模板编程** _(generic programming)_。[cpp-pl]

为了实现面向对象编程，C++ 提供了 **类** _(class)_ —— 一种利用基本类型建立新类型的工具。通过类，我们可以构造出利用抽象的基本数据类型，构造出真实世界中的数据类型。而在模板编程方面，在 C++ 中通过提供 **模板** 的支持，以一种直观的方式，来表示 **通用概念** _(general concept)_。

模板编程在 C++ 中分为两种：**泛型编程** _(generic programming)_ 和 **元编程** _(meta-programming)_。前者注重于编写通用的 **类型** _(type)_ 或 **算法** _(algorithm)_，以及设计他们的接口，不需要关注编译器如何推导生成的代码；而后者注重于设计模板推导过程中的 **选择** _(selection)_ 和 **迭代** _(iteration)_，更接近于普通的编程。[cpp-pl]

### C++ 元编程的历史

C++ 中的元编程是基于模板的。模板的概念最早在 1988 年，由 David R. Musser 和 Alexander A. Stepanov 提出 [generic-programming]，并最早应用于 C++ 程序设计语言。Alexander A. Stepanov 在 C++ 之父的邀请下，参与了 C++ **标准模板库** _(STL)_ （简称 标准库） 的设计。[cpp-evo]

模板的设计初衷是用于泛型编程，进行代码生成。假设给定 $M$ 个已有的 **数据类型** _(data type)_，我们需要为每个数据类型实现 $N$ 个具有相似功能的 类 或 函数。如果不使用模板，就像 C 语言那样，我们需要编写 $M \times N$ 份代码，即分别为每种数据类型编写 $N$ 个具有相似功能的 类 或 函数；如果使用了模板，我们仅需要编写 $N$ 份代码，即编写 $N$ 个独立于具体数据类型的模板。

而到了现代 C++，人们发现模板也被用于元编程中。1994 年的圣地亚哥 C++ 标准委员会会议中，Erwin Unruh 演示了一段 编译时通过错误信息计算素数 的代码。[calc-prime] 1995 年的 Todd Veldhuizen 在 C++ Report 会议上，首次提出了 C++ **模板元编程** 的概念。[using-cpp-tmp] 之后，K. Czarnecki 和 U. Eisenecker 对元编程深入研究，提出了利用纯模板实现 Lisp 解释器的方法。[gererative-programming]

Andrei Alexandrescu 深入研究了模板元编程的相关内容，并设计了一个 C++ 的模板元编程库 —— Loki。[modern-cpp-design] 受限于 C++ 对模板本身的限制，Andrei Alexandrescu 等人有发明了 D 语言，并在 D 语言中优化了元编程的特性。[d-lang]

元编程已被广泛的应用于现代 C++ 的程序设计中。由于元编程不同于一般的编程，在程序的设计上更具有挑战性，所以收到了许多学者和工程师的广泛关注。

## 元编程的语言支持

### C++ 中的模板类型

元编程完全依赖于 C++ 中的模板机制。目前最新的 C++ 标准（C++ 17）将模板分成了 4 类 [cppref-template]：

- 类模板 _(class template)_
- 函数模板 _(function template)_
- 别名模板 _(alias template)_
- 变量模板 _(variable template)_

其中，类模板 和 函数模板 在 C++ 的早期版本已经引入，而 别名模板 和 变量模板 则分别在 C++ 11 和 C++ 14 引入。前两者能产生新的类型，属于 **类型构造器** _(type constructor)_；而后两者仅是语言提供的简化记法，属于 **语法糖** _(syntactic sugar)_。

#### 类模板

**类模板** 用于定义一类具有相似功能的类，属于是泛型中对 **类型** 的抽象，在标准库中的 **容器** _(container)_ 里广泛使用。例如，我们经常使用的 `std::vector` 就是一个类模板。

当我们构造 `std::vector<int>` 时，编译器为我们生成了一个针对于 `int` 类型的具有 `vector` 操作（例如，尾部插入、下标访问）的类，命名为 `vector_int`；当我们构造 `std::vector<double>` 时，类似的，编译器为我们生成了一个针对于 `double` 类型的具有 `vector` 操作的类，命名为 `vector_double`。（命名因编译器而异）这就相当于我们写了一份具有 `vector` 操作的代码（模板），在使用的时候，编译器为我们生成了不同类型的多份代码。

#### 函数模板

**函数模板** 用于定义一类具有相似功能的函数，属于是泛型中对 **算法** 的抽象，在标准库中的 **函数** _(function)_ 里广泛使用。例如，我们可以通过使用 `std::max` 得到两个元素的最大值，它的一个重载可以通过代码 [code|max-template] 实现。

[code|&max-template]

``` cpp
template <typename T>
const T &max (const T &a, const T &b) {
    return (a < b) ? b : a;
}
```

[align-center]

代码 [code||max-template] - `std::max` 模板实现

其中，`typename T` 是函数模板的 **模板参数**，属于是 **类型参数** _(type parameter)_。在函数定义的部分，使用了类型 `T`：函数的参数和返回值的类型都是 `const T &`。在编写代码时，我们不需要知道这个类型 `T` 是什么，只需要直到它可以重载运算符 `<` 就足够了。

#### 别名模板

**别名模板** 提供了一种支持参数传递的设置 **类型别名** 的方法。实际的应用中，它能提供复杂模板的类型的简化写法，并不产生新的类型或算法。

例如，每个不同的标准库的容器，有着不同类型的 **迭代器** _(iterator)_；而这些迭代器的类型被定义在每个容器中 —— `std::vector<T>` 的迭代器的类型是 `std::vector<T>::iterator`。在 C++ 11 之前，如果要声明或定义一个 `std::vector<int>` 的迭代器，我们需要写 `std::vector<int>::iterator`；如果是 `std::vector<double>` 的迭代器，又需要写 `std::vector<double>::iterator`。

为了化简这个记法，C++ 11 引入了别名模板的概念。通过代码 [code|alias-template] 的模板，可以用 `VecIter<int>` 代替 `std::vector<int>::iterator`，用 `VecIter<double>` 代替 `std::vector<double>::iterator`。

[code|&alias-template]

``` cpp
template <typename T>
using VecIter = std::vector<T>::iterator;
```

[align-center]

代码 [code||alias-template] - 别名模板实现 `std::vector<T>` 迭代器

#### 变量模板

**变量模板** 是 C++ 14 标准中新加入的特性，它相当于是一种具有模板特性的 **常量** 定义方法。而这种常量模板可以通过 类模板中的静态数据成员 或 函数模板的返回值 实现。

例如，通过代码 [code|variable-template]，可以在编译时获得不同类型的 $\pi$ 的值，即 `pi<int>` 对应 `3`，`pi<double>` 对应 `3.14159`（因编译器而异）。

[code|&variable-template]

``` cpp
template <typename T>
constexpr T pi = T(3.1415926535897932385);
```

[align-center]

代码 [code||variable-template] - 不同类型的常量 $\pi$

### C++ 中的模板参数

#### 模板参数的分类

C++ 中的 **模板参数** _(template parameter / argument)_ 可以分为三种：非类型参数，类型参数，模板参数。其中，类型和模板 作为 **实参** _(argument)_ 传入时，已经可以确定参数的内容；而非类型的 **形参** _(parameter)_ 仅接受编译时确定的值（常量表达式）。而对于模板参数也可以当作一般的类型参数进行传递（模板也是一个类型），但单独把它提出来，可以实现对参数的参数匹配。代码 [code|template-param] 展示了这三类模板参数的各种形式。

[code|&template-param]

``` cpp
template <Type Val> ...
template <Type Val = Default> ...
template <Type... Vals> ...
template <auto Val> ...  // C++ 17

template <typename|class T> ...
template <typename|class T = Default> ...
template <typename|class... Ts> ...

template <template<Args> typename C> ...
template <template<Args> typename C = Default> ...
template <template<Args> typename... Cs> ...
```

[align-center]

代码 [code||template-param] - 三类模板参数

其中，`Type` 是一个已知的给定类型，`Args` 是一系类已知的给定类型，`Default` 是三种模板参数的默认值，`...` 表示变长模板（[sec|变长模板]）。

#### 变长模板

在许多应用场景中，模板接受的参数可能是不定长的。所以，从 C++ 11 开始，C++ 支持了 **变长模板** _(variadic template)_。变长模板的参数叫做 **参数包** _(parameter pack)_，用 `...` 表示，可以接受 `0` 个或多个参数。代码 [code|variadic-template] 展示了如何利用变长模板，实现打印所有参数，并在参数之间加空格的功能。

[code|&variadic-template]

``` cpp
template<typename T, typename... Ts>
void PrintAll (T arg, Ts... args) {
    std::cout << arg;
    int dummy[sizeof...(Ts) + 1] = { 0,
        (std::cout << " " << args, 0)... };
    std::cout << std::endl;
}

PrintAll (0);  // one param
PrintAll (1, .1, true, "str");  // multi params

// Print:
// 0
// 1 .1 true str
```

[align-center]

代码 [code||variadic-template] - 变长模板打印参数（直接展开）

#### 模板特化

**模板特化** _(template specialization)_ 是重载 给定了全部模板参数取值的模板代码（完全特化）或 给定了部分模板参数取值的模板代码（部分特化）的方法。C++ 规定，对所有模板支持完全特化；而仅有 **类模板** 支持部分特化。（函数模板的部分特化可以通过函数的重载实现）

在标准库的实现中，模板特化已被广泛的应用。例如，部分 `std::vector` 的实现对 `T *` 和 `void *` 进行了特化（代码 [code|spec-vector]）；然后将所有的 `T *` 的实现 **继承** 到 `void *` 的实现上，并在公开的函数里通过强制类型转换，进行 `void *` 和 `T *` 的交互；最后这使得所有的指针的 `std::vector` 就可以共享同一份实现了，从而缩小了最终代码的体积。[cpp-pl]

[code|&spec-vector]

``` cpp
template <typename T> class vector;  // general
template <typename T> class vector<T *>;  // partial spec
template <> class vector<void *>;  // complete spec

template <typename T>
class vector<T *> : private vector<void *> { ... }
```

[align-center]

代码 [code||spec-vector] - `std::vector` 的特化

模板的特化，一方面常常被用于实现元编程的逻辑演算（[sec|元编程的演算规则]）；另一方面可以用于实现 **编译时多态** _(compile-time polymorphism)_。

## 元编程的演算规则

和普通编程不一样，C++ 的模板机制仅仅提供了 **纯函数** _(pure functional)_ 的方法，即不支持变量，而且所有的模板推导必须能在编译时完成。这些限制使得元编程的难度大大增加，但是我们可以使用 **编译时测试** _(compile-time test)_ 和 **编译时迭代** _(compile-time iteration)_ 的方法进行模板推导。由于 C++ 中的模板是图灵完备的 [template-turing-complete]，我们可以利用这两种编译时 **演算规则** _(calculus rule)_，实现完整的元编程。

### 编译时测试

**编译时测试** 相当于面向过程编程中的 **选择语句** _(selection statement)_，可以实现 `if-else` / `switch` 的逻辑。编译时测试的对象有两种：**常量表达式** _(constexpr)_ 和 **类型**。而后者的实现则是通过前者的测试完成。

#### 测试常量表达式

类似于 **静态断言** `static_assert`，编译时测试的对象可以是常量表达式，即编译时能得出结果的表达式。例如，代码 [code|static-assert] 中，常量 `i` 和 `j` 的值可以在编译时确定。

[code|&static-assert]

``` cpp
constexpr int i = 1;
constexpr long j = 2;
static_assert (i + j == 3, "compile error");
```

[align-center]

代码 [code||static-assert] - 编译时静态断言

利用常量表达式，我们就可以实现对模板的条件重载，即实现模板 “变量” 功能。例如代码 [code|test-value] 演示了如何编译时利用 `isZero<Val>` 判断 `Val` 是不是 `0`。

[code|&test-value]

``` cpp
template <unsigned Val> struct _isZero {
    constexpr static bool value = false;
};

template <> struct _isZero <0> {
    constexpr static bool value = true;
};

template <unsigned Val>
constexpr bool isZero = _isZero<Val>::value;

static_assert (!isZero<1>, "compile error");
static_assert (isZero<0>, "compile error");
```

[align-center]

代码 [code||test-value] - 编译时测试表达式

#### 测试类型

**测试类型** 的实质是利用 `type_traits` 生成常量表达式，再测试常量表达式。也就是，先生成一个测试类型的结果（常量），再对这个结果的表达式进行测试（常量表达式）。

测试类型又分为两种：测试一个类型是不是某种特定的类型 和 测试一个类型是否满足某些条件。利用对类型的测试，我们可以实现针对于不同类型的模板特殊化。例如，代码 [code|test-type] 中，我们实现了一个通用的将 C 语言的基本数据结构转换为 `std::string` 的函数 `ToString`。

[code|&test-type]

``` cpp
template <typename T>
constexpr bool isNum = std::is_arithmetic<T>::value;

template <typename T>
constexpr bool isStr = std::is_same<T, const char *>::value;

template <typename T>
constexpr bool isBad = !isNum<T> && !isStr<T>;

template <typename T>
std::enable_if_t<isNum<T>, std::string> ToString (T num) {
    return std::to_string (num);
}

template <typename T>
std::enable_if_t<isStr<T>, std::string> ToString (T str) {
    return std::string (str);
}

template <typename T>
std::enable_if_t<isBad<T>, std::string> ToString (T bad) {
    static_assert (!isBad<T>, "neither arithmetic nor string");
}

auto a = ToString (1);  // std::to_string (num);
auto b = ToString (1.0);  // std::to_string (num);
auto c = ToString ("0x0");  // std::string (str);
auto d = ToString (std::string {});  // not compile :-(
```

[align-center]

代码 [code||test-type] - 编译时测试类型

首先，我们定义了三个 **变量模板**：`isNum`，`isStr`，`isBad`，分别对应了三个判断类型是否匹配的 **谓词** _(predicate)_；这里使用了 `type_tratis` 中的 `std::is_arithmetic` 和 `std::is_same` 模板。然后重载函数 `ToString`，分别对应了数值类型，C 风格字符串和非法类型（不能转化为 `std::string`）。最后，在前两个重载中，分别调用 `std::to_string` 和 `std::string` 的构造函数；对于第三种情况，直接通过 `static_assert` 编译报错。

这个例子具有代表性的体现了元编程和普通编程的不同。先看一个错误的写法（代码 [code|not-test-type]）。

[code|&not-test-type]

``` cpp
template <typename T>
std::string ToString (T val) {
    if (isNum<T>) return std::to_string (val);
    else if (isStr<T>) return std::string (val);
    else static_assert (!isBad<T>, "neither arithmetic nor string");
}
```

[align-center]

代码 [code||not-test-type] - 编译时测试类型的错误用法

假设是脚本语言，这是没有问题的：因为脚本语言没有编译的概念，所有函数的绑定都在 解释时 完成；而对于需要编译的语言，函数的绑定是在 编译时 完成的。在编译代码 [code|not-test-type] 中的函数 `ToString` 时，对于给定的类型 `T`，需要执行两次函数绑定 —— `val` 作为参数调用 `std::to_string` 和 `std::string` 的构造函数，另外执行一次静态断言 —— 判断 `!isBad<T>` 是否为 `true`。假设我们调用 `ToString ("")` 时，会生成代码 [code|not-test-type-instance]。

[code|&not-test-type-instance]

``` cpp
std::string ToString (const char *val) {
    if (false) return std::to_string (val);
    else if (true) return std::string (val);
    else static_assert (true, "neither arithmetic nor string");
}
```

[align-center]

代码 [code||not-test-type-instance] - 编译时测试类型的错误用法的一个实例

生成的代码 [code|not-test-type-instance] 中，`std::string (val)` 可以正确的重载到 `const char *` 为参数的构造函数；但是 `std::to_string (val)` 是并不能正确重载。所以这段代码是不能被成功编译的。

#### C++ 17 的 `constexpr-if`

为了使得让代码 [code|not-test-type] 风格的代码用于元编程，C++ 17 引入了 `constexpr-if`。[constexpr-if] 我们只需要把以上代码 [code|not-test-type] 中的 `if` 改为 `if constexpr` 就可以编译了。在代码 [code|test-type-constexpr] 中展示。

[code|&test-type-constexpr]

``` cpp
template <typename T>
std::string ToString (T val)
{
    static_assert (!isBad<T>, "neither arithmetic nor string");

    if constexpr (isNum<T>) return std::to_string (val);
    else if constexpr (isStr<T>) return std::string (val);
}
```

[align-center]

代码 [code||test-type-constexpr] - `constexpr-if` 的用法

`constexpr-if` 的引入一方面大大降低元编程的逻辑难度，另一方面提高了模板代码的可读性。

### 编译时迭代

**编译时迭代** 和面向过程编程中的 **循环语句** _(loop statement)_ 类似，用于实现与 `for` / `while` / `do` 类似的逻辑。和普通的编程不同，元编程的演算规则属于是函数式的，不能通过一般的变量 **迭代** _(iteration)_ 实现编译时迭代，只能利用 **递归** _(recursion)_ 和 **特化** _(specialization)_ 组合实现。

#### 简单的迭代

代码 [code|calc-factor] 展示了如何使用 **编译时迭代** 实现编译时计算阶乘（$N!$）。

[code|&calc-factor]

``` cpp
template <unsigned N>
constexpr unsigned _Factor () { return N * _Factor<N - 1> (); }

template <>
constexpr unsigned _Factor<0> () { return 1; }

template <unsigned N>
constexpr unsigned Factor = _Factor<N> ();

static_assert (Factor<0> == 1, "compile error");
static_assert (Factor<1> == 1, "compile error");
static_assert (Factor<4> == 24, "compile error");
```

[align-center]

代码 [code||calc-factor] - 编译时迭代计算阶乘（$N!$）

#### 变长模板的迭代

变长模板提供了多个参数的可能，而为了遍历传入的每个参数，我们可以使用 **编译时迭代** 的方法。代码 [code|variadic-template] 可以用递归的方式实现。代码 [code|variadic-template-recursion] 除了打印所有参数，及在参数之间加空格的功能外，还能接受 0 个参数。

[code|&variadic-template-recursion]

``` cpp
void PrintAll () {
    std::cout << std::endl;
}

template<typename T, typename... Ts>
void PrintAll (T arg, Ts... args) {
    std::cout << arg;
    if (sizeof... (args)) std::cout << " ";
    PrintAll (args...);
}
```

[align-center]

代码 [code||variadic-template-recursion] - 变长模板打印参数（递归展开）

## 元编程的分类

### 常数推导

### 类型推导

### 代码生成

## C++ 元编程的应用实例 —— 设计一个 ORM

BOT Man 利用元编程设计了一个 **对象关系映射** _(object-relation mapping, ORM)_。[naive-orm] [better-orm]

## C++ 元编程的不足

### 调试代码 (debugging)

- 实例化
- 动态调试

### 代码膨胀 (code bloat)

- 死代码 (dead code) 链接时优化
- 冗余运算 (redundant calculations) 设计时优化

### 缺少编程时检查 (programming-time checking)

- 概念 (concept)
- 契约 (constract)
- 其他语言的接口 (interface)

## 总结

## 参考文献

- [cpp-pl]: Bjarne Stroustrup. _The C++ Programming Language (Fourth Edition)_ [M] Addison-Wesley, 2013.
- [cpp-evo]: Bjarne Stroustrup: _The Design and Evolution of C++_ [M] Addison-Wesley, 1994.
- [generic-programming]: David R. Musser, Alexander A. Stepanov. _Generic Programming_ [C] // P. Gianni. In _Symbolic and Algebraic Computation: International symposium ISSAC_, 1988: 13–25.
- [calc-prime]: Erwin Unruh. _Primzahlen Original_. http://www.erwin-unruh.de/primorig.html
- [using-cpp-tmp]: Todd Veldhuizen. _Using C++ template metaprograms_ [C] // In _C++ Report_, 1995: 36-43.
- [gererative-programming]: K. Czarnecki, U. Eisenecker. _Generative Programming: Methods, Tools,
and Applications_ [M] Addison-Wesley, 2000.
- [modern-cpp-design]: Andrei Alexandrescu. _Modern C++ Design_ [M] Addison-Wesley, 2001.
- [d-lang]: D Language Foundation. _Home - D Programming Language_. https://dlang.org/
- [cppref-template]: cppreference.com. _Templates_. http://en.cppreference.com/w/cpp/language/templates
- [cppref-template-param]: cppreference.com. _Template parameters and template arguments_. http://en.cppreference.com/w/cpp/language/template_parameters
- [constexpr-if]: cppreference.com. _if statement_. http://en.cppreference.com/w/cpp/language/if
- [naive-orm]: BOT Man JL. _How to Design a Naive C++ ORM_. https://bot-man-jl.github.io/articles/?post=2016/How-to-Design-a-Naive-Cpp-ORM
- [better-orm]: BOT Man JL. _How to Design a Better C++ ORM_. https://bot-man-jl.github.io/articles/?post=2016/How-to-Design-a-Better-Cpp-ORM
- [template-turing-complete]: Todd L. Veldhuizen. _C++ Templates are Turing Complete_ [R] Indiana University Computer Science Technical Report. 2003.