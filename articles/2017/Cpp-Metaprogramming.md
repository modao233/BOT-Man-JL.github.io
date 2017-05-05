# C++ 元编程 —— 让编译器帮你写程序

> 2017/5/2
>
> 本文是对 C++ 元编程 相关内容的理解和总结，为 2017 程序设计语言结课论文。

[heading-numbering]

## [no-number] [no-toc] 目录

[TOC]

[align-center]

## [no-number] [no-toc] 摘要

[align-center]

## [no-number] [no-toc] 关键字

[align-center]

C++, 元编程, 模板, 元编程演算

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

C++ 中的元编程是基于模板的。模板的概念最早在 1988 年，由 David R. Musser 和 Alexander A. Stepanov 提出 [generic-programming]，并最早应用于 C++ 程序设计语言。Alexander A. Stepanov 等人在 C++ 之父的邀请下，参与了 C++ **标准模板库** _(Standard Template Library, STL)_ （简称 标准库） 的设计。[cpp-evo]

模板的设计初衷是用于泛型编程，进行代码生成。假设给定 $M$ 个已有的 **数据类型** _(data type)_，我们需要为每个数据类型实现 $N$ 个具有相似功能的 类 或 函数。如果不使用模板，就像 C 语言那样，我们需要编写 $M \times N$ 份代码，即分别为每种数据类型编写 $N$ 个具有相似功能的 类 或 函数；如果使用了模板，我们仅需要编写 $N$ 份代码，即编写 $N$ 个独立于具体数据类型的模板。

而到了现代 C++，人们发现模板也被用于元编程中。1994 年的圣地亚哥 C++ 标准委员会会议中，Erwin Unruh 演示了一段 编译时通过错误信息计算素数 的代码。[calc-prime] 1995 年的 Todd Veldhuizen 在 C++ Report 上，首次提出了 C++ **模板元编程** 的概念，并指出了其在数值计算上的应用前景。[using-cpp-tmp] 随后，Andrei Alexandrescu 深入研究了模板元编程，提出了除了数值计算之外的元编程用途，并设计了一个 C++ 的模板元编程库 —— Loki。[modern-cpp-design] 受限于 C++ 对模板本身的限制，Andrei Alexandrescu 等人有发明了 D 语言，并在 D 语言中优化了元编程的特性。[d-lang]

元编程已被广泛的应用于现代 C++ 的程序设计中。由于元编程不同于一般的编程，在程序的设计上更具有挑战性，所以受到了许多学者和工程师的广泛关注。

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

在普通的编程中，编译器的 **编译** 能根据传入的参数的个数和类型，将被调用的 **函数**（包括成员函数）**绑定** _(binding)_ 到具体的函数重载上；而元编程中，编译器的 **推导** 也是根据传入的参数的个数和类型，将 **模板**（所有类型的模板）**实例化** _(instantiation)_ 到具体的模板重载上。

#### 模板参数的分类

C++ 中的 **模板参数** _(template parameter / argument)_ 可以分为三种：值参数，类型参数，模板参数。[cppref-template-param]

其中，类型和模板 作为 **实参** _(argument)_ 传入时，已经可以确定参数的内容；而值类型的 **形参** _(parameter)_ 仅接受编译时确定的值（常量表达式）。而对于模板也可以当作一般的类型参数进行传递（模板也是一个类型），但单独把它提出来，可以实现对参数模板的参数类型匹配。代码 [code|template-param] 展示了这三类模板参数的各种形式。

[code|&template-param]

