#include "test.h"

int main()
{
    using namespace std::string_literals;
    using namespace std::string_view_literals;

    test (KwargsValue value(nullptr)) expect (value.value<std::nullptr_t>() == nullptr);

    test (KwargsValue value(nullptr)) expect (value.value<void*>() == nullptr);

    test (KwargsValue value(nullptr)) expect(value.value<const void*>() == nullptr);

    test (KwargsValue value(nullptr)) expect (value.value<int*>() == nullptr);

    test (KwargsValue value((int*)(5201314))) expect (value.value<int*>() == (int*)(5201314));

    test (KwargsValue value((int*)(5201314))) expect (value.value<void*>() == (int*)(5201314));

    test (KwargsValue value((int*)(5201314))) expect (value.value<std::nullptr_t>() == nullptr);

    return testing_completed;
}