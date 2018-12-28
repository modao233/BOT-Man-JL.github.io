// clang++ raw_json.cc -std=c++14 -Wall -o raw_json && ./raw_json

#include <iostream>
#include <string>
#include <vector>

#include "optional_json.h"

struct SimpleStruct {
  bool bool_;
  int int_;
  double double_;
  std::string string;
  std::vector<double> vector;
  std::unique_ptr<bool> optional;
};

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

int main() {
  using nlohmann::json;
  std::cout << json(json::parse("{"
                                "  \"bool\": true,"
                                "  \"int\": 1,"
                                "  \"double\": 1.0,"
                                "  \"string\": \"hello raw json\","
                                "  \"vector\": [1, 1.0]"
                                "}")
                        .get<SimpleStruct>())
            << std::endl
            << json(json::parse("{"
                                "  \"bool\": true,"
                                "  \"int\": 1,"
                                "  \"double\": 1.0,"
                                "  \"string\": \"hello raw json\","
                                "  \"vector\": [1, 1.0],"
                                "  \"optional\": true"
                                "}")
                        .get<SimpleStruct>())
            << std::endl;
  return 0;
}
