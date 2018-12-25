// clang++ reflection_json.cc -std=c++14 -Wall -o ref_json && ./ref_json

#include <iostream>
#include <string>
#include <vector>

#include "optional_json.h"
#include "static_reflection.h"

namespace detail {

template <typename T>
struct is_optional : std::false_type {};

template <typename T>
struct is_optional<tl::optional<T>> : std::true_type {};

template <typename T>
constexpr auto is_optional_v = is_optional<T>::value;

}  // namespace detail

namespace nlohmann {

template <typename T>
struct adl_serializer<T,
                      std::enable_if_t<std::tuple_size<decltype(
                                           StructSchema<T>())>::value != 0>> {
  template <typename BasicJsonType>
  static void to_json(BasicJsonType& j, const T& value) {
    ForEachField(value, [&j](auto&& field, auto&& name) { j[name] = field; });
  }

  template <typename BasicJsonType>
  static void from_json(const BasicJsonType& j, T& value) {
    ForEachField(value, [&j](auto&& field, auto&& name) {
      // ignore missing field of optional
      if (::detail::is_optional_v<std::decay_t<decltype(field)>> &&
          j.find(name) == j.end())
        return;

      j.at(name).get_to(field);
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
                                "  \"int\": 2,"
                                "  \"double\": 2.0,"
                                "  \"string\": \"hello reflection json\","
                                "  \"vector\": [2, 2.0]"
                                "}")
                        .get<SimpleStruct>()}
            << std::endl
            << json{json::parse("{"
                                "  \"bool\": true,"
                                "  \"int\": 2,"
                                "  \"double\": 2.0,"
                                "  \"string\": \"hello reflection json\","
                                "  \"vector\": [2, 2.0],"
                                "  \"optional\": true"
                                "}")
                        .get<SimpleStruct>()}
            << std::endl;
  return 0;
}