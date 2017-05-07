# C++ 元编程 —— 让编译器帮你写程序

> 2017/5/2
>
> 本文是对 C++ 元编程 相关内容的理解和总结，为 2017 程序设计语言结课论文。

[heading-numbering]

## [no-number] [no-toc] 目录

[TOC]

[align-center]

## [no-number] [no-toc] 摘要

随着 C++ 新标准的不断更新，“C++ 越来越像一门新的语言了” [cpp-pl]。元编程作为一种新兴的编程方式，受到了越来越多的广泛关注。本文结合已有文献和个人实践，对有关 C++ 元编程进行了系统的分析。本文首先介绍了 C++ 元编程中的 **相关概念** 和 **语言支持**，然后利用科学的方法分析了元编程的 **演算规则**、**基本应用** 和实践过程中的 **主要难点**，最后提出了对 C++ 元编程发展的 **展望**。

[align-center]

## [no-number] [no-toc] 关键字

[align-center]

C++ 元编程 / 元编程理论 / 模板编程 / 现代 C++

## C++ 中的元编程

### 什么是元编程

**元编程** _(metaprogramming)_ 通过操作 **程序实体** _(program entity)_，在 **编译时** _(compile time)_ 计算出 **运行时** _(run time)_ 需要的常数、类型、代码的方法。

一般的编程是通过直接编写 **程序** _(program)_，通过编译器 **编译** _(compile)_，产生目标代码，并用于 **运行时** 执行。与普通的编程不同，元编程则是借助语言提供的 **模板** _(template)_ 机制，通过编译器 **推导** _(deduce)_，在 **编译时** 生成程序。元编程经过编译器推导得到的程序，再进一步通过编译器编译，产生最终的目标代码。在 [sec|使用 `if` 进行编译时测试] 中，我们用了一个例子说明了两者的区别。

因此，元编程又被成为 **两级编程** _(two-level programming)_，**生成式编程** _(generative programming)_ 或 **模板元编程** (template metaprogramming)。[cpp-pl]

### 元编程在 C++ 中的位置

> C++ 语言 = C 语言的超集 + 抽象机制 + 标准库

C++ 的 **抽象机制** _(abstraction mechanisms)_ 主要有两种：**面向对象编程** _(object-oriented programming)_ 和 **模板编程** _(generic programming)_。[cpp-pl]

为了实现面向对象编程，C++ 提供了 **类** _(class)_，用 C++ 的已有 **类型** _(type)_ 构造出新的类型。而在模板编程方面，C++ 提供了 **模板** _(template)_，以一种直观的方式表示 **通用概念** _(general concept)_。

模板编程的应用主要有两种：**泛型编程** _(generic programming)_ 和 **元编程** _(meta-programming)_。前者注重于 **通用概念** 的抽象，设计通用的 **类型** 或 **算法** _(algorithm)_，不需要过于关心编译器如何生成具体的代码；而后者注重于设计模板推导时的 **选择** _(selection)_ 和 **迭代** _(iteration)_，通过模板技巧设计程序。[cpp-pl]

### C++ 元编程的历史

1988 年，David R. Musser 和 Alexander A. Stepanov 提出了 **模板** [generic-programming]，并最早应用于 C++ 语言。Alexander A. Stepanov 等人在 Bjarne Stroustrup 的邀请下，参与了 **C++ 标准模板库** _(C++ Standard Template Library, C++ STL)_ （简称 **标准库**） 的设计。[cpp-evo] 模板的设计初衷仅是用于泛型编程，对数据结构和算法进行 **抽象** _(abstraction)_。

而在现代 C++ 的时代，人们发现模板可以用于元编程。1994 年的 C++ 标准委员会会议上，Erwin Unruh 演示了一段利用编译器错误信息计算素数的代码。[calc-prime] 1995 年的 Todd Veldhuizen 在 C++ Report 上，首次提出了 C++ **模板元编程** 的概念，并指出了其在数值计算上的应用前景。[using-cpp-tmp] 随后，Andrei Alexandrescu 提出了除了数值计算之外的元编程应用，并设计了一个通用的 C++ 的模板元编程库 —— Loki。[modern-cpp-design] 受限于 C++ 对模板本身的限制，Andrei Alexandrescu 等人又发明了 D 语言，把元编程提升为语言自身的一个特性。[d-lang]

