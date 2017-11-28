//
// Minimal Tuple Library
// by BOT Man, 2017
//

#ifndef BOT_TUPLE_H
#define BOT_TUPLE_H

#include <functional>  // std::reference_wrapper for make_tuple
#include <type_traits>

namespace bot {

    /// tuple
    template<typename ...Ts>
    class tuple;

    /// operators
    template<typename ...Ts1, typename ...Ts2>
    constexpr bool operator== (const tuple<Ts1 ...> &, const tuple<Ts2 ...> &);
    template<typename ...Ts1, typename ...Ts2>
    constexpr bool operator!= (const tuple<Ts1 ...> &, const tuple<Ts2 ...> &);
    template<typename ...Ts1, typename ...Ts2>
    constexpr bool operator< (const tuple<Ts1 ...> &, const tuple<Ts2 ...> &);
    template<typename ...Ts1, typename ...Ts2>
    constexpr bool operator<= (const tuple<Ts1 ...> &, const tuple<Ts2 ...> &);
    template<typename ...Ts1, typename ...Ts2>
    constexpr bool operator> (const tuple<Ts1 ...> &, const tuple<Ts2 ...> &);
    template<typename ...Ts1, typename ...Ts2>
    constexpr bool operator>= (const tuple<Ts1 ...> &, const tuple<Ts2 ...> &);

    /// tuple_size
    template<typename Tuple>
    struct tuple_size;
    template<typename Tuple>
    constexpr std::size_t tuple_size_v = tuple_size<Tuple>::value;

    /// tuple_element
    template<std::size_t Index, typename Tuple>
    struct tuple_element;
    template<std::size_t Index, typename Tuple>
    using tuple_element_t = typename tuple_element<Index, Tuple>::type;

    /// get (by index)
    template<std::size_t Index, typename Tuple>
    constexpr tuple_element_t<Index, Tuple> &
        get (Tuple &) noexcept;
    template<std::size_t Index, typename Tuple>
    constexpr const tuple_element_t<Index, Tuple> &
        get (const Tuple &) noexcept;
    template<std::size_t Index, typename Tuple>
    constexpr tuple_element_t<Index, Tuple> &&
        get (Tuple &&) noexcept;
    template<std::size_t Index, typename Tuple>
    constexpr const tuple_element_t<Index, Tuple> &&
        get (const Tuple &&) noexcept;

    /// get (by type)
    template<typename T, typename ...Ts>
    constexpr T &get (tuple<Ts ...> &) noexcept;
    template<typename T, typename ...Ts>
    constexpr const T &get (const tuple<Ts ...> &) noexcept;
    template<typename T, typename ...Ts>
    constexpr T &&get (tuple<Ts ...> &&) noexcept;
    template<typename T, typename ...Ts>
    constexpr const T &&get (const tuple<Ts ...> &&) noexcept;

    /// helpers
    namespace detail {
        template<std::size_t Index, typename Tuple>
        struct shrink;
        template<std::size_t Index, typename Tuple>
        using shrink_t = typename shrink<Index, Tuple>::type;

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

        template<typename ...Ts>
        using base_t = typename shrink<1, tuple<Ts ...>>::type;
        template<typename ...Ts>
        using value_t = typename tuple_element<0, tuple<Ts ...>>::type;
    }

    /// tuple
    //  - tuple_element
    //  - shrink

    template<>
    class tuple<> {
    public:
        constexpr tuple () noexcept = default;

        tuple (const tuple &) = default;
        tuple (tuple &&) = default;
        tuple &operator= (const tuple &) = default;
        tuple &operator= (tuple &&) = default;

        void swap (tuple &) noexcept {}
    };

    template<typename T, typename ...Ts>
    class tuple<T, Ts ...> : tuple<Ts ...> {
        // - store the only first element at a class
        // - use inheritance to get element
        T _val;

        // base view
        using Base = tuple<Ts ...>;

        Base &_base () { return *this; }
        const Base &_base () const { return *this; }

