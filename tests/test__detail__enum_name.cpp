#include "test.h"

int main()
{
    enum Enum { A, B, C };

    test (auto res = kwargs::detail::enum_name<C>()) expect (res == "C");

    enum class ClassEnum { A, B, C };

    test (auto res = kwargs::detail::enum_name<ClassEnum::A>()) expect (res == "A");

    return testing_completed;
}