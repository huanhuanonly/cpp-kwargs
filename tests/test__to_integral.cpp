#include "test.h"

int main()
{
    using namespace std::string_literals;
    using namespace std::string_view_literals;

    test (KwargsValue value(false)) expect (value == 0);

    test (KwargsValue value(true)) expect (value == 1);

    test (KwargsValue value(13456789)) expect (value == 13456789);

    test (KwargsValue value(-13456789)) expect (value == -13456789);

    test (KwargsValue value(5.2)) expect (value == 5);

    test (KwargsValue value(-5.2)) expect (value == -5);

    test (KwargsValue value("447")) expect (value == 447);

    test (KwargsValue value("-996")) expect (value == -996);

    test (KwargsValue value("112244"s)) expect (value == 112244);

    test (KwargsValue value("-112244"sv)) expect (value == -112244LL);

    test (KwargsValue value(std::vector<char>{'1', '2', '3', '9', '8', '7', '6'})) expect (value == 1239876ULL);

    test (KwargsValue value(std::array<char, sizeof("-443")>{{'-', '4', '4', '3'}})) expect (value == -443LL);

    test (KwargsValue value({'-', '9', '8', '3', '\0'})) expect (value == static_cast<short>(-983));


    test (KwargsValue value("false")) expect (value == 0);
    test (KwargsValue value("true")) expect (value == 1);

    test (KwargsValue value(nullptr)) expect (value == 0ULL);
    test (KwargsValue value((int*)(0x5201314))) expect (value == 0x5201314ULL);
    test (KwargsValue value((int*)(0x5201314))) expect (value == 0x5201314U);


    test (KwargsValue value("max")) expect (value == std::numeric_limits<std::int8_t>::max());
    test (KwargsValue value("min")) expect (value == std::numeric_limits<std::int8_t>::min());

    test (KwargsValue value("max")) expect (value == std::numeric_limits<std::int16_t>::max());
    test (KwargsValue value("min")) expect (value == std::numeric_limits<std::int16_t>::min());

    test (KwargsValue value("max")) expect (value == std::numeric_limits<std::int32_t>::max());
    test (KwargsValue value("min")) expect (value == std::numeric_limits<std::int32_t>::min());

    test (KwargsValue value("max")) expect (value == std::numeric_limits<std::int64_t>::max());
    test (KwargsValue value("min")) expect (value == std::numeric_limits<std::int64_t>::min());


    test (KwargsValue value("max")) expect (value == std::numeric_limits<std::uint8_t>::max());
    test (KwargsValue value("min")) expect (value == std::numeric_limits<std::uint8_t>::min());

    test (KwargsValue value("max")) expect (value == std::numeric_limits<std::uint16_t>::max());
    test (KwargsValue value("min")) expect (value == std::numeric_limits<std::uint16_t>::min());

    test (KwargsValue value("max")) expect (value == std::numeric_limits<std::uint32_t>::max());
    test (KwargsValue value("min")) expect (value == std::numeric_limits<std::uint32_t>::min());

    test (KwargsValue value("max")) expect (value == std::numeric_limits<std::uint64_t>::max());
    test (KwargsValue value("min")) expect (value == std::numeric_limits<std::uint64_t>::min());

    return testing_completed;
}