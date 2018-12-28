# 简单的 C++ 结构体字段反射

> 2019/1/1
> 
> 基于 C++ 原生语法，不到 100 行代码：让编译器帮你写 **序列化/反序列化** 代码，告别体力劳动 🙃

## TOC

[TOC]

## 背景

很多人喜欢把程序员称为 **码农**，程序员也经常嘲讽自己每天都在 **搬砖**。这时候，大家会想：能否构造出一些 **更好的工具**，代替我们做那些无意义的 **体力劳动** 中呢？

在实际 C++ 项目中，我们经常需要实现一些与外部系统交互的 **接口** —— 外部系统传入 JSON 参数，我们的程序处理后，再以 JSON 的格式传回外部系统。这个过程就涉及到了两次数据结构的转换：

- 输入的 JSON 转换为 C++ 数据结构（**反序列化**, _deserialization_）
- C++ 数据结构 转换为 输出的 JSON（**序列化**, _serialization_）

如果传输的 JSON 数据格式比较复杂，那么序列化/反序列化的代码也会变得非常复杂 —— 需要处理 **结构嵌套**、**可选字段**、**输入合法性检查** 等问题。如果为每个 JSON 数据结构都 **人工手写** 一套序列化/反序列化代码，那么 **工作量** 会特别大。

例如，[chromium headless 的 devtools 接口](https://github.com/chromium/chromium/blob/master/headless/public/internal/headless_devtools_client_impl.h) 里就定义了 33 个 **领域模型** _(domain model)_，而每个模型中又定义了许多字段。如果针对每个模型编写序列化/反序列化代码，那么 Google 的工程师是不会乐意的。。。😑 所以，他们构建了一套 [代码生成工具](https://github.com/chromium/chromium/tree/master/components/autofill_assistant/browser/devtools)，帮助程序员完成这些体力劳动。

如果觉得引入一套新的代码生成工具的成本比较高，那么我们不妨考虑让 **编译器** 帮我们完成 **代码生成** 的工作。

## 目标

- 基于 C++ **原生语法**，不需要引入第三方库
- 提供 **声明式** 的方法，只需要声明 **格式** _(schema)_，不需要实现具体逻辑
- 不会带来 **额外的运行时负担**，能达到和手写代码一样的运行时效率

给定一个 C++ 结构体 `SimpleStruct`：

``` cpp
struct SimpleStruct {
  bool bool_;
  int int_;
  double double_;
  std::string string;
  std::vector<double> vector;
  std::unique_ptr<bool> optional;
};
```

- 由于 [`std::optional`](https://en.cppreference.com/w/cpp/utility/optional) 需要 C++ 17 支持，所以我们使用 [`std::unique_ptr`](https://en.cppreference.com/w/cpp/memory/unique_ptr) 表示 **可选字段**
- 针对 **可选字段** 的 JSON 序列化/反序列化 **扩展**，见 [`optional_json.h`](Cpp-Struct-Field-Reflection/optional_json.h)（参考：[How do I convert third-party types? | nlohmann/json](https://github.com/nlohmann/json#how-do-i-convert-third-party-types)）

基于 [nlohmann 的 C++ JSON 库](https://github.com/nlohmann/json)，我们需要：

``` cpp
// input
json json_input = json::parse(
    "{"
    "  \"bool\": true,"
    "  \"int\": 1,"
    "  \"double\": 1.0,"
    "  \"string\": \"hello json\","
    "  \"vector\": [1, 1.0]"
    "}");
SimpleStruct object = json_input.get<SimpleStruct>();

// use
object.string += " in simple struct";

// output
json json_output = json(object);
std::string string_output = json_output.dump();
```

- 调用 `json::parse` 从字符串得到输入 JSON 对象（JSON 库已实现）
- 通过反序列化接口 `json::get<SimpleStruct>()` 得到 `object`（需要我们实现）
- 通过序列化，用修改后的 `object` 构造输出 JSON 对象 `json_output`（需要我们实现）
- 调用 `json::dump` 将 JSON 对象转为用于输出的字符串（JSON 库已实现）

实现从 C++ 结构体到 JSON 的序列化/反序列化操作，需要以下几个信息：

- 结构体有哪些字段
- 每个字段在结构体中的什么位置
- 每个字段在 JSON 中对应的名称是什么

你只需要告诉编译器上述信息，他就可以帮你生成类似自己手写的代码了。

## 实现

### 人工手写 序列化/反序列化 代码

> [代码链接](Cpp-Struct-Field-Reflection/raw_json.cc)

``` cpp
void to_json(nlohmann::json& j, const SimpleStruct& value) {
  j["bool"] = value.bool_;
  j["int"] = value.int_;
  j["double"] = value.double_;
  j["string"] = value.string;
  j["vector"] = value.vector;
  j["optional"] = value.optional;
}

void from_json(const nlohmann::json& j, SimpleStruct& value) {
  j.at("bool").get_to(value.bool_);
  j.at("int").get_to(value.int_);
  j.at("double").get_to(value.double_);
  j.at("string").get_to(value.string);
  j.at("vector").get_to(value.vector);
  if (j.find("optional") != j.cend()) {
    j.at("optional").get_to(value.optional);
  }
}
```

- 通过人工编写的方法，包含了字段的位置信息、名称信息
- 如果字段实际类型和输入类型不匹配，函数 `json::get_to` 抛出异常
- 如果某个字段不存在，函数 `json::at` 抛出异常
- 如果可选字段不存在，我们就不能调用 `json::at`

得益于 [nlohmann 的 C++ JSON 库](https://github.com/nlohmann/json) 易用性比较好，已经支持了 **结构嵌套** 的处理，并通过异常处理机制实现 **输入合法性检查**。如果我们使用 [chromium base 的 Value](https://github.com/chromium/chromium/blob/master/base/values.h) 处理 JSON，还需要写更多行代码。

### 动态反射

> [代码链接](Cpp-Struct-Field-Reflection/dynamic_reflection.h) | [样例代码链接](Cpp-Struct-Field-Reflection/dynamic_iostream.cc)

Google 的工程师也发现了解析 `base::Value` 的复杂性，于是构建了一种基于 **动态反射** _(dynamic reflection)_ 的反序列化机制，支持 **结构嵌套** 的情况。

### 静态反射

> [代码链接](Cpp-Struct-Field-Reflection/static_reflection.h) | [样例代码链接](Cpp-Struct-Field-Reflection/static_iostream.cc)

### 编译器生成 序列化/反序列化 代码

> [代码链接](Cpp-Struct-Field-Reflection/reflection_json.cc)

## 写在最后 [no-toc]

如果还有人认为 C++ 元编程就是 **屠龙之技**，那可能是因为他们还在 **手写重复的代码**。掌握 C++ 元编程，自己打造工具，解放生产力，告别搬砖的生活！

> 延伸阅读：
> 
> - [浅谈 C++ 元编程](../2017/Cpp-Metaprogramming.md)：程序设计语言结课论文，总结了 C++ 元编程的原理、发展和应用
> - [如何设计一个简单的 C++ ORM](../2016/How-to-Design-a-Naive-Cpp-ORM.md) / [如何设计一个更好的 C++ ORM](../2016/How-to-Design-a-Better-Cpp-ORM.md)：早期两篇个人技术博客，介绍了如何使用元编程技巧实现 C++ ORM（由于当时还在学习阶段，介绍的技术方案有待优化）

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2019, BOT Man