元编程已被广泛的应用于现代 C++ 的程序设计中。由于元编程不同于一般的编程，在程序设计上更具有挑战性，所以受到了许多学者和工程师的广泛关注。

## 元编程的语言支持

### C++ 中的模板类型

元编程完全依赖于 C++ 中的模板机制。目前最新的 C++ 将模板分成了 4 类：**类模板** _(class template)_，**函数模板** _(function template)_，**别名模板** _(alias template)_ 和 **变量模板** _(variable template)_。[cppref-template] 前两者能产生新的类型，属于 **类型构造器** _(type constructor)_；而后两者仅是语言提供的简化记法，属于 **语法糖** _(syntactic sugar)_。

**类模板** 和 **函数模板** 分别用于定义具有相似功能的 **类** 和 **函数** _(function)_，是泛型中对 **类型** 和 **算法** 的抽象。在标准库中，**容器** _(container)_ 和 **函数** 都是 **类模板** 和 **函数模板** 的应用。

**别名模板** 和 **变量模板** 分别在 C++ 11 和 C++ 14 引入，分别提供了具有模板特性的 **类型别名** _(type alias)_ 和 **常量** _(constant)_ 的简记方法。前者只能用于简记 已知类型，并不产生新的类型；后者则可以通过 函数模板返回值 等方法实现。尽管这两类模板不是必须的，但可以增加程序的可读性（[sec|复杂性]）。例如，C++ 14 中的 **别名模板** `std::enable_if_t<T>` 等价于 `typename std::enable_if<T>::type`。

### C++ 中的模板参数

C++ 中的 **模板参数** _(template parameter / argument)_ 可以分为三种：值参数，类型参数，模板参数。[cppref-template-param]（**函数参数** _(function parameter)_ 仅有 值类型 一种）其中，类型 和 模板 作为 **实参** _(argument)_ 传入时，已经可以确定参数的内容；而 值类型的 **形参** _(parameter)_ 仅接受编译时确定的值（常量表达式）。

从 C++ 11 开始，C++ 支持了 **变长模板** _(variadic template)_，即模板参数的个数可以是不确定的。标准库中的 **元组** _(tuple)_ —— `std::tuple` 就是变长模板的一个应用（元组的 **类型参数** 是不定长的，可以用 `template<typename... Ts> ...` 匹配）。

尽管 模板参数 也可以当作一般的 类型参数 进行传递（模板也是一个类型），但之所以单独提出来，是因为它可以实现对传入模板的参数匹配。[sec|类型推导] 的例子（代码 [code|orm-to-nullable]）使用 `std::tuple` 作为参数，然后通过匹配的方法，提取 `std::tuple` 内部的变长参数。

### 模板的重载和绑定 —— 特化和实例化

**特化** _(specialization)_ 类似于函数的 **重载** _(overload)_，即给出 全部模板参数取值（完全特化）或 部分模板参数取值（部分特化） 的模板实现。模板的特化常常被用于 [sec|元编程的基本演算] 介绍的元编程的逻辑演算。另外，特化还可以用于优化元编程代码，避免代码膨胀（在 [sec|代码膨胀] 给出了一个实例）。

**实例化** _(instantiation)_ 类似于函数的 **绑定** _(binding)_，是编译器根据参数的个数和类型，判断使用哪个重载的过程。由于函数和模板的重载具有相似性，所以他们的参数 **重载规则** _(overloading rule)_ 也是相似的。

## 元编程的基本演算

C++ 中的元编程主要依靠模板的支持完成，而 C++ 的模板机制仅仅提供了 **纯函数** _(pure functional)_ 的方法，即不支持变量，且所有的推导必须在编译时完成。但是 C++ 中提供的模板是 **图灵完备** _(turing complete)_ 的 [template-turing-complete]，所以我们可以使用模板实现完整的元编程。除了模板，现代 C++ 还允许使用 `constexpr` 函数进行常量计算。[cppref-constexpr]

元编程的基本 **演算规则** _(calculus rule)_ 有两种：**编译时测试** _(compile-time test)_ 和 **编译时迭代** _(compile-time iteration)_ [cpp-pl]，分别实现了 **控制结构** _(control structure)_ 中的 **选择** _(selection)_ 和 **迭代** _(iteration)_。基于这两种基本的演算方法，我们可以完成更复杂的演算。

