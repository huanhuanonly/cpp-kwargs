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

static void _test_general(Kwargs<"name"_opt, "data"_opt, "old"_opt, "class"_opt> __dict)
{
    _Test_Begin_

    std::cout << __dict["name"_opt]->valueOr<std::string>("EmptyName") << '\n';
    std::cout << __dict["old"_opt]->valueOr<int>() << '\n';

    std::cout << __dict["class"_opt]->valueOr<std::string>("EmptyClass") << '\n';

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

    _test_empty();

    _test_print_list({ 1, 4, 3, 3, 2, 2, 3 }, { {"sep"_opt, " | "}, {"end"_opt, "."} });

    _test_same_container__same_value_type({ { "set", std::vector<int>{ 1, 4, 3, 3, 2, 2, 3 } } });

    _test_same_container__different_value_type({ { "set", std::vector<int>{ 1, 4, 3, 3, 2, 2, 3 } } });
    
    _test_different_container__same_value_type({ { "set", std::vector<int>{ 1, 4, 3, 3, 2, 2, 3 } } });

    _test_different_container__different_value_type({ { "set", std::vector<int>{ 1, 4, 3, 3, 2, 2, 3 } } });

    return 0;
}