        // friends
        template<typename ...>
        friend class tuple;

        template<std::size_t Index, typename Tuple>
        friend constexpr tuple_element_t<Index, Tuple> &
            get (Tuple &tuple) noexcept;
        template<std::size_t Index, typename Tuple>
        friend constexpr const tuple_element_t<Index, Tuple> &
            get (const Tuple &tuple) noexcept;
        template<std::size_t Index, typename Tuple>
        friend constexpr tuple_element_t<Index, Tuple> &&
            get (Tuple &&tuple) noexcept;
        template<std::size_t Index, typename Tuple>
        friend constexpr const tuple_element_t<Index, Tuple> &&
            get (const Tuple &&tuple) noexcept;

        template<typename ...Ts1, typename ...Ts2>
        friend constexpr bool operator== (const tuple<Ts1 ...> &,
            const tuple<Ts2 ...> &);
        template<typename ...Ts1, typename ...Ts2>
        friend constexpr bool operator< (const tuple<Ts1 ...> &,
            const tuple<Ts2 ...> &);
    public:
        constexpr tuple () noexcept = default;

        explicit tuple (T arg, Ts ...args) :
            Base (std::forward<Ts> (args)...),
            _val (std::forward<T> (arg)) {}

        tuple (const tuple &) = default;
        tuple (tuple &&) = default;
        tuple &operator= (const tuple &) = default;
        tuple &operator= (tuple &&) = default;

        template<typename ...Rhs>
        tuple (const tuple<Rhs ...> &rhs) :
            Base { rhs._base () }, _val { rhs._val } {}
        template<typename ...Rhs>
        tuple (tuple<Rhs ...> &&rhs) :
            Base { std::forward<detail::base_t<Rhs ...> &&> (rhs._base ()) },
            _val (std::forward<detail::value_t<Rhs ...> &&> (rhs._val)) {}

        template<typename ...Rhs>
        tuple &operator= (const tuple<Rhs ...> &rhs) {
            _base () = rhs._base ();
            _val = rhs._val;
            return *this;
        }
        template<typename ...Rhs>
        tuple &operator= (tuple<Rhs ...> &&rhs) {
            _base () = std::forward<detail::base_t<Rhs ...> &&> (rhs._base ());
            _val = std::forward<detail::value_t<Rhs ...> &&> (rhs._val);
            return *this;
        }

        void swap (tuple &rhs) {
            std::swap (_val, rhs._val);
            _base ().swap (rhs._base ());
        }
    };

    /// operators
    //  - get

    template<> constexpr bool operator== <> (
        const tuple<> &, const tuple<> &) {
        return true;
    }
    template<> constexpr bool operator< <> (
        const tuple<> &, const tuple<> &) {
        return false;
    }

    template<typename ...Ts1, typename ...Ts2>
    constexpr bool operator== (
        const tuple<Ts1 ...> &lhs, const tuple<Ts2 ...> &rhs) {
        return lhs._val == rhs._val && lhs._base () == rhs._base ();
    }
    template<typename ...Ts1, typename ...Ts2>
    constexpr bool operator!= (
        const tuple<Ts1 ...> &lhs, const tuple<Ts2 ...> &rhs) {
        return !(lhs == rhs);
    }

