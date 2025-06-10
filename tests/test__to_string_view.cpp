#include "test.h"

int main()
{
    using namespace std::string_literals;
    using namespace std::string_view_literals;


    test (KwargsValue value(false)) expect (value.value<std::string_view>() == "false");

    test (KwargsValue value(true)) expect (value.value<std::string_view>() == "true");

    test (KwargsValue value('a')) expect (value.value<std::string_view>().front() == 'a');

    test (KwargsValue value("Welcome to kwargs!")) expect (value.value<std::string_view>() == "Welcome to kwargs!");

    test (KwargsValue value("Welcome to kwargs!"s)) expect (value.value<std::string_view>() == "Welcome to kwargs!");

    test (KwargsValue value("Welcome to kwargs!"sv)) expect (value.value<std::string_view>() == "Welcome to kwargs!");

    test (KwargsValue value(std::vector<char>{'K', 'w', 'a', 'r', 'g', 's', '!'})) expect (value.value<std::string_view>() == "Kwargs!");

    test (KwargsValue value(std::array<char, sizeof("Kwargs!") - 1>{{'K', 'w', 'a', 'r', 'g', 's', '!'}})) expect(value.value<std::string_view>() == "Kwargs!");

    test (KwargsValue value({'K', 'w', 'a', 'r', 'g', 's', '!', '\0'})) expect (value.value<std::string_view>() == "Kwargs!");

    return testing_completed;
}