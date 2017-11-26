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
    using namespace std::string_literals;

    // Default ctor

    std::tuple<> std_empty_tuple;
    bot::tuple<> bot_empty_tuple;

    std::tuple<int> std_single_tuple;
    bot::tuple<int> bot_single_tuple;

    std::tuple<int, double> std_double_tuple;
    bot::tuple<int, double> bot_double_tuple;

    std::tuple<int, double, std::string> std_triple_tuple;
    bot::tuple<int, double, std::string> bot_triple_tuple;

    // Explicit ctor

    std::tuple<int, std::string> std_val_ctor_tuple (1, "str"s);
    bot::tuple<int, std::string> bot_val_ctor_tuple (1, "str"s);

    int ref_int = 2;
    std::tuple<int &, std::string> std_ref_ctor_tuple (ref_int, "str"s);
    bot::tuple<int &, std::string> bot_ref_ctor_tuple (ref_int, "str"s);

    std::tuple<std::string> std_move_ctor_tuple (
        std::tuple<std::string> (""s));
    bot::tuple<std::string> bot_move_ctor_tuple (
        bot::tuple<std::string> (""s));

    std::tuple<std::string> std_copy_ctor_tuple (std_move_ctor_tuple);
    bot::tuple<std::string> bot_copy_ctor_tuple (bot_move_ctor_tuple);

    auto std_move_assign_tuple = std::tuple<std::string> (""s);
    auto bot_move_assign_tuple = bot::tuple<std::string> (""s);

    auto std_copy_assign_tuple = std_move_assign_tuple;
    auto bot_copy_assign_tuple = bot_move_assign_tuple;

    // tuple_size

    static_assert(
        std::tuple_size<decltype (std_empty_tuple)>::value ==
        bot::tuple_size_v<decltype (bot_empty_tuple)>, "");

    static_assert(
        std::tuple_size<decltype (std_single_tuple)>::value ==
        bot::tuple_size_v<decltype (bot_single_tuple)>, "");

    static_assert(std::tuple_size<decltype (std_double_tuple)>::value ==
        bot::tuple_size_v<decltype (bot_double_tuple)>, "");

    static_assert(std::tuple_size<decltype (std_triple_tuple)>::value ==
        bot::tuple_size_v<decltype (bot_triple_tuple)>, "");

    // tuple_element

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
        std::tuple_element_t<0, decltype (std_ref_ctor_tuple)>,
        bot::tuple_element_t<0, decltype (bot_ref_ctor_tuple)>
    >::value, "");

    // make_tuple

    auto std_empty_made_tuple = std::make_tuple ();
    auto bot_empty_made_tuple = bot::make_tuple ();

    auto std_single_made_tuple = std::make_tuple (1);
    auto bot_single_made_tuple = bot::make_tuple (1);

    auto std_double_made_tuple = std::make_tuple (2, 2.2);
    auto bot_double_made_tuple = bot::make_tuple (2, 2.2);

    auto std_triple_made_tuple = std::make_tuple (3, 3.3, "str"s);
    auto bot_triple_made_tuple = bot::make_tuple (3, 3.3, "str"s);

    // get (by index)

    //assert (
    //    std::get<0> (std_empty_made_tuple) ==
    //    bot::get<0> (bot_empty_made_tuple)
    //);
    assert (
        std::get<0> (std_single_made_tuple) ==
        bot::get<0> (bot_single_made_tuple)
    );

    assert (
        std::get<0> (std_double_made_tuple) ==
        bot::get<0> (bot_double_made_tuple)
    );
    assert (
        std::get<1> (std_double_made_tuple) ==
        bot::get<1> (bot_double_made_tuple)
    );

    assert (
        std::get<0> (std_triple_made_tuple) ==
        bot::get<0> (bot_triple_made_tuple)
    );
    assert (
        std::get<1> (std_triple_made_tuple) ==
        bot::get<1> (bot_triple_made_tuple)
    );
    assert (
        std::get<2> (std_triple_made_tuple) ==
        bot::get<2> (bot_triple_made_tuple)
    );

    // get (by type)

    assert (
        std::get<int> (std_triple_made_tuple) ==
        bot::get<int> (bot_triple_made_tuple)
    );
    assert (
        std::get<double> (std_triple_made_tuple) ==
        bot::get<double> (bot_triple_made_tuple)
    );
    assert (
        std::get<std::string> (std_triple_made_tuple) ==
        bot::get<std::string> (bot_triple_made_tuple)
    );

    // tuple_cat

    auto std_cat_tuple = std::tuple_cat (
        std_single_made_tuple, std_double_made_tuple, std_triple_made_tuple);
    auto bot_cat_tuple = bot::tuple_cat (
        bot_single_made_tuple, bot_double_made_tuple, bot_triple_made_tuple);

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

    return 0;
}
