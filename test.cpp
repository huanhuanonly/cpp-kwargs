#include <iostream>
#include <vector>
#include <set>
#include <queue>
#include <stack>

#include "CppKwargs.h"

struct _Test
{
    _Test(const char* const __funcName)
    { std::cout << __funcName << "():\n+++\n"; }

    ~_Test()
    { std::cout << "\n---\n\n"; }
};

#define _Test_Begin_  _Test _l_test(__func__);

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

static void _test_empty(Kwargs<> kwargs = { })
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
    auto data_int = kwargs["data_int"_opt].valueOr<_Test_Container_with_Multiple_insertion_methods<int>>();
    auto data_float = kwargs["data_float"_opt].valueOr<_Test_Container_with_Multiple_insertion_methods<float>>();

    for (const auto& i : data_int)
        std::cout << i <<' ';

    std::cout.put('\n');

    for (const auto& i : data_float)
        std::cout << i << ' ';
}

int main(void)
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr), std::cout.tie(nullptr);

    _test_general({ {"name"_opt, "huanhuanonly"}, {"data"_opt, std::vector<int>{1, 4, 3, 3, 2, 2, 3}}, {"old"_opt, std::string_view("1314.520") } });
    
    std::string param_string("Hello huanhuanonly");
    _test_general({ {"name"_opt, param_string}, {"class"_opt, param_string} });
    
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

    return 0;
}