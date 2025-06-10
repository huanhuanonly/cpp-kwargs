#include "test.h"

int main()
{
    test (KwargsValue value(std::array<int, 4>{{ 1, 3, 1, 4 }})) expect (value == std::array<int, 4>{{ 1, 3, 1, 4 }});

    int a[]{ 9, 9, 6 };
    test (KwargsValue value(a)) expect (value.value<int[]>() == static_cast<int*>(a));

    test (KwargsValue value(a)) expect (value == std::array<int, 3>{{ 9, 9, 6 }});

    test (KwargsValue value(a)) expect (value == std::array<int, 10>{{ 9, 9, 6 }});

    test (KwargsValue value(a)) expect (value == std::array<int, 2>{{ 9, 9 }});

    test (KwargsValue value(a)) expect (value == std::array<std::string, 2>{{ "9", "9" }});

    test (KwargsValue value(a)) expect (value.value<std::string[2]>() == std::array<std::string, 2>{{ "9", "9" }});

    return testing_completed;
}