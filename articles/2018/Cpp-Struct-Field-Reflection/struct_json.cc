// clang++ struct_json.cc -std=c++14 -Wall -o struct_json && ./struct_json

#include <iostream>
#include <string>
#include <vector>

// JSON for Modern C++ (see: https://github.com/nlohmann/json)
#include "json.hpp"

// optional implementation (see: https://github.com/TartanLlama/optional)
#include "optional.hpp"

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

}  // namespace nlohmann

struct SimpleStruct {
  bool bool_;
  int int_;
  double double_;
  std::string string;
  std::vector<double> vector;
  tl::optional<bool> optional;
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
