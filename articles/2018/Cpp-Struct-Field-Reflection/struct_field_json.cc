// clang++ struct_field_json.cc -std=c++14 -Wall -o json && ./json

#include <iostream>
#include <string>
#include <vector>

// JSON for Modern C++ (downloaded here: https://github.com/nlohmann/json)
#include "json.hpp"

// optional implementation (see: https://github.com/TartanLlama/optional)
#include "optional.hpp"

#include "struct_field.h"

namespace detail {

template <typename T>
struct is_optional : std::false_type {};

template <typename T>
struct is_optional<tl::optional<T>> : std::true_type {};

template <class T>
struct get_field_type;

template <class C, class T>
struct get_field_type<T C::*> {
  using type = T;
};

}  // namespace detail

namespace nlohmann {

template <typename T>
struct adl_serializer<tl::optional<T>> {
  static void to_json(json& j, const tl::optional<T>& opt) {
    if (!opt) {
      j = nullptr;
    } else {
      j = *opt;
    }
  }

  static void from_json(const json& j, tl::optional<T>& opt) {
    if (j.is_null()) {
      opt = tl::nullopt;
    } else {
      opt = j.get<T>();
    }
  }
};

template <typename T>
struct adl_serializer<T,
                      std::enable_if_t<std::tuple_size<decltype(
                                           StructSchema<T>())>::value != 0>> {
  template <typename BasicJsonType>
  static void to_json(BasicJsonType& j, const T& value) {
    ForEachField(value, [&j](auto&& value, auto&& pointer, auto&& name) {
      j[name] = value.*pointer;
    });
  }

  template <typename BasicJsonType>
  static void from_json(const BasicJsonType& j, T& value) {
    ForEachField(value, [&j](auto&& value, auto&& pointer, auto&& name) {
      using type = typename ::detail::get_field_type<
          std::decay_t<decltype(pointer)>>::type;
      constexpr auto is_optional = ::detail::is_optional<type>::value;

      // ignore missing field of optional
      if (is_optional && j.find(name) == j.end())
        return;

      j.at(name).get_to(value.*pointer);
    });
  }
};

}  // namespace nlohmann

struct SimpleStruct {
  bool bool_;
  int int_;
  double double_;
  std::string string;
  std::vector<double> vector;
  tl::optional<bool> optional;
};

template <>
inline constexpr auto StructSchema<SimpleStruct>() {
  return std::make_tuple(std::make_tuple(&SimpleStruct::bool_, "bool"),
                         std::make_tuple(&SimpleStruct::int_, "int"),
                         std::make_tuple(&SimpleStruct::double_, "double"),
                         std::make_tuple(&SimpleStruct::string, "string"),
                         std::make_tuple(&SimpleStruct::vector, "vector"),
                         std::make_tuple(&SimpleStruct::optional, "optional"));
}

int main() {
  using nlohmann::json;
  std::cout << json{json::parse("{"
                                "  \"bool\": true,"
                                "  \"double\": 0,"
                                "  \"int\": 0,"
                                "  \"string\": \"hi\","
                                "  \"vector\": []"
                                "}")
                        .get<SimpleStruct>()}
            << std::endl
            << json{json::parse("{"
                                "  \"bool\": true,"
                                "  \"double\": 0,"
                                "  \"int\": 0,"
                                "  \"string\": \"hi\","
                                "  \"vector\": [],"
                                "  \"optional\": false"
                                "}")
                        .get<SimpleStruct>()}
            << std::endl;
  return 0;
}
