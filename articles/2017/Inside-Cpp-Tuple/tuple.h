//
// Minimal Tuple Library
// by BOT Man, 2017
//

#ifndef BOT_TUPLE_H
#define BOT_TUPLE_H

#include <type_traits>
#include <utility>

namespace {
    template<class T>
    class reference_wrapper;
}

namespace bot {

    /// tuple
    template<typename ...Ts>
    class tuple;

    /// operators
    template<typename ...Ts, typename ...Us>
    constexpr bool operator== (const tuple<Ts ...> &, const tuple<Us ...> &);
    template<typename ...Ts, typename ...Us>
    constexpr bool operator!= (const tuple<Ts ...> &, const tuple<Us ...> &);
    template<typename ...Ts, typename ...Us>
    constexpr bool operator< (const tuple<Ts ...> &, const tuple<Us ...> &);
    template<typename ...Ts, typename ...Us>
    constexpr bool operator<= (const tuple<Ts ...> &, const tuple<Us ...> &);
    template<typename ...Ts, typename ...Us>
    constexpr bool operator> (const tuple<Ts ...> &, const tuple<Us ...> &);
    template<typename ...Ts, typename ...Us>
    constexpr bool operator>= (const tuple<Ts ...> &, const tuple<Us ...> &);

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

    /// swap
    template<typename ...Ts>
    void swap (tuple<Ts...> &, tuple<Ts...> &) noexcept;

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
        using head_t = typename tuple_element<0, tuple<Ts ...>>::type;
        template<typename ...Ts>
        using tail_t = typename shrink<1, tuple<Ts ...>>::type;
    }

    /// tuple
    //  - head_t
    //  - tail_t

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

    template<typename Head, typename ...Tails>
    class tuple<Head, Tails ...> : tuple<Tails ...>
    {
        // - store head element in class
        // - inherit tail elements from base
        Head _val;
        using Tail = tuple<Tails ...>;

        // head & tail self-encapsulation
        Head &_head () { return _val; }
        const Head &_head () const { return _val; }

        Tail &_tail () { return *this; }
        const Tail &_tail () const { return *this; }

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

        template<typename ...Ts, typename ...Us>
        friend constexpr bool operator== (const tuple<Ts ...> &,
            const tuple<Us ...> &);
        template<typename ...Ts, typename ...Us>
        friend constexpr bool operator< (const tuple<Ts ...> &,
            const tuple<Us ...> &);

    public:
        constexpr tuple () noexcept = default;

        explicit tuple (Head arg, Tails ...args) :
            Tail (std::forward<Tails> (args)...),
            _val (std::forward<Head> (arg)) {}

        tuple (const tuple &) = default;
        tuple (tuple &&) = default;
        tuple &operator= (const tuple &) = default;
        tuple &operator= (tuple &&) = default;

        template<typename ...Rhs>
        tuple (const tuple<Rhs ...> &rhs) :
            Tail (rhs._tail ()),
            _val (rhs._head ()) {}
        template<typename ...Rhs>
        tuple (tuple<Rhs ...> &&rhs) :
            Tail (std::forward<detail::tail_t<Rhs ...> &&> (rhs._tail ())),
            _val (std::forward<detail::head_t<Rhs ...> &&> (rhs._head ())) {}

        template<typename ...Rhs>
        tuple &operator= (const tuple<Rhs ...> &rhs) {
            _tail () = rhs._tail ();
            _head () = rhs._head ();
            return *this;
        }
        template<typename ...Rhs>
        tuple &operator= (tuple<Rhs ...> &&rhs) {
            _tail () = std::forward<detail::tail_t<Rhs ...> &&> (rhs._tail ());
            _head () = std::forward<detail::head_t<Rhs ...> &&> (rhs._head ());
            return *this;
        }

        void swap (tuple &rhs) {
            std::swap (_head (), rhs._head ());
            _tail ().swap (rhs._tail ());
        }
    };

    /// operators
    //  - _head ()
    //  - _tail ()

    template<> constexpr bool operator== <> (
        const tuple<> &, const tuple<> &) {
        return true;
    }
    template<> constexpr bool operator< <> (
        const tuple<> &, const tuple<> &) {
        return false;
    }

    template<typename ...Ts, typename ...Us>
    constexpr bool operator== (
        const tuple<Ts ...> &lhs, const tuple<Us ...> &rhs) {
        return lhs._head () == rhs._head () && lhs._tail () == rhs._tail ();
    }
    template<typename ...Ts, typename ...Us>
    constexpr bool operator!= (
        const tuple<Ts ...> &lhs, const tuple<Us ...> &rhs) {
        return !(lhs == rhs);
    }

    template<typename ...Ts, typename ...Us>
    constexpr bool operator< (
        const tuple<Ts ...> &lhs, const tuple<Us ...> &rhs) {
        return lhs._head () < rhs._head () || (
            !(lhs._head () < rhs._head ()) && lhs._tail () < rhs._tail ());
    }
    template<typename ...Ts, typename ...Us>
    constexpr bool operator>= (
        const tuple<Ts ...> &lhs, const tuple<Us ...> &rhs) {
        return !(lhs < rhs);
    }
    template<typename ...Ts, typename ...Us>
    constexpr bool operator> (
        const tuple<Ts ...> &lhs, const tuple<Us ...> &rhs) {
        return rhs < lhs;
    }
    template<typename ...Ts, typename ...Us>
    constexpr bool operator<= (
        const tuple<Ts ...> &lhs, const tuple<Us ...> &rhs) {
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

    template<std::size_t Index, typename Tuple>
    struct tuple_element<Index, const Tuple> {
        using type = std::add_const_t<tuple_element_t<Index, Tuple>>;
    };
    template<std::size_t Index, typename Tuple>
    struct tuple_element<Index, volatile Tuple> {
        using type = std::add_volatile_t<tuple_element_t<Index, Tuple>>;
    };
    template<std::size_t Index, typename Tuple>
    struct tuple_element<Index, const volatile Tuple> {
        using type = std::add_cv_t<tuple_element_t<Index, Tuple>>;
    };

    /// get (by index)
    //  - tuple_element
    //  - shrink
    //  - _head ()

    template<std::size_t Index, typename Tuple>
    constexpr tuple_element_t<Index, Tuple> &
        get (Tuple &t) noexcept
    {
        using T = detail::shrink_t<Index, Tuple>;
        using R = tuple_element_t<Index, Tuple>;
        return std::forward<R &> (((T &) t)._head ());
    }
    template<std::size_t Index, typename Tuple>
    constexpr const tuple_element_t<Index, Tuple> &
        get (const Tuple &t) noexcept
    {
        using T = detail::shrink_t<Index, Tuple>;
        using R = tuple_element_t<Index, Tuple>;
        return std::forward<const R &> (((const T &) t)._head ());
    }
    template<std::size_t Index, typename Tuple>
    constexpr tuple_element_t<Index, Tuple> &&
        get (Tuple &&t) noexcept
    {
        using T = detail::shrink_t<Index, Tuple>;
        using R = tuple_element_t<Index, Tuple>;
        return std::forward<R &&> (((T &) t)._head ());
    }
    template<std::size_t Index, typename Tuple>
    constexpr const tuple_element_t<Index, Tuple> &&
        get (const Tuple &&t) noexcept
    {
        using T = detail::shrink_t<Index, Tuple>;
        using R = tuple_element_t<Index, Tuple>;
        return std::forward<const R &&> (((const T &) t)._head ());
    }

    /// get (by type)
    //  - get (by index)

    namespace detail {
        template<typename T, std::size_t Index, typename ...Ts>
        struct type_index {
            static constexpr int value = -1;
        };

        template<typename T, std::size_t Index, typename Head, typename ...Tail>
        struct type_index<T, Index, Head, Tail ...> {
            static constexpr int value = std::is_same<T, Head>::value ?
                (int) Index : type_index<T, Index + 1, Tail...>::value;

        private:
            static constexpr auto found_twice =
                std::is_same<T, Head>::value &&
                type_index<T, Index + 1, Tail...>::value != -1;
            static_assert(!found_twice, "duplicate type in bot::get");
        };

        template<typename T, std::size_t Index, typename ...Ts>
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

    /// swap

    template<typename ...Ts>
    void swap (tuple<Ts...> &lhs, tuple<Ts...> &rhs) noexcept {
        lhs.swap (rhs);
    }

    /// make_tuple

    namespace detail {
        template<typename T>
        struct extract_ref {
            using type = T;
        };

        template<typename T>
        struct extract_ref<std::reference_wrapper<T>> {
            using type = T&;
        };

        template<typename T>
        using decay_unref = typename extract_ref<std::decay_t<T>>::type;
    }

    template<typename ...Ts>
    constexpr tuple<detail::decay_unref<Ts>...> make_tuple (Ts &&...args) {
        using Ret = tuple<detail::decay_unref<Ts>...>;
        return Ret { std::forward<Ts> (args)... };
    }

    /// tie && ignore

    template<typename ...Ts>
    constexpr tuple<Ts &...> tie (Ts &...args) noexcept {
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
    constexpr tuple<Ts &&...> forward_as_tuple (Ts &&...args) noexcept {
        using Ret = tuple<Ts &&...>;
        return Ret { std::forward<Ts> (args)... };
    }

    /// tuple_cat
    //  - get (by index)

    namespace detail {
        template<typename, typename>
        struct two_tuple_cat_type;

        template<typename ...Ts, typename ...Us>
        struct two_tuple_cat_type<tuple<Ts ...>, tuple<Us ...>> {
            using type = tuple<Ts ..., Us ...>;
        };

        template<
            typename Ret, typename Tuple1, typename Tuple2,
            std::size_t ...Indices1, std::size_t ...Indices2
        > constexpr Ret two_tuple_cat (Tuple1 &&t1, Tuple2 &&t2,
            std::index_sequence<Indices1 ...>,
            std::index_sequence<Indices2 ...>)
        {
            return Ret {
                get<Indices1> (std::forward<Tuple1> (t1)) ...,
                get<Indices2> (std::forward<Tuple2> (t2)) ...
            };
        }

        template<typename Tuple1, typename Tuple2>
        constexpr auto two_tuple_cat (Tuple1 &&t1, Tuple2 &&t2) {
            using Ret = typename two_tuple_cat_type<
                std::decay_t<Tuple1>,
                std::decay_t<Tuple2>
            >::type;
            constexpr auto size1 = tuple_size_v<std::decay_t<Tuple1>>;
            constexpr auto size2 = tuple_size_v<std::decay_t<Tuple2>>;
            return two_tuple_cat<Ret> (
                std::forward<Tuple1> (t1),
                std::forward<Tuple2> (t2),
                std::make_index_sequence<size1> {},
                std::make_index_sequence<size2> {}
            );
        }
    }

    constexpr tuple<> tuple_cat () {
        return tuple<>{};
    }

    template<typename Tuple>
    constexpr Tuple tuple_cat (Tuple &&t) {
        return std::forward<Tuple> (t);
    }

    template<typename Tuple1, typename Tuple2, typename ...Tuples>
    constexpr auto tuple_cat (Tuple1 &&t1, Tuple2 &&t2, Tuples &&...ts) {
        // bad name lookup in VS
        return bot::tuple_cat (detail::two_tuple_cat (
            std::forward<Tuple1> (t1),
            std::forward<Tuple2> (t2)
        ), std::forward<Tuples> (ts) ...);
    }

    // Improvement:
    //  It's a more efficent to expand all params at a time
    //  Ret{std::get<Js>(std::get<Is>(std::forward<Tuples>(tpls)))...};
    // See:
    // https://github.com/ericniebler/meta/blob/master/example/tuple_cat.cpp
    // http://blogs.microsoft.co.il/sasha/2015/02/22/implementing-tuple-part-7
}

#endif // !BOT_TUPLE_H
