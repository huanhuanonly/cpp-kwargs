#include "test.h"

int main()
{
    using namespace std::string_literals;
    using namespace std::string_view_literals;

    test (KwargsValue value(false)) expect (value.value<double>() == 0);

    test (KwargsValue value(true)) expect (value.value<double>() == 1);

    test (KwargsValue value(13456789)) expect(value.value<double>() == 13456789);

    test (KwargsValue value(-13456789)) expect (value.value<double>() == -13456789);

    test (KwargsValue value(5.222)) expect (value.value<double>() == 5.222);

    test (KwargsValue value(-5.222)) expect (value.value<double>() == -5.222);

    test (KwargsValue value("4.47")) expect (value.value<double>() == 4.47);

    test (KwargsValue value("-99.6")) expect (value.value<double>() == -99.6);

    test (KwargsValue value("11.2244"s)) expect (value.value<double>() == 11.2244);

    test (KwargsValue value("-11.2244"sv)) expect (value.value<double>() == -11.2244);

    test (KwargsValue value(std::vector<char>{'1', '2', '3', '9', '.', '8', '7', '6'})) expect (value.value<double>() == 1239.876);

    test (KwargsValue value(std::array<char, sizeof("-44.3")>{{'-', '4', '4', '.', '3'}})) expect (value.value<double>() == -44.3);

    test (KwargsValue value({'-', '9', 'e', '3', '\0'})) expect(value.value<double>() == -9e3);
    

    test (KwargsValue value("max")) expect(value.value<float>() == std::numeric_limits<float>::max());
    test (KwargsValue value("min")) expect(value.value<float>() == std::numeric_limits<float>::min());

    test (KwargsValue value("max")) expect(value.value<double>() == std::numeric_limits<double>::max());
    test (KwargsValue value("min")) expect(value.value<double>() == std::numeric_limits<double>::min());

    test (KwargsValue value("max")) expect(value.value<long double>() == std::numeric_limits<long double>::max());
    test (KwargsValue value("min")) expect(value.value<long double>() == std::numeric_limits<long double>::min());

    return testing_completed;
}