``` cpp
template <Type Val> ...
template <Type Val = Default> ...
template <Type... Vals> ...
template <auto Val> ...  // C++ 17

template <typename|class T> ...
template <typename|class T = Default> ...
template <typename|class... Ts> ...

template <template<Args> typename|class C> ...
template <template<Args> typename|class C = Default> ...
template <template<Args> typename|class... Cs> ...
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

在标准库的实现中，模板特化已被广泛的应用。例如，Bjarne Stroustrup 提出可以在 `std::vector` 的实现中，对 `T *` 和 `void *` 进行了特化（代码 [code|spec-vector]）；然后将所有的 `T *` 的实现 **继承** 到 `void *` 的实现上，并在公开的函数里通过强制类型转换，进行 `void *` 和 `T *` 的交互；最后这使得所有的指针的 `std::vector` 就可以共享同一份实现了，从而缩小最终代码的体积。[cpp-pl]

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

## 元编程的基本演算

和普通编程不一样，C++ 的模板机制仅仅提供了 **纯函数** _(pure functional)_ 的方法，即不支持变量，而且所有的模板推导必须能在编译时完成。这些限制使得元编程的难度大大增加，但是 C++ 中提供的模板是图灵完备的 [template-turing-complete]，即我们可以使用模板实现元编程。

元编程基本的编译时 **演算规则** _(calculus rule)_ 有两种：**编译时测试** _(compile-time test)_ 和 **编译时迭代** _(compile-time iteration)_，分别对应了 **控制结构** _(control structure)_ 中的 **选择** _(selection)_ 和 **迭代** _(iteration)_。基于这两种基本的演算方法，我们可以组合出复杂的结构，从而实现完整的元编程。

### 编译时测试

**编译时测试** 相当于面向过程编程中的 **选择语句** _(selection statement)_，可以实现 `if-else` / `switch` 的逻辑。

C++ 17 之前，编译时测试的基础是 模板的最优匹配原则 和 模板特化 —— 每次找到最适合的最特殊的模板进行匹配（[sec|测试表达式], [sec|测试类型]）；而 C++ 17 中，有人提出可以使用 `constexpr-if` 实现更方便的测试（[sec|C++ 17 的 `constexpr-if`]）。

测试的对象有两种：**常量表达式** _(constexpr)_ 和 **类型**；前者通过模板特化直接实现，而后者的实现则是通过对常量表达式的测试实现。

#### 测试表达式

类似于 **静态断言** `static_assert`，编译时测试的对象可以是 常量表达式，即编译时能得出结果的表达式。例如，代码 [code|static-assert] 中，常量 `i` 和 `j` 的值可以在编译时确定。

[code|&static-assert]

``` cpp
constexpr int i = 1;
constexpr long j = 2;
static_assert (i + j == 3, "compile error");
```

[align-center]

代码 [code||static-assert] - 编译时静态断言

利用常量表达式，我们就可以实现对模板的条件重载，即重载为满足条件的那个模板。例如代码 [code|test-value] 演示了如何编译时利用 `isZero<Val>` 判断 `Val` 是不是 `0`。

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

首先，我们定义了三个 **变量模板**：`isNum`，`isStr`，`isBad`，分别对应了三个判断类型是否匹配的 **谓词** _(predicate)_；这里使用了 `type_tratis` 中的 `std::is_arithmetic` 和 `std::is_same` 模板，并把对应的 `value` 转化为常量表达式。然后利用 **SFINAE** _(Substitution Failure Is Not An Error)_ 规则 [cppref-SFINAE]（这里直接使用了 `type_traits` 中的 `std::enable_if`），重载函数 `ToString`，分别对应了数值类型，C 风格字符串和非法类型（不能转化为 `std::string`）。最后，在前两个重载中，分别调用 `std::to_string` 和 `std::string` 的构造函数；对于第三种情况，直接通过 `static_assert` 编译报错。

这个例子具有代表性的体现了元编程和普通编程的不同。代码 [code|not-test-type] 是一个错误的写法。

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

假设是脚本语言，这是没有问题的：因为脚本语言没有编译的概念，所有函数的绑定都在 **解释时** 完成；而对于需要编译的语言，函数的绑定是在 **编译时** 完成的。在编译代码 [code|not-test-type] 中的函数 `ToString` 时，对于给定的类型 `T`，需要执行两次函数绑定 —— `val` 作为参数调用 `std::to_string` 和 `std::string` 的构造函数，另外执行一次静态断言 —— 判断 `!isBad<T>` 是否为 `true`。假设我们调用 `ToString ("")` 时，会生成代码 [code|not-test-type-instance]。

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

为了使得让代码 [code|not-test-type] 风格的代码用于元编程，C++ 17 引入了 `constexpr-if`。[cppref-constexpr-if] 我们只需要把以上代码 [code|not-test-type] 中的 `if` 改为 `if constexpr` 就可以编译了。在代码 [code|test-type-constexpr] 中展示。

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

**编译时迭代** 和面向过程编程中的 **循环语句** _(loop statement)_ 类似，用于实现与 `for` / `while` / `do` 类似的逻辑。

和普通的编程不同，元编程的演算规则属于是纯函数的，不能通过一般的变量 **迭代** _(iteration)_ 实现编译时迭代，只能利用 **递归** _(recursion)_ 和 **特化** _(specialization)_ 组合实现。

#### 定长模板的迭代

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

变长模板提供了多个参数的可能，而为了遍历传入的每个参数，我们可以使用 **编译时迭代** 的方法实现元程序中的循环结构。代码 [code|variadic-template-recursion] 实现了对参数基本数据类型求和的功能。

[code|&variadic-template-recursion]

``` cpp
template <typename T>
constexpr auto Sum () {
    return T (0);
}

