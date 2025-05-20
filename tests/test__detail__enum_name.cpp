#include <kwargs.h>

int main()
{
    enum Enum { A, B, C };

    if (auto res = kwargs::detail::enum_name<C>(); res != "C")
    {
        return 1;
    }

    enum class ClassEnum { A, B, C };

    if (auto res = kwargs::detail::enum_name<ClassEnum::A>(); res != "A")
    {
        return 2;
    }

    return 0;
}