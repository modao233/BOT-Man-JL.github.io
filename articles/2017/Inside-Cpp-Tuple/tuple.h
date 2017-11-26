//
// Minimal Tuple Library
// by BOT Man, 2017
//

#ifndef BOT_TUPLE_H
#define BOT_TUPLE_H

#include <type_traits>

namespace bot {
    //
    // tuple
    //

    template<typename ...Ts>
    class tuple;

    template<>  // Empty Tuple
    class tuple<> {
    public:
        constexpr tuple () noexcept {}
        void swap (tuple &) noexcept {}
    };

    template<typename T, typename ...Ts>  // Recursive Definition
    class tuple<T, Ts ...> : tuple<Ts ...> {
    public:
        constexpr tuple () :
            _val (), tuple<Ts ...> () {}
        constexpr tuple (T arg, Ts ...args) :
            _val (arg), tuple<Ts ...> (args...) {}

        void swap (tuple &other) {
            std::swap (_val, other._val);
            tuple<Ts ...>::swap (other);
        }

        // - store the only first element at a class
        // - use inheritance to get element
        T _val;
    };

    //
    // tuple_size
    //

    template<typename ...Ts>
    struct tuple_size;

    template<typename ...Ts>
    struct tuple_size<tuple<Ts ...>> :
        public std::integral_constant<std::size_t, sizeof... (Ts)> {};

    template<typename Tuple>
    constexpr std::size_t tuple_size_v = tuple_size<Tuple>::value;

    //
    // tuple_element
    //

    template<std::size_t Index, typename ...Ts>
    struct tuple_element;

    template<std::size_t Index, typename T, typename ...Ts>
    struct tuple_element<Index, tuple<T, Ts ...>>
        : tuple_element<Index - 1, tuple<Ts ...>> {};

    template<typename T, typename ...Ts>
    struct tuple_element<0, tuple<T, Ts ...>> {
        using type = T;
    };

    template<std::size_t Index, typename Tuple>
    using tuple_element_t = typename tuple_element<Index, Tuple>::type;

    //
    // get (by index)
    //  - tuple_element
    //

    namespace detail {
        template<std::size_t Index, typename ...Ts>
        struct tuple_shrink;

        template<std::size_t Index, typename T, typename ...Ts>
        struct tuple_shrink<Index, tuple<T, Ts ...>>
            : tuple_shrink<Index - 1, tuple<Ts ...>> {};

        template<typename T, typename ...Ts>
        struct tuple_shrink<0, tuple<T, Ts ...>> {
            // Key Point: resolve to super class at Index to get value
            using type = tuple<T, Ts ...>;
        };
    }

    // More about const &&
    // https://codesynthesis.com/~boris/blog/2012/07/24/const-rvalue-references

    template<std::size_t Index, typename Tuple>
    constexpr decltype (auto) get (Tuple &tuple) noexcept
    {
        using T = typename detail::tuple_shrink<Index, Tuple>::type;
        return ((T &) tuple)._val;
    }
    template<std::size_t Index, typename Tuple>
    constexpr decltype (auto) get (const Tuple &tuple) noexcept
    {
        using T = typename detail::tuple_shrink<Index, Tuple>::type;
        return ((const T &) tuple)._val;
    }
    template<std::size_t Index, typename Tuple>
    constexpr decltype (auto) get (Tuple &&tuple) noexcept
    {
        using T = typename detail::tuple_shrink<Index, Tuple>::type;
        using R = tuple_element_t<Index, Tuple>;
        return std::forward<R &&> (((T &&) tuple)._val);
    }
    template<std::size_t Index, typename Tuple>
    constexpr decltype (auto) get (const Tuple &&tuple) noexcept
    {
        using T = typename detail::tuple_shrink<Index, Tuple>::type;
        using R = tuple_element_t<Index, Tuple>;
        return std::forward<const R &&> (((const T &&) tuple)._val);
    }

    //
    // get (by type)
    //  - get (by index)
    //  - different from std::get<Type, Tuple>: don't check dup
    //

    namespace detail {
        template<typename T>
        constexpr int find (int) { return -1; }

        template<typename T, typename Head, typename ...Tail>
        constexpr int find (int index) {
            return std::is_same<T, Head>::value ?
                index : find<T, Tail...> (index + 1);
        }
    }

