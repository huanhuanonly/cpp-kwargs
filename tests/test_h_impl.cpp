#include "test.h"

#if defined(_WIN32)

#if defined(_MSC_VER)
#   pragma warning (disable: 4668 5039)
#endif

#include <Windows.h>

namespace kt
{

static int default_color = -1;

static HANDLE get_std_handle(std::ostream& __os) noexcept
{
    if (std::addressof(__os) == std::addressof(std::cerr))
    {
        return ::GetStdHandle(STD_ERROR_HANDLE);
    }
    else
    {
        return ::GetStdHandle(STD_OUTPUT_HANDLE);
    }
}

void output_stream::_M_before_output() const noexcept
{
    if (default_color == -1)
    {
        CONSOLE_SCREEN_BUFFER_INFO info;
        ::GetConsoleScreenBufferInfo(get_std_handle(_M_ostream), &info);
        default_color = info.wAttributes;
    }

    ::SetConsoleTextAttribute(get_std_handle(_M_ostream), static_cast<WORD>(_M_current_color));
}

void output_stream::_M_after_output() const noexcept
{
    assert(~default_color);
    ::SetConsoleTextAttribute(get_std_handle(_M_ostream), static_cast<WORD>(default_color));
}

output_stream failings   (std::cerr, 0x0C);
output_stream warnings   (std::clog, 0x06);
output_stream information(std::cout, 0x0D);

}  // namespace kt

#endif  // defined(_WIN32)