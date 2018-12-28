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

DEFINE_STRUCT_SCHEMA(SimpleStruct,
                     DEFINE_STRUCT_FIELD(bool_field, "bool"),
                     DEFINE_STRUCT_FIELD(int_field, "int"),
                     DEFINE_STRUCT_FIELD(double_field, "double"),
                     DEFINE_STRUCT_FIELD(string_field, "string"));

int main() {
  ForEachField(SimpleStruct{true, 1, 1.0, "hello static reflection"},
               [](auto&& field, auto&& name) {
                 std::cout << std::boolalpha << std::fixed << name << ": "
                           << field << std::endl;
               });
  return 0;
}