### 编译时测试

**编译时测试** 相当于面向过程编程中的 **选择语句** _(selection statement)_，可以实现 `if-else` / `switch` 的选择逻辑。

在 C++ 17 之前，编译时测试是通过模板的 实例化 和 特化 实现的（[sec|模板的重载和绑定 —— 特化和实例化]）—— 每次找到最特殊的模板进行匹配；而 C++ 17 提出了使用 `constexpr-if` 的编译时测试方法。

#### 测试表达式

类似于 **静态断言** _(static assert)_，编译时测试的对象是 **常量表达式** _(constexpr)_，即编译时能得出结果的表达式。以不同的常量表达式作为参数，我们可以构造各种需要的模板重载。例如，代码 [code|test-value] 演示了如何构造 **谓词** _(predicate)_ `isZero<Val>`，编译时判断 `Val` 是不是 `0`。

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

在元编程的很多应用场景中，我们需要对类型进行测试，即对不同的类型实现不同的功能。而常见的测试类型又分为两种：判断一个类型 **是否为特定的类型** 和 **是否满足某些条件**。前者可以通过对模板的 **特化**（[sec|模板的重载和绑定 —— 特化和实例化]）直接实现；后者的测试可以通过 **测试表达式**（[sec|测试表达式]）间接完成的，即利用 C++ 11 的 `type_traits` 判断条件是否满足，生成出对应的常量表达式，再测试常量表达式。

**是否为特定的类型** 的判断，类似于代码 [code|test-value]，将 `unsigned Val` 改为 `typename Type`；并把传入的模板参数由 值参数 改为 类型参数（[sec|C++ 中的模板参数]），根据最优原则匹配重载。

**是否满足某些条件** 的判断，在代码 [code|test-type] 中，展示了如何将 C 语言的基本类型数据，转换为 `std::string` 的函数 `ToString`。代码具体分为三个部分：

1. 首先定义三个 **变量模板** `isNum`，`isStr`，`isBad`，分别对应了三个类型条件的谓词（使用了 `type_tratis` 中的 `std::is_arithmetic` 和 `std::is_same`）；
2. 根据 **SFINAE** _(Substitution Failure Is Not An Error)_ 规则 [cppref-SFINAE]（直接使用了 `type_traits` 中的 `std::enable_if`）重载函数 `ToString`，分别对应了数值、C 风格字符串和非法类型；
3. 在前两个重载中，分别调用 `std::to_string` 和 `std::string` 构造函数；在最后一个重载中，直接使用 `static_assert` 编译报错。

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

#### 使用 `if` 进行编译时测试

对于初次接触元编程的人，往往会使用 `if` 语句进行编译时测试。代码 [code|not-test-type] 是 代码 [code|test-type] 一个 **错误的写法**，很代表性的体现了元编程和普通编程的不同之处（[sec|什么是元编程]）。

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

代码 [code|not-test-type] 中的错误在于：编译代码的函数 `ToString` 时，对于给定的类型 `T`，需要进行两次函数绑定 —— `val` 作为参数分别调用 `std::to_string (val)` 和 `std::string (val)`，再进行一次静态断言 —— 判断 `!isBad<T>` 是否为 `true`。这会使得两次绑定中，有一次会失败。假设我们调用 `ToString ("str")`，在编译这段代码时，`std::string (const char *)` 可以正确的重载，但是 `std::to_string (const char *)` 并不能找到正确的重载，导致编译失败。

假设是脚本语言，这段代码是没有问题的：因为脚本语言没有编译的概念，所有函数的绑定都在 **运行时** 完成；而静态语言的函数绑定是在 **编译时** 完成的。为了使得代码 [code|not-test-type] 的风格用于元编程，C++ 17 引入了 `constexpr-if`。[cppref-constexpr-if] 我们只需要把以上代码 [code|not-test-type] 中的 `if` 改为 `if constexpr` 就可以编译了。`constexpr-if` 的引入让模板测试更加直观，提高了模板代码的可读性（[sec|复杂性]）。

### 编译时迭代

**编译时迭代** 和面向过程编程中的 **循环语句** _(loop statement)_ 类似，用于实现与 `for` / `while` / `do` 类似的循环逻辑。

