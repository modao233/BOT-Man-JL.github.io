# 如何写更好的 C++ 代码

> 2020/3/1
> 
> 结合风格规范及核心指南，通过例子介绍 C++ 常见的“坑” 😂

[slide-mode]

---

### 目录 [no-toc]

[TOC]

---

<br/>
<br/>

[align-center]

## 文档和工具

<br/>
<br/>

---

### 文档

- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- [Chromium C++ Style Guide](https://github.com/chromium/chromium/blob/master/styleguide/c%2B%2B/c%2B%2B.md)
- [Chromium C++ Dos and Don'ts](https://github.com/chromium/chromium/blob/master/styleguide/c%2B%2B/c%2B%2B-dos-and-donts.md)
- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)

### 工具

- [cpplint.py](https://github.com/google/styleguide/blob/gh-pages/cpplint/cpplint.py) / [clang-format](http://clang.llvm.org/docs/ClangFormat.html) / [clang-tidy](http://clang.llvm.org/extra/clang-tidy/)

---

<br/>
<br/>

[align-center]

## Google 风格

<br/>
<br/>

---

### Self-contained Headers

``` cpp
// helpers.h
#include <vector>
void Foo(std::vector<std::string> param);

// helpers.cc
#include <string>
#include "helpers.h"     // need <string>
std::vector<int> Bar();  // need <vector>
```

> - [来源](https://google.github.io/styleguide/cppguide.html#Self_contained_Headers)
> - [SF.10: Avoid dependencies on implicitly `#include`d names](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rs-implicit)
> - [SF.11: Header files should be self-contained](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rs-contained)

---

### `#define` Guard

``` cpp
#ifndef FOO_BAR_BAZ_H_
#define FOO_BAR_BAZ_H_

// Format: <PROJECT>_<PATH>_<FILE>_H_
// Example: |foo/src/bar/baz.h| in |foo|

#endif  // FOO_BAR_BAZ_H_
```

> - [来源](https://google.github.io/styleguide/cppguide.html#The__define_Guard)
> - [SF.8: Use `#include` guards for all `.h` files](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rs-guards)
> - 可以通过 `cpplint --root=PATH` 检查
> - 允许使用非标准的 `#pragma once`（Tencent Style）

---

### Forward Declarations

``` cpp
class Foo;
class Bar;
class Baz {
  Bar* bar_ = nullptr;
 public:
  void UseFoo(const Foo& foo);
  Bar* bar() { return bar_; }
};
```

> - [来源](https://google.github.io/styleguide/cppguide.html#Forward_Declarations)
> - [SF.9: Avoid cyclic dependencies among source files](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rs-cycles)
> - 优点：节省编译时间，避免重新编译，消除循环依赖
> - 缺点：隐藏依赖关系，可能错误重载，不支持嵌套类
> - 建议：对于值类型的类成员变量，不要改用为指针类型，应该 `#include` 对应的头文件

---

### Names and Order of Includes

``` cpp
#include "foo/bar/baz.h"  // self

#include <stdio.h>        // C & system

#include <string>         // C++

#include "base/macros.h"  // other libs
#include "foo/bar/qux.h"  // this project
```

> - [来源](https://google.github.io/styleguide/cppguide.html#Names_and_Order_of_Includes)
> - [SF.5: A `.cpp` file must include the `.h` file(s) that defines its interface](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rs-consistency)
> - 顺序：自身头文件（编译时检查接口一致性）+ 空行 + C/系统头文件 + 空行 + C++ 库 + 空行 + 其他库/当前项目

---

### Namespaces

``` cpp
namespace foo {
namespace bar {

using namespace std;  // Bad
using std::string;    // Good

}  // namespace bar
}  // namespace foo
```

> - [来源](https://google.github.io/styleguide/cppguide.html#Namespaces)
> - [SF.6: Use `using namespace` directives for transition, for foundation libraries (such as `std`), or within a local scope (only)](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rs-using)（Google Style 不允许）
> - [SF.7: Don’t write `using namespace` at global scope in a header file](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rs-using-directive)
> - 不要使用 [`inline namespace`](http://www.stroustrup.com/C++11FAQ.html#inline-namespace)

---

### Unnamed Namespaces and Static

``` cpp
// In .cc file
namespace {
constexpr auto kFoo = 42;
void BarImpl() { ... }
}  // namespace

static constexpr auto kFoo = 42;
static BarImpl() { ... }
```

> - [来源](https://google.github.io/styleguide/cppguide.html#Unnamed_Namespaces_and_Static_Variables)
> - [SF.21: Don’t use an unnamed (anonymous) namespace in a header](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rs-unnamed)
> - [SF.22: Use an unnamed (anonymous) namespace for all internal/nonexported entities](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rs-unnamed2)
> - 也可以使用 `static` 修饰常量/函数

---

### Local Variables

``` cpp
int i;               // Bad
i = f();
std::vector<int> v;  // Bad
v.push_back(1);
v.push_back(2);

int i = f();                  // Good
std::vector<int> v = {1, 2};  // Good
```

> - [来源](https://google.github.io/styleguide/cppguide.html#Unnamed_Namespaces_and_Static_Variables)
> - [ES.20: Always initialize an object](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Res-always)
> - [ES.21: Don’t introduce a variable (or constant) before you need to use it](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Res-introduce)
> - [ES.22: Don’t declare a variable until you have a value to initialize it with](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Res-init)

---

### Constructors

``` cpp
class Foo {
 public:
  Foo() noexcept { /* don't throw */ }
  void Init();            // Bad
  bool is_valid() const;  // Bad

  static Foo* CreateFoo(...);  // Good
};
```

> - [来源](https://google.github.io/styleguide/cppguide.html#Doing_Work_in_Constructors)
> - [NR.5: Don’t use two-phase initialization](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rnr-two-phase-init)
> - [C.40: Define a constructor if a class has an invariant](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rc-ctor)
> - [C.41: A constructor should create a fully initialized object](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rc-complete)
> - 可用异常的情况下，[C.42: If a constructor cannot construct a valid object, throw an exception](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rc-throw)
> - 禁用异常的情况下，可以使用工厂方法 `Create()` 构造（但不要同时提供两种接口，[Don't mix `Create()` factory methods and public constructors in one class](https://github.com/chromium/chromium/blob/master/styleguide/c%2B%2B/blink-c++.md#dont-mix-create-factory-methods-and-public-constructors-in-one-class)）
> - [C.82: Don’t call virtual functions in constructors and destructors](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rc-ctor-virtual)
> - [C.50: Use a factory function if you need “virtual behavior” during initialization](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rc-factory)

---

### Implicit Conversions

``` cpp
class Foo {
 public:
  Foo(Bar bar);             // Bad
  explicit Foo(Bar bar);    // Good
  operator Baz();           // Bad
  explicit operator Baz();  // Better
};
```

> - [来源](https://google.github.io/styleguide/cppguide.html#Implicit_Conversions)
> - [C.46: By default, declare single-argument constructors explicit](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rc-explicit)
> - [C.164: Avoid implicit conversion operators](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Ro-conversion)

---

### Copyable Types

``` cpp
class Foo {  // Movable?
 public:
  Foo(const Foo&) = default;
  Foo& operator=(const Foo&) = default;
  // move operations implicit deleted
  Foo(Foo&&) /* = delete */;
  Foo& operator=(Foo&&) /* = delete */;
};
```

> - [来源](https://google.github.io/styleguide/cppguide.html#Copyable_Movable_Types)
> - 可拷贝（可移动）：
>   - 需要写 `copy = default;` 和 `move = default;`
>   - 都不写（[C.20: If you can avoid defining default operations, do](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rc-zero)）
> - 可拷贝（不可移动）：只需要写 `copy = default;`，不需要写 `move = delete;`

---

### MoveOnly Types

``` cpp
class Foo {
 public:
  Foo(Foo&&) = default;
  Foo& operator=(Foo&&) = default;
  // copy operations implicit deleted
  // Foo(const Foo&) = delete;
  // Foo& operator=(const Foo&) = delete;
};
```

> - [来源](https://google.github.io/styleguide/cppguide.html#Copyable_Movable_Types)
> - 仅移动（不可拷贝）：只需要写 `move = default;`，不需要写 `copy = delete;`

---

### Not Copyable Or Movable Types

``` cpp
class Foo {
 public:
  Foo(const Foo&) = delete;
  Foo& operator=(const Foo&) = delete;
  // move operations implicit deleted
  // Foo(Foo&&) = delete;
  // Foo& operator=(Foo&&) = delete;
};
```

> - [来源](https://google.github.io/styleguide/cppguide.html#Copyable_Movable_Types)
> - [C.21: If you define or `=delete` any default operation, define or `=delete` them all](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rc-five)
> - 不可拷贝或移动：只需要写 `copy = delete;`，不需要写 `move = delete;`

---

### Structs vs. Classes

``` cpp
struct Foo {
  Foo();                // Bad
  void Init();          // Bad
  size_t size() const;  // Bad

  Bar bar_;  // Good if data-only
};
```

> - [来源](https://google.github.io/styleguide/cppguide.html#Structs_vs._Classes)
> - [C.2: Use `class` if the class has an invariant; use `struct` if the data members can vary independently](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rc-struct)
> - [C.8: Use `class` rather than `struct` if any member is non-public](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rc-class)

---

### Access Control

``` cpp
class Foo {
 public:
  Bar bar_;                     // Bad
  static constexpr int k = 42;  // Good
 private:
  Baz baz_;                     // Good
  static Qux qux_;              // Good
};
```

> - [来源](https://google.github.io/styleguide/cppguide.html#Access_Control)
> - [C.9: Minimize exposure of members](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rc-private)
> - [C.133: Avoid `protected` data](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-protected)

---

### Declaration Order

``` cpp
class Foo {
 public:
  using Bar = std::string;  // +constants
  static Foo* CreateFoo();  // +ctor/dtor
  size_t size() const;
 private:
  Bar bar_;
};
```

> - [来源](https://google.github.io/styleguide/cppguide.html#Declaration_Order)
> - [NL.16: Use a conventional class member declaration order](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rl-order)
> - 顺序：`public` + `protected` + `private`（空则不写）
> - 顺序：类型（`typedef`/`using`/嵌套类）+ 常量（`constexpr`/`enum`）+ 工厂方法 + 构造函数 + 构造函数（拷贝/移动）+ 赋值函数（拷贝/移动）+ 析构函数 + 成员函数（方法）+ 数据成员
> - 注意：不要在类定义中实现长函数，因为会内联

---

### Inheritance

``` cpp
class Foo : private Bar,    // Bad
            protected Baz,  // Bad
            public Qux {    // Good
  virtual void FromBar();   // Bad
  void FromBar() override;  // Good
  int FromBar() override;   // Error
};
```

> - [来源](https://google.github.io/styleguide/cppguide.html#Inheritance)
> - [C.128: Virtual functions should specify exactly one of `virtual`, `override`, or `final`](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-override)
> - [C.135: Use multiple inheritance to represent multiple distinct interfaces](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-mi-interface)
> - [C.136: Use multiple inheritance to represent the union of implementation attributes](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-mi-implementation)（Google Style 不允许）
> - 编译器无法检查 [C.140: Do not provide different default arguments for a virtual function and an overrider](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-virtual-default-arg)
> - 不要使用 `private` 继承，避免使用 `protected` 继承

---

### Parameters and Arguments

``` cpp
bool Foo(T in_mutable,
         const T& in_const,
         T&& in_will_move_from,
         T* out,
         T* /* T& */ in_out = nullptr) {
  DCHECK(out);     // not-checked
  if (!in_out) {}  // checked if nullable
}
```

> - [来源 1](https://google.github.io/styleguide/cppguide.html#Output_Parameters) / [来源 2](https://google.github.io/styleguide/cppguide.html#Reference_Arguments) / [来源 3](https://google.github.io/styleguide/cppguide.html#Rvalue_references)
> - 可变的输入参数：`T`，不变的输入参数：`const T&`（Google Style & [F.16: For “in” parameters, pass cheaply-copied types by value and others by reference to `const`](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-in)）
> - 被移动的输入参数：`T&&`（[F.18: For “will-move-from” parameters, pass by `X&&` and `std::move` the parameter](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-consume)）
> - 输出参数：
>   - `T*`（Google Style）
>   - 通过（多个）返回值化简（[F.21: To return multiple “out” values, prefer returning a struct or tuple](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-out-multi)）
> - 输入/输出参数（避免使用）：
>   - `T*`（Google Style）
>   - `T&`（[F.17: For “in-out” parameters, pass by reference to non-`const`](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-inout)）

---

### Smart Pointers Ownership

``` cpp
class Foo {
  std::unique_ptr<T> owned_;
  std::shared_ptr<T> shared_;
  std::weak_ptr<T> referred_;
  T* referred_without_check_;
};
```

> - [来源](https://google.github.io/styleguide/cppguide.html#Ownership_and_Smart_Pointers)
> - [R.20: Use `unique_ptr` or `shared_ptr` to represent ownership](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rr-owner)
> - [R.21: Prefer `unique_ptr` over `shared_ptr` unless you need to share ownership](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rr-unique)
> - [R.24: Use `std::weak_ptr` to break cycles of `shared_ptr`s](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rr-weak_ptr)

---

### Smart Pointers Parameters

``` cpp
void f(T*);                   // use
void f(std::unique_ptr<T>);   // own
void f(std::shared_ptr<T>);   // share
void f(std::unique_ptr<T>&);  // reseat
void f(std::shared_ptr<T>&);  // reseat?
```

> - [来源](https://google.github.io/styleguide/cppguide.html#Ownership_and_Smart_Pointers)
> - [R.30: Take smart pointers as parameters only to explicitly express lifetime semantics](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rr-smartptrparam)
> - [R.32: Take a `unique_ptr<widget>` parameter to express that a function assumes ownership of a `widget`](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rr-uniqueptrparam)
> - [R.33: Take a `unique_ptr<widget>&` parameter to express that a function reseats the `widget`](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rr-reseat)
> - [R.34: Take a `shared_ptr<widget>` parameter to express that a function is part owner](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rr-sharedptrparam-owner)
> - [R.35: Take a `shared_ptr<widget>&` parameter to express that a function might reseat the shared pointer](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rr-sharedptrparam)

---

### Smart Pointers Dangling

``` cpp
std::shared_ptr<Bar> Foo::bar();

UseBarPtr(foo.bar().get());  // Bad
foo.bar()->BarMemFunc();     // Bad

auto bar_pinned = foo.bar();
UseBarPtr(bar_pinned);       // Good
bar_pinned->BarMemFunc();    // Good
```

> - [来源](https://google.github.io/styleguide/cppguide.html#Ownership_and_Smart_Pointers)
> - [R.37: Do not pass a pointer or reference obtained from an aliased smart pointer](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rr-smartptrget)

---

### Exceptions

``` cpp
try {
  MayThrowBar();   // throw Bar
  auto x = i / 0;  // crash only
} catch (const Baz& baz) {
  // Missed, Bar is not Baz
}
```

> - [来源](https://google.github.io/styleguide/cppguide.html#Exceptions)
> - 避免使用异常，尽量使用显式的错误处理
> - 不同观点：[NR.3: Don’t avoid exceptions](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rnr-no-exceptions)

---

### Run-Time Type Information (RTTI)

``` cpp
Derived* derived =
    dynamic_cast<Derived*>(base);
if (!derived) return;  // not-derived
derived->Foo();        // is-derived

// use polymorphic virtual function
int Base::Foo() { /* do nothing */ }
int Derived::Foo() { /* do something */ }
```

> - [来源](https://google.github.io/styleguide/cppguide.html#Run-Time_Type_Information__RTTI_)
> - [C.153: Prefer virtual function to casting](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-use-virtual)
>   - 避免使用 `typeid` 检查类型 或 `dynamic_cast` 向下转换（以及 type-tag 和 `static_cast`），可用 类层次关系 _(class hierarchy)_ 改写：
>   - 用 虚函数多态 实现 单派发 _(single dispatch)_
>   - 用 [访问者模式](../2017/Design-Patterns-Notes-3.md#Visitor) 实现 双派发 _(double dispatch)_
> - 不同观点：[C.146: Use `dynamic_cast` where class hierarchy navigation is unavoidable](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-dynamic_cast)
>   - [C.148: Use `dynamic_cast` to a pointer type when failure to find the required class is considered a valid alternative](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-ptr-cast)
>   - [Capability Query](https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Capability_Query)
>   - [Acyclic Visitor - Robert C. Martin](https://condor.depaul.edu/dmumaugh/OOT/Design-Principles/acv.pdf)

---

### Lambda expressions

``` cpp
void Foo::Bar(Executor executor) {
  Baz baz;
  executor->Schedule(
    [&] { Use(baz); }          // Bad
    [=] { Use(baz); }          // Bad
    [this, baz] { Use(baz); }  // Good
  );  // Use() may be member function!
}
```

> - [来源](https://google.github.io/styleguide/cppguide.html#Lambda_expressions)
> - [F.52: Prefer capturing by reference in lambdas that will be used locally, including passed to algorithms](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-reference-capture)
> - [F.53: Avoid capturing by reference in lambdas that will be used nonlocally, including returned, stored on the heap, or passed to another thread](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-value-capture)
> - [F.54: If you capture `this`, capture all variables explicitly (no default capture)](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-this-capture)
> - 如果能保证上下文有效性，可以使用隐式捕获（[考虑闭包上下文的有效性](../2019/Inside-Cpp-Callback.md#回调时（弱引用）上下文会不会失效)）

---

### `sizeof`

``` cpp
/* Foo */ Bar data;
memset(&data, 0, sizeof(Foo));   // Bad
memset(&data, 0, sizeof(data));  // Good
```

> - [来源](https://google.github.io/styleguide/cppguide.html#sizeof)
> - 如果和变量有关，尽量使用 `sizeof(varname)`，而不是 `sizeof(type)`（因为类型会变化，但不易察觉）

### Preincrement and Predecrement

``` cpp
for (auto i = ; i != ; ++i) {}  // Good
for (auto i = ; i != ; i++) {}  // Bad
```

> - [来源](https://google.github.io/styleguide/cppguide.html#Preincrement_and_Predecrement)
> - 尽量使用 `++i`/`--i`，避免使用 `i++`/`i--` 生成不需要的临时对象（如果需要，可以使用）

---

### Use of `const`/`constexpr`

``` cpp
class Foo {
 public:
  const Ret& Bar(const Param*) const;
  static constexpr auto kBaz = 42;
};
for (const auto& item : arr) {
  const auto size = ...;  // Not Required
}
```

> - [来源 1](http://google.github.io/styleguide/cppguide.html#Use_of_const) / [来源 2](https://google.github.io/styleguide/cppguide.html#Use_of_constexpr)
> - 需要 `const`：函数引用/指针-参数/返回值、成员函数限定符、常量（优先使用 `constexpr`）、循环迭代器引用
> - 不用 `const`：局部变量
> - [Con.2: By default, make member functions `const`](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rconst-fct)
> - [Con.3: By default, pass pointers and references to `const`s](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rconst-ref)
> - [Con.5: Use `constexpr` for values that can be computed at compile time](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rconst-constexpr)

---

### Pointer and Reference Expressions

``` cpp
int x, y;         // Good
int* x;           // Good
int* x, y;        // Bad, y is not ptr
int& x = ;        // Good
int& x = , y = ;  // Bad, y is not ref
```

> - [来源](https://google.github.io/styleguide/cppguide.html#Pointer_and_Reference_Expressions)
> - [ES.10: Declare one name (only) per declaration](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Res-name-one)（Google Style 放宽限制）
> - [NL.18: Use C++-style declarator layout](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rl-ptr)

---

### Naming & Formatting

``` cpp
class MyClass : public OtherClass {
 public:
  MyClass();
  explicit MyClass(int var);
  ~MyClass() = default;

  void SomeFunction();
  void SomeFunctionThatDoesNothing() {}

  void set_var(int var) { var_ = var; }
  int var() const { return var_; }

 private:
  bool SomeInternalFunction();

  int var_;
  int other_var_;
};
```

> - [来源 1](https://google.github.io/styleguide/cppguide.html#Naming) / [来源 2](https://google.github.io/styleguide/cppguide.html#Formatting)
> - ![Google C++ Style](Cpp-Better-Practice/google-cpp-style.png)

---

<br/>
<br/>

[align-center]

## Chromium 风格

<br/>
<br/>

---

### Variable Initialization

``` cpp
int i = 1;
std::string s = "Hello";
std::vector<int> v = {1, 2, 3};

Foo foo(1.7, false, "test");
std::vector<double> v(500, 0.97);

Bar bar{std::string()};  // explicit only
```

> - [来源](https://github.com/chromium/chromium/blob/master/styleguide/c++/c++-dos-and-donts.md#variable-initialization)
> - 优先级：赋值写法 > 构造写法 >（仅当前两种写法不可用时）C++ 11 `{}` 写法
> - 上述例子中，`T var(T());` 会导致 [most vexing parse](https://www.fluentcpp.com/2018/01/30/most-vexing-parse/) 歧义
> - 不同观点：[ES.23: Prefer the `{}`-initializer syntax](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Res-list)

---

### In-class Initializers

``` cpp
class Foo {
  int b_ = 42;          // use |b| or 42
  std::string c_;       // default ctor
  Bar<Foo> bar_{this};  // use |this|
 public:
  Foo() = default;       // default
  Foo(int b) : b_(b) {}  // set |b_|
};
```

> - [来源](https://github.com/chromium/chromium/blob/master/styleguide/c++/c++-dos-and-donts.md#initialize-members-in-the-declaration-where-possible)
> - [C.45: Don’t define a default constructor that only initializes data members; use member initializers instead](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rc-default)
> - [C.48: Prefer in-class initializers to member initializers in constructors for constant initializers](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rc-in-class-initializer)

---

### Prefer `=default`

``` cpp
class Foo {
 public:
  Foo();                      // Below
  Foo(const Foo&) = default;  // Good
  Foo(Foo&&) /* noexcept */;  // Bad
  Foo(/* const */ Foo&);      // Bad
};
Foo::Foo() = default;         // Good
```

> - [来源](https://github.com/chromium/chromium/blob/master/styleguide/c++/c++-dos-and-donts.md#prefer-to-use-default)
> - [C.80: Use `=default` if you have to be explicit about using the default semantics](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rc-eqdefault)
> - 移动构造函数容易漏掉 `noexcept`（`=default` 会自动加上），导致 [不能高效使用标准库和语言工具](../2018/Cpp-Rvalue-Reference.md#误解：手写错误的移动构造函数)
> - 编译器无法检查 拷贝构造函数参数 `const T&` 误写为 `T&`，从而导致 [无法重载右值参数](../2018/Cpp-Rvalue-Reference.md#引用参数重载优先级)

---

### `auto* p = ...;`

``` cpp
auto item = new Item();   // Bad
auto* item = new Item();  // Good
```

> - [来源](https://github.com/chromium/chromium/blob/master/styleguide/c++/c++-dos-and-donts.md#do-not-use-auto-to-deduce-a-raw-pointer)
> - 增强可读性，编译时检查 `item` 必须为指针类型

<br/>

### Comment Style

``` cpp
// FooImpl implements the FooBase class.
// FooFunction() modifies |foo_member_|.
```

> - [来源](https://github.com/chromium/chromium/blob/master/styleguide/c++/c++-dos-and-donts.md#comment-style)
> - `FooImpl`/`FooBase` 为类名
> - `FooFunction` 函数后边加 `()`
> - `foo_member_` 变量外边加 `||`

---

<br/>
<br/>

[align-center]

## 核心指南

<br/>
<br/>

---

### Intent

``` cpp
TODO
```

> - [P.1: Express ideas directly in code](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rp-direct)
> - [P.3: Express intent](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rp-what)
> - [NL.1: Don’t say in comments what can be clearly stated in code](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rl-comments)

---

### Names and Scopes

``` cpp
auto i = GetStartIndex();
for (i = ; i != ; i++) {}       // Bad
for (auto i = ; i != ; ++i) {}  // Good

if (auto foo = GetFoo()) {      // Good
  Reuse(i);                     // Bad
  auto i = GetObject();         // Bad
} else { /* handle nullptr */ }
```

> - [ES.5: Keep scopes small](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Res-scope)
> - [ES.6: Declare names in for-statement initializers and conditions to limit scope](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Res-cond)
> - [ES.12: Do not reuse names in nested scopes](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Res-reuse)
> - [ES.26: Don’t use a variable for two unrelated purposes](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Res-recycle)

---

### Lambda for Complex Initialization

``` cpp
using Map =
    std::map<int, std::unique_ptr<Foo>>;
const Map foo_map = [] {
  Map r;
  r.emplace(1, std::make_unique<Bar>());
  r.emplace(2, std::make_unique<Baz>());
  return r;
}();
```

> - [ES.28: Use lambdas for complex initialization, especially of `const` variables](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Res-lambda-init)
> - 不能直接用 C++ 11 `{}` 写法构造，因为 `std::unique_ptr` 不可拷贝：[How to uniform initialize `map` of `unique_ptr`?](https://stackoverflow.com/questions/17180673/how-to-uniform-initialize-map-of-unique-ptr)

---

### RAII

``` cpp
// Bad
FILE* file = fopen(filename, mode);
if (file) return;        // check nullptr
/* ... */ fclose(file);  // multi-close()

// Good
std::unique_ptr<FILE, decltype(fclose)*>
    file(fopen(filename, mode), fclose);
```

> - [R.1: Manage resources automatically using resource handles and RAII (Resource Acquisition Is Initialization)](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rr-raii)
> - [E.19: Use a final_action object to express cleanup if no suitable resource handle is available](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Re-finally)

---

### Virtual Destructors

``` cpp
class Strategy {
 public:
  virtual ~Strategy() = default;
};
class Observer {
 protected:
  ~Observer() = default;
};
```

> - [C.35: A base class destructor should be either public and virtual, or protected and nonvirtual](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rc-dtor-virtual)
> - [C.127: A class with a virtual function should have a virtual or protected destructor](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-dtor)
> - 不同用法：
>   - `Strategy` 通过基类 管理生命周期，可 析构派生类
>   - `Observer` 仅使用 基类的接口，自行管理 生命周期

---

### Polymorphic Class Slicing

``` cpp
void PolyFoo(Base base);         // Bad
void PolyFoo(const Base& base);  // Good
void PolyFoo(Base* base);        // Good

std::vector<Base> foos;          // Bad
std::vector<Base*> foos;         // Good
std::vector<std::unique_ptr<Base>>
    foos;                       // Better
```

> - [ES.63: Don’t slice](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Res-slice)
> - [C.67: A polymorphic class should suppress copying](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rc-copy-virtual)
> - [C.130: For making deep copies of polymorphic classes prefer a virtual clone function instead of copy construction/assignment](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-copy)
> - [C.145: Access polymorphic objects through pointers and references](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-poly)

---

### Assertion

``` cpp
TODO
```

> - TODO

---

### Q & A [no-toc]

<br/>
<br/>
<br/>
<br/>

[align-right]

# 谢谢 🙂 [no-toc]

---