template <typename T, typename... Ts>
constexpr auto Sum (T arg, Ts... args) {
    return arg + Sum<T> (args...);
}

static_assert (Sum (1) == 1, "compile error");
static_assert (Sum (1, 2, 3) == 6, "compile error");
```

[align-center]

代码 [code||variadic-template-recursion] - 变长模板打印参数（递归展开）

## 元编程的基本用途

利用元编程，我们可以很方便的设计出 **类型安全** _(type safe)_、**运行时高效** _(runtime effective)_ 的程序。到现在，元编程已被广泛的应用于 C++ 的编程实践中。例如，Todd Veldhuizen 提出了使用元编程的方法构造 **表达式模板** _(expression template)_，使用惰性求值和表达式优化的方法，提升向量计算的运行速度 [expr-template]；K. Czarnecki 和 U. Eisenecker 对元编程深入研究，提出了利用纯模板实现 Lisp 解释器的方法 [gererative-programming]；BOT Man 利用元编程，设计了一个类型安全、运行时高效 **对象关系映射** _(object-relation mapping, ORM)_ [naive-orm] [better-orm]。

尽管元编程的应用场景各不相同，但是归根到底，各种应用场景相当于是对其基本用途的组合。元编程的三大基本用途分别是：**数值计算** _(numeric computation)_、**类型推导** _(type deduction)_ 和 **代码生成** _(code generation)_。

例如，在 BOT Man 设计的 ORM 中，使用了 类型推导 和 代码生成 的功能：根据 **对象** _(object)_ 在 C++ 中的类型，推导出对应的 **关系** _(relation)_ 中元组各个字段的类型；将对 C++ 对象的操作，映射到对应的数据库语句上，并生成相应的代码。另外，BOT Man 还利用元编程的方法，改进表达式系统、优化模板匹配失败的报错等。[naive-orm] [better-orm]

### 数值计算

作为元编程的最早的用途，数值计算可以用于 **编译时常数计算** 或 **优化运行时表达式计算**。

编译时常数计算能让我们使用有实际意义的语言，写出编译时确定的常量；而不是直接写计算的结果（**迷之数字** _(magic number)_）或 通过运行时计算这些常数。在代码 [code|variadic-template-recursion] 的基础上，我们可以实现一个编译时计算从 $1$ 到 $N$ 之和的代码（代码 [code|calc-sum]）。

[code|&calc-sum]

``` cpp
template <std::size_t... I>
constexpr auto _SeqSum (std::index_sequence<I...>)
{
    return Sum (I...) + sizeof... (I);
}

template <std::size_t... N>
constexpr auto SeqSum = _SeqSum (std::make_index_sequence<N> {});

