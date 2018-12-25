// clang++ static_iostream.cc -std=c++14 -Wall -o s_iostream && ./s_iostream

#include <iostream>
#include <string>

#include "static_reflection.h"

struct SimpleStruct {
  bool bool_field;
  int int_field;
  double double_field;
  std::string string_field;
};

template <>
inline constexpr auto StructSchema<SimpleStruct>() {
  return std::make_tuple(
      std::make_tuple(&SimpleStruct::bool_field, "bool"),
      std::make_tuple(&SimpleStruct::int_field, "int"),
      std::make_tuple(&SimpleStruct::double_field, "double"),
      std::make_tuple(&SimpleStruct::string_field, "string"));
}

int main() {
  ForEachField(SimpleStruct{true, 1, 1.0, "hello static reflection"},
               [](auto&& field, auto&& name) {
                 std::cout << std::boolalpha << std::fixed << name << ": "
                           << field << std::endl;
               });
  return 0;
}
