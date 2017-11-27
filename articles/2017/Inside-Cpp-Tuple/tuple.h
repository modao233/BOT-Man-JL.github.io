//
// Minimal Tuple Library
// by BOT Man, 2017
//

#ifndef BOT_TUPLE_H
#define BOT_TUPLE_H

#include <functional>  // std::reference_wrapper for make_tuple
#include <type_traits>

// More about 'const &&'
// https://codesynthesis.com/~boris/blog/2012/07/24/const-rvalue-references

namespace bot {

    template<typename ...Ts>
    class tuple;

    template<typename Tuple>
    struct tuple_size;
    template<typename Tuple>
    constexpr std::size_t tuple_size_v = tuple_size<Tuple>::value;

    template<std::size_t Index, typename Tuple>
    struct tuple_element;
    template<std::size_t Index, typename Tuple>
    using tuple_element_t = typename tuple_element<Index, Tuple>::type;

    // helper for operator= and get (by index)
    // resolve to super-class of Tuple at Index to get value
    namespace detail {
        template<std::size_t Index, typename Tuple>
        struct shrink;

        template<std::size_t Index, typename T, typename ...Ts>
        struct shrink<Index, tuple<T, Ts ...>>
            : shrink<Index - 1, tuple<Ts ...>> {};

        template<typename T, typename ...Ts>
        struct shrink<0, tuple<T, Ts ...>> {
            using type = tuple<T, Ts ...>;
        };

        template<>
        struct shrink<0, tuple<>> {
            using type = tuple<>;
        };
    }

    /// tuple
    //  - tuple_element
    //  - shrink

    template<>
    class tuple<> {
    public:
        constexpr tuple () noexcept = default;
        constexpr tuple (const tuple &) = default;
        constexpr tuple (tuple &&) = default;
        constexpr tuple &operator=(const tuple &) = default;
        constexpr tuple &operator=(tuple &&) = default;

        template<typename ...Rhs>
        tuple &operator=(const tuple<Rhs ...> &rhs) { return *this; }
        template<typename ...Rhs>
        tuple &operator=(tuple<Rhs ...> &&rhs) { return *this; }

        void swap (tuple &) noexcept {}
    };

    template<typename T, typename ...Ts>
    class tuple<T, Ts ...> : tuple<Ts ...> {
        using Base = tuple<Ts ...>;

    public:
        constexpr tuple () noexcept = default;
        constexpr tuple (const tuple &) = default;
        constexpr tuple (tuple &&) = default;
        constexpr tuple &operator=(const tuple &) = default;
        constexpr tuple &operator=(tuple &&) = default;

        constexpr tuple (T arg, Ts ...args) :
            Base (args...), _val (arg) {}

        template<typename ...Rhs>
        tuple &operator=(const tuple<Rhs ...> &rhs) {
            using RhsTail = typename detail::shrink<1, tuple<Rhs ...>>::type;
            _val = rhs._val;
            Base (*this) = (const RhsTail &) (rhs);
            return *this;
        }

        template<typename ...Rhs>
        tuple &operator=(tuple<Rhs ...> &&rhs) {
            using RhsHead = typename tuple_element<0, tuple<Rhs ...>>::type;
            using RhsTail = typename detail::shrink<1, tuple<Rhs ...>>::type;
            _val = std::forward<RhsHead> (rhs._val);
            Base (*this) = std::forward<RhsTail> ((RhsTail &) (rhs));
            return *this;
        }

        void swap (tuple &other) {
            std::swap (_val, other._val);
            Base::swap (other);
        }

        // - store the only first element at a class
        // - use inheritance to get element
        T _val;
    };

    /// tuple_size

    template<typename ...Ts>
    struct tuple_size<tuple<Ts ...>> :
        public std::integral_constant<std::size_t, sizeof... (Ts)> {};

    /// tuple_element

    template<std::size_t Index, typename T, typename ...Ts>
    struct tuple_element<Index, tuple<T, Ts ...>>
        : tuple_element<Index - 1, tuple<Ts ...>> {};

    template<typename T, typename ...Ts>
    struct tuple_element<0, tuple<T, Ts ...>> {
        using type = T;
    };

    /// get (by index)
    //  - tuple_element
    //  - shrink

    template<std::size_t Index, typename Tuple>
    constexpr tuple_element_t<Index, Tuple> &
        get (Tuple &tuple) noexcept
    {
        using T = typename detail::shrink<Index, Tuple>::type;
        using R = tuple_element_t<Index, Tuple>;
        return std::forward<R &> (((T &) tuple)._val);
    }
    template<std::size_t Index, typename Tuple>
    constexpr const tuple_element_t<Index, Tuple> &
        get (const Tuple &tuple) noexcept
    {
        using T = typename detail::shrink<Index, Tuple>::type;
        using R = tuple_element_t<Index, Tuple>;
        return std::forward<const R &> (((const T &) tuple)._val);
    }
    template<std::size_t Index, typename Tuple>
    constexpr tuple_element_t<Index, Tuple> &&
        get (Tuple &&tuple) noexcept
    {
        using T = typename detail::shrink<Index, Tuple>::type;
        using R = tuple_element_t<Index, Tuple>;
        return std::forward<R &&> (((T &) tuple)._val);
    }
    template<std::size_t Index, typename Tuple>
    constexpr const tuple_element_t<Index, Tuple> &&
        get (const Tuple &&tuple) noexcept
    {
        using T = typename detail::shrink<Index, Tuple>::type;
        using R = tuple_element_t<Index, Tuple>;
        return std::forward<const R &&> (((const T &) tuple)._val);
    }

