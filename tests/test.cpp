//  ___    ___    ___       _   _  _       _  _____  ___    ___    ___   
// (  _`\ (  _`\ (  _`\    ( ) ( )( )  _  ( )(  _  )|  _`\ (  _`\ (  _`\ 
// | ( (_)| |_) )| |_) )   | |/'/'| | ( ) | || (_) || (_) )| ( (_)| (_(_)
// | |  _ | ,__/'| ,__/'   | , <  | | | | | ||  _  || ,  / | |___ `\__ \ 
// | (_( )| |    | |       | |\`\ | (_/ \_) || | | || |\ \ | (_, )( )_) |
// (____/'(_)    (_)       (_) (_)`\___x___/'(_) (_)(_) (_)(____/'`\____)
//                                                                       
//                         https://github.com/huanhuanonly/cpp-kwargs    
//                                                                       
// Copyright (c) 2024-2025 Yang Huanhuan (3347484963@qq.com).
// 
// Before using this file, please read its license:
//
//     https://github.com/huanhuanonly/cpp-kwargs/blob/main/LICENSE
//
// THIS  SOFTWARE  IS  PROVIDED  "AS IS",  WITHOUT  WARRANTY OF ANY KIND,
// EXPRESS  OR IMPLIED.  THE  AUTHOR  RESERVES  THE  RIGHT  TO MODIFY THE
// LICENSE OF THIS FILE AT ANY TIME.
//

/**
* test.cpp
* 
* Created by Yang Huanhuan on December 29, 2024, 14:40:45
*/

#include <iostream>
#include <iomanip>
#include <vector>
#include <set>
#include <queue>
#include <stack>

#if __has_include(<kwargs.h>)
#   include <kwargs.h>
#else
#   include "../include/kwargs.h"
#endif

#if defined(_MSC_VER)
#   pragma warning (disable : 4866)  // 'file(line_number)' compiler may not enforce left-to-right evaluation order for call to operator_name
#   pragma warning (disable : 5045)  // C5045: Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
#endif

struct _Test
{
    _Test(const char* const __funcName)
    { std::cout << __funcName << "():\n+++\n"; }

    ~_Test()
    { std::cout << "\n---\n\n"; }
};

#define _Test_Begin_  _Test _l_test(__func__);


template<typename _Container>
auto operator<<(std::ostream& __os, const _Container& __c)
    -> std::enable_if_t<
        kwargs::detail::is_iterable_container_v<_Container> &&
        kwargs::detail::not_v<kwargs::detail::is_any_of_v<_Container, std::string, std::string_view>>, std::ostream&>
{
    __os.put('[');
    for (const auto& value : __c)
    {
        __os << value << ", ";
    }
    __os.put(']');

    return __os;
}


static void _test_general(Kwargs<"name"_opt, "data"_opt, "old"_opt, "class"_opt, 2024_opt> __dict)
{
    _Test_Begin_

    std::cout << __dict["name"_opt]->valueOr<std::string>("EmptyName") << '\n';
    std::cout << __dict["old"_opt]->valueOr<int>() << '\n';

    std::cout << __dict["class"_opt]->valueOr<std::string>("EmptyClass") << '\n';

    if (__dict[2024_opt].hasValue())
    {
        std::cout << __dict[2024_opt].valueOr<const char*>() << 2024 << '\n';
    }

    auto data = __dict["data"_opt]->valueOr<std::vector<int>>();

    for (const auto& i : data)
        std::cout << i << ' ';
}

static void _test_empty([[maybe_unused]] Kwargs<> kwargs = { })
{
    _Test_Begin_
}

static void _test_print_list(
    const std::vector<int>& value,
    Kwargs<"sep"_opt, "end"_opt> kwargs = { })
{
    _Test_Begin_

    if (value.empty())
        return;

    for (std::size_t i = 0; i < value.size() - 1; ++i)
        std::cout << value[i],
        std::cout << kwargs["sep"].valueOr<std::string_view>(", ");

    std::cout << value.back();
    std::cout << kwargs["end"].valueOr<std::string_view>("\n");
}

static void _test_same_container__same_value_type(Kwargs<"set"_opt> kwargs)
{
    _Test_Begin_

    std::vector<int> list = kwargs["set"]->valueOr<std::vector<int>>();

    for (const auto& i : list)
    {
        std::cout << i <<' ';
    }
}

