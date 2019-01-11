// clang++ static_iostream.cc -std=c++14 -Wall -o s_iostream && ./s_iostream

#include <iostream>
#include <string>

#include "static_reflection.h"

struct SimpleStruct {
  bool bool_;
  int int_;
  double double_;
  std::string string_;
};

DEFINE_STRUCT_SCHEMA(SimpleStruct,
                     DEFINE_STRUCT_FIELD(bool_, "bool"),
                     DEFINE_STRUCT_FIELD(int_, "int"),
                     DEFINE_STRUCT_FIELD(double_, "double"),
                     DEFINE_STRUCT_FIELD(string_, "string"));

struct GenericFunctor {
  // ... context data

  template <typename Field, typename Name>
  void operator()(Field&& field, Name&& name) {
    std::cout << std::boolalpha << std::fixed << name << ": " << field
              << std::endl;
  }
};

int main() {
  ForEachField(SimpleStruct{true, 1, 1.0, "hello static reflection"},
               [](auto&& field, auto&& name) {
                 std::cout << std::boolalpha << std::fixed << name << ": "
                           << field << std::endl;
               });

  ForEachField(SimpleStruct{true, 1, 1.0, "hello static reflection"},
               GenericFunctor{/* ... context data */});
  return 0;
}