    /// get (by type)
    //  - get (by index)
    //  - different from std::get<Type, Tuple>: don't check dup

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

    /// make_tuple

    namespace detail {
        template<typename T>
        struct extract_ref_wrapper {
            using type = T;
        };

        template<typename T>
        struct extract_ref_wrapper<std::reference_wrapper<T>> {
            using type = T&;
        };

        template<typename T>
        using decay_and_extract_ref =
            typename extract_ref_wrapper<std::decay_t<T>>::type;
    }

    template<typename ...Ts>
    constexpr tuple<detail::decay_and_extract_ref<Ts>...>
        make_tuple (Ts &&...args)
    {
        using Ret = tuple<detail::decay_and_extract_ref<Ts>...>;
        return Ret { std::forward<Ts> (args)... };
    }

    /// tie && ignore

    template<typename ...Ts>
    constexpr tuple<Ts &...>
        tie (Ts &...args) noexcept
    {
        using Ret = tuple<Ts &...>;
        return Ret { args... };
    }

    namespace detail {
        struct ignore_t {
            template<typename T>
            const ignore_t& operator=(const T&) const { return *this; }
        };
    }
    constexpr detail::ignore_t ignore {};

    /// forward_as_tuple

    template<typename ...Ts>
    constexpr tuple<Ts &&...>
        forward_as_tuple (Ts &&...args) noexcept
    {
        using Ret = tuple<Ts &&...>;
        return Ret { std::forward<Ts> (args)... };
    }

    /// tuple_cat
    //  - get (by index)

    namespace detail {
        template<typename, typename>
        struct two_tuple_cat_type;

        template<typename ...Ts1, typename ...Ts2>
        struct two_tuple_cat_type<tuple<Ts1 ...>, tuple<Ts2 ...>> {
            using type = tuple<Ts1 ..., Ts2 ...>;
        };

        template<
            typename Ret, typename Tuple1, typename Tuple2,
            std::size_t ...Indices1, std::size_t ...Indices2
        > constexpr Ret two_tuple_cat_by_indices (
            Tuple1 &&tuple1, Tuple2 &&tuple2,
            std::index_sequence<Indices1 ...>,
            std::index_sequence<Indices2 ...>)
        {
            // bad name lookup in both VS and clang
            return Ret {
                bot::get<Indices1> (std::forward<Tuple1> (tuple1)) ...,
                bot::get<Indices2> (std::forward<Tuple2> (tuple2)) ...
            };
        }

        template<typename Tuple1, typename Tuple2>
        constexpr auto two_tuple_cat (Tuple1 &&tuple1, Tuple2 &&tuple2) {
            using Ret = typename two_tuple_cat_type<
                std::decay_t<Tuple1>,
                std::decay_t<Tuple2>
            >::type;
            constexpr auto size1 = tuple_size_v<std::decay_t<Tuple1>>;
            constexpr auto size2 = tuple_size_v<std::decay_t<Tuple2>>;
            return two_tuple_cat_by_indices<Ret> (
                std::forward<Tuple1> (tuple1),
                std::forward<Tuple2> (tuple2),
                std::make_index_sequence<size1> {},
                std::make_index_sequence<size2> {}
            );
        }
    }

    constexpr tuple<> tuple_cat () {
        return tuple<>{};
    }

    template<typename Tuple>
    constexpr Tuple
        tuple_cat (Tuple &&tuple)
    {
        return std::forward<Tuple> (tuple);
    }

    template<typename Tuple1, typename Tuple2, typename ...Tuples>
    constexpr auto
        tuple_cat (Tuple1 &&tuple1, Tuple2 &&tuple2, Tuples &&...tuples)
    {
        // bad name lookup in VS
        return bot::tuple_cat (detail::two_tuple_cat (
            std::forward<Tuple1> (tuple1),
            std::forward<Tuple2> (tuple2)
        ), std::forward<Tuples> (tuples) ...);
    }

    // Improvement:
    //  It's a more efficent to expand all params at a time
    //  Ret{std::get<Js>(std::get<Is>(std::forward<Tuples>(tpls)))...};
    // See:
    // https://github.com/ericniebler/meta/blob/master/example/tuple_cat.cpp
    // http://blogs.microsoft.co.il/sasha/2015/02/22/implementing-tuple-part-7
}

#endif // !BOT_TUPLE_H
