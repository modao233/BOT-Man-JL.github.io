// clang++ raw_json.cc -std=c++14 -Wall -o raw_json && ./raw_json

#include <iostream>
#include <string>
#include <vector>

#include "optional_json.h"

struct SimpleStruct {
  bool bool_;
  int int_;
  double double_;
  std::string string_;
  std::vector<double> vector_;
  std::unique_ptr<bool> optional_;
};

void to_json(nlohmann::json& j, const SimpleStruct& value) {
  j["_bool"] = value.bool_;
  j["_int"] = value.int_;
  j["_double"] = value.double_;
  j["_string"] = value.string_;
  j["_vector"] = value.vector_;
  j["_optional"] = value.optional_;
}

void from_json(const nlohmann::json& j, SimpleStruct& value) {
  j.at("_bool").get_to(value.bool_);
  j.at("_int").get_to(value.int_);
  j.at("_double").get_to(value.double_);
  j.at("_string").get_to(value.string_);
  j.at("_vector").get_to(value.vector_);
  if (j.find("_optional") != j.cend()) {
    j.at("_optional").get_to(value.optional_);
  }
}

int main() {
  using nlohmann::json;
  std::cout << json(json::parse("{"
                                "  \"_bool\": true,"
                                "  \"_int\": 1,"
                                "  \"_double\": 1.0,"
                                "  \"_string\": \"hello raw json\","
                                "  \"_vector\": [1, 1.0]"
                                "}")
                        .get<SimpleStruct>())
            << std::endl
            << json(json::parse("{"
                                "  \"_bool\": true,"
                                "  \"_int\": 1,"
                                "  \"_double\": 1.0,"
                                "  \"_string\": \"hello raw json\","
                                "  \"_vector\": [1, 1.0],"
                                "  \"_optional\": true"
                                "}")
                        .get<SimpleStruct>())
            << std::endl;
  return 0;
}
