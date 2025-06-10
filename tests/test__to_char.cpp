#include "test.h"

int main()
{
    using namespace std::string_literals;
    using namespace std::string_view_literals;


    test (KwargsValue value(false)) expect (value.value<char>() == 'f');

    test (KwargsValue value(true)) expect (value.value<char>() == 't');

    test (KwargsValue value('a')) expect (value.value<char>() == 'a');
    test (KwargsValue value('a')) expect (value.value<signed char>() == 'a');
    test (KwargsValue value('a')) expect (value.value<unsigned char>() == 'a');

    test (KwargsValue value("Welcome to kwargs!")) expect (value.value<char>() == 'W');

    test (KwargsValue value("Welcome to kwargs!"s)) expect (value.value<char>() == 'W');

    test (KwargsValue value("Welcome to kwargs!"sv)) expect (value.value<char>() == 'W');

    test (KwargsValue value(std::vector<char>{'K', 'w', 'a', 'r', 'g', 's', '!', '\0'})) expect (value.value<char>() == 'K');

    test (KwargsValue value(std::array<char, sizeof("Kwargs!")>{{'K', 'w', 'a', 'r', 'g', 's', '!', '\0'}})) expect(value.value<char>() == 'K');

    test (KwargsValue value({'K', 'w', 'a', 'r', 'g', 's', '!', '\0'})) expect (value.value<char>() == 'K');
    
    return testing_completed;
}