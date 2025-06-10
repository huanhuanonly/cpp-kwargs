#include "test.h"

#include <algorithm>
#include <random>

static constexpr std::size_t size = sizeof(std::uint64_t) * 1'000'000 + sizeof(std::uint32_t) * 1 + sizeof(std::uint8_t) * 3;

char a[size];
char b[size];

char c[size];
char d[size];

char e[size];
char f[size];

char g[size];
char h[size];

int main()
{
    disable_test_interrupts();

    std::string sa(size - 1, '\0');
    std::string sb(size - 1, '\0');

    auto rng = []() noexcept -> char
    {
        static std::mt19937 r(std::random_device{}());

        return static_cast<char>(r() % 26 + 'A');
    };

    // ----- kwargs::detail::bit_swap

    std::generate(sa.begin(), sa.end(), rng);
    std::generate(sb.begin(), sb.end(), rng);

    std::copy(sa.begin(), sa.end(), std::begin(a));
    std::copy(sb.begin(), sb.end(), std::begin(b));

    auto count_1 = measure(kwargs::detail::bit_swap<decltype(a), decltype(b)>, a, b);

    test () expect (sa == b && sb == a);

    // ----- std::swap

    std::generate(std::begin(c), std::end(c) - 1, rng);
    std::generate(std::begin(d), std::end(d) - 1, rng);

    auto count_2 = measure([](auto& __lv, auto& __rv) -> void { static_assert(std::is_array_v<std::remove_reference_t<decltype(__lv)>>); std::swap(__lv, __rv); }, c, d);

    test () pursue (count_1 <= count_2);

    // ----- ::memcpy
    
    std::generate(std::begin(e), std::end(e) - 1, rng);
    std::generate(std::begin(f), std::end(f) - 1, rng);

    auto count_3 = measure([](auto& __lv, auto& __rv) -> void { static char t[size]; ::memcpy(t, __lv, size); ::memcpy(__lv, __rv, size); ::memcpy(__rv, t, size); }, e, f);

    test () pursue (count_1 <= count_3);
    
    // ----- std::swap_ranges
    
    std::generate(std::begin(g), std::end(g) - 1, rng);
    std::generate(std::begin(h), std::end(h) - 1, rng);

    auto count_4 = measure([](auto& __lv, auto& __rv) -> void { static_assert(std::is_array_v<std::remove_reference_t<decltype(__lv)>>); std::swap_ranges(std::begin(__lv), std::end(__lv), std::begin(__rv)); }, g, h);

    test () pursue (count_1 <= count_4);

    return testing_completed;
}