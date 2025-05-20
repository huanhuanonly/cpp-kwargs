#include <kwargs.h>

int main()
{
    if (auto res = kwargs::detail::constant_name<1>(); res != "1" and res != "0x1")
    {
        return 1;
    }

    if (auto res = kwargs::detail::constant_name<0.25>(); res.substr(0, 4) != "0.25")
    {
        return 2;
    }

    if (auto res = kwargs::detail::constant_name<' '>(); res != " " and res != "0x20")
    {
        return 3;
    }

    enum Enum { A, B, C };

    if (auto res = kwargs::detail::constant_name<C>(); res != "Enum::C" and res != "main::C")
    {
        return 4;
    }

    enum class ClassEnum { A, B, C };

    if (auto res = kwargs::detail::constant_name<ClassEnum::A>(); res != "ClassEnum::A" and res != "main::ClassEnum::A")
    {
        return 5;
    }

    return 0;
}