和普通的编程不同，元编程的演算规则是纯函数的，不能通过 变量迭代 实现编译时迭代，只能用 **递归** _(recursion)_ 和 **特化**（[sec|模板的重载和绑定 —— 特化和实例化]）的组合实现。一般思路是：提供两类重载 —— 一类接受 **任意参数**，内部 **递归** 调用自己；另一类是前者的 **模板特化** 或 **函数重载**，直接返回结果，相当于 **递归终止条件**。它们的重载条件可以是 表达式 或 类型（[sec|编译时测试]）。

#### 定长模板的迭代

代码 [code|calc-factor] 展示了如何使用 **编译时迭代** 实现编译时计算阶乘（$N!$）。函数 `_Factor` 有两个重载：一个是对任意非负整数的，一个是对 `0` 为参数的。前者利用递归产生结果，后者直接返回结果。当调用 `_Factor<2>` 时，编译器会展开为 `2 * _Factor<1>`，然后 `_Factor<1>` 再展开为 `1 * _Factor<0>`，最后 `_Factor<0>` 直接匹配到参数为 `0` 的重载。

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

为了遍历变长模板的每个参数，我们可以使用 **编译时迭代** 实现循环遍历。代码 [code|calc-sum] 实现了对所有参数求和的功能。函数 `Sum` 有两个重载：一个是对没有函数参数的情况，一个是对函数参数个数至少为 `1` 的情况。和定长模板的迭代类似（[sec|定长模板的迭代]），这里也是通过 **递归** 调用实现参数遍历。

[code|&calc-sum]

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

代码 [code||calc-sum] - 编译时迭代计算和（$\Sigma$）

## 元编程的基本应用

利用元编程，我们可以很方便的设计出 **类型安全** _(type safe)_、**运行时高效** _(runtime effective)_ 的程序。到现在，元编程已被广泛的应用于 C++ 的编程实践中。例如，Todd Veldhuizen 提出了使用元编程的方法构造 **表达式模板** _(expression template)_，使用表达式优化的方法，提升向量计算的运行速度 [expr-template]；K. Czarnecki 和 U. Eisenecker 利用模板实现 Lisp 解释器 [gererative-programming]。

尽管元编程的应用场景各不相同，但都是三类基本应用的组合：**数值计算** _(numeric computation)_、**类型推导** _(type deduction)_ 和 **代码生成** _(code generation)_。例如，在 BOT Man 设计的 **对象关系映射** _(object-relation mapping, ORM)_ 中，主要使用了 类型推导 和 代码生成 的功能。根据 **对象** _(object)_ 在 C++ 中的类型，推导出对应数据库 **关系** _(relation)_ 中元组各个字段的类型；将对 C++ 对象的操作，映射到对应的数据库语句上，并生成相应的代码。[naive-orm] [better-orm]

### 数值计算

作为元编程的最早的应用，数值计算可以用于 **编译时常数计算** 和 **优化运行时表达式计算**。

**编译时常数计算** 能让我们使用程序设计语言，写编译时确定的常量；而不是直接写常数（**迷之数字** _(magic number)_）或 在运行时计算这些常数。例如，[sec|编译时迭代] 的两个例子（代码 [code|calc-factor], [code|calc-sum]）都是编译时对常数的计算。

最早的有关元编程 **优化表达式计算** 的思路是 Todd Veldhuizen 提出的。[expr-template] 利用表达式模板，我们可以实现部分求值、惰性求值、表达式化简等特性。

### 类型推导

除了基本的数值计算之外，我们还可以利用元编程进行任意类型之间的相互推导。例如，在 **领域特定语言** _(domain-specific language)_ 和 C++ 语言原生结合时，类型推导可以帮助我们将这些语言中的类型，转化为 C++ 的类型，并保证类型安全。

BOT Man 提出了一种能编译时进行 SQL 语言元组类型推导的方法。[better-orm] C++ 所有的数据类型都不能为 `NULL`；而 SQL 的字段是允许为 `NULL` 的，所以我们可以在 C++ 中使用 `std::optional` 容器存储可以为空的字段。通过 SQL 的 `outer-join` 拼接得到的元组的所有字段都可以为 `NULL`，所以 ORM 需要一种方法：把字段可能是 `std::optional<T>` 或 `T` 的元组，转化为全部字段都是 `std::optional<T>` 的新元组。

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

