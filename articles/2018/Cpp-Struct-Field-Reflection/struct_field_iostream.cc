// clang++ struct_field_iostream.cc -std=c++14 -Wall -o iostream && ./iostream

#include <iostream>
#include <string>

#include "struct_field.h"

struct SimpleStruct {
  bool bool_;
  int int_;
  double double_;
  std::string string;
};

template <>
inline constexpr auto StructSchema<SimpleStruct>() {
  return std::make_tuple(std::make_tuple(&SimpleStruct::bool_, "bool"),
                         std::make_tuple(&SimpleStruct::int_, "int"),
                         std::make_tuple(&SimpleStruct::double_, "double"),
                         std::make_tuple(&SimpleStruct::string, "string"));
}

int main() {
  ForEachField(SimpleStruct{true, 1, 1.0, "hello world"},
               [](auto&& value, auto&& pointer, auto&& name) {
                 std::cout << std::boolalpha << name << ": " << value.*pointer
                           << std::endl;
               });
  return 0;
}
