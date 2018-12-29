# 简单的 C++ 结构体字段反射

> 2019/1/1
> 
> 基于 C++ 原生语法，不到 100 行代码：让编译器帮你写 **序列化/反序列化** 代码，告别体力劳动 🙃

## 背景

很多人喜欢把程序员称为 **码农**，程序员也经常嘲讽自己每天都在 **搬砖**。这时候，大家会想：能否构造出一些 **更好的工具**，代替我们做那些无意义的 **体力劳动** 中呢？

在实际 C++ 项目中，我们经常需要实现一些与外部系统交互的 **接口** —— 外部系统传入 JSON 参数，我们的程序处理后，再以 JSON 的格式传回外部系统。这个过程就涉及到了两次数据结构的转换：

- 输入的 JSON 转换为 C++ 数据结构（**反序列化**, _deserialization_）
- C++ 数据结构 转换为 输出的 JSON（**序列化**, _serialization_）

如果传输的 JSON 数据格式比较复杂，那么序列化/反序列化的代码也会变得非常复杂 —— 需要处理 **结构嵌套**、**可选字段**、**输入合法性检查** 等问题。如果为每个 JSON 数据结构都 **人工手写** 一套序列化/反序列化代码，那么 **工作量** 会特别大。

例如，[chromium/headless 的 devtools 接口](https://github.com/chromium/chromium/blob/master/headless/public/internal/headless_devtools_client_impl.h) 里就定义了 33 个 **领域模型** _(domain model)_，而每个模型中又定义了许多字段。如果针对每个模型编写序列化/反序列化代码，那么 Google 的工程师是不会乐意的。。。😑 所以，他们构建了一套 [代码生成工具](https://github.com/chromium/chromium/tree/master/components/autofill_assistant/browser/devtools)，帮助程序员完成这些体力劳动。

如果觉得引入一套新的代码生成工具的成本比较高，那么我们不妨考虑让 **编译器** 帮我们完成 **代码生成** 的工作。

## 目标

- 基于 C++ **原生语法**，不需要引入第三方库
- 提供 **声明式** 的方法，只需要声明 **格式** _(schema)_，不需要实现具体逻辑
- 不会带来 **额外的运行时开销**，能达到和手写代码一样的运行时效率

基于 [nlohmann 的 C++ JSON 库](https://github.com/nlohmann/json)，给定一个 C++ 结构体 `SimpleStruct`：

``` cpp
struct SimpleStruct {
  bool bool_;
  int int_;
  double double_;
  std::string string_;
  std::vector<double> vector_;
  std::unique_ptr<bool> optional_;
};
```

- 由于 [`std::optional`](https://en.cppreference.com/w/cpp/utility/optional) 需要 C++ 17 支持，所以我们使用 [`std::unique_ptr`](https://en.cppreference.com/w/cpp/memory/unique_ptr) 表示 **可选字段**
- 针对 **可选字段** 的 JSON 序列化/反序列化 **扩展代码**，见 [`optional_json.h`](Cpp-Struct-Field-Reflection/optional_json.h)（参考：[How do I convert third-party types? | nlohmann/json](https://github.com/nlohmann/json#how-do-i-convert-third-party-types)）

一般的业务处理，往往包括三部分：

- 解析输入（字符串到 JSON 对象的转换 + JSON 对象到领域模型的 **反序列化**）
- 处理业务逻辑（实现接口的核心代码）
- 转储输出（领域模型到 JSON 对象的 **序列化** + JSON 对象到字符串的转换）

``` cpp
// input
json json_input = json::parse(
    "{"
    "  \"_bool\": true,"
    "  \"_int\": 1,"
    "  \"_double\": 1.0,"
    "  \"_string\": \"hello json\","
    "  \"_vector\": [1, 1.0]"
    "}");
SimpleStruct object = json_input.get<SimpleStruct>();

// use
object.string += " in simple struct";

// output
json json_output = json(object);
std::string string_output = json_output.dump();
```

- 对于 JSON 对象和字符串之间的转换，主流的 **JSON 库都实现** 了：
  - 调用 `json::parse` 从字符串得到输入 JSON 对象
  - 调用 `json::dump` 将 JSON 对象转为用于输出的字符串
- 而 JSON 对象和 C++ 结构体之间的转换，**需要我们实现**：
  - 通过反序列化，调用 `json::get<SimpleStruct>()` 得到 `SimpleStruct object`
  - 通过序列化，使用 `object` 构造输出 JSON 对象

## 实现

实现从 C++ 结构体到 JSON 的序列化/反序列化操作，需要以下几个信息：

- 结构体有 **哪些字段**（`bool_`/`int_`/`double_` ...）
- 每个字段在 **结构体中** 的什么 **位置**（`&SimpleStruct::bool_`/`&SimpleStruct::int_`/`&SimpleStruct::double_` ...）
- 每个字段在 **JSON 中** 对应的 **名称** 是什么（`_bool`/`_int`/`_double` ...）
- 每个字段如何从 C++ 到 JSON 进行 **类型映射**（`bool` 对应 `Boolean`，`int` 对应 `Number(Integer)`，`double` 对应 `Number` ...）
- 如何处理序列化/反序列化中的 **异常**（抛出异常 or 返回空值 or 跳过可选字段）

对于很多支持 [**反射** _(reflection)_](https://en.wikipedia.org/wiki/Reflection_%28computer_programming%29) 的语言，**JSON 的解析者** 可以通过反射接口，查询到 `SimpleStruct` 的这些信息。尽管 C++ 支持 [**运行时类型信息** _(RTTI, run-time type information)_](https://en.wikipedia.org/wiki/Run-time_type_information)，但能取到的信息有限。所以，对于暂时还 **不支持反射的 C++ 语言**，**JSON 的解析者** 得不到这些信息，**`SimpleStruct` 的定义者** 需要把这些信息告诉 **JSON 的解析者**。

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

- 通过人工编写的方法，在 `to_json`/`from_json` 定义字段的位置信息、名称信息
- 如果字段实际类型和输入类型不匹配，函数 `json::get_to` 抛出异常
- 如果某个字段不存在，函数 `json::at` 抛出异常
- 如果可选字段不存在，我们就不能调用 `json::at`

### 动态反射

手写 `to_json`/`from_json` 需要写 2 份类似的代码，导致 **代码冗余**；另外，这两份代码还需要做不相似的处理（例如，判断字段是否为 **可选字段**）。

得益于 [nlohmann 的 C++ JSON 库](https://github.com/nlohmann/json) 易用性比较好 —— 已经支持了 **结构嵌套** 的处理，并通过异常处理机制实现 **输入合法性检查** —— 即使手写代码，写起来也比较简单。如果我们使用 [chromium 的 `base::Value`](https://github.com/chromium/chromium/blob/master/base/values.h) 处理 JSON，还需要 **手写更多代码**。

Google 的工程师也发现了解析 `base::Value` 是个 **体力劳动**，于是构建了一种基于 **动态反射** _(dynamic reflection)_ 的反序列化机制，实现 **写更少的代码，实现更复杂的操作**。（参考：[`JSONValueConverter`](https://github.com/chromium/chromium/blob/master/base/json/json_value_converter.h)）

**核心原理** 是：利用 [**适配器模式** _(adapter pattern)_](../2017/Design-Patterns-Notes-2.md#Adapter)，通过 **运行时多态** _(runtime polymorphism)_ 机制，实现对具体类型的抹除。

> [代码链接](Cpp-Struct-Field-Reflection/dynamic_reflection.h)

首先，为不同 **字段类型** 定义一个通用的转换接口 `ValueConverter<FieldType>`，用于存储实际的 C++ 类型与 JSON 类型的转换操作（**仅关联操作的字段类型，抹除具体的转换动作**）：

``` cpp
template <typename FieldType>
using ValueConverter =
    std::function<void(FieldType* field, const std::string& name)>;
```

然后，为不同类型的 **结构体** 定义一个通用的转换接口 `FieldConverterBase<StructType>`，用于存储任意字段的转换操作（**仅关联结构体的类型，抹除操作的字段信息**）：

``` cpp
template <typename StructType>
class FieldConverterBase {
 public:
  virtual ~FieldConverterBase() = default;
  virtual void operator()(StructType* obj) const = 0;
};
```

接着，通过 `FieldConverter<StructType, FieldType>` 将上边两个接口关联起来，存储特定 **结构体** 的特定 **字段** 的实际转换操作，同时关联到 **字段名称** `field_name_` 和 **字段位置** `field_pointer_`（**实现  `FieldConverterBase` 接口，关联具体结构体、字段信息**）：

``` cpp
template <typename StructType, typename FieldType>
class FieldConverter : public FieldConverterBase<StructType> {
 public:
  FieldConverter(const std::string& name,
                 FieldType StructType::*pointer,
                 ValueConverter<FieldType> converter)
      : field_name_(name),
        field_pointer_(pointer),
        value_converter_(converter) {}

  void operator()(StructType* obj) const override {
    return value_converter_(&(obj->*field_pointer_), field_name_);
  }

 private:
  std::string field_name_;
  FieldType StructType::*field_pointer_;
  ValueConverter<FieldType> value_converter_;
};
```

最后，针对特定 **结构体** 定义一个存储 **所有字段** 信息（名称、位置、转换函数）的容器 `StructValueConverter<StructType>`，并提供添加字段、执行所有字段操作的接口（**仅关联结构体的类型，利用 `FieldConverterBase` 抹除操作的字段信息**）：

``` cpp
template <class StructType>
class StructValueConverter {
 public:
  template <typename FieldType>
  void RegisterField(FieldType StructType::*field_pointer,
                     const std::string& field_name,
                     ValueConverter<FieldType> value_converter) {
    fields_.push_back(std::make_unique<FieldConverter<StructType, FieldType>>(
        field_name, field_pointer, std::move(value_converter)));
  }

  void operator()(StructType* obj) const {
    for (const auto& field_converter : fields_) {
      (*field_converter)(obj);
    }
  }

 private:
  std::vector<std::unique_ptr<FieldConverterBase<StructType>>> fields_;
};
```

> [使用样例代码链接](Cpp-Struct-Field-Reflection/dynamic_iostream.cc)

具体使用时，只需要两步：

1. 构造 `converter`，调用 `RegisterField` 动态 **绑定字段信息**（名称、位置、转换函数）
2. 调用 `converter(&simple)` 对所有注册了的字段 **进行转换**

``` cpp
struct SimpleStruct {
  int int_;
  std::string string_;
};

// setup converter
auto int_converter = [](int* field, const std::string& name) {
  std::cout << name << ": " << *field << std::endl;
};
auto string_converter = [](std::string* field, const std::string& name) {
  std::cout << name << ": " << *field << std::endl;
};

StructValueConverter<SimpleStruct> converter;
converter.RegisterField(&SimpleStruct::int_, "int",
                        ValueConverter<int>(int_converter));
converter.RegisterField(&SimpleStruct::string_, "string",
                        ValueConverter<std::string>(string_converter));

// use converter
SimpleStruct simple{2, "hello dynamic reflection"};
converter(&simple);

// output:
//   int: 2
//   string: hello dynamic reflection
```

### 静态反射

实际上，实现序列化/反序列化所需要的信息（有哪些字段，每个字段的位置、名称、映射方法，如何处理异常），在 **编译时** _(compile-time)_ 就已经确定了。所以，我们可以利用 **静态反射** _(static reflection)_ 的方法，把这些信息 **告诉编译器**，让它帮我们 **生成代码**，并且不会带来额外的运行时开销。

**核心原理** 是：利用 [`std::tuple`](https://en.cppreference.com/w/cpp/utility/tuple) 存储 **字段信息**（位置、名称），然后遍历所有字段（有哪些字段），进行针对某个 **字段类型** 的转换操作（映射方法）。

> [代码链接](Cpp-Struct-Field-Reflection/static_reflection.h)

首先，定义一个 `StructSchema<StructType>` 函数模板，针对特定的 **结构体** **重载**，返回所有字段信息（用 `std::tuple` 存储，默认返回空信息）：

``` cpp
template <typename T>
inline constexpr auto StructSchema() {
  return std::make_tuple();
}
```

然后，定义一个 `ForEachField<StructType>` 函数，从 `StructSchema` 取出对应存储 **结构体** 所有 **字段信息** 的 `std::tuple`，然后遍历这些信息，从中取出 **字段的位置和名称**，作为参数调用 `fn`：

``` cpp
template <typename T, typename Fn>
inline constexpr void ForEachField(T&& value, Fn&& fn) {
  constexpr auto struct_schema = StructSchema<std::decay_t<T>>();
  detail::ForEachTuple(struct_schema, [&value, &fn](auto&& field_schema) {
    fn(value.*(std::get<0>(std::forward<decltype(field_schema)>(field_schema))),
       std::get<1>(std::forward<decltype(field_schema)>(field_schema)));
  });
}
```

- `StructSchema` 返回 tuple 结构是：`((&field1, name1), (&field2, name2), ...)`
- `fn` 接受的参数分别为：`(field_value, field_name)`，其中通过结构体成员指针得出 `field_value == value.*field`
- 具体的 `ForEachTuple` 实现和 **静态断言** _(static assert)_ 检查，见 [static_reflection.h](Cpp-Struct-Field-Reflection/static_reflection.h)

> [使用样例代码链接](Cpp-Struct-Field-Reflection/static_iostream.cc)

### 编译器生成 序列化/反序列化 代码

> [代码链接](Cpp-Struct-Field-Reflection/reflection_json.cc)

利用基于静态反射的 `ForEachField`，我们就可以实现通用的结构体序列化/反序列化机制了：

``` cpp
template <typename T>
struct adl_serializer<T, std::enable_if_t<::has_schema<T>>> {
  template <typename BasicJsonType>
  static void to_json(BasicJsonType& j, const T& value) {
    ForEachField(value, [&j](auto&& field, auto&& name) {
      j[name] = field;
    });
  }

  template <typename BasicJsonType>
  static void from_json(const BasicJsonType& j, T& value) {
    ForEachField(value, [&j](auto&& field, auto&& name) {
      // ignore missing field of optional
      if (::is_optional_v<decltype(field)> &&
          j.find(name) == j.end())
        return;

      j.at(name).get_to(field);
    });
  }
};
```

## 写在最后

如果还有人认为 C++ 元编程就是 **屠龙之技**，那可能是因为他们还在 **手写重复的代码**。掌握 C++ 元编程，自己打造工具，解放生产力，告别搬砖的生活！

> 延伸阅读：
> 
> - [浅谈 C++ 元编程](../2017/Cpp-Metaprogramming.md)：程序设计语言结课论文，总结了 C++ 元编程的原理、发展和应用
> - [如何设计一个简单的 C++ ORM](../2016/How-to-Design-a-Naive-Cpp-ORM.md) / [如何设计一个更好的 C++ ORM](../2016/How-to-Design-a-Better-Cpp-ORM.md)：早期两篇个人技术博客，介绍了如何使用元编程技巧实现 C++ ORM（由于当时还在学习阶段，介绍的技术方案有待优化）

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2019, BOT Man
