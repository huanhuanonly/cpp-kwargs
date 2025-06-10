#include "test.h"

int main()
{
    using namespace std::string_literals;
    using namespace std::string_view_literals;


    test (KwargsValue value(false)) expect (std::string(value.value<const char*>()) == "false");

    test (KwargsValue value(true)) expect (std::string(value.value<const char*>()) == "true");

    test (KwargsValue value('a')) expect (*value.value<const char*>() == 'a');

    test (KwargsValue value("Welcome to kwargs!")) expect (std::string(value.value<const char*>()) == "Welcome to kwargs!");

    test (KwargsValue value("Welcome to kwargs!"s)) expect (std::string(value.value<const char*>()) == "Welcome to kwargs!");

    test (KwargsValue value("Welcome to kwargs!"sv)) expect (std::string(value.value<const char*>()) == "Welcome to kwargs!");

    test (KwargsValue value(std::vector<char>{'K', 'w', 'a', 'r', 'g', 's', '!', '\0'})) expect (std::string(value.value<const char*>()) == "Kwargs!");

    test (KwargsValue value(std::array<char, sizeof("Kwargs!")>{{'K', 'w', 'a', 'r', 'g', 's', '!', '\0'}})) expect(std::string(value.value<const char*>()) == "Kwargs!");

    test (KwargsValue value({'K', 'w', 'a', 'r', 'g', 's', '!', '\0'})) expect (std::string(value.value<const char*>()) == "Kwargs!");
    
    return testing_completed;
}