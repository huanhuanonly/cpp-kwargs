#include "CppKwargs.h"

#include <iostream>
#include <vector>

static void test(Kwargs<"name"_opt, "data"_opt, "old"_opt, "class"_opt> __dict)
{
    std::cout << __dict["name"_opt]->valueOr<std::string>("EmptyName") << '\n';
    std::cout << __dict["old"_opt]->valueOr<int>() << '\n';

    std::cout << __dict["class"_opt]->valueOr<std::string>("EmptyClass") << '\n';

    auto data = __dict["data"_opt]->valueOr<std::vector<int>>();

    for (const auto& i : data)
        std::cout << i << ' ';
}

int main(void)
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr), std::cout.tie(nullptr);

    test({ {"name"_opt, "huanhuanonly"}, {"data"_opt, std::vector<int>{1, 4, 3, 3, 2, 2, 3}}, {"old"_opt, std::string_view("1314.520") } });
    
    return 0;
}