#include "test.h"

#include <set>

int main()
{
    using namespace std::string_literals;
    using namespace std::string_view_literals;

    test (KwargsValue value(std::make_pair(5, 2))) expect (value == std::make_pair(5, 2));

    test (KwargsValue value(std::make_pair('c', 7))) expect (value == std::make_pair('c', 7));

    test (KwargsValue value(std::make_pair("123", "456"))) expect (value == std::make_pair(123, 456));

    test (KwargsValue value(std::make_pair(std::set<int>{3, 1, 2, 1}, "1e4"))) expect (value == std::make_pair(std::vector<int>{1, 2, 3}, 1e4));

    test (KwargsValue value(std::make_pair(3.14, 2.71))) expect (value == std::make_pair(3.14, 2.71));

    test (KwargsValue value(std::make_pair(42, "hello"s))) expect (value == std::make_pair(42, "hello"s));

    test (KwargsValue value(std::make_pair(1, 0))) expect (value == std::make_pair(true, false));

    test (KwargsValue value(std::make_pair(std::make_pair(1, 2), std::make_pair(3, 4)))) expect (value == std::make_pair(std::make_pair(1, 2), std::make_pair(3, 4)));

    test (KwargsValue value(std::make_pair(""sv, 0))) expect (value == std::make_pair(""sv, 0));

    test (KwargsValue value(std::make_tuple(std::make_pair(1, 2), std::make_pair(3, 4)))) expect (value == std::make_pair(std::make_tuple(1, 2), std::make_tuple(3, 4)));

    return testing_completed;
}