代码 [code||orm-to-nullable] - 类型推导

代码 [code|orm-to-nullable] 展示了这个功能：

1. 定义 `TypeToNullable`，并对 `std::optional<T>` 进行特化，作用是将 `std::optional<T>` 和 `T` 自动转换为 `std::optional<T>`；
2. 定义 `TupleToNullable`，拆解元组中的所有类型，转化为参数包（[sec|C++ 中的模板参数]），再把参数包中所有类型分别传入 `TypeToNullable`，最后得到的结果重新组装为新的元组。

### 代码生成

和泛型编程一样，元编程也常常被用于代码的生成。但是和简单的泛型编程不同，元编程生成的代码往往是通过 **编译时测试** 和 **编译时迭代** 的演算推导出来的。例如，[sec|测试类型] 中的代码 [code|test-type] 就是一个将 C 语言基本类型转化为 `std::string` 的代码的生成代码。

## 元编程的主要难点

由于 C++ 语言设计层面上没有专门考虑元编程的相关问题，所以实际元编程难度较大。元编程的难点主要有四类：复杂性、实例化错误、代码膨胀、调试模板。

### 复杂性

由于元编程的语言层面上的限制较大，所以许多的元编程代码使用了很多的 **编译时测试** 和 **编译时迭代** 技巧，**可读性** _(readability)_ 都比较差。另外，由于巧妙的设计出编译时能完成的演算也是很困难的，相较于一般的 C++ 程序，元编程的 **可写性** _(writability)_ 也不是很好。

现代 C++ 也不断地增加语言的特性，致力于降低元编程的复杂性。例如，[sec|C++ 中的模板类型] 的 **别名模板** 提供了对模板中的类型的简记方法，**变量模板** 提供了对模板中常量的简记方法，都增强可读性；C++ 17 的 `constexpr-if`（[sec|使用 `if` 进行编译时测试]）提供了 **编译时测试** 的新写法，增强可写性。

### 实例化错误

模板的实例化 和 函数的绑定 不同（[sec|模板的重载和绑定 —— 特化和实例化]）：在编译前，前者对传入的参数是什么，没有太多的限制；而后者则根据函数的声明，确定了应该传入参数的类型。而对于模板实参内容的检查，则是在实例化的过程中完成的（[sec|实例化错误]）。所以，程序的设计者在编译前，很难发现实例化时可能产生的错误。

为了减少可能产生的错误，Bjarne Stroustrup 等人提出了在 **语言层面** 上，给模板上引入 **概念** _(concept)_。[cpp-pl] 利用概念，我们可以对传入的参数加上 **限制** _(constraint)_，即只有满足特定限制的类型才能作为参数传入模板。[cppref-concept] 例如，模板 `std::max` 限制接受支持运算符 `<` 的类型传入。但是由于各种原因，这个语言特性一直没有能正式加入 C++ 标准。不过在编译时，我们仍可以通过 **编译时测试** 和 **静态断言** 等方法（[sec|测试类型]）实现检查。

另外，编译时模板的实例化出错位置，在调用层数较深处时，编译器会提示每一层实例化的状态，这使得报错信息包含了很多的无用信息，很难让人较快的发现问题所在。BOT Man 提出了一种 **短路编译** _(short-circuit compiling)_ 的方法，能让基于元编程的 **库** _(library)_，给用户提供更人性化的编译时报错。具体方法是，在 **实现** _(implementation)_ 调用需要的操作之前，**接口** _(interface)_ 先检查是传入的参数否有对应的操作；如果没有，就通过短路的方法，转到一个用于报错的接口，然后停止编译并使用 **静态断言** 提供报错信息。[better-orm] Paul Fultz II 提出了一种更通用的优化报错信息的方法。 [fit-lib]

### 代码膨胀

由于模板会对所有不同模板实参都进行一次实例化，所以当参数的组合很多的时候，很可能会发生 **代码膨胀** _(code bloat)_，即产生体积巨大的代码。这些代码可以分为两种：**死代码**  _(dead code)_ 和 **有效代码** _(effective code)_。

