// Struct Field Schema Extractor by BOT Man, 2018

#ifndef STRUCT_SCHEMA_H_
#define STRUCT_SCHEMA_H_

#include <tuple>
#include <type_traits>

template <typename T>
inline constexpr auto StructSchema() {
  return std::make_tuple();
}

template <typename Fn, typename Tuple, std::size_t... I>
inline constexpr void ForEachTuple(Tuple&& tuple,
                                   Fn&& fn,
                                   std::index_sequence<I...>) {
  using Expander = int[];
  (void)Expander{0, ((void)fn(std::get<I>(std::forward<Tuple>(tuple))), 0)...};
}

template <typename Fn, typename Tuple>
inline constexpr void ForEachTuple(Tuple&& tuple, Fn&& fn) {
  ForEachTuple(
      std::forward<Tuple>(tuple), std::forward<Fn>(fn),
      std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{});
}

template <typename T, typename Fn>
inline constexpr void ForEachField(T&& value, Fn&& fn) {
  constexpr auto struct_schema = StructSchema<std::decay_t<T>>();
  static_assert(std::tuple_size<decltype(struct_schema)>::value != 0,
                "StructSchema<T>() for type T should be specialized to return "
                "FieldSchema tuples, like ((&T::field, field_name), ...)");

  ForEachTuple(struct_schema, [&value, &fn](auto&& field_schema) {
    using FieldSchema = std::remove_reference_t<decltype(field_schema)>;
    static_assert(std::tuple_size<FieldSchema>::value >= 2,
                  "FieldSchema tuple should be (&T::field, field_name)");

    fn(std::forward<T>(value),
       std::get<0>(std::forward<decltype(field_schema)>(field_schema)),
       std::get<1>(std::forward<decltype(field_schema)>(field_schema)));
  });
}

#endif  // STRUCT_SCHEMA_H_
