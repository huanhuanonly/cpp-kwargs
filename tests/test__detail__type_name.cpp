#include <kwargs.h>

int main()
{
    if (kwargs::detail::type_name<int>() != "int")
    {
        return 1;
    }

    if (kwargs::detail::type_name<unsigned int>() != "unsigned int")
    {
        return 2;
    }

    if (kwargs::detail::type_name<float>() != "float")
    {
        return 3;
    }

    if (kwargs::detail::type_name<double>() != "double")
    {
        return 4;
    }

    if (kwargs::detail::type_name<long double>() != "long double")
    {
        return 5;
    }

    if (kwargs::detail::type_name<const char*>() != "const char*")
    {
        return 6;
    }

    if (kwargs::detail::type_name<int&>() != "int&")
    {
        return 7;
    }

    return 0;
}