    template<typename ...Ts1, typename ...Ts2>
    constexpr bool operator< (
        const tuple<Ts1 ...> &lhs, const tuple<Ts2 ...> &rhs) {
        return lhs._val < rhs._val || (
            !(lhs._val < rhs._val) && lhs._base () < rhs._base ());
    }
    template<typename ...Ts1, typename ...Ts2>
    constexpr bool operator>= (
        const tuple<Ts1 ...> &lhs, const tuple<Ts2 ...> &rhs) {
        return !(lhs < rhs);
    }
    template<typename ...Ts1, typename ...Ts2>
    constexpr bool operator> (
        const tuple<Ts1 ...> &lhs, const tuple<Ts2 ...> &rhs) {
        return rhs < lhs;
    }
    template<typename ...Ts1, typename ...Ts2>
    constexpr bool operator<= (
        const tuple<Ts1 ...> &lhs, const tuple<Ts2 ...> &rhs) {
        return !(rhs < lhs);
    }

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
        get (Tuple &t) noexcept
    {
        using T = detail::shrink_t<Index, Tuple>;
        using R = tuple_element_t<Index, Tuple>;
        return std::forward<R &> (((T &) t)._val);
    }
    template<std::size_t Index, typename Tuple>
    constexpr const tuple_element_t<Index, Tuple> &
        get (const Tuple &t) noexcept
    {
        using T = detail::shrink_t<Index, Tuple>;
        using R = tuple_element_t<Index, Tuple>;
        return std::forward<const R &> (((const T &) t)._val);
    }
    template<std::size_t Index, typename Tuple>
    constexpr tuple_element_t<Index, Tuple> &&
        get (Tuple &&t) noexcept
    {
        using T = detail::shrink_t<Index, Tuple>;
        using R = tuple_element_t<Index, Tuple>;
        return std::forward<R &&> (((T &) t)._val);
    }
    template<std::size_t Index, typename Tuple>
    constexpr const tuple_element_t<Index, Tuple> &&
        get (const Tuple &&t) noexcept
    {
        using T = detail::shrink_t<Index, Tuple>;
        using R = tuple_element_t<Index, Tuple>;
        return std::forward<const R &&> (((const T &) t)._val);
    }

    /// get (by type)
    //  - get (by index)

    namespace detail {
        template<typename T, size_t Index, typename ...Ts>
        struct type_index {
            static constexpr int value = -1;
        };

        template<typename T, size_t Index, typename Head, typename ...Tail>
        struct type_index<T, Index, Head, Tail ...> {
            static constexpr int value = std::is_same<T, Head>::value ?
                (int) Index : type_index<T, Index + 1, Tail...>::value;

        private:
            static constexpr auto found_twice =
                std::is_same<T, Head>::value &&
                type_index<T, Index + 1, Tail...>::value != -1;
            static_assert(!found_twice, "duplicate type in bot::get");
        };

        template<typename T, size_t Index, typename ...Ts>
        constexpr auto type_index_v = type_index<T, Index, Ts ...>::value;
    }

    template<typename T, typename ...Ts>
    constexpr T &get (tuple<Ts ...> &t) noexcept {
        constexpr auto idx = detail::type_index_v<T, 0, Ts ...>;
        static_assert(idx != -1, "missing type in bot::get");
        return get<idx == -1 ? 0 : idx> (
            (tuple<Ts ...> &) (t));
    }
    template<typename T, typename ...Ts>
    constexpr const T &get (const tuple<Ts ...> &t) noexcept {
        constexpr auto idx = detail::type_index_v<T, 0, Ts ...>;
        static_assert(idx != -1, "missing type in bot::get");
        return get<idx == -1 ? 0 : idx> (
            (const tuple<Ts ...> &) (t));
    }
    template<typename T, typename ...Ts>
    constexpr T &&get (tuple<Ts ...> &&t) noexcept {
        constexpr auto idx = detail::type_index_v<T, 0, Ts ...>;
        static_assert(idx != -1, "missing type in bot::get");
        return get<idx == -1 ? 0 : idx> (
            std::forward<tuple<Ts ...> &&> ((tuple<Ts ...> &) t));
    }
    template<typename T, typename ...Ts>
    constexpr const T &&get (const tuple<Ts ...> &&t) noexcept {
        constexpr auto idx = detail::type_index_v<T, 0, Ts ...>;
        static_assert(idx != -1, "missing type in bot::get");
        return get<idx == -1 ? 0 : idx> (
            std::forward<const tuple<Ts ...> &&> ((const tuple<Ts ...> &) t));
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
            return Ret {
                get<Indices1> (std::forward<Tuple1> (tuple1)) ...,
                get<Indices2> (std::forward<Tuple2> (tuple2)) ...
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