static_assert (SeqSum<3> == 6, "compile error");
static_assert (SeqSum<5> == 15, "compile error");
```

[align-center]

代码 [code||calc-sum] - 编译时计算常数

最早的有关表达式计算优化的思路是 Todd Veldhuizen 提出的。[expr-template] 目前支持向量、矩阵表达式运算的库 `uBLAS` 已经加入了 `boost` 库。[boost-ublas] 利用表达式模板，我们可以实现部分求值、惰性求值、表达式化简等特性。

另外，现代 C++ 还允许定义 `constexpr` 函数，用于实现常量计算。如果该函数 编译时能确定所有的参数，那么就可以在编译时计算出结果的常量。[cppref-constexpr] 例如，代码 [code|calc-factor-constexpr] 的功能和 代码 [code||calc-factor] 相同，在编译时计算阶乘。

[code|&calc-factor-constexpr]

``` cpp
constexpr auto Factor (unsigned n)
{
    if (n <= 1) return 1;
    else return n * Factor (n - 1);
}

static_assert (Factor (3) == 6, "compile error");
```

[align-center]

代码 [code||calc-factor-constexpr] - `constexpr` 函数

### 类型推导

除了最基本的数值计算之外，我们还可以利用元编程进行任意类型之间的相互推导。在 **领域特定语言** _(domain-specific language)_ 和 C++ 语言原生结合时，类型推导可以帮助我们将这些语言中的类型，转化为 C++ 的类型。例如，BOT Man 提出了一种能编译时进行 SQL 语言元组类型推导的方法。

在 C++ 中，所有的基本数据类型都不能为 `NULL`；而在 SQL 中，有的字段是允许为 `NULL` 的，所以我们在 C++ 中使用 `std::optional` 容器存储可以为空的字段。当我们通过 `outer-join` 拼接得到的元组，所有的字段都可以为 `NULL`。所以，ORM 中需要一种方法能把一个 可能含有 `std::optional` 字段的元组，先将原元组中不带有 `std::optional` 的字段转换为 `std::optional<T>`，再拼接为新元组。

[code|&orm-to-nullable]

``` cpp
template <typename T> struct TypeToNullable {
    using type = std::optional<T>;
};
template <typename T> struct TypeToNullable <std::optional<T>> {
    using type = std::optional<T>;
};

template <typename... Args>
auto TupleToNullable (const std::tuple<Args...> &) {
    return std::tuple<typename TypeToNullable<Args>::type...> {};
}