static void _test_same_container__different_value_type(Kwargs<"set"_opt> kwargs)
{
    _Test_Begin_

    std::vector<std::string> list = kwargs["set"]->valueOr<std::vector<std::string>>();

    for (const auto& i : list)
    {
        std::cout << i <<' ';
    }
}

static void _test_different_container__same_value_type(Kwargs<"set"_opt> kwargs)
{
    _Test_Begin_

    std::set<int> st = kwargs["set"]->valueOr<std::set<int>>();

    for (const auto& i : st)
    {
        std::cout << i <<' ';
    }

    std::cout.put('\n');

    std::queue<int> que = kwargs["set"]->valueOr<std::queue<int>>();

    while (not que.empty())
    {
        std::cout << que.front() <<' ';
        que.pop();
    }
}

static void _test_different_container__different_value_type(Kwargs<"set"_opt> kwargs)
{
    _Test_Begin_

    std::set<int> st = kwargs["set"]->valueOr<std::set<int>>();

    for (const auto& i : st)
    {
        std::cout << i <<' ';
    }

    std::cout.put('\n');

    std::queue<std::string> que = kwargs["set"]->valueOr<std::queue<std::string>>();

    while (not que.empty())
    {
        std::cout << que.front() <<' ';
        que.pop();
    }
}

template<typename... _Args>
static void _test_args_general(_Args&&... __args)
{
    _Test_Begin_

    Args args(__args...);
    std::cout << args[0].value<int>() << '\n';
    std::cout << args[1].value<char>() << '\n';
    std::cout << args[-1].value<std::string_view>() << '\n';
}


template<typename _Tp>
struct _Test_Container_with_Multiple_insertion_methods : std::vector<_Tp>
{
    using std::vector<_Tp>::value_type;
    using std::vector<_Tp>::vector;
    
    // void push_back(const _Tp&) = delete;
    // void push_back(_Tp&&) = delete;
    
    void insert(const _Tp& __value)
    { std::vector<_Tp>::insert(std::vector<_Tp>::end(), __value); }

    void insert(_Tp&& __value)
    { std::vector<_Tp>::insert(std::vector<_Tp>::end(), std::forward<_Tp>(__value)); }
};

static void _test_container_with_multiple_insertion_methods(Kwargs<> kwargs = { })
{
    _Test_Begin_

    auto data_int = kwargs["data_int"_opt].valueOr<_Test_Container_with_Multiple_insertion_methods<int>>();
    auto data_float = kwargs["data_float"_opt].valueOr<_Test_Container_with_Multiple_insertion_methods<float>>();

    for (const auto& i : data_int)
        std::cout << i <<' ';

    std::cout.put('\n');

    for (const auto& i : data_float)
        std::cout << i << ' ';
}

static void _test_iterator(Kwargs<> kwargs = { })
{
    _Test_Begin_

    for (auto it = kwargs.begin(); it != kwargs.end(); ++it)
    {
        std::cout << static_cast<std::uint64_t>(it->first) << '\n';
    }

    std::cout.put('\n');

    for (auto& [key, value] : kwargs)
    {
        std::cout << static_cast<std::uint64_t>(key) << '\n';
    }
}

static void _test_joinable_kwargskey(Kwargs<"name"_opt, "class"_opt> kwargs)
{
    _Test_Begin_

    std::cout << kwargs["name"_opt or "class"].valueOr<std::string>("name or class");
}

static void _test_case_insensitive()
{
    _Test_Begin_

    std::cout << std::boolalpha;

    std::cout << ("abc"_opt == "ABC"_opt) << '\n';
    std::cout << ("abc"_opt == "AbC"_opt) << '\n';
    std::cout << ("abc"_opt == "aBc"_opt) << '\n';
    std::cout << ("Abc"_opt == "aBC"_opt) << '\n';
}

static void _test_string_to_bool(Kwargs<"true"_opt, "false"_opt> kwargs)
{
    _Test_Begin_

    std::cout << kwargs["true"].valueOr<bool>() << '\n';
    std::cout << kwargs["false"].valueOr<bool>() << '\n';
}

enum class Enum : std::uint32_t
{
    EnumA = 1,
    EnumB,
    EnumC
};

static void _test_to_enum(Kwargs<"enum"_opt> kwargs)
{
    _Test_Begin_

    std::cout << static_cast<std::underlying_type_t<Enum>>(kwargs["enum"_opt].valueOr<Enum>()) << '\n';
}

