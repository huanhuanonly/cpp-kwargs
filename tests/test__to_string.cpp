#include <set>
#include <sstream>
#include <iomanip>

#include "test.h"

#if defined(_MSC_VER)
#	pragma warning (disable: 5045)
#endif


int main()
{
    using namespace std::string_literals;
    using namespace std::string_view_literals;

    disable_test_interrupts();

    test (KwargsValue value(false)) expect (value.value<std::string>() == "false");

    test (KwargsValue value(true))  expect (value.value<std::string>() == "true");

    test (KwargsValue value('a'))   expect (value.value<std::string>().front() == 'a');

    test (KwargsValue value("Welcome to kwargs!"))   expect (value.value<std::string>() == "Welcome to kwargs!");

    test (KwargsValue value("Welcome to kwargs!"s))  expect (value.value<std::string>() == "Welcome to kwargs!");

    test (KwargsValue value("Welcome to kwargs!"sv)) expect (value.value<std::string>() == "Welcome to kwargs!");

    test (KwargsValue value(std::vector<char>{'K', 'w', 'a', 'r', 'g', 's', '!'})) expect (value.value<std::string>() == "Kwargs!");

    test (KwargsValue value(std::array<char, sizeof("Kwargs!") - 1>{{'K', 'w', 'a', 'r', 'g', 's', '!'}})) expect(value.value<std::string>() == "Kwargs!");

    test (KwargsValue value(std::set<char>{'b', 'c', 'a'})) expect (value.value<std::string>() == "abc");

    test (KwargsValue value({'K', 'w', 'a', 'r', 'g', 's', '!', '\0'})) expect (value.value<std::string>() == "Kwargs!");
    
    enum Number : std::int8_t
    {
        MinusOne = -1,
        Nine = 9
    };

    test (KwargsValue value(MinusOne)) expect (value.value<std::string>() == std::to_string(std::underlying_type_t<Number>(MinusOne)));
    test (KwargsValue value(Nine))     expect (value.value<std::string>() == std::to_string(std::underlying_type_t<Number>(Nine)));

    test (KwargsValue value(std::numeric_limits<std::int16_t>::min()))  expect (value.value<std::string>() == std::to_string(std::numeric_limits<std::int16_t>::min()));
    test (KwargsValue value(std::numeric_limits<std::int16_t>::max()))  expect (value.value<std::string>() == std::to_string(std::numeric_limits<std::int16_t>::max()));

    test (KwargsValue value(std::numeric_limits<std::int32_t>::min()))  expect (value.value<std::string>() == std::to_string(std::numeric_limits<std::int32_t>::min()));
    test (KwargsValue value(std::numeric_limits<std::int32_t>::max()))  expect (value.value<std::string>() == std::to_string(std::numeric_limits<std::int32_t>::max()));

    test (KwargsValue value(std::numeric_limits<std::int64_t>::min()))  expect (value.value<std::string>() == std::to_string(std::numeric_limits<std::int64_t>::min()));
    test (KwargsValue value(std::numeric_limits<std::int64_t>::max()))  expect (value.value<std::string>() == std::to_string(std::numeric_limits<std::int64_t>::max()));

    test (KwargsValue value(std::numeric_limits<std::uint64_t>::max())) expect (value.value<std::string>() == std::to_string(std::numeric_limits<std::uint64_t>::max()));


    test (KwargsValue value(nullptr)) expect (value.value<std::string>() == "0");

    test (KwargsValue value((void*)(0x1433223AFFD))) expect (value.value<std::string>() == "1433223AFFD");


    auto shrink = [](std::string_view s) -> std::string_view {
        if (s.find_first_of('.') != s.npos)
        {
            while (s.size() > 1 && (s.back() == '0' || s.back() == '.'))
            {
                if (s.back() == '.')
                {
                    s.remove_suffix(1);
                    break;
                }
                else
                {
                    s.remove_suffix(1);
                }
            }
        }

        return s;
    };

    auto to_string = [&shrink](auto value) -> std::string {
        std::stringstream ss;

        ss << std::fixed << std::setprecision(std::numeric_limits<decltype(value)>::digits10) << value;

        auto str = ss.str();
        auto sv = shrink(str);

        return std::string(sv);
    };


    test (KwargsValue value(0.0))    expect (value.value<std::string>() == "0");

    test (KwargsValue value(-0.0))   expect (value.value<std::string>() == "-0");

    test (KwargsValue value(1e-5))   expect (value.value<std::string>() == "0.00001");

    test (KwargsValue value(-1e-5))  expect (value.value<std::string>() == "-0.00001");

    test (KwargsValue value(0.1f))   expect (value.value<std::string>() == "0.1");

    test (KwargsValue value(1.0f))   expect (value.value<std::string>() == "1");

    test (KwargsValue value(1.5f))   expect (value.value<std::string>() == "1.5");

    test (KwargsValue value(1e18))   expect (value.value<std::string>() == "1"s + std::string(18, '0'));

    test (KwargsValue value(1e20))   expect (value.value<std::string>() == "1"s + std::string(20, '0'));

    test (KwargsValue value(-1e20))  expect (value.value<std::string>() == "-1"s + std::string(20, '0'));

    // In Python:
    // 
    // Input:  int(1e30)
    // Output: 1000000000000000019884624838656

    test (KwargsValue value(1e30))   expect (value.value<std::string>() == "1000000000000000019884624838656");

    // In Python:
    // 
    // Input:  int(1e308)
    // Output: 100000000000000001097906362944045541740492309677311846336810682903157585404911491537163328978494688899061249669721172515611590283743140088328307009198146046031271664502933027185697489699588559043338384466165001178426897626212945177628091195786707458122783970171784415105291802893207873272974885715430223118336
    
    test (KwargsValue value(1e308))  expect (value.value<std::string>() == "100000000000000001097906362944045541740492309677311846336810682903157585404911491537163328978494688899061249669721172515611590283743140088328307009198146046031271664502933027185697489699588559043338384466165001178426897626212945177628091195786707458122783970171784415105291802893207873272974885715430223118336");


    test (KwargsValue value(0.0f))   expect (value.value<std::string>() == "0");
    test (KwargsValue value(-0.0f))  expect (value.value<std::string>() == "-0");
    test (KwargsValue value(1.0f))   expect (value.value<std::string>() == "1");
    test (KwargsValue value(0.5f))   expect (value.value<std::string>() == "0.5");
    test (KwargsValue value(0.25f))  expect (value.value<std::string>() == "0.25");
    test (KwargsValue value(0.125f)) expect (value.value<std::string>() == "0.125");
    test (KwargsValue value(0.75f))  expect (value.value<std::string>() == "0.75");
    test (KwargsValue value(1.5f))   expect (value.value<std::string>() == "1.5");
    test (KwargsValue value(3.25f))  expect (value.value<std::string>() == "3.25");

    test (KwargsValue value(0.0))    expect (value.value<std::string>() == "0");
    test (KwargsValue value(-0.0))   expect (value.value<std::string>() == "-0");
    test (KwargsValue value(1.0))    expect (value.value<std::string>() == "1");
    test (KwargsValue value(0.1))    expect (value.value<std::string>() == "0.1");
    test (KwargsValue value(0.01))   expect (value.value<std::string>() == "0.01");
    test (KwargsValue value(0.001))  expect (value.value<std::string>() == "0.001");
    test (KwargsValue value(0.0001)) expect (value.value<std::string>() == "0.0001");
    test (KwargsValue value(1.25))   expect (value.value<std::string>() == "1.25");
    test (KwargsValue value(2.5))    expect (value.value<std::string>() == "2.5");
    test (KwargsValue value(5.75))   expect (value.value<std::string>() == "5.75");

    test (KwargsValue value(0.1L))               expect (value.value<std::string>() == "0.1");
    test (KwargsValue value(0.625L))             expect (value.value<std::string>() == "0.625");
    test (KwargsValue value(0.125L))             expect (value.value<std::string>() == "0.125");
    test (KwargsValue value(123456789.0L))       expect (value.value<std::string>() == "123456789");
    test (KwargsValue value(1.234567890123456L)) expect (value.value<std::string>().substr(0, sizeof("1.234567890123456") - 1) == "1.234567890123456");


    test (KwargsValue value(std::numeric_limits<float>::min()))       expect (value.value<std::string>().substr(0, 46) == "0.00000000000000000000000000000000000001175494");
    test (KwargsValue value(std::numeric_limits<float>::max()))       expect (value.value<std::string>() == to_string(std::numeric_limits<float>::max()));
    test (KwargsValue value(std::numeric_limits<double>::max()))      expect (value.value<std::string>() == to_string(std::numeric_limits<double>::max()));
    test (KwargsValue value(std::numeric_limits<long double>::max())) expect (value.value<std::string>() == to_string(std::numeric_limits<long double>::max()));


    test (KwargsValue value(std::numeric_limits<float>::quiet_NaN()))           expect (value.value<std::string>() == "NaN");
    test (KwargsValue value(std::numeric_limits<double>::quiet_NaN()))          expect (value.value<std::string>() == "NaN");
    test (KwargsValue value(std::numeric_limits<long double>::quiet_NaN()))     expect (value.value<std::string>() == "NaN");

    test (KwargsValue value(std::numeric_limits<float>::signaling_NaN()))       expect (value.value<std::string>() == "NaN");
    test (KwargsValue value(std::numeric_limits<double>::signaling_NaN()))      expect (value.value<std::string>() == "NaN");
    test (KwargsValue value(std::numeric_limits<long double>::signaling_NaN())) expect (value.value<std::string>() == "NaN");

    test (KwargsValue value(std::numeric_limits<float>::infinity()))            expect (value.value<std::string>() == "inf");
    test (KwargsValue value(std::numeric_limits<double>::infinity()))           expect (value.value<std::string>() == "inf");
    test (KwargsValue value(std::numeric_limits<long double>::infinity()))      expect (value.value<std::string>() == "inf");

    test (KwargsValue value(-std::numeric_limits<float>::infinity()))           expect (value.value<std::string>() == "-inf");
    test (KwargsValue value(-std::numeric_limits<double>::infinity()))          expect (value.value<std::string>() == "-inf");
    test (KwargsValue value(-std::numeric_limits<long double>::infinity()))     expect (value.value<std::string>() == "-inf");

    return testing_completed;
}