    template<typename T, typename ...Ts>
    constexpr decltype (auto) get (tuple<Ts ...> &t) noexcept {
        constexpr auto idx = detail::find<T, Ts ...> (0);
        static_assert(idx != -1, "type is not found at bot::get");
        return get<idx> (std::forward<tuple<Ts ...>> (t));
    }
    template<typename T, typename ...Ts>
    constexpr decltype (auto) get (const tuple<Ts ...> &t) noexcept {
        constexpr auto idx = detail::find<T, Ts ...> (0);
        static_assert(idx != -1, "type is not found at bot::get");
        return get<idx> (std::forward<tuple<Ts ...>> (t));
    }
    template<typename T, typename ...Ts>
    constexpr decltype (auto) get (tuple<Ts ...> &&t) noexcept {
        constexpr auto idx = detail::find<T, Ts ...> (0);
        static_assert(idx != -1, "type is not found at bot::get");
        return get<idx> (std::forward<tuple<Ts ...>> (t));
    }
    template<typename T, typename ...Ts>
    constexpr decltype (auto) get (const tuple<Ts ...> &&t) noexcept {
        constexpr auto idx = detail::find<T, Ts ...> (0);
        static_assert(idx != -1, "type is not found at bot::get");
        return get<idx> (std::forward<tuple<Ts ...>> (t));
    }

    //
    // make_tuple
    //

    template<typename ...Ts>
    constexpr auto make_tuple (Ts &&...args) {
        return tuple<std::decay_t<Ts>...> (std::forward<Ts> (args)...);
    }

    //
    // tie && ignore
    //

    template<typename ...Ts>
    constexpr auto tie (Ts &&...args) noexcept {
        return tuple<Ts &...> (args...);
    }

    namespace detail {
        struct ignore_t {
            template<typename T>
            const ignore_t& operator=(const T&) const { return *this; }
        };
    }
    constexpr detail::ignore_t ignore {};

    //
    // forward_as_tuple
    //

    template<typename ...Ts>
    constexpr auto forward_as_tuple (Ts &&...args) noexcept {
        return tuple<Ts &&...> (std::forward<Ts> (args)...);
    }

    //
    // tuple_cat
    //  - make_tuple
    //  - get (by index)
    //

    namespace detail {
        template<
            typename Tuple1, typename Tuple2,
            std::size_t ...Indices1, std::size_t ...Indices2
        > constexpr auto two_tuple_cat_by_indices (
            Tuple1 &&tuple1, Tuple2 &&tuple2,
            std::index_sequence<Indices1 ...>,
            std::index_sequence<Indices2 ...>)
        {
            // bad name lookup in both VS and clang
            return bot::make_tuple (
                bot::get<Indices1> (std::forward<Tuple1> (tuple1)) ...,
                bot::get<Indices2> (std::forward<Tuple2> (tuple2)) ...
            );
        }

        template<typename Tuple1, typename Tuple2>
        constexpr auto two_tuple_cat (Tuple1 &&tuple1, Tuple2 &&tuple2) {
            constexpr auto size1 = tuple_size_v<std::decay_t<Tuple1>>;
            constexpr auto size2 = tuple_size_v<std::decay_t<Tuple2>>;
            return two_tuple_cat_by_indices (
                std::forward<Tuple1> (tuple1),
                std::forward<Tuple2> (tuple2),
                std::make_index_sequence<size1> {},
                std::make_index_sequence<size2> {}
            );
        }
    }

    template<typename Tuple>
    constexpr auto tuple_cat (Tuple &&tuple) {
        return std::forward<Tuple> (tuple);
    }

    template<typename Tuple1, typename Tuple2, typename ...Tuples>
    constexpr auto tuple_cat (
        Tuple1 &&tuple1, Tuple2 &&tuple2, Tuples &&...tuples)
    {
        // bad name lookup in VS
        return bot::tuple_cat (detail::two_tuple_cat (
            std::forward<Tuple1> (tuple1),
            std::forward<Tuple2> (tuple2)
        ), std::forward<Tuples> (tuples) ...);
    }
}

#endif // !BOT_TUPLE_H