static void _test_kwargsvalue_typename()
{
    _Test_Begin_

    std::cout << KwargsValue(1).typeName() << '\n';
    std::cout << KwargsValue(1ull).typeName() << '\n';
    std::cout << KwargsValue("abc").typeName() << '\n';
    std::cout << KwargsValue(std::string("abc")).typeName() << '\n';
    std::cout << KwargsValue(std::set<int>{1}).typeName() << '\n';
}

int main(void)
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr), std::cout.tie(nullptr);

    _test_general({ {"name"_opt, "huanhuanonly"}, {"data"_opt, std::vector<int>{1, 4, 3, 3, 2, 2, 3}}, {"old"_opt, std::string_view("1314.520") } });
    _test_general({ "name"_opt = "huanhuanonly", "data"_opt = std::vector<int>{1, 4, 3, 3, 2, 2, 3}, "old"_opt = std::string_view("1314.520") });
    
    std::string param_string("Hello huanhuanonly");
    _test_general({ {"name"_opt, param_string}, {"class"_opt, param_string} });
    _test_general({ "name"_opt = param_string, "class"_opt = param_string });
    
    _test_general({ {"name"_opt, 2024}, {"class"_opt, 2025} });
    
    const int param_int = 996;
    _test_general({ {"name"_opt, param_int}, {"class"_opt, param_int} });

    std::array<char, 7> array_char{ 'h', 'e', 'l', 'l', 'o', ' ', '\0' };
    _test_general({ {2024_opt, array_char} });

    _test_empty();

    _test_print_list({ 1, 4, 3, 3, 2, 2, 3 }, { {"sep"_opt, " | "}, {"end"_opt, "."} });

    _test_same_container__same_value_type({ { "set", std::vector<int>{ 1, 4, 3, 3, 2, 2, 3 } } });

    _test_same_container__different_value_type({ { "set", std::vector<int>{ 1, 4, 3, 3, 2, 2, 3 } } });
    
    _test_different_container__same_value_type({ { "set", std::vector<int>{ 1, 4, 3, 3, 2, 2, 3 } } });

    _test_different_container__different_value_type({ { "set", std::vector<int>{ 1, 4, 3, 3, 2, 2, 3 } } });

    _test_args_general(65, 98, std::string("c"));

    _test_container_with_multiple_insertion_methods({ {"data_int", std::set<int>{ 1, 3, 1, 4, 5, 2, 0 }}, {"data_float", std::set<int>{ 1, 3, 1, 4, 5, 2, 0 }} });

    _test_iterator({ { 'a'_opt, 0 }, { 'b'_opt, 0 }, { 9999999999999999.999999999999999999_opt, 0 } });

    _test_joinable_kwargskey({ {"name", "name: huanhuanonly"} });
    _test_joinable_kwargskey({ {"class", "class: huanhuanonly"} });
    _test_joinable_kwargskey({ {"name", "name: huanhuanonly"}, {"class", "class: huanhuanonly"} });
    _test_joinable_kwargskey({ });

    _test_case_insensitive();

    _test_string_to_bool({ {"true", "true"}, {"false", std::string("false")} });
    _test_string_to_bool({ {"true", "True"}, {"false", std::string_view("False")} });

    _test_general({ {"name"_opt, Enum::EnumC}, {"old"_opt, Enum::EnumB} });
    _test_to_enum({ {"enum", 1} });

    _test_kwargsvalue_typename();

    return 0;
}

//  _                           _                                         _           //
// ( )                         ( )                                       (_ )         //
// | |__   _   _    _ _   ___  | |__   _   _    _ _   ___     _     ___   | |  _   _  //
// |  _ `\( ) ( ) /'_` )/' _ `\|  _ `\( ) ( ) /'_` )/' _ `\ /'_`\ /' _ `\ | | ( ) ( ) //
// | | | || (_) |( (_| || ( ) || | | || (_) |( (_| || ( ) |( (_) )| ( ) | | | | (_) | //
// (_) (_)`\___/'`\__,_)(_) (_)(_) (_)`\___/'`\__,_)(_) (_)`\___/'(_) (_)(___)`\__, | //
// https://github.com/huanhuanonly                                            ( )_| | //
//                                                                            `\___/' //