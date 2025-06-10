#include "test.h"

int main()
{
    test (auto res = kwargs::detail::constant_name<1>()) expect (res == "1" or res == "0x1");

#if defined(__cpp_nontype_template_args) && __cpp_nontype_template_args >= 201911L
    test (auto res = kwargs::detail::constant_name<0.25>()) expect (res.substr(0, 4) == "0.25");
#endif

    test (auto res = kwargs::detail::constant_name<' '>()) expect (res == " " or res == "0x20");

    enum Enum { A, B, C };

    test (auto res = kwargs::detail::constant_name<C>()) expect (res == "Enum::C" or res == "main::C");

    enum class ClassEnum { A, B, C };

    test (auto res = kwargs::detail::constant_name<ClassEnum::A>()) expect (res == "ClassEnum::A" or res == "main::ClassEnum::A");

    return testing_completed;
}