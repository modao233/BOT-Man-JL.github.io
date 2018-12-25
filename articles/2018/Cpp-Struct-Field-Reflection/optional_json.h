// Optional Json Helper by BOT Man, 2018

#ifndef OPTIONAL_JSON_H_
#define OPTIONAL_JSON_H_

// JSON for Modern C++ (see: https://github.com/nlohmann/json)
#include "third_party/json.hpp"

// optional implementation (see: https://github.com/TartanLlama/optional)
#include "third_party/optional.hpp"

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

#endif  // OPTIONAL_JSON_H_
