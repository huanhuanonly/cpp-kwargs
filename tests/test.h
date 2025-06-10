#if defined(_MSC_VER)
#	pragma warning (disable: 4514)  // C4514: 'function' : unreferenced inline function has been removed
#   pragma warning (disable: 4710)  // C4710: 'function' : function not inlined
#   pragma warning (disable: 4711)  // C4711: function 'function' selected for inline expansion
#endif

#if !defined(CPP_KWARGS_H)
#   if __has_include(<kwargs.h>)
#       include <kwargs.h>
#   else
#       include "../include/kwargs.h"
#   endif
#endif

#include <iostream>
#include <chrono>

template<typename _Container,
    std::enable_if_t<std::conjunction_v<
        kwargs::detail::is_iterable<_Container>,
        std::negation<kwargs::detail::is_any_of<_Container, std::string, std::string_view>>,
        std::negation<std::is_array<_Container>>>, int> = 0>
std::ostream& operator<<(std::ostream& __os, const _Container& __c)
{
    __os.put('[');

    for (auto it = std::begin(__c); it != std::end(__c); ++it)
    {
        __os << *it;
        
        if (std::next(it) != std::end(__c))
        {
            __os.put(',').put(' ');
        }
    }

    __os.put(']');

    return __os;
}

namespace kt
{

class output_stream
{
public:

    output_stream(std::ostream& __os, int __color) noexcept
        : _M_ostream(__os), _M_current_color(__color)
    { }

    output_stream(const output_stream&) = delete;
    output_stream(output_stream&&) = delete;

    output_stream& operator=(const output_stream&) = delete;
    output_stream& operator=(output_stream&&) = delete;

    template<typename _Tp>
    output_stream& operator<<(const _Tp& __value)
    {
        _M_before_output();
        _M_ostream << __value;
        _M_after_output();
        return *this;
    }

    output_stream& put_start_character(char __c)
    {
        _M_ostream.put(__c);
        return *this;
    }

protected:

    std::ostream& _M_ostream;
    std::int64_t  _M_current_color;

private:

    void _M_before_output() const noexcept;
    void _M_after_output() const noexcept;
};

extern output_stream failings;
extern output_stream warnings;
extern output_stream information;

}  // namespace kt


#undef measure

template<typename _Function, typename... _Args>
inline std::chrono::nanoseconds __kwargs_measure(const char* __filePath, std::size_t __lineNumber, const char* __funcName, _Function __func, _Args&&... __args) noexcept
{
    const auto start = std::chrono::steady_clock::now();

    __func(std::forward<_Args>(__args)...);

    const auto count = std::chrono::steady_clock::now() - start;

    kt::information.put_start_character('+')
        <<  " In the function." << __funcName << "\n\n"
        << "        Template: " << kwargs::detail::type_name<_Function>() << "\n"
        << "        Duration: " << std::chrono::duration_cast<std::chrono::milliseconds>(count).count() << "ms | "
                                << std::chrono::duration_cast<std::chrono::microseconds>(count).count() << "us | "
                                << std::chrono::duration_cast<std::chrono::nanoseconds>(count).count()  << "ns\n\n"
        << "    file: " << __filePath << "\n"
        << "    line: " << __lineNumber << "\n\n";

    return std::chrono::duration_cast<std::chrono::nanoseconds>(count);
}

#define measure(__func, ...) __kwargs_measure(__FILE__, __LINE__, #__func, __func, ##__VA_ARGS__)

#undef test
#undef expect

[[maybe_unused]] static inline int __kwargs_testcase_count = 0;

static inline bool __kwargs_interrupts = true;

[[maybe_unused]] static inline int testing_completed = 0;

inline void disable_test_interrupts() noexcept
{ __kwargs_interrupts = false; }

#define test(...) do { auto __kwargs_test_statement = #__VA_ARGS__; ++__kwargs_testcase_count; if (__VA_ARGS__;
#define expect(...) !(__VA_ARGS__)) { kt::failings.put_start_character('+') << " In the testcase." << __kwargs_testcase_count << " " << std::string(std::size_t(__kwargs_testcase_count), '-') << "\n\n        " << "Evaluate: " << __kwargs_test_statement << "\n          Expect: " #__VA_ARGS__ << "\n\n    " << "file: " << __FILE__ << "\n    line: " << __LINE__ << "\n\n"; if (__kwargs_interrupts) { ::exit(__kwargs_testcase_count); } else if (testing_completed == 0) { testing_completed = __kwargs_testcase_count; } } } while (false)
#define pursue(...) !(__VA_ARGS__)) { kt::warnings.put_start_character('+') << " In the testcase." << __kwargs_testcase_count << " " << std::string(std::size_t(__kwargs_testcase_count), '-') << "\n\n        " << "Evaluate: " << __kwargs_test_statement << "\n          Pursue: " #__VA_ARGS__ << "\n\n    " << "file: " << __FILE__ << "\n    line: " << __LINE__ << "\n\n"; } } while (false)