在元编程中，我们很多时候只关心推导的结果，而不是过程。例如，[sec|定长模板的迭代] 的代码 [code|calc-factor] 中，我们只关心最后的 `Factor<4> == 24`，而不需要中间过程中产生的临时模板。但是在 `N` 很大的时候，编译会产生很多临时模板。这些临时模板是 **死代码**，即不被执行的代码。所以，编译器会自动优化最终的代码生成，在 **链接时** _(link-time)_ 移除这些无用代码，使得最终的目标代码不会包含它们。

另一种情况下，我们展开的代码都是 **有效代码**，即都是被执行的，但是又由于需要的参数的类型繁多，最后的代码体积仍然很大。编译器很难优化这些代码，所以程序员应该在 **设计时编码代码膨胀**。Bjarne Stroustrup 提出了一种消除 **冗余运算** _(redundant calculation)_ 的方法，用于缩小模板实例体积。具体思路是，将不同参数实例化得到的模板的 **相同部分** 抽象为一个 **基类** _(base class)_，然后 “继承” 并 “重载” 每种参数情况的 **不同部分**，从而实现更多代码的共享。

例如，在 `std::vector` 的实现中，对 `T *` 和 `void *` 进行了特化（[sec|模板的重载和绑定 —— 特化和实例化]，见代码 [code|spec-vector]）；然后将所有的 `T *` 的实现 **继承** 到 `void *` 的实现上，并在公开的函数里通过强制类型转换，进行 `void *` 和 `T *` 的相互转换；最后这使得所有的指针的 `std::vector` 就可以共享同一份实现，从而避免了代码膨胀。[cpp-pl]

[code|&spec-vector]

``` cpp
template <typename T> class vector;  // general
template <typename T> class vector<T *>;  // partial spec
template <> class vector<void *>;  // complete spec

template <typename T>
class vector<T *> : private vector<void *> { ... }
```

[align-center]

代码 [code||spec-vector] - 特化 `std::vector` 避免代码膨胀

### 调试模板

元编程在运行时主要的难点在于：对模板代码的 **调试** _(debugging)_。如果我们需要调试的是一段通过很多次的 **编译时测试**（[sec|编译时测试]）和 **编译时迭代**（[sec|编译时迭代]）展开的代码，即这段代码是各个模板的拼接生成的（而且展开的层数很多）；那么，调试时需要不断地在各个模板的 **实例** _(instance)_ 间来回切换。这种情景下，调试人员很难把具体的问题定位到展开后的代码上。

## 总结

C++ 元编程的出现，是一个无心插柳的偶然 —— 人们发现 C++ 语言提供的模板抽象机制，能很好的被应用于元编程上。借助元编程，我们可以写出 类型安全、运行时高效 的代码。但是，过度的使用元编程，一方面会增加编译时间，另一方面会降低程序的可读性。不过，在 C++ 不断地演化中，新的语言特性被不断提出，为元编程提供更多的可能。

本文主要内容是我对 C++ 元编程的 **个人理解**。对本文有什么问题，**欢迎斧正**。😉

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
- [cppref-constexpr]: cppreference.com. _constexpr specifier_. http://en.cppreference.com/w/cpp/language/constexpr
- [cppref-SFINAE]: cppreference.com. _SFINAE_. http://en.cppreference.com/w/cpp/language/sfinae
- [cppref-constexpr-if]: cppreference.com. _if statement_. http://en.cppreference.com/w/cpp/language/if
- [expr-template]: Todd Veldhuizen. _Expression Templates_ [C] // In _C++ Report_, 1995, 7(5): 26–31.
- [gererative-programming]: K. Czarnecki, U. Eisenecker. _Generative Programming: Methods, Tools,
and Applications_ [M] Addison-Wesley, 2000.
- [naive-orm]: BOT Man JL. _How to Design a Naive C++ ORM_. https://bot-man-jl.github.io/articles/?post=2016/How-to-Design-a-Naive-Cpp-ORM
- [better-orm]: BOT Man JL. _How to Design a Better C++ ORM_. https://bot-man-jl.github.io/articles/?post=2016/How-to-Design-a-Better-Cpp-ORM
- [cppref-concept]: cppreference.com. _Constraints and concepts_. http://en.cppreference.com/w/cpp/language/constraints
- [fit-lib]: Paul Fultz II. _Goodbye metaprogramming, and hello functional: Living in a post-metaprogramming era in C++ (MODERN GENERIC PROGRAMMING)_ [C] // In _C++ Now_, 2016.