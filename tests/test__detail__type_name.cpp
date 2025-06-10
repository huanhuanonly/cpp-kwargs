#include "test.h"

int main()
{
    test () expect (kwargs::detail::type_name<int>() == "int");

    test () expect  (kwargs::detail::type_name<unsigned int>() == "unsigned int");

    test () expect (kwargs::detail::type_name<float>() == "float");

    test () expect (kwargs::detail::type_name<double>() == "double");

    test () expect (kwargs::detail::type_name<long double>() == "long double");

    test () expect (kwargs::detail::type_name<const char*>() == "const char*");

    test () expect (kwargs::detail::type_name<int&>() == "int&");

    return testing_completed;
}