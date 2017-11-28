//
// Test Minimal Tuple Library
// by BOT Man, 2017
//

#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include <cassert>

#include "tuple.h"

int main (int argc, char *argv[])
{
    // helpers

    using namespace std::string_literals;

    int ref_int = 1;
    const int cref_int = 0;
    auto constRValueGen = [] (const auto &val) -> const auto { return val; };

    /// tuple

    // implicit
    std::tuple<> std_empty_implicit_ctor_tuple;
    bot::tuple<> bot_empty_implicit_ctor_tuple;

    std::tuple<int> std_single_implicit_ctor_tuple;
    bot::tuple<int> bot_single_implicit_ctor_tuple;

    std::tuple<int, double> std_double_implicit_ctor_tuple;
    bot::tuple<int, double> bot_double_implicit_ctor_tuple;

    std::tuple<int, double, std::string> std_triple_implicit_ctor_tuple;
    bot::tuple<int, double, std::string> bot_triple_implicit_ctor_tuple;

    //std::tuple<int &> std_ref_implicit_ctor_tuple ();
    //bot::tuple<int &> bot_ref_implicit_ctor_tuple ();

    // explicit
    std::tuple<> std_empty_tuple {};
    bot::tuple<> bot_empty_tuple {};

    std::tuple<int> std_single_tuple (1);
    bot::tuple<int> bot_single_tuple (1);

    std::tuple<int, double> std_double_tuple (2, 2.2);
    bot::tuple<int, double> bot_double_tuple (2, 2.2);

    std::tuple<int, double, std::string> std_triple_tuple (3, 3.3, "str"s);
    bot::tuple<int, double, std::string> bot_triple_tuple (3, 3.3, "str"s);

    std::tuple<int &> std_ref_tuple (ref_int);
    bot::tuple<int &> bot_ref_tuple (ref_int);

    std::tuple<const int &> std_cref_tuple (cref_int);
    bot::tuple<const int &> bot_cref_tuple (cref_int);

    std::tuple<int &&> std_rref_tuple (1);
    bot::tuple<int &&> bot_rref_tuple (1);

    // move & copy
    std::tuple<int> std_move_ctor_tuple { std::tuple<int> (2) };
    bot::tuple<int> bot_move_ctor_tuple { bot::tuple<int> (2) };

    std::tuple<int> std_copy_ctor_tuple { std_move_ctor_tuple };
    bot::tuple<int> bot_copy_ctor_tuple { bot_move_ctor_tuple };

    std::tuple<int &> std_move_ctor_ref_tuple { std::tuple<int &> (ref_int) };
    bot::tuple<int &> bot_move_ctor_ref_tuple { bot::tuple<int &> (ref_int) };

    std::tuple<int &> std_copy_ctor_ref_tuple { std_move_ctor_ref_tuple };
    bot::tuple<int &> bot_copy_ctor_ref_tuple { bot_move_ctor_ref_tuple };

    auto std_move_assign_tuple = std::tuple<int> (3);
    auto bot_move_assign_tuple = bot::tuple<int> (3);

    auto std_copy_assign_tuple = std_move_assign_tuple;
    auto bot_copy_assign_tuple = bot_move_assign_tuple;

    auto std_move_assign_ref_tuple = std::tuple<int &> (ref_int);
    auto bot_move_assign_ref_tuple = bot::tuple<int &> (ref_int);

    auto std_copy_assign_ref_tuple = std_move_assign_ref_tuple;
    auto bot_copy_assign_ref_tuple = bot_move_assign_ref_tuple;

    // convert move & copy
    std::tuple<long> std_conv_copy_assign_tuple = std_move_assign_tuple;
    bot::tuple<long> bot_conv_copy_assign_tuple = bot_move_assign_tuple;

    /// tuple_size

    static_assert(
        std::tuple_size<decltype (std_empty_tuple)>::value ==
        bot::tuple_size_v<decltype (bot_empty_tuple)>, "");

    static_assert(
        std::tuple_size<decltype (std_single_tuple)>::value ==
        bot::tuple_size_v<decltype (bot_single_tuple)>, "");

    static_assert(
        std::tuple_size<decltype (std_double_tuple)>::value ==
        bot::tuple_size_v<decltype (bot_double_tuple)>, "");

    static_assert(
        std::tuple_size<decltype (std_triple_tuple)>::value ==
        bot::tuple_size_v<decltype (bot_triple_tuple)>, "");

    /// tuple_element

    //static_assert(std::is_same<
    //    std::tuple_element_t<0, decltype (std_empty_tuple)>,
    //    bot::tuple_element_t<0, decltype (bot_empty_tuple)>
    //>::value, "");

    static_assert(std::is_same<
        std::tuple_element_t<0, decltype (std_single_tuple)>,
        bot::tuple_element_t<0, decltype (bot_single_tuple)>
    >::value, "");

    static_assert(std::is_same<
        std::tuple_element_t<0, decltype (std_double_tuple)>,
        bot::tuple_element_t<0, decltype (bot_double_tuple)>
    >::value, "");
    static_assert(std::is_same<
        std::tuple_element_t<1, decltype (std_double_tuple)>,
        bot::tuple_element_t<1, decltype (bot_double_tuple)>
    >::value, "");

    static_assert(std::is_same<
        std::tuple_element_t<0, decltype (std_triple_tuple)>,
        bot::tuple_element_t<0, decltype (bot_triple_tuple)>
    >::value, "");
    static_assert(std::is_same<
        std::tuple_element_t<1, decltype (std_triple_tuple)>,
        bot::tuple_element_t<1, decltype (bot_triple_tuple)>
    >::value, "");
    static_assert(std::is_same<
        std::tuple_element_t<2, decltype (std_triple_tuple)>,
        bot::tuple_element_t<2, decltype (bot_triple_tuple)>
    >::value, "");

    static_assert(std::is_same<
        std::tuple_element_t<0, decltype (std_ref_tuple)>,
        bot::tuple_element_t<0, decltype (bot_ref_tuple)>
    >::value, "");
    static_assert(std::is_same<
        std::tuple_element_t<0, decltype (std_ref_tuple)>,
        bot::tuple_element_t<0, decltype (bot_ref_tuple)>
    >::value, "");

    /// get (by index)

    //assert (
    //    std::get<0> (std_empty_tuple) ==
    //    bot::get<0> (bot_empty_tuple)
    //);

    assert (
        std::get<0> (std_single_tuple) ==
        bot::get<0> (bot_single_tuple)
    );

    assert (
        std::get<0> (std_double_tuple) ==
        bot::get<0> (bot_double_tuple)
    );
    assert (
        std::get<1> (std_double_tuple) ==
        bot::get<1> (bot_double_tuple)
    );

    assert (
        std::get<0> (std_triple_tuple) ==
        bot::get<0> (bot_triple_tuple)
    );
    assert (
        std::get<1> (std_triple_tuple) ==
        bot::get<1> (bot_triple_tuple)
    );
    assert (
        std::get<2> (std_triple_tuple) ==
        bot::get<2> (bot_triple_tuple)
    );

    ref_int = 2;
    assert (
        std::get<0> (std_ref_tuple) ==
        bot::get<0> (bot_ref_tuple)
    );

    std::get<0> (std_ref_tuple) = 3;
    assert (
        std::get<0> (std_ref_tuple) ==
        bot::get<0> (bot_ref_tuple)
    );

    bot::get<0> (bot_ref_tuple) = 4;
    assert (
        std::get<0> (std_ref_tuple) ==
        bot::get<0> (bot_ref_tuple)
    );

    const auto std_const_single_tuple = std_single_tuple;
    const auto bot_const_single_tuple = bot_single_tuple;

    assert (
        std::get<0> (std_const_single_tuple) ==
        bot::get<0> (bot_const_single_tuple)
    );

    assert (
        std::get<0> (std::make_tuple (5)) ==
        bot::get<0> (bot::make_tuple (5))
    );

    assert (
        std::get<0> (constRValueGen (std_single_tuple)) ==
        bot::get<0> (constRValueGen (bot_single_tuple))
    );

    /// get (by type)

    assert (
        std::get<int> (std_triple_tuple) ==
        bot::get<int> (bot_triple_tuple)
    );
    assert (
        std::get<double> (std_triple_tuple) ==
        bot::get<double> (bot_triple_tuple)
    );
    assert (
        std::get<std::string> (std_triple_tuple) ==
        bot::get<std::string> (bot_triple_tuple)
    );

    ref_int = 2;
    assert (
        std::get<int &> (std_ref_tuple) ==
        bot::get<int &> (bot_ref_tuple)
    );

    std::get<0> (std_ref_tuple) = 3;
    assert (
        std::get<int &> (std_ref_tuple) ==
        bot::get<int &> (bot_ref_tuple)
    );

    bot::get<0> (bot_ref_tuple) = 4;
    assert (
        std::get<int &> (std_ref_tuple) ==
        bot::get<int &> (bot_ref_tuple)
    );

    /// make_tuple

    auto std_empty_made_tuple = std::make_tuple ();
    auto bot_empty_made_tuple = bot::make_tuple ();

    auto std_single_made_tuple = std::make_tuple (1);
    auto bot_single_made_tuple = bot::make_tuple (1);

    auto std_double_made_tuple = std::make_tuple (2, 2.2);
    auto bot_double_made_tuple = bot::make_tuple (2, 2.2);

    auto std_triple_made_tuple = std::make_tuple (3, 3.3, "str"s);
    auto bot_triple_made_tuple = bot::make_tuple (3, 3.3, "str"s);

    auto std_ref_made_tuple = std::make_tuple (std::ref (ref_int));
    auto bot_ref_made_tuple = bot::make_tuple (std::ref (ref_int));

    auto std_non_ref_made_tuple = std::make_tuple (ref_int);
    auto bot_non_ref_made_tuple = bot::make_tuple (ref_int);

    static_assert(std::is_same<
        decltype(std_empty_made_tuple),
        std::tuple<>
    >::value, "");
    static_assert(std::is_same<
        decltype(bot_empty_made_tuple),
        bot::tuple<>
    >::value, "");

    static_assert(std::is_same<
        decltype(std_single_made_tuple),
        std::tuple<int>
    >::value, "");
    static_assert(std::is_same<
        decltype(bot_single_made_tuple),
        bot::tuple<int>
    >::value, "");

    static_assert(std::is_same<
        decltype(std_double_made_tuple),
        std::tuple<int, double>
    >::value, "");
    static_assert(std::is_same<
        decltype(bot_double_made_tuple),
        bot::tuple<int, double>
    >::value, "");

    static_assert(std::is_same<
        decltype(std_triple_made_tuple),
        std::tuple<int, double, std::string>
    >::value, "");
    static_assert(std::is_same<
        decltype(bot_triple_made_tuple),
        bot::tuple<int, double, std::string>
    >::value, "");

    static_assert(std::is_same<
        decltype(std_ref_made_tuple),
        std::tuple<int &>
    >::value, "");
    static_assert(std::is_same<
        decltype(bot_ref_made_tuple),
        bot::tuple<int &>
    >::value, "");

    static_assert(std::is_same<
        decltype(std_non_ref_made_tuple),
        std::tuple<int>
    >::value, "");
    static_assert(std::is_same<
        decltype(bot_non_ref_made_tuple),
        bot::tuple<int>
    >::value, "");

    ref_int = 2;
    assert (
        std::get<0> (std_ref_made_tuple) ==
        bot::get<0> (bot_ref_made_tuple)
    );

    std::get<0> (std_ref_made_tuple) = 3;
    assert (
        std::get<0> (std_ref_made_tuple) ==
        bot::get<0> (bot_ref_made_tuple)
    );

    bot::get<0> (bot_ref_made_tuple) = 4;
    assert (
        std::get<0> (std_ref_made_tuple) ==
        bot::get<0> (bot_ref_made_tuple)
    );

    ref_int = 2;
    assert (
        std::get<0> (std_non_ref_made_tuple) ==
        bot::get<0> (bot_non_ref_made_tuple)
    );

    std::get<0> (std_non_ref_made_tuple) = 3;
    assert (
        std::get<0> (std_non_ref_made_tuple) !=
        bot::get<0> (bot_non_ref_made_tuple)
    );

    bot::get<0> (bot_non_ref_made_tuple) = 4;
    assert (
        std::get<0> (std_non_ref_made_tuple) !=
        bot::get<0> (bot_non_ref_made_tuple)
    );

    /// tie && ignore

    int std_int, bot_int;
    double std_double, bot_double;
    std::string std_string, bot_string;

    std::tie (std_int, std_double, std_string) = std_triple_tuple;
    bot::tie (bot_int, bot_double, bot_string) = bot_triple_tuple;

    // 0 -> 3
    assert (std_int == bot_int);
    assert (std_double == bot_double);
    assert (std_string == bot_string);

    std::tie (std_int, std_double, std_string) = std::make_tuple (1, .1, ""s);
    bot::tie (bot_int, bot_double, bot_string) = bot::make_tuple (1, .1, ""s);

    // 3 -> 1
    assert (std_int == bot_int);
    assert (std_double == bot_double);
    assert (std_string == bot_string);

    std::tie (std_int, std::ignore) = std::make_tuple (2, ""s);
    bot::tie (bot_int, bot::ignore) = bot::make_tuple (2, ""s);

    // 1 -> 2
    assert (std_int == bot_int);

    static_assert (std::is_same<
        decltype(std::tie (std_int, std_double, std_string, std::ignore)),
        std::tuple<int &, double &, std::string &, decltype(std::ignore) &>
    >::value, "");
    static_assert (std::is_same<
        decltype(bot::tie (bot_int, bot_double, bot_string, bot::ignore)),
        bot::tuple<int &, double &, std::string &, decltype(bot::ignore) &>
    >::value, "");

    /// forward_as_tuple

    static_assert (std::is_same<decltype(
        std::forward_as_tuple (ref_int, cref_int, 1.1, constRValueGen (""s))),
        std::tuple<int &, const int &, double &&, const std::string &&>
    >::value, "");
    static_assert (std::is_same<decltype(
        std::forward_as_tuple (ref_int, cref_int, 1.1, constRValueGen (""s))),
        std::tuple<int &, const int &, double &&, const std::string &&>
    >::value, "");

    std::forward_as_tuple (std_int, std_double, std_string) = std_triple_tuple;
    bot::forward_as_tuple (bot_int, bot_double, bot_string) = bot_triple_tuple;

    // 2 -> 3
    assert (std_int == bot_int);
    assert (std_double == bot_double);
    assert (std_string == bot_string);

    static_assert (std::is_same<
        decltype(std::forward_as_tuple (std_int, std_double, std_string)),
        decltype(std::tie (std_int, std_double, std_string))
    >::value, "");
    static_assert (std::is_same<
        decltype(bot::forward_as_tuple (bot_int, bot_double, bot_string)),
        decltype(bot::tie (bot_int, bot_double, bot_string))
    >::value, "");

    /// tuple_cat

    auto std_cat_tuple = std::tuple_cat (std_empty_tuple,
        std_single_tuple, std_double_tuple, std_triple_tuple);
    auto bot_cat_tuple = bot::tuple_cat (bot_empty_tuple,
        bot_single_tuple, bot_double_tuple, bot_triple_tuple);

    static_assert(std::is_same<decltype(std_cat_tuple),
        std::tuple<int, int, double, int, double, std::string>
    >::value, "");
    static_assert(std::is_same<decltype(bot_cat_tuple),
        bot::tuple<int, int, double, int, double, std::string>
    >::value, "");

    assert (
        std::get<0> (std_cat_tuple) ==
        bot::get<0> (bot_cat_tuple)
    );
    assert (
        std::get<1> (std_cat_tuple) ==
        bot::get<1> (bot_cat_tuple)
    );
    assert (
        std::get<2> (std_cat_tuple) ==
        bot::get<2> (bot_cat_tuple)
    );
    assert (
        std::get<3> (std_cat_tuple) ==
        bot::get<3> (bot_cat_tuple)
    );
    assert (
        std::get<4> (std_cat_tuple) ==
        bot::get<4> (bot_cat_tuple)
    );
    assert (
        std::get<5> (std_cat_tuple) ==
        bot::get<5> (bot_cat_tuple)
    );

    auto std_ref_cat_tuple = std::tuple_cat (std_empty_tuple, std_ref_tuple);
    auto bot_ref_cat_tuple = bot::tuple_cat (bot_empty_tuple, bot_ref_tuple);

    static_assert(std::is_same<
        decltype(std_ref_cat_tuple),
        std::tuple<int &>
    >::value, "");
    static_assert(std::is_same<
        decltype(bot_ref_cat_tuple),
        bot::tuple<int &>
    >::value, "");

    ref_int = 2;
    assert (
        std::get<0> (std_ref_cat_tuple) ==
        bot::get<0> (bot_ref_cat_tuple)
    );

    std::get<0> (std_ref_cat_tuple) = 3;
    assert (
        std::get<0> (std_ref_cat_tuple) ==
        bot::get<0> (bot_ref_cat_tuple)
    );

    bot::get<0> (bot_ref_cat_tuple) = 4;
    assert (
        std::get<0> (std_ref_cat_tuple) ==
        bot::get<0> (bot_ref_cat_tuple)
    );

    auto std_empty_cat_tuple = std::tuple_cat (std_empty_tuple);
    auto bot_empty_cat_tuple = bot::tuple_cat (bot_empty_tuple);

    auto std_null_cat_tuple = std::tuple_cat ();
    auto bot_null_cat_tuple = bot::tuple_cat ();

    static_assert(std::is_same<
        decltype(std_empty_cat_tuple),
        decltype(std_null_cat_tuple)
    >::value, "");
    static_assert(std::is_same<
        decltype(bot_empty_cat_tuple),
        decltype(bot_null_cat_tuple)
    >::value, "");

    return 0;
}