auto t1 = std::make_tuple (std::optional<int> {}, int {});
auto t2 = TupleToNullable (t1);
static_assert (!std::is_same<
               std::tuple_element_t<0, decltype (t1)>,
               std::tuple_element_t<1, decltype (t1)>
>::value, "compile error");
static_assert (std::is_same<
               std::tuple_element_t<0, decltype (t2)>,
               std::tuple_element_t<1, decltype (t2)>
>::value, "compile error");
```

[align-center]

代码 [code||orm-to-nullable] - 编译时计算常数

代码 [code|orm-to-nullable] 中，首先定义了 `TypeToNullable` 并对 `std::optional<T>` 进行特化，作用是将带有 `std::optional` 和 不带 `std::optional` 的类型自动转换为带有 `std::optional` 的版本；然后利用 `TupleToNullable` 拆解元组中的所有类型（转化为参数包），并把所有类型传入 `TypeToNullable`，并将结果组装为新的元组。[better-orm]

### 代码生成

和泛型编程一样，元编程也常常被用于代码的生成。但是和简单的泛型编程不同，元编程生成的代码往往是通过 **编译时测试** 和 **编译时迭代** 的演算推导出来的。例如，[sec|测试类型] 中的代码 [code|test-type] 就是一个为 C 语言基本类型生成 `std::string` 的程序。

## C++ 元编程的主要难点

尽管元编程的好处有很多，但是 C++ 语言设计层面上没有专门考虑元编程的相关问题。所以这些限制给元编程增加了难度。我们认为元编程的主要难点可以分为三类：**设计时** _(design-time)_、编译时、运行时。

### 设计时

#### 复杂性

由于元编程的语言层面上的限制较大，所以那些利用了很多 **编译时测试** 和 **编译时迭代** 技巧的代码，**可读性** _(readability)_ 比较差。另外，为了实现设计出编译时能完成的演算，相较于一般的 C++，其 **可写性** _(writability)_ 也不是很好。

#### 实例化检查

在编译时，模板的实例化 和 函数的绑定 不同：前者对传入的参数是什么，没有太多的限制；而后者则根据函数的声明，确定了应该传入参数的类型。而对于模板实参内容的检查，则是在实例化的过程中完成的。（[sec|实例化错误]）所以，在设计程序时，程序员往往会因为没有足够的警告，犯一些低级错误。

因此，Bjarne Stroustrup 等人提出了在语言层面上，给模板上引入 **概念** _(concept)_。[cpp-pl] 利用概念，我们可以对传入的参数加上 **限制**，即只有满足特定限制的类型才能作为参数传入模板。[cppref-concept] 例如，模板 `std::max` （代码 [code|max-template]）只能接受支持运算符 `<` 的类型。

但是由于各种原因，这个语言特性一直没有能正式加入 C++ 标准。尽管现在没有在语言上实现概念，我们仍可以通过 **静态断言** 等方法，实现对概念的检查。（[sec|实例化错误]）

目前许多高级的 **集成开发环境** _(Integrated Development Environment, IDE)_（例如，Microsoft Visual Studio）支持了智能提示的功能，可以辅助用户进行实例化检查。[visual-studio]

### 编译时

#### 实例化错误

模板在实例化时，需要对实参执行的 **操作** _(operation)_ 和 **函数** _(function)_ 进行绑定。如果在绑定的过程中，没有匹配的操作（或函数），编译就会报错。[sec|设计时检查] 提到，如果语言支持了概念或者契约，就可以在设计代码的时候避免这个问题。例如，传入一个 `const` 的对象，而操作（或函数）要求改变对象的状态，这是不允许的，编译会报错。

当模板的调用层数很大的时候，尽管编译器会提示每一层实例化的状态，但是这些报错信息会非常复杂，很难让人较快的发现问题的所在。例如，一个库要求用户给传入的类定义一个成员函数 `fn`，并在库的实现中调用 `fn`；如果用户没有定义 `fn`，那么编译器实例化到库的实现时，就会报错找不到成员函数 `fn`，同时会把实例化的每一步的状态提示出来。因编译器而异，这些提示信息由于信息过多，用户很难察觉到问题所在。

为了解决这个问题，BOT Man 提出了一种 **短路编译** _(short-cut compiling)_ 的方法，能让基于元编程的库，给用户提供更人性化的编译时报错。具体方法是，在调用需要的操作（或函数）之前，先检查是否有对应的操作（或函数）；如果没有，就通过短路的方法停止编译，并使用 **静态断言** 提供报错信息。[better-orm]

#### 代码膨胀

由于模板会对所有不同模板实参进行一次实例化，所以当参数的种类很多的时候，很可能会发生 **代码膨胀** _(code bloat)_，即产生很多无用的代码。

在元编程中，我们很多时候只关心推导的结果，而不是过程。例如，[sec|数值计算] 的代码 [code|calc-sum] 中，我们只关心最后的 `SeqSum<3> == 6`，而不需要中间过程中产生的临时模板。但是在 `N` 很大的时候，类似这样的运算会产生很多临时模板。好在编译器为我们解决了这个问题，那些临时模板被认为是 **死代码** _(dead code)_，即不被执行的代码，在 **链接时** _(link-time)_ 编译器会移除这些无用代码，所以最终的目标代码不会包含它们。

还有另一种情况，我们展开的代码都是有效代码，即都是被执行的，但是又由于需要的参数的类型繁多，最后的代码体积仍然很大。在 [sec|模板特化] 中介绍了，Bjarne Stroustrup 提出了一种 **设计时** 消除 **冗余运算** _(redundant calculation)_ 的方法，用于缩小模板实例体积。具体思路是，将不同参数实例化得到的模板的 **相同部分** 抽象为一个 **基类** _(base class)_，然后 “继承” 并 “重载” 每种参数情况的 **不同部分**，从而实现更多代码的共享。

### 运行时

元编程在运行时主要的难点在于：对模板代码的 **调试** _(debugging)_。

许多调试器都支持了  **断点** _(breakpoint)_ 的功能，即用户在代码的某一条语句上设置断点，当程序运行到断点位置时，暂停程序的运行，并导出所有的运行时信息。而许多的模板实例共享同一个模板代码，如果在模板代码上设置断点，所有的实例运行到断点位置都会暂停。例如，`std::vector<int>` 和 `std::vector<double>` 共享同一模板代码，在 `std::vector<T>` 上的断点同时对这两份实例生效。然而这只是最简单的情况。

假设我们需要调试的是一段通过很多次的 编译时测试（[sec|编译时测试]）和 编译时迭代（[sec|编译时迭代]）展开的代码，即这段代码是各个模板的拼接生成的；那么，对这段生成的代码的调试会变得非常的复杂 —— 实际过程中需要不断地在各个模板间来回切换。

## 总结

C++ 元编程的出现，是一个无心插柳的偶然 —— 人们发现 C++ 语言提供的模板抽象机制，能很好的被应用于元编程上。借助元编程，我们可以写出 类型安全、运行时高效 的代码。但 C++ 模板的限制，大大增加了元编程的复杂性。在 C++ 不断地演化中，新的语言特性被不断提出，这也为元编程提供更多的可能。

本文仅是我对 C++ 元编程的 **个人理解**。对本文有什么问题，**欢迎斧正**。😉

This article is published under MIT License &copy; 2017, BOT Man

## [no-number] 参考文献

- [cpp-pl]: Bjarne Stroustrup. _The C++ Programming Language (Fourth Edition)_ [M] Addison-Wesley, 2013.
- [generic-programming]: David R. Musser, Alexander A. Stepanov. _Generic Programming_ [C] // P. Gianni. In _Symbolic and Algebraic Computation: International symposium ISSAC_, 1988: 13–25.
- [cpp-evo]: Bjarne Stroustrup: _The Design and Evolution of C++_ [M] Addison-Wesley, 1994.
- [calc-prime]: Erwin Unruh. _Primzahlen Original_. http://www.erwin-unruh.de/primorig.html
- [using-cpp-tmp]: Todd Veldhuizen. _Using C++ template metaprograms_ [C] // In _C++ Report_, 1995, 7(4): 36-43.
- [modern-cpp-design]: Andrei Alexandrescu. _Modern C++ Design_ [M] Addison-Wesley, 2001.
- [d-lang]: D Language Foundation. _Home - D Programming Language_. https://dlang.org/
- [cppref-template]: cppreference.com. _Templates_. http://en.cppreference.com/w/cpp/language/templates
- [cppref-template-param]: cppreference.com. _Template parameters and template arguments_. http://en.cppreference.com/w/cpp/language/template_parameters
- [template-turing-complete]: Todd L. Veldhuizen. _C++ Templates are Turing Complete_ [R] Indiana University Computer Science Technical Report. 2003.
- [cppref-SFINAE]: cppreference.com. _SFINAE_. http://en.cppreference.com/w/cpp/language/sfinae
- [cppref-constexpr-if]: cppreference.com. _if statement_. http://en.cppreference.com/w/cpp/language/if
- [cppref-constexpr]: cppreference.com. _constexpr specifier_. http://en.cppreference.com/w/cpp/language/constexpr
- [expr-template]: Todd Veldhuizen. _Expression Templates_ [C] // In _C++ Report_, 1995, 7(5): 26–31.
- [gererative-programming]: K. Czarnecki, U. Eisenecker. _Generative Programming: Methods, Tools,
and Applications_ [M] Addison-Wesley, 2000.
- [naive-orm]: BOT Man JL. _How to Design a Naive C++ ORM_. https://bot-man-jl.github.io/articles/?post=2016/How-to-Design-a-Naive-Cpp-ORM
- [better-orm]: BOT Man JL. _How to Design a Better C++ ORM_. https://bot-man-jl.github.io/articles/?post=2016/How-to-Design-a-Better-Cpp-ORM
- [boost-ublas]: Boost Org. _uBLAS_. https://github.com/boostorg/ublas
- [visual-studio]: Microsoft. Visual Studio. https://www.visualstudio.com
- [cppref-concept]: cppreference.com. _Constraints and concepts_. http://en.cppreference.com/w/cpp/language/constraints