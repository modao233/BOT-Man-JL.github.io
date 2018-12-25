// clang++ dynamic_iostream.cc -std=c++14 -Wall -o d_iostream && ./d_iostream

#include <iostream>
#include <string>

#include "dynamic_reflection.h"

struct SimpleStruct {
  bool bool_field;
  int int_field;
  double double_field;
  std::string string_field;
};

int main() {
  auto bool_converter = [](bool* field, const std::string& name) {
    std::cout << std::boolalpha << name << ": " << *field << std::endl;
  };
  auto int_converter = [](int* field, const std::string& name) {
    std::cout << name << ": " << *field << std::endl;
  };
  auto double_converter = [](double* field, const std::string& name) {
    std::cout << std::fixed << name << ": " << *field << std::endl;
  };
  auto string_converter = [](std::string* field, const std::string& name) {
    std::cout << name << ": " << *field << std::endl;
  };

  StructValueConverter<SimpleStruct> converter;
  converter.RegisterField(&SimpleStruct::bool_field, "bool",
                          ValueConverter<bool>(bool_converter));
  converter.RegisterField(&SimpleStruct::int_field, "int",
                          ValueConverter<int>(int_converter));
  converter.RegisterField(&SimpleStruct::double_field, "double",
                          ValueConverter<double>(double_converter));
  converter.RegisterField(&SimpleStruct::string_field, "string",
                          ValueConverter<std::string>(string_converter));

  SimpleStruct simple{true, 2, 2.0, "hello dynamic reflection"};
  converter(&simple);
  return 0;
}
