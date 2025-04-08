//  ___    ___    ___       _   _  _       _  _____  ___    ___    ___   
// (  _`\ (  _`\ (  _`\    ( ) ( )( )  _  ( )(  _  )|  _`\ (  _`\ (  _`\ 
// | ( (_)| |_) )| |_) )   | |/'/'| | ( ) | || (_) || (_) )| ( (_)| (_(_)
// | |  _ | ,__/'| ,__/'   | , <  | | | | | ||  _  || ,  / | |___ `\__ \ 
// | (_( )| |    | |       | |\`\ | (_/ \_) || | | || |\ \ | (_, )( )_) |
// (____/'(_)    (_)       (_) (_)`\___x___/'(_) (_)(_) (_)(____/'`\____)
//                                                                       
//                         https://github.com/huanhuanonly/cpp-kwargs    
//                                                                       
// Copyright (c) 2024-2025 Yang Huanhuan (3347484963@qq.com).
// 
// Before using this file, please read its license:
//
//     https://github.com/huanhuanonly/cpp-kwargs/blob/main/LICENSE
//
// THIS  SOFTWARE  IS  PROVIDED  "AS IS",  WITHOUT  WARRANTY OF ANY KIND,
// EXPRESS  OR IMPLIED.  THE  AUTHOR  RESERVES  THE  RIGHT  TO MODIFY THE
// LICENSE OF THIS FILE AT ANY TIME.
//

/**
* kwargs.h
* 
* Created by Yang Huanhuan on December 29, 2024, 14:40:45
* 
* --- This file is the main header for cpp-kwargs ---
* 
* @brief Implement Python's **kwargs style parameter passing in C++.
*/

#pragma once

#ifndef CPP_KWARGS_H
#define CPP_KWARGS_H

#include <type_traits>
#include <typeinfo>

#include <numeric>
#include <limits>

#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <set>

#include <stdexcept>

#include <cassert>
#include <cstdlib>

namespace kwargs
{

#if defined(_MSC_VER) && defined(_CONSTEXPR20)
#   define _KWARGS_DESTRUCTOR_CONSTEXPR    _CONSTEXPR20
#elif defined(__GNUC__) && defined(_GLIBCXX20_CONSTEXPR)
#   define _KWARGS_DESTRUCTOR_CONSTEXPR    _GLIBCXX20_CONSTEXPR
#elif defined(__clang__) && __cpp_constexpr >= 201907L
#   define _KWARGS_DESTRUCTOR_CONSTEXPR    constexpr
#else
#   define _KWARGS_DESTRUCTOR_CONSTEXPR    inline
#endif


#if __cplusplus >= 202002L || (defined(_MSC_VER) && defined(_HAS_CXX20) && _HAS_CXX20)
#   define _KWARGS_VARIABLE_OPTIONAL_INITIALIZATION_CONSTEXPR
#else
#   define _KWARGS_VARIABLE_OPTIONAL_INITIALIZATION_CONSTEXPR  { }
#endif


#if defined(_MSC_VER)
#   define _KWARGS_INT128
#   define _KWARGS_UINT128
#elif defined(__GNUC__) || defined(__clang__)
#   define _KWARGS_INT128       __int128_t
#   define _KWARGS_UINT128      __uint128_t

#   define _KWARGS_HAS_INT128   true
#   define _KWARGS_HAS_UINT128  true
#endif


#if defined(_MSC_VER) && !defined(_GLIBCXX_USE_FLOAT128) && !defined(__FLT128_MAX__)
#   define _KWARGS_LONG_DOUBLE_IS_DOUBLE  true
#endif


#if defined(_MSC_VER)
#   pragma warning (push)
#   pragma warning (disable : 5051)  // C5051: attribute [[attribute-name]] requires at least 'standard_version'; ignored
#endif


namespace detail
{

#if defined(_DEBUG)
#   define _KWARGS_Test_(__test, ...)  static_assert(all_equal(__test, ##__VA_ARGS__), "Kwargs: a test failed.")
#else
#   define _KWARGS_Test_(__test, ...)
#endif

template<typename _Tp, typename... _Args>
[[nodiscard]] constexpr bool all_equal(_Tp&& __first, _Args&& ...__args) noexcept
{
    if constexpr (sizeof...(_Args) == 0)
    {
        return static_cast<bool>(__first);
    }
    else if constexpr (sizeof...(_Args) >= 1)
    {
        return ((__first == __args) && ...);
    }
}

template<typename _Tp, typename... _Types>
static constexpr inline bool is_any_of_v = (std::is_same_v<_Tp, _Types> || ...);

template<typename _Tp>
static constexpr inline bool is_char_v = is_any_of_v<std::remove_cv_t<_Tp>, char, signed char, unsigned char>;

template<typename _Tp>
static constexpr inline bool is_integral_v = std::is_integral_v<_Tp> || std::is_enum_v<_Tp>;

template<auto _V>
static constexpr inline bool not_v = !_V;

template<typename _Tp>
static constexpr inline bool use_value_flag_v = (is_integral_v<_Tp> || std::is_pointer_v<_Tp> || std::is_floating_point_v<_Tp>) && sizeof(_Tp) <= sizeof(void*);


template<typename _Tp>
using remove_reference_cv_t = std::remove_cv_t<std::remove_reference_t<_Tp>>;

template<typename _Tp>
using remove_pointer_cv_t = std::remove_cv_t<std::remove_pointer_t<_Tp>>;


template<typename _Tp> struct promote_numeric_impl    { using type = _Tp; };

template<> struct promote_numeric_impl<std::int8_t>   { using type = std::int16_t; };
template<> struct promote_numeric_impl<std::int16_t>  { using type = std::int32_t; };
template<> struct promote_numeric_impl<std::int32_t>  { using type = std::int64_t; };

#if defined(_KWARGS_HAS_INT128)
template<> struct promote_numeric_impl<std::int64_t>  { using type = _KWARGS_INT128; };
#endif

template<> struct promote_numeric_impl<std::uint8_t>  { using type = std::uint16_t; };
template<> struct promote_numeric_impl<std::uint16_t> { using type = std::uint32_t; };
template<> struct promote_numeric_impl<std::uint32_t> { using type = std::uint64_t; };

#if defined(_KWARGS_HAS_UINT128)
template<> struct promote_numeric_impl<std::uint64_t> { using type = _KWARGS_UINT128; };
#endif

template<> struct promote_numeric_impl<float>         { using type = double; };
template<> struct promote_numeric_impl<double>        { using type = long double; };

#if true
template<> struct promote_numeric_impl<char>          { using type = std::conditional_t<std::is_signed_v<char>, promote_numeric_impl<std::int8_t>, promote_numeric_impl<std::uint8_t>>; };
#else
template<> struct promote_numeric_impl<char>          { using type = wchar_t; };
#endif

template<typename _Tp>
struct promote_numeric { using type = promote_numeric_impl<std::remove_cv_t<_Tp>>::type; };

template<typename _Tp>
using promote_numeric_t = promote_numeric<_Tp>::type;


template<typename _Tp, typename = std::void_t<>>
struct enum_underlying_type
{ using type = _Tp; };
    
template<typename _Tp>
struct enum_underlying_type<_Tp, std::enable_if_t<std::is_enum_v<_Tp>>>
{ using type = std::underlying_type_t<_Tp>; };
    
template<typename _Tp>
using enum_underlying_type_t = typename enum_underlying_type<_Tp>::type;


template<typename _Tp, typename = std::void_t<>>
struct has_container_value_type : std::false_type { };

template<typename _Tp>
struct has_container_value_type<_Tp, std::void_t<typename std::remove_reference_t<_Tp>::value_type>>
    : std::true_type { };

template<typename _Tp>
static constexpr inline bool has_container_value_type_v = has_container_value_type<_Tp>::value;


template<typename _Tp, typename = std::void_t<>>
struct container_value_type
{ using type = _Tp; };

template<typename _Tp>
struct container_value_type<_Tp, std::void_t<typename std::remove_reference_t<_Tp>::value_type>>
{ using type =  typename _Tp::value_type; };

template<typename _Tp>
using container_value_type_t = typename container_value_type<_Tp>::type;


template<typename _Tp>
static constexpr inline bool is_std_array_v = std::is_same_v<
                std::remove_reference_t<_Tp>,
                std::array<detail::container_value_type_t<_Tp>, sizeof(std::remove_reference_t<_Tp>) / sizeof(detail::container_value_type_t<_Tp>)>>;


template<typename _Tp, typename = std::void_t<>>
struct is_iterable_container : std::false_type { };

template<typename _Tp>
struct is_iterable_container<
    _Tp,
    std::void_t<
        decltype(std::declval<_Tp>().begin()),
        decltype(std::declval<_Tp>().end())>> : std::true_type
{ };

template<typename _Tp>
static constexpr inline bool is_iterable_container_v = is_iterable_container<_Tp>::value;


template<typename _Tp, typename = std::void_t<>>
struct container_iterator { using type = std::byte*; };

template<typename _Tp>
struct container_iterator<_Tp, std::void_t<typename _Tp::iterator>>
{ using type = typename _Tp::iterator; };

template<typename _Tp>
using container_iterator_t = typename container_iterator<_Tp>::type;



template<typename _Container>
[[nodiscard]] constexpr auto container_begin_iterator(void* __cp) noexcept
        -> typename std::enable_if_t<is_iterable_container_v<_Container>, typename _Container::iterator>
{ return reinterpret_cast<_Container*>(__cp)->begin(); }

template<typename _Container>
[[nodiscard]] constexpr auto  container_begin_iterator([[maybe_unused]] void* __cp) noexcept
        -> typename std::enable_if_t<not_v<is_iterable_container_v<_Container>>, typename std::byte*>
{ return nullptr; }

template<typename _Container>
[[nodiscard]] constexpr auto container_end_iterator(void* __cp) noexcept
        -> typename std::enable_if_t<is_iterable_container_v<_Container>, typename _Container::iterator>
{ return reinterpret_cast<_Container*>(__cp)->end(); }

template<typename _Container>
[[nodiscard]] constexpr auto container_end_iterator([[maybe_unused]] void* __cp) noexcept
        -> typename std::enable_if_t<not_v<is_iterable_container_v<_Container>>, typename std::byte*>
{ return nullptr; }



/// .append()

template<typename _Container, typename _ValueType, typename = std::void_t<>>
struct has_append_member_function : std::false_type { };

template<typename _Container, typename _ValueType>
struct has_append_member_function<
    _Container,
    _ValueType,
    std::void_t<decltype(std::declval<_Container>().append(std::declval<_ValueType>()))>>
        : std::true_type { };

template<typename _Container, typename _ValueType,
    std::enable_if_t<has_append_member_function<_Container, _ValueType>::value, int> = 0>
constexpr void container_insert(_Container& __container, _ValueType&& __element) noexcept
{ __container.append(std::forward<_ValueType>(__element)); }

/// .push_back()

template<typename _Container, typename _ValueType, typename = std::void_t<>>
struct has_pushback_member_function : std::false_type { };

template<typename _Container, typename _ValueType>
struct has_pushback_member_function<
    _Container,
    _ValueType,
    std::void_t<decltype(std::declval<_Container>().push_back(std::declval<_ValueType>()))>>
        : std::true_type { };

template<typename _Container, typename _ValueType,
    std::enable_if_t<has_pushback_member_function<_Container, _ValueType>::value &&
        std::negation_v<has_append_member_function<_Container, _ValueType>>, int> = 0>
constexpr void container_insert(_Container& __container, _ValueType&& __element) noexcept
{ __container.push_back(std::forward<_ValueType>(__element)); }

/// .push()

template<typename _Container, typename _ValueType, typename = std::void_t<>>
struct has_push_member_function : std::false_type { };

template<typename _Container, typename _ValueType>
struct has_push_member_function<
    _Container,
    _ValueType,
    std::void_t<decltype(std::declval<_Container>().push(std::declval<_ValueType>()))>>
        : std::true_type { };

template<typename _Container, typename _ValueType,
    std::enable_if_t<has_push_member_function<_Container, _ValueType>::value &&
        std::negation_v<has_append_member_function<_Container, _ValueType>> &&
        std::negation_v<has_pushback_member_function<_Container, _ValueType>>, int> = 0>
constexpr void container_insert(_Container& __container, _ValueType&& __element) noexcept
{ __container.push(std::forward<_ValueType>(__element)); }

/// .insert()

template<typename _Container, typename _ValueType, typename = std::void_t<>>
struct has_insert_member_function : std::false_type { };

template<typename _Container, typename _ValueType>
struct has_insert_member_function<
    _Container,
    _ValueType,
    std::void_t<decltype(std::declval<_Container>().insert(std::declval<_ValueType>()))>>
        : std::true_type { };

template<typename _Container, typename _ValueType,
    std::enable_if_t<has_insert_member_function<_Container, _ValueType>::value &&
        std::negation_v<has_append_member_function<_Container, _ValueType>> &&
        std::negation_v<has_pushback_member_function<_Container, _ValueType>> &&
        std::negation_v<has_push_member_function<_Container, _ValueType>>, int> = 0>
constexpr void container_insert(_Container& __container, _ValueType&& __element) noexcept
{ __container.insert(std::forward<_ValueType>(__element)); }

/// .insert(.end(), element)

template<typename _Container, typename _ValueType, typename = std::void_t<>>
struct has_insert_with_end_member_function : std::false_type { };

template<typename _Container, typename _ValueType>
struct has_insert_with_end_member_function<
    _Container,
    _ValueType,
    std::void_t<decltype(std::declval<_Container>().insert(std::declval<typename _Container::iterator>(), std::declval<_ValueType>()))>>
        : std::true_type { };

template<typename _Container, typename _ValueType,
    std::enable_if_t<has_insert_with_end_member_function<_Container, _ValueType>::value &&
        std::negation_v<has_append_member_function<_Container, _ValueType>> &&
        std::negation_v<has_pushback_member_function<_Container, _ValueType>> &&
        std::negation_v<has_push_member_function<_Container, _ValueType>> &&
        std::negation_v<has_insert_member_function<_Container, _ValueType>>, int> = 0>
constexpr void container_insert(_Container& __container, _ValueType&& __element) noexcept
{ __container.insert(__container.end(), std::forward<_ValueType>(__element)); }

/// .push_front()

template<typename _Container, typename _ValueType, typename = std::void_t<>>
struct has_pushfront_member_function : std::false_type { };

template<typename _Container, typename _ValueType>
struct has_pushfront_member_function<
    _Container,
    _ValueType,
    std::void_t<decltype(std::declval<_Container>().push_front(std::declval<_ValueType>()))>>
        : std::true_type { };

template<typename _Container, typename _ValueType = container_value_type_t<_Container>>
constexpr inline bool is_insertable_container_v =
    has_container_value_type_v<_Container> && (
    has_append_member_function<_Container, _ValueType>::value ||
    has_pushback_member_function<_Container, _ValueType>::value ||
    has_push_member_function<_Container, _ValueType>::value ||
    has_insert_member_function<_Container, _ValueType>::value ||
    has_insert_with_end_member_function<_Container, _ValueType>::value ||
    has_pushfront_member_function<_Container, _ValueType>::value);


template<typename _Container, typename _ValueType,
    std::enable_if_t<has_pushfront_member_function<_Container, _ValueType>::value &&
        std::negation_v<has_append_member_function<_Container, _ValueType>> &&
        std::negation_v<has_pushback_member_function<_Container, _ValueType>> &&
        std::negation_v<has_push_member_function<_Container, _ValueType>> &&
        std::negation_v<has_insert_member_function<_Container, _ValueType>> &&
        std::negation_v<has_insert_with_end_member_function<_Container, _ValueType>>, int> = 0>
constexpr void container_insert(_Container& __container, _ValueType&& __element) noexcept
{ __container.push_front(std::forward<_ValueType>(__element)); }



[[nodiscard]] constexpr char tolower(char __c) noexcept
{
    if (__c >= 'A' && __c <= 'Z')
    {
        return __c - ('A' - 'a');
    }
    else
    {
        return __c;
    }
}

_KWARGS_Test_(tolower('A'), 'a');
_KWARGS_Test_(tolower('a'), 'a');

/**
 * @return _ResultOnFailure if failed.
 */
template<int _ResultOnFailure = -1>
[[nodiscard]] constexpr int radix_character_to_integer(char __c) noexcept
{
    if (__c >= '0' && __c <= '9')
    {
        return __c - '0';
    }
    else
    {
        __c = tolower(__c);

        if (__c >= 'a' && __c <= 'z')
        {
            return __c - 'a' + 10;
        }
        else
        {
            return _ResultOnFailure;
        }
    }
}

[[nodiscard]] constexpr bool is_radix_character(char __c, int __radix = 36) noexcept
{
    int res = radix_character_to_integer<-1>(__c);

    return res != -1 && res < __radix;
}

_KWARGS_Test_(is_radix_character('1', 2) && not_v<is_radix_character('2', 2)>);
_KWARGS_Test_(is_radix_character('z', 36) && is_radix_character('Z', 36));


[[nodiscard]] constexpr bool is_same_char_ignore_case(char __c1, char __c2) noexcept
{
    if constexpr ('a' > 'A')
    {
        return __c1 == __c2 || (__c1 ^ 'a' - 'A') == __c2;
    }
    else
    {
        return __c1 == __c2 || (__c1 ^ 'A' - 'a') == __c2;
    }
}

template<typename... _Char>
[[nodiscard]] constexpr bool is_same_char_ignore_case(char __c1, char __c2, char __c3, _Char... __cn) noexcept
{
    return is_same_char_ignore_case(__c1, __c2) && is_same_char_ignore_case(__c2, __c3, __cn...);
}


_KWARGS_Test_(is_same_char_ignore_case('a', 'a')); _KWARGS_Test_(is_same_char_ignore_case('A', 'A'));
_KWARGS_Test_(is_same_char_ignore_case('A', 'a')); _KWARGS_Test_(is_same_char_ignore_case('a', 'A'));


[[nodiscard]] constexpr bool is_same_string_ignore_case(std::string_view __s1, std::string_view __s2) noexcept
{
    if (__s1.size() != __s2.size())
    {
        return false;
    }

    for (std::size_t i = 0; i < __s1.size(); ++i)
    {
        if (is_same_char_ignore_case(__s1[i], __s2[i]) == false)
        {
            return false;
        }
    }

    return true;
}

template<typename... _StringView>
[[nodiscard]] constexpr bool is_same_string_ignore_case(std::string_view __s1, std::string_view __s2, std::string_view __s3, _StringView... __sn) noexcept
{
    return is_same_string_ignore_case(__s1, __s2) && is_same_string_ignore_case(__s2, __s3, __sn...);
}


template<typename _Tp>
[[nodiscard]] constexpr auto abs(_Tp __n) noexcept -> std::make_unsigned_t<_Tp>
{
    if constexpr (std::is_unsigned_v<_Tp>)
    {
        return __n;
    }
    else
    {
        return static_cast<std::make_unsigned_t<_Tp>>(__n < 0 ? -__n : __n);
    }
}

_KWARGS_Test_(abs(-1), 1);
_KWARGS_Test_(abs(1), 1);

template<typename _Tp>
[[nodiscard]] constexpr auto power10(std::make_signed_t<std::size_t> __i) noexcept -> std::enable_if_t<std::is_same_v<std::remove_cv_t<_Tp>, float>, _Tp>
{
    constexpr float power[] = {
        1e00f, 1e01f, 1e02f, 1e03f, 1e04f, 1e05f, 1e06f, 1e07f, 1e08f, 1e09f, 1e10f, 1e11f, 1e12f, 1e13f, 1e14f, 1e15f, 1e16f, 1e17f, 1e18f, 1e19f,
        1e20f, 1e21f, 1e22f, 1e23f, 1e24f, 1e25f, 1e26f, 1e27f, 1e28f, 1e29f, 1e30f, 1e31f, 1e32f, 1e33f, 1e34f, 1e35f, 1e36f, 1e37f, 1e38f
    };

    constexpr float power_negative[] = {
        1e-00f, 1e-01f, 1e-02f, 1e-03f, 1e-04f, 1e-05f, 1e-06f, 1e-07f, 1e-08f, 1e-09f, 1e-10f, 1e-11f, 1e-12f, 1e-13f, 1e-14f, 1e-15f, 1e-16f, 1e-17f, 1e-18f, 1e-19f,
        1e-20f, 1e-21f, 1e-22f, 1e-23f, 1e-24f, 1e-25f, 1e-26f, 1e-27f, 1e-28f, 1e-29f, 1e-30f, 1e-31f, 1e-32f, 1e-33f, 1e-34f, 1e-35f, 1e-36f, 1e-37f, 1e-38f
    };
    
    if (__i >= 0)
    {
        assert(__i < static_cast<decltype(__i)>(std::size(power)));
        return power[__i];
    }
    else
    {
        assert(-__i < static_cast<decltype(__i)>(std::size(power_negative)));
        return power_negative[-__i];
    }
}

template<typename _Tp>
[[nodiscard]] constexpr auto power10(std::make_signed_t<std::size_t> __i) noexcept -> std::enable_if_t<std::is_same_v<std::remove_cv_t<_Tp>, double>, _Tp>
{
    constexpr double power[] = {
        1e000, 1e001, 1e002, 1e003, 1e004, 1e005, 1e006, 1e007, 1e008, 1e009, 1e010, 1e011, 1e012, 1e013, 1e014, 1e015, 1e016, 1e017, 1e018, 1e019,
        1e020, 1e021, 1e022, 1e023, 1e024, 1e025, 1e026, 1e027, 1e028, 1e029, 1e030, 1e031, 1e032, 1e033, 1e034, 1e035, 1e036, 1e037, 1e038, 1e039,
        1e040, 1e041, 1e042, 1e043, 1e044, 1e045, 1e046, 1e047, 1e048, 1e049, 1e050, 1e051, 1e052, 1e053, 1e054, 1e055, 1e056, 1e057, 1e058, 1e059,
        1e060, 1e061, 1e062, 1e063, 1e064, 1e065, 1e066, 1e067, 1e068, 1e069, 1e070, 1e071, 1e072, 1e073, 1e074, 1e075, 1e076, 1e077, 1e078, 1e079,
        1e080, 1e081, 1e082, 1e083, 1e084, 1e085, 1e086, 1e087, 1e088, 1e089, 1e090, 1e091, 1e092, 1e093, 1e094, 1e095, 1e096, 1e097, 1e098, 1e099,
        1e100, 1e101, 1e102, 1e103, 1e104, 1e105, 1e106, 1e107, 1e108, 1e109, 1e110, 1e111, 1e112, 1e113, 1e114, 1e115, 1e116, 1e117, 1e118, 1e119,
        1e120, 1e121, 1e122, 1e123, 1e124, 1e125, 1e126, 1e127, 1e128, 1e129, 1e130, 1e131, 1e132, 1e133, 1e134, 1e135, 1e136, 1e137, 1e138, 1e139,
        1e140, 1e141, 1e142, 1e143, 1e144, 1e145, 1e146, 1e147, 1e148, 1e149, 1e150, 1e151, 1e152, 1e153, 1e154, 1e155, 1e156, 1e157, 1e158, 1e159,
        1e160, 1e161, 1e162, 1e163, 1e164, 1e165, 1e166, 1e167, 1e168, 1e169, 1e170, 1e171, 1e172, 1e173, 1e174, 1e175, 1e176, 1e177, 1e178, 1e179,
        1e180, 1e181, 1e182, 1e183, 1e184, 1e185, 1e186, 1e187, 1e188, 1e189, 1e190, 1e191, 1e192, 1e193, 1e194, 1e195, 1e196, 1e197, 1e198, 1e199,
        1e200, 1e201, 1e202, 1e203, 1e204, 1e205, 1e206, 1e207, 1e208, 1e209, 1e210, 1e211, 1e212, 1e213, 1e214, 1e215, 1e216, 1e217, 1e218, 1e219,
        1e220, 1e221, 1e222, 1e223, 1e224, 1e225, 1e226, 1e227, 1e228, 1e229, 1e230, 1e231, 1e232, 1e233, 1e234, 1e235, 1e236, 1e237, 1e238, 1e239,
        1e240, 1e241, 1e242, 1e243, 1e244, 1e245, 1e246, 1e247, 1e248, 1e249, 1e250, 1e251, 1e252, 1e253, 1e254, 1e255, 1e256, 1e257, 1e258, 1e259,
        1e260, 1e261, 1e262, 1e263, 1e264, 1e265, 1e266, 1e267, 1e268, 1e269, 1e270, 1e271, 1e272, 1e273, 1e274, 1e275, 1e276, 1e277, 1e278, 1e279,
        1e280, 1e281, 1e282, 1e283, 1e284, 1e285, 1e286, 1e287, 1e288, 1e289, 1e290, 1e291, 1e292, 1e293, 1e294, 1e295, 1e296, 1e297, 1e298, 1e299,
        1e300, 1e301, 1e302, 1e303, 1e304, 1e305, 1e306, 1e307, 1e308
    };
    
    constexpr double power_negative[] = {
        1e-000, 1e-001, 1e-002, 1e-003, 1e-004, 1e-005, 1e-006, 1e-007, 1e-008, 1e-009, 1e-010, 1e-011, 1e-012, 1e-013, 1e-014, 1e-015, 1e-016, 1e-017, 1e-018, 1e-019,
        1e-020, 1e-021, 1e-022, 1e-023, 1e-024, 1e-025, 1e-026, 1e-027, 1e-028, 1e-029, 1e-030, 1e-031, 1e-032, 1e-033, 1e-034, 1e-035, 1e-036, 1e-037, 1e-038, 1e-039,
        1e-040, 1e-041, 1e-042, 1e-043, 1e-044, 1e-045, 1e-046, 1e-047, 1e-048, 1e-049, 1e-050, 1e-051, 1e-052, 1e-053, 1e-054, 1e-055, 1e-056, 1e-057, 1e-058, 1e-059,
        1e-060, 1e-061, 1e-062, 1e-063, 1e-064, 1e-065, 1e-066, 1e-067, 1e-068, 1e-069, 1e-070, 1e-071, 1e-072, 1e-073, 1e-074, 1e-075, 1e-076, 1e-077, 1e-078, 1e-079,
        1e-080, 1e-081, 1e-082, 1e-083, 1e-084, 1e-085, 1e-086, 1e-087, 1e-088, 1e-089, 1e-090, 1e-091, 1e-092, 1e-093, 1e-094, 1e-095, 1e-096, 1e-097, 1e-098, 1e-099,
        1e-100, 1e-101, 1e-102, 1e-103, 1e-104, 1e-105, 1e-106, 1e-107, 1e-108, 1e-109, 1e-110, 1e-111, 1e-112, 1e-113, 1e-114, 1e-115, 1e-116, 1e-117, 1e-118, 1e-119,
        1e-120, 1e-121, 1e-122, 1e-123, 1e-124, 1e-125, 1e-126, 1e-127, 1e-128, 1e-129, 1e-130, 1e-131, 1e-132, 1e-133, 1e-134, 1e-135, 1e-136, 1e-137, 1e-138, 1e-139,
        1e-140, 1e-141, 1e-142, 1e-143, 1e-144, 1e-145, 1e-146, 1e-147, 1e-148, 1e-149, 1e-150, 1e-151, 1e-152, 1e-153, 1e-154, 1e-155, 1e-156, 1e-157, 1e-158, 1e-159,
        1e-160, 1e-161, 1e-162, 1e-163, 1e-164, 1e-165, 1e-166, 1e-167, 1e-168, 1e-169, 1e-170, 1e-171, 1e-172, 1e-173, 1e-174, 1e-175, 1e-176, 1e-177, 1e-178, 1e-179,
        1e-180, 1e-181, 1e-182, 1e-183, 1e-184, 1e-185, 1e-186, 1e-187, 1e-188, 1e-189, 1e-190, 1e-191, 1e-192, 1e-193, 1e-194, 1e-195, 1e-196, 1e-197, 1e-198, 1e-199,
        1e-200, 1e-201, 1e-202, 1e-203, 1e-204, 1e-205, 1e-206, 1e-207, 1e-208, 1e-209, 1e-210, 1e-211, 1e-212, 1e-213, 1e-214, 1e-215, 1e-216, 1e-217, 1e-218, 1e-219,
        1e-220, 1e-221, 1e-222, 1e-223, 1e-224, 1e-225, 1e-226, 1e-227, 1e-228, 1e-229, 1e-230, 1e-231, 1e-232, 1e-233, 1e-234, 1e-235, 1e-236, 1e-237, 1e-238, 1e-239,
        1e-240, 1e-241, 1e-242, 1e-243, 1e-244, 1e-245, 1e-246, 1e-247, 1e-248, 1e-249, 1e-250, 1e-251, 1e-252, 1e-253, 1e-254, 1e-255, 1e-256, 1e-257, 1e-258, 1e-259,
        1e-260, 1e-261, 1e-262, 1e-263, 1e-264, 1e-265, 1e-266, 1e-267, 1e-268, 1e-269, 1e-270, 1e-271, 1e-272, 1e-273, 1e-274, 1e-275, 1e-276, 1e-277, 1e-278, 1e-279,
        1e-280, 1e-281, 1e-282, 1e-283, 1e-284, 1e-285, 1e-286, 1e-287, 1e-288, 1e-289, 1e-290, 1e-291, 1e-292, 1e-293, 1e-294, 1e-295, 1e-296, 1e-297, 1e-298, 1e-299,
        1e-300, 1e-301, 1e-302, 1e-303, 1e-304, 1e-305, 1e-306, 1e-307, 1e-308
    };

    if (__i >= 0)
    {
        assert(__i < static_cast<decltype(__i)>(std::size(power)));
        return power[__i];
    }
    else
    {
        assert(-__i < static_cast<decltype(__i)>(std::size(power_negative)));
        return power_negative[-__i];
    }
}

template<typename _Tp>
[[nodiscard]] constexpr auto power10(std::make_signed_t<std::size_t> __i) noexcept -> std::enable_if_t<std::is_same_v<std::remove_cv_t<_Tp>, long double>, _Tp>
{
#if defined(_KWARGS_LONG_DOUBLE_IS_DOUBLE)

    static_assert(sizeof(long double) == sizeof(double));

    return static_cast<_Tp>(power10<double>(__i));

#else

#define _KWARGS_GENERATE_LITERAL(T) \
    T(0000)T(0001)T(0002)T(0003)T(0004)T(0005)T(0006)T(0007)T(0008)T(0009)T(0010)T(0011)T(0012)T(0013)T(0014)T(0015)T(0016)T(0017)T(0018)T(0019)T(0020)T(0021)T(0022)T(0023)T(0024)T(0025)T(0026)T(0027)T(0028)T(0029)T(0030)T(0031)T(0032)T(0033)T(0034)T(0035)T(0036)T(0037)T(0038)T(0039)T(0040)T(0041)T(0042)T(0043)T(0044)T(0045)T(0046)T(0047)T(0048)T(0049)T(0050)T(0051)T(0052)T(0053)T(0054)T(0055)T(0056)T(0057)T(0058)T(0059)T(0060)T(0061)T(0062)T(0063)T(0064)T(0065)T(0066)T(0067)T(0068)T(0069)T(0070)T(0071)T(0072)T(0073)T(0074)T(0075)T(0076)T(0077)T(0078)T(0079)T(0080)T(0081)T(0082)T(0083)T(0084)T(0085)T(0086)T(0087)T(0088)T(0089)T(0090)T(0091)T(0092)T(0093)T(0094)T(0095)T(0096)T(0097)T(0098)T(0099) \
    T(0100)T(0101)T(0102)T(0103)T(0104)T(0105)T(0106)T(0107)T(0108)T(0109)T(0110)T(0111)T(0112)T(0113)T(0114)T(0115)T(0116)T(0117)T(0118)T(0119)T(0120)T(0121)T(0122)T(0123)T(0124)T(0125)T(0126)T(0127)T(0128)T(0129)T(0130)T(0131)T(0132)T(0133)T(0134)T(0135)T(0136)T(0137)T(0138)T(0139)T(0140)T(0141)T(0142)T(0143)T(0144)T(0145)T(0146)T(0147)T(0148)T(0149)T(0150)T(0151)T(0152)T(0153)T(0154)T(0155)T(0156)T(0157)T(0158)T(0159)T(0160)T(0161)T(0162)T(0163)T(0164)T(0165)T(0166)T(0167)T(0168)T(0169)T(0170)T(0171)T(0172)T(0173)T(0174)T(0175)T(0176)T(0177)T(0178)T(0179)T(0180)T(0181)T(0182)T(0183)T(0184)T(0185)T(0186)T(0187)T(0188)T(0189)T(0190)T(0191)T(0192)T(0193)T(0194)T(0195)T(0196)T(0197)T(0198)T(0199) \
    T(0200)T(0201)T(0202)T(0203)T(0204)T(0205)T(0206)T(0207)T(0208)T(0209)T(0210)T(0211)T(0212)T(0213)T(0214)T(0215)T(0216)T(0217)T(0218)T(0219)T(0220)T(0221)T(0222)T(0223)T(0224)T(0225)T(0226)T(0227)T(0228)T(0229)T(0230)T(0231)T(0232)T(0233)T(0234)T(0235)T(0236)T(0237)T(0238)T(0239)T(0240)T(0241)T(0242)T(0243)T(0244)T(0245)T(0246)T(0247)T(0248)T(0249)T(0250)T(0251)T(0252)T(0253)T(0254)T(0255)T(0256)T(0257)T(0258)T(0259)T(0260)T(0261)T(0262)T(0263)T(0264)T(0265)T(0266)T(0267)T(0268)T(0269)T(0270)T(0271)T(0272)T(0273)T(0274)T(0275)T(0276)T(0277)T(0278)T(0279)T(0280)T(0281)T(0282)T(0283)T(0284)T(0285)T(0286)T(0287)T(0288)T(0289)T(0290)T(0291)T(0292)T(0293)T(0294)T(0295)T(0296)T(0297)T(0298)T(0299) \
    T(0300)T(0301)T(0302)T(0303)T(0304)T(0305)T(0306)T(0307)T(0308)T(0309)T(0310)T(0311)T(0312)T(0313)T(0314)T(0315)T(0316)T(0317)T(0318)T(0319)T(0320)T(0321)T(0322)T(0323)T(0324)T(0325)T(0326)T(0327)T(0328)T(0329)T(0330)T(0331)T(0332)T(0333)T(0334)T(0335)T(0336)T(0337)T(0338)T(0339)T(0340)T(0341)T(0342)T(0343)T(0344)T(0345)T(0346)T(0347)T(0348)T(0349)T(0350)T(0351)T(0352)T(0353)T(0354)T(0355)T(0356)T(0357)T(0358)T(0359)T(0360)T(0361)T(0362)T(0363)T(0364)T(0365)T(0366)T(0367)T(0368)T(0369)T(0370)T(0371)T(0372)T(0373)T(0374)T(0375)T(0376)T(0377)T(0378)T(0379)T(0380)T(0381)T(0382)T(0383)T(0384)T(0385)T(0386)T(0387)T(0388)T(0389)T(0390)T(0391)T(0392)T(0393)T(0394)T(0395)T(0396)T(0397)T(0398)T(0399) \
    T(0400)T(0401)T(0402)T(0403)T(0404)T(0405)T(0406)T(0407)T(0408)T(0409)T(0410)T(0411)T(0412)T(0413)T(0414)T(0415)T(0416)T(0417)T(0418)T(0419)T(0420)T(0421)T(0422)T(0423)T(0424)T(0425)T(0426)T(0427)T(0428)T(0429)T(0430)T(0431)T(0432)T(0433)T(0434)T(0435)T(0436)T(0437)T(0438)T(0439)T(0440)T(0441)T(0442)T(0443)T(0444)T(0445)T(0446)T(0447)T(0448)T(0449)T(0450)T(0451)T(0452)T(0453)T(0454)T(0455)T(0456)T(0457)T(0458)T(0459)T(0460)T(0461)T(0462)T(0463)T(0464)T(0465)T(0466)T(0467)T(0468)T(0469)T(0470)T(0471)T(0472)T(0473)T(0474)T(0475)T(0476)T(0477)T(0478)T(0479)T(0480)T(0481)T(0482)T(0483)T(0484)T(0485)T(0486)T(0487)T(0488)T(0489)T(0490)T(0491)T(0492)T(0493)T(0494)T(0495)T(0496)T(0497)T(0498)T(0499) \
    T(0500)T(0501)T(0502)T(0503)T(0504)T(0505)T(0506)T(0507)T(0508)T(0509)T(0510)T(0511)T(0512)T(0513)T(0514)T(0515)T(0516)T(0517)T(0518)T(0519)T(0520)T(0521)T(0522)T(0523)T(0524)T(0525)T(0526)T(0527)T(0528)T(0529)T(0530)T(0531)T(0532)T(0533)T(0534)T(0535)T(0536)T(0537)T(0538)T(0539)T(0540)T(0541)T(0542)T(0543)T(0544)T(0545)T(0546)T(0547)T(0548)T(0549)T(0550)T(0551)T(0552)T(0553)T(0554)T(0555)T(0556)T(0557)T(0558)T(0559)T(0560)T(0561)T(0562)T(0563)T(0564)T(0565)T(0566)T(0567)T(0568)T(0569)T(0570)T(0571)T(0572)T(0573)T(0574)T(0575)T(0576)T(0577)T(0578)T(0579)T(0580)T(0581)T(0582)T(0583)T(0584)T(0585)T(0586)T(0587)T(0588)T(0589)T(0590)T(0591)T(0592)T(0593)T(0594)T(0595)T(0596)T(0597)T(0598)T(0599) \
    T(0600)T(0601)T(0602)T(0603)T(0604)T(0605)T(0606)T(0607)T(0608)T(0609)T(0610)T(0611)T(0612)T(0613)T(0614)T(0615)T(0616)T(0617)T(0618)T(0619)T(0620)T(0621)T(0622)T(0623)T(0624)T(0625)T(0626)T(0627)T(0628)T(0629)T(0630)T(0631)T(0632)T(0633)T(0634)T(0635)T(0636)T(0637)T(0638)T(0639)T(0640)T(0641)T(0642)T(0643)T(0644)T(0645)T(0646)T(0647)T(0648)T(0649)T(0650)T(0651)T(0652)T(0653)T(0654)T(0655)T(0656)T(0657)T(0658)T(0659)T(0660)T(0661)T(0662)T(0663)T(0664)T(0665)T(0666)T(0667)T(0668)T(0669)T(0670)T(0671)T(0672)T(0673)T(0674)T(0675)T(0676)T(0677)T(0678)T(0679)T(0680)T(0681)T(0682)T(0683)T(0684)T(0685)T(0686)T(0687)T(0688)T(0689)T(0690)T(0691)T(0692)T(0693)T(0694)T(0695)T(0696)T(0697)T(0698)T(0699) \
    T(0700)T(0701)T(0702)T(0703)T(0704)T(0705)T(0706)T(0707)T(0708)T(0709)T(0710)T(0711)T(0712)T(0713)T(0714)T(0715)T(0716)T(0717)T(0718)T(0719)T(0720)T(0721)T(0722)T(0723)T(0724)T(0725)T(0726)T(0727)T(0728)T(0729)T(0730)T(0731)T(0732)T(0733)T(0734)T(0735)T(0736)T(0737)T(0738)T(0739)T(0740)T(0741)T(0742)T(0743)T(0744)T(0745)T(0746)T(0747)T(0748)T(0749)T(0750)T(0751)T(0752)T(0753)T(0754)T(0755)T(0756)T(0757)T(0758)T(0759)T(0760)T(0761)T(0762)T(0763)T(0764)T(0765)T(0766)T(0767)T(0768)T(0769)T(0770)T(0771)T(0772)T(0773)T(0774)T(0775)T(0776)T(0777)T(0778)T(0779)T(0780)T(0781)T(0782)T(0783)T(0784)T(0785)T(0786)T(0787)T(0788)T(0789)T(0790)T(0791)T(0792)T(0793)T(0794)T(0795)T(0796)T(0797)T(0798)T(0799) \
    T(0800)T(0801)T(0802)T(0803)T(0804)T(0805)T(0806)T(0807)T(0808)T(0809)T(0810)T(0811)T(0812)T(0813)T(0814)T(0815)T(0816)T(0817)T(0818)T(0819)T(0820)T(0821)T(0822)T(0823)T(0824)T(0825)T(0826)T(0827)T(0828)T(0829)T(0830)T(0831)T(0832)T(0833)T(0834)T(0835)T(0836)T(0837)T(0838)T(0839)T(0840)T(0841)T(0842)T(0843)T(0844)T(0845)T(0846)T(0847)T(0848)T(0849)T(0850)T(0851)T(0852)T(0853)T(0854)T(0855)T(0856)T(0857)T(0858)T(0859)T(0860)T(0861)T(0862)T(0863)T(0864)T(0865)T(0866)T(0867)T(0868)T(0869)T(0870)T(0871)T(0872)T(0873)T(0874)T(0875)T(0876)T(0877)T(0878)T(0879)T(0880)T(0881)T(0882)T(0883)T(0884)T(0885)T(0886)T(0887)T(0888)T(0889)T(0890)T(0891)T(0892)T(0893)T(0894)T(0895)T(0896)T(0897)T(0898)T(0899) \
    T(0900)T(0901)T(0902)T(0903)T(0904)T(0905)T(0906)T(0907)T(0908)T(0909)T(0910)T(0911)T(0912)T(0913)T(0914)T(0915)T(0916)T(0917)T(0918)T(0919)T(0920)T(0921)T(0922)T(0923)T(0924)T(0925)T(0926)T(0927)T(0928)T(0929)T(0930)T(0931)T(0932)T(0933)T(0934)T(0935)T(0936)T(0937)T(0938)T(0939)T(0940)T(0941)T(0942)T(0943)T(0944)T(0945)T(0946)T(0947)T(0948)T(0949)T(0950)T(0951)T(0952)T(0953)T(0954)T(0955)T(0956)T(0957)T(0958)T(0959)T(0960)T(0961)T(0962)T(0963)T(0964)T(0965)T(0966)T(0967)T(0968)T(0969)T(0970)T(0971)T(0972)T(0973)T(0974)T(0975)T(0976)T(0977)T(0978)T(0979)T(0980)T(0981)T(0982)T(0983)T(0984)T(0985)T(0986)T(0987)T(0988)T(0989)T(0990)T(0991)T(0992)T(0993)T(0994)T(0995)T(0996)T(0997)T(0998)T(0999) \
    T(1000)T(1001)T(1002)T(1003)T(1004)T(1005)T(1006)T(1007)T(1008)T(1009)T(1010)T(1011)T(1012)T(1013)T(1014)T(1015)T(1016)T(1017)T(1018)T(1019)T(1020)T(1021)T(1022)T(1023)T(1024)T(1025)T(1026)T(1027)T(1028)T(1029)T(1030)T(1031)T(1032)T(1033)T(1034)T(1035)T(1036)T(1037)T(1038)T(1039)T(1040)T(1041)T(1042)T(1043)T(1044)T(1045)T(1046)T(1047)T(1048)T(1049)T(1050)T(1051)T(1052)T(1053)T(1054)T(1055)T(1056)T(1057)T(1058)T(1059)T(1060)T(1061)T(1062)T(1063)T(1064)T(1065)T(1066)T(1067)T(1068)T(1069)T(1070)T(1071)T(1072)T(1073)T(1074)T(1075)T(1076)T(1077)T(1078)T(1079)T(1080)T(1081)T(1082)T(1083)T(1084)T(1085)T(1086)T(1087)T(1088)T(1089)T(1090)T(1091)T(1092)T(1093)T(1094)T(1095)T(1096)T(1097)T(1098)T(1099) \
    T(1100)T(1101)T(1102)T(1103)T(1104)T(1105)T(1106)T(1107)T(1108)T(1109)T(1110)T(1111)T(1112)T(1113)T(1114)T(1115)T(1116)T(1117)T(1118)T(1119)T(1120)T(1121)T(1122)T(1123)T(1124)T(1125)T(1126)T(1127)T(1128)T(1129)T(1130)T(1131)T(1132)T(1133)T(1134)T(1135)T(1136)T(1137)T(1138)T(1139)T(1140)T(1141)T(1142)T(1143)T(1144)T(1145)T(1146)T(1147)T(1148)T(1149)T(1150)T(1151)T(1152)T(1153)T(1154)T(1155)T(1156)T(1157)T(1158)T(1159)T(1160)T(1161)T(1162)T(1163)T(1164)T(1165)T(1166)T(1167)T(1168)T(1169)T(1170)T(1171)T(1172)T(1173)T(1174)T(1175)T(1176)T(1177)T(1178)T(1179)T(1180)T(1181)T(1182)T(1183)T(1184)T(1185)T(1186)T(1187)T(1188)T(1189)T(1190)T(1191)T(1192)T(1193)T(1194)T(1195)T(1196)T(1197)T(1198)T(1199) \
    T(1200)T(1201)T(1202)T(1203)T(1204)T(1205)T(1206)T(1207)T(1208)T(1209)T(1210)T(1211)T(1212)T(1213)T(1214)T(1215)T(1216)T(1217)T(1218)T(1219)T(1220)T(1221)T(1222)T(1223)T(1224)T(1225)T(1226)T(1227)T(1228)T(1229)T(1230)T(1231)T(1232)T(1233)T(1234)T(1235)T(1236)T(1237)T(1238)T(1239)T(1240)T(1241)T(1242)T(1243)T(1244)T(1245)T(1246)T(1247)T(1248)T(1249)T(1250)T(1251)T(1252)T(1253)T(1254)T(1255)T(1256)T(1257)T(1258)T(1259)T(1260)T(1261)T(1262)T(1263)T(1264)T(1265)T(1266)T(1267)T(1268)T(1269)T(1270)T(1271)T(1272)T(1273)T(1274)T(1275)T(1276)T(1277)T(1278)T(1279)T(1280)T(1281)T(1282)T(1283)T(1284)T(1285)T(1286)T(1287)T(1288)T(1289)T(1290)T(1291)T(1292)T(1293)T(1294)T(1295)T(1296)T(1297)T(1298)T(1299) \
    T(1300)T(1301)T(1302)T(1303)T(1304)T(1305)T(1306)T(1307)T(1308)T(1309)T(1310)T(1311)T(1312)T(1313)T(1314)T(1315)T(1316)T(1317)T(1318)T(1319)T(1320)T(1321)T(1322)T(1323)T(1324)T(1325)T(1326)T(1327)T(1328)T(1329)T(1330)T(1331)T(1332)T(1333)T(1334)T(1335)T(1336)T(1337)T(1338)T(1339)T(1340)T(1341)T(1342)T(1343)T(1344)T(1345)T(1346)T(1347)T(1348)T(1349)T(1350)T(1351)T(1352)T(1353)T(1354)T(1355)T(1356)T(1357)T(1358)T(1359)T(1360)T(1361)T(1362)T(1363)T(1364)T(1365)T(1366)T(1367)T(1368)T(1369)T(1370)T(1371)T(1372)T(1373)T(1374)T(1375)T(1376)T(1377)T(1378)T(1379)T(1380)T(1381)T(1382)T(1383)T(1384)T(1385)T(1386)T(1387)T(1388)T(1389)T(1390)T(1391)T(1392)T(1393)T(1394)T(1395)T(1396)T(1397)T(1398)T(1399) \
    T(1400)T(1401)T(1402)T(1403)T(1404)T(1405)T(1406)T(1407)T(1408)T(1409)T(1410)T(1411)T(1412)T(1413)T(1414)T(1415)T(1416)T(1417)T(1418)T(1419)T(1420)T(1421)T(1422)T(1423)T(1424)T(1425)T(1426)T(1427)T(1428)T(1429)T(1430)T(1431)T(1432)T(1433)T(1434)T(1435)T(1436)T(1437)T(1438)T(1439)T(1440)T(1441)T(1442)T(1443)T(1444)T(1445)T(1446)T(1447)T(1448)T(1449)T(1450)T(1451)T(1452)T(1453)T(1454)T(1455)T(1456)T(1457)T(1458)T(1459)T(1460)T(1461)T(1462)T(1463)T(1464)T(1465)T(1466)T(1467)T(1468)T(1469)T(1470)T(1471)T(1472)T(1473)T(1474)T(1475)T(1476)T(1477)T(1478)T(1479)T(1480)T(1481)T(1482)T(1483)T(1484)T(1485)T(1486)T(1487)T(1488)T(1489)T(1490)T(1491)T(1492)T(1493)T(1494)T(1495)T(1496)T(1497)T(1498)T(1499) \
    T(1500)T(1501)T(1502)T(1503)T(1504)T(1505)T(1506)T(1507)T(1508)T(1509)T(1510)T(1511)T(1512)T(1513)T(1514)T(1515)T(1516)T(1517)T(1518)T(1519)T(1520)T(1521)T(1522)T(1523)T(1524)T(1525)T(1526)T(1527)T(1528)T(1529)T(1530)T(1531)T(1532)T(1533)T(1534)T(1535)T(1536)T(1537)T(1538)T(1539)T(1540)T(1541)T(1542)T(1543)T(1544)T(1545)T(1546)T(1547)T(1548)T(1549)T(1550)T(1551)T(1552)T(1553)T(1554)T(1555)T(1556)T(1557)T(1558)T(1559)T(1560)T(1561)T(1562)T(1563)T(1564)T(1565)T(1566)T(1567)T(1568)T(1569)T(1570)T(1571)T(1572)T(1573)T(1574)T(1575)T(1576)T(1577)T(1578)T(1579)T(1580)T(1581)T(1582)T(1583)T(1584)T(1585)T(1586)T(1587)T(1588)T(1589)T(1590)T(1591)T(1592)T(1593)T(1594)T(1595)T(1596)T(1597)T(1598)T(1599) \
    T(1600)T(1601)T(1602)T(1603)T(1604)T(1605)T(1606)T(1607)T(1608)T(1609)T(1610)T(1611)T(1612)T(1613)T(1614)T(1615)T(1616)T(1617)T(1618)T(1619)T(1620)T(1621)T(1622)T(1623)T(1624)T(1625)T(1626)T(1627)T(1628)T(1629)T(1630)T(1631)T(1632)T(1633)T(1634)T(1635)T(1636)T(1637)T(1638)T(1639)T(1640)T(1641)T(1642)T(1643)T(1644)T(1645)T(1646)T(1647)T(1648)T(1649)T(1650)T(1651)T(1652)T(1653)T(1654)T(1655)T(1656)T(1657)T(1658)T(1659)T(1660)T(1661)T(1662)T(1663)T(1664)T(1665)T(1666)T(1667)T(1668)T(1669)T(1670)T(1671)T(1672)T(1673)T(1674)T(1675)T(1676)T(1677)T(1678)T(1679)T(1680)T(1681)T(1682)T(1683)T(1684)T(1685)T(1686)T(1687)T(1688)T(1689)T(1690)T(1691)T(1692)T(1693)T(1694)T(1695)T(1696)T(1697)T(1698)T(1699) \
    T(1700)T(1701)T(1702)T(1703)T(1704)T(1705)T(1706)T(1707)T(1708)T(1709)T(1710)T(1711)T(1712)T(1713)T(1714)T(1715)T(1716)T(1717)T(1718)T(1719)T(1720)T(1721)T(1722)T(1723)T(1724)T(1725)T(1726)T(1727)T(1728)T(1729)T(1730)T(1731)T(1732)T(1733)T(1734)T(1735)T(1736)T(1737)T(1738)T(1739)T(1740)T(1741)T(1742)T(1743)T(1744)T(1745)T(1746)T(1747)T(1748)T(1749)T(1750)T(1751)T(1752)T(1753)T(1754)T(1755)T(1756)T(1757)T(1758)T(1759)T(1760)T(1761)T(1762)T(1763)T(1764)T(1765)T(1766)T(1767)T(1768)T(1769)T(1770)T(1771)T(1772)T(1773)T(1774)T(1775)T(1776)T(1777)T(1778)T(1779)T(1780)T(1781)T(1782)T(1783)T(1784)T(1785)T(1786)T(1787)T(1788)T(1789)T(1790)T(1791)T(1792)T(1793)T(1794)T(1795)T(1796)T(1797)T(1798)T(1799) \
    T(1800)T(1801)T(1802)T(1803)T(1804)T(1805)T(1806)T(1807)T(1808)T(1809)T(1810)T(1811)T(1812)T(1813)T(1814)T(1815)T(1816)T(1817)T(1818)T(1819)T(1820)T(1821)T(1822)T(1823)T(1824)T(1825)T(1826)T(1827)T(1828)T(1829)T(1830)T(1831)T(1832)T(1833)T(1834)T(1835)T(1836)T(1837)T(1838)T(1839)T(1840)T(1841)T(1842)T(1843)T(1844)T(1845)T(1846)T(1847)T(1848)T(1849)T(1850)T(1851)T(1852)T(1853)T(1854)T(1855)T(1856)T(1857)T(1858)T(1859)T(1860)T(1861)T(1862)T(1863)T(1864)T(1865)T(1866)T(1867)T(1868)T(1869)T(1870)T(1871)T(1872)T(1873)T(1874)T(1875)T(1876)T(1877)T(1878)T(1879)T(1880)T(1881)T(1882)T(1883)T(1884)T(1885)T(1886)T(1887)T(1888)T(1889)T(1890)T(1891)T(1892)T(1893)T(1894)T(1895)T(1896)T(1897)T(1898)T(1899) \
    T(1900)T(1901)T(1902)T(1903)T(1904)T(1905)T(1906)T(1907)T(1908)T(1909)T(1910)T(1911)T(1912)T(1913)T(1914)T(1915)T(1916)T(1917)T(1918)T(1919)T(1920)T(1921)T(1922)T(1923)T(1924)T(1925)T(1926)T(1927)T(1928)T(1929)T(1930)T(1931)T(1932)T(1933)T(1934)T(1935)T(1936)T(1937)T(1938)T(1939)T(1940)T(1941)T(1942)T(1943)T(1944)T(1945)T(1946)T(1947)T(1948)T(1949)T(1950)T(1951)T(1952)T(1953)T(1954)T(1955)T(1956)T(1957)T(1958)T(1959)T(1960)T(1961)T(1962)T(1963)T(1964)T(1965)T(1966)T(1967)T(1968)T(1969)T(1970)T(1971)T(1972)T(1973)T(1974)T(1975)T(1976)T(1977)T(1978)T(1979)T(1980)T(1981)T(1982)T(1983)T(1984)T(1985)T(1986)T(1987)T(1988)T(1989)T(1990)T(1991)T(1992)T(1993)T(1994)T(1995)T(1996)T(1997)T(1998)T(1999) \
    T(2000)T(2001)T(2002)T(2003)T(2004)T(2005)T(2006)T(2007)T(2008)T(2009)T(2010)T(2011)T(2012)T(2013)T(2014)T(2015)T(2016)T(2017)T(2018)T(2019)T(2020)T(2021)T(2022)T(2023)T(2024)T(2025)T(2026)T(2027)T(2028)T(2029)T(2030)T(2031)T(2032)T(2033)T(2034)T(2035)T(2036)T(2037)T(2038)T(2039)T(2040)T(2041)T(2042)T(2043)T(2044)T(2045)T(2046)T(2047)T(2048)T(2049)T(2050)T(2051)T(2052)T(2053)T(2054)T(2055)T(2056)T(2057)T(2058)T(2059)T(2060)T(2061)T(2062)T(2063)T(2064)T(2065)T(2066)T(2067)T(2068)T(2069)T(2070)T(2071)T(2072)T(2073)T(2074)T(2075)T(2076)T(2077)T(2078)T(2079)T(2080)T(2081)T(2082)T(2083)T(2084)T(2085)T(2086)T(2087)T(2088)T(2089)T(2090)T(2091)T(2092)T(2093)T(2094)T(2095)T(2096)T(2097)T(2098)T(2099) \
    T(2100)T(2101)T(2102)T(2103)T(2104)T(2105)T(2106)T(2107)T(2108)T(2109)T(2110)T(2111)T(2112)T(2113)T(2114)T(2115)T(2116)T(2117)T(2118)T(2119)T(2120)T(2121)T(2122)T(2123)T(2124)T(2125)T(2126)T(2127)T(2128)T(2129)T(2130)T(2131)T(2132)T(2133)T(2134)T(2135)T(2136)T(2137)T(2138)T(2139)T(2140)T(2141)T(2142)T(2143)T(2144)T(2145)T(2146)T(2147)T(2148)T(2149)T(2150)T(2151)T(2152)T(2153)T(2154)T(2155)T(2156)T(2157)T(2158)T(2159)T(2160)T(2161)T(2162)T(2163)T(2164)T(2165)T(2166)T(2167)T(2168)T(2169)T(2170)T(2171)T(2172)T(2173)T(2174)T(2175)T(2176)T(2177)T(2178)T(2179)T(2180)T(2181)T(2182)T(2183)T(2184)T(2185)T(2186)T(2187)T(2188)T(2189)T(2190)T(2191)T(2192)T(2193)T(2194)T(2195)T(2196)T(2197)T(2198)T(2199) \
    T(2200)T(2201)T(2202)T(2203)T(2204)T(2205)T(2206)T(2207)T(2208)T(2209)T(2210)T(2211)T(2212)T(2213)T(2214)T(2215)T(2216)T(2217)T(2218)T(2219)T(2220)T(2221)T(2222)T(2223)T(2224)T(2225)T(2226)T(2227)T(2228)T(2229)T(2230)T(2231)T(2232)T(2233)T(2234)T(2235)T(2236)T(2237)T(2238)T(2239)T(2240)T(2241)T(2242)T(2243)T(2244)T(2245)T(2246)T(2247)T(2248)T(2249)T(2250)T(2251)T(2252)T(2253)T(2254)T(2255)T(2256)T(2257)T(2258)T(2259)T(2260)T(2261)T(2262)T(2263)T(2264)T(2265)T(2266)T(2267)T(2268)T(2269)T(2270)T(2271)T(2272)T(2273)T(2274)T(2275)T(2276)T(2277)T(2278)T(2279)T(2280)T(2281)T(2282)T(2283)T(2284)T(2285)T(2286)T(2287)T(2288)T(2289)T(2290)T(2291)T(2292)T(2293)T(2294)T(2295)T(2296)T(2297)T(2298)T(2299) \
    T(2300)T(2301)T(2302)T(2303)T(2304)T(2305)T(2306)T(2307)T(2308)T(2309)T(2310)T(2311)T(2312)T(2313)T(2314)T(2315)T(2316)T(2317)T(2318)T(2319)T(2320)T(2321)T(2322)T(2323)T(2324)T(2325)T(2326)T(2327)T(2328)T(2329)T(2330)T(2331)T(2332)T(2333)T(2334)T(2335)T(2336)T(2337)T(2338)T(2339)T(2340)T(2341)T(2342)T(2343)T(2344)T(2345)T(2346)T(2347)T(2348)T(2349)T(2350)T(2351)T(2352)T(2353)T(2354)T(2355)T(2356)T(2357)T(2358)T(2359)T(2360)T(2361)T(2362)T(2363)T(2364)T(2365)T(2366)T(2367)T(2368)T(2369)T(2370)T(2371)T(2372)T(2373)T(2374)T(2375)T(2376)T(2377)T(2378)T(2379)T(2380)T(2381)T(2382)T(2383)T(2384)T(2385)T(2386)T(2387)T(2388)T(2389)T(2390)T(2391)T(2392)T(2393)T(2394)T(2395)T(2396)T(2397)T(2398)T(2399) \
    T(2400)T(2401)T(2402)T(2403)T(2404)T(2405)T(2406)T(2407)T(2408)T(2409)T(2410)T(2411)T(2412)T(2413)T(2414)T(2415)T(2416)T(2417)T(2418)T(2419)T(2420)T(2421)T(2422)T(2423)T(2424)T(2425)T(2426)T(2427)T(2428)T(2429)T(2430)T(2431)T(2432)T(2433)T(2434)T(2435)T(2436)T(2437)T(2438)T(2439)T(2440)T(2441)T(2442)T(2443)T(2444)T(2445)T(2446)T(2447)T(2448)T(2449)T(2450)T(2451)T(2452)T(2453)T(2454)T(2455)T(2456)T(2457)T(2458)T(2459)T(2460)T(2461)T(2462)T(2463)T(2464)T(2465)T(2466)T(2467)T(2468)T(2469)T(2470)T(2471)T(2472)T(2473)T(2474)T(2475)T(2476)T(2477)T(2478)T(2479)T(2480)T(2481)T(2482)T(2483)T(2484)T(2485)T(2486)T(2487)T(2488)T(2489)T(2490)T(2491)T(2492)T(2493)T(2494)T(2495)T(2496)T(2497)T(2498)T(2499) \
    T(2500)T(2501)T(2502)T(2503)T(2504)T(2505)T(2506)T(2507)T(2508)T(2509)T(2510)T(2511)T(2512)T(2513)T(2514)T(2515)T(2516)T(2517)T(2518)T(2519)T(2520)T(2521)T(2522)T(2523)T(2524)T(2525)T(2526)T(2527)T(2528)T(2529)T(2530)T(2531)T(2532)T(2533)T(2534)T(2535)T(2536)T(2537)T(2538)T(2539)T(2540)T(2541)T(2542)T(2543)T(2544)T(2545)T(2546)T(2547)T(2548)T(2549)T(2550)T(2551)T(2552)T(2553)T(2554)T(2555)T(2556)T(2557)T(2558)T(2559)T(2560)T(2561)T(2562)T(2563)T(2564)T(2565)T(2566)T(2567)T(2568)T(2569)T(2570)T(2571)T(2572)T(2573)T(2574)T(2575)T(2576)T(2577)T(2578)T(2579)T(2580)T(2581)T(2582)T(2583)T(2584)T(2585)T(2586)T(2587)T(2588)T(2589)T(2590)T(2591)T(2592)T(2593)T(2594)T(2595)T(2596)T(2597)T(2598)T(2599) \
    T(2600)T(2601)T(2602)T(2603)T(2604)T(2605)T(2606)T(2607)T(2608)T(2609)T(2610)T(2611)T(2612)T(2613)T(2614)T(2615)T(2616)T(2617)T(2618)T(2619)T(2620)T(2621)T(2622)T(2623)T(2624)T(2625)T(2626)T(2627)T(2628)T(2629)T(2630)T(2631)T(2632)T(2633)T(2634)T(2635)T(2636)T(2637)T(2638)T(2639)T(2640)T(2641)T(2642)T(2643)T(2644)T(2645)T(2646)T(2647)T(2648)T(2649)T(2650)T(2651)T(2652)T(2653)T(2654)T(2655)T(2656)T(2657)T(2658)T(2659)T(2660)T(2661)T(2662)T(2663)T(2664)T(2665)T(2666)T(2667)T(2668)T(2669)T(2670)T(2671)T(2672)T(2673)T(2674)T(2675)T(2676)T(2677)T(2678)T(2679)T(2680)T(2681)T(2682)T(2683)T(2684)T(2685)T(2686)T(2687)T(2688)T(2689)T(2690)T(2691)T(2692)T(2693)T(2694)T(2695)T(2696)T(2697)T(2698)T(2699) \
    T(2700)T(2701)T(2702)T(2703)T(2704)T(2705)T(2706)T(2707)T(2708)T(2709)T(2710)T(2711)T(2712)T(2713)T(2714)T(2715)T(2716)T(2717)T(2718)T(2719)T(2720)T(2721)T(2722)T(2723)T(2724)T(2725)T(2726)T(2727)T(2728)T(2729)T(2730)T(2731)T(2732)T(2733)T(2734)T(2735)T(2736)T(2737)T(2738)T(2739)T(2740)T(2741)T(2742)T(2743)T(2744)T(2745)T(2746)T(2747)T(2748)T(2749)T(2750)T(2751)T(2752)T(2753)T(2754)T(2755)T(2756)T(2757)T(2758)T(2759)T(2760)T(2761)T(2762)T(2763)T(2764)T(2765)T(2766)T(2767)T(2768)T(2769)T(2770)T(2771)T(2772)T(2773)T(2774)T(2775)T(2776)T(2777)T(2778)T(2779)T(2780)T(2781)T(2782)T(2783)T(2784)T(2785)T(2786)T(2787)T(2788)T(2789)T(2790)T(2791)T(2792)T(2793)T(2794)T(2795)T(2796)T(2797)T(2798)T(2799) \
    T(2800)T(2801)T(2802)T(2803)T(2804)T(2805)T(2806)T(2807)T(2808)T(2809)T(2810)T(2811)T(2812)T(2813)T(2814)T(2815)T(2816)T(2817)T(2818)T(2819)T(2820)T(2821)T(2822)T(2823)T(2824)T(2825)T(2826)T(2827)T(2828)T(2829)T(2830)T(2831)T(2832)T(2833)T(2834)T(2835)T(2836)T(2837)T(2838)T(2839)T(2840)T(2841)T(2842)T(2843)T(2844)T(2845)T(2846)T(2847)T(2848)T(2849)T(2850)T(2851)T(2852)T(2853)T(2854)T(2855)T(2856)T(2857)T(2858)T(2859)T(2860)T(2861)T(2862)T(2863)T(2864)T(2865)T(2866)T(2867)T(2868)T(2869)T(2870)T(2871)T(2872)T(2873)T(2874)T(2875)T(2876)T(2877)T(2878)T(2879)T(2880)T(2881)T(2882)T(2883)T(2884)T(2885)T(2886)T(2887)T(2888)T(2889)T(2890)T(2891)T(2892)T(2893)T(2894)T(2895)T(2896)T(2897)T(2898)T(2899) \
    T(2900)T(2901)T(2902)T(2903)T(2904)T(2905)T(2906)T(2907)T(2908)T(2909)T(2910)T(2911)T(2912)T(2913)T(2914)T(2915)T(2916)T(2917)T(2918)T(2919)T(2920)T(2921)T(2922)T(2923)T(2924)T(2925)T(2926)T(2927)T(2928)T(2929)T(2930)T(2931)T(2932)T(2933)T(2934)T(2935)T(2936)T(2937)T(2938)T(2939)T(2940)T(2941)T(2942)T(2943)T(2944)T(2945)T(2946)T(2947)T(2948)T(2949)T(2950)T(2951)T(2952)T(2953)T(2954)T(2955)T(2956)T(2957)T(2958)T(2959)T(2960)T(2961)T(2962)T(2963)T(2964)T(2965)T(2966)T(2967)T(2968)T(2969)T(2970)T(2971)T(2972)T(2973)T(2974)T(2975)T(2976)T(2977)T(2978)T(2979)T(2980)T(2981)T(2982)T(2983)T(2984)T(2985)T(2986)T(2987)T(2988)T(2989)T(2990)T(2991)T(2992)T(2993)T(2994)T(2995)T(2996)T(2997)T(2998)T(2999) \
    T(3000)T(3001)T(3002)T(3003)T(3004)T(3005)T(3006)T(3007)T(3008)T(3009)T(3010)T(3011)T(3012)T(3013)T(3014)T(3015)T(3016)T(3017)T(3018)T(3019)T(3020)T(3021)T(3022)T(3023)T(3024)T(3025)T(3026)T(3027)T(3028)T(3029)T(3030)T(3031)T(3032)T(3033)T(3034)T(3035)T(3036)T(3037)T(3038)T(3039)T(3040)T(3041)T(3042)T(3043)T(3044)T(3045)T(3046)T(3047)T(3048)T(3049)T(3050)T(3051)T(3052)T(3053)T(3054)T(3055)T(3056)T(3057)T(3058)T(3059)T(3060)T(3061)T(3062)T(3063)T(3064)T(3065)T(3066)T(3067)T(3068)T(3069)T(3070)T(3071)T(3072)T(3073)T(3074)T(3075)T(3076)T(3077)T(3078)T(3079)T(3080)T(3081)T(3082)T(3083)T(3084)T(3085)T(3086)T(3087)T(3088)T(3089)T(3090)T(3091)T(3092)T(3093)T(3094)T(3095)T(3096)T(3097)T(3098)T(3099) \
    T(3100)T(3101)T(3102)T(3103)T(3104)T(3105)T(3106)T(3107)T(3108)T(3109)T(3110)T(3111)T(3112)T(3113)T(3114)T(3115)T(3116)T(3117)T(3118)T(3119)T(3120)T(3121)T(3122)T(3123)T(3124)T(3125)T(3126)T(3127)T(3128)T(3129)T(3130)T(3131)T(3132)T(3133)T(3134)T(3135)T(3136)T(3137)T(3138)T(3139)T(3140)T(3141)T(3142)T(3143)T(3144)T(3145)T(3146)T(3147)T(3148)T(3149)T(3150)T(3151)T(3152)T(3153)T(3154)T(3155)T(3156)T(3157)T(3158)T(3159)T(3160)T(3161)T(3162)T(3163)T(3164)T(3165)T(3166)T(3167)T(3168)T(3169)T(3170)T(3171)T(3172)T(3173)T(3174)T(3175)T(3176)T(3177)T(3178)T(3179)T(3180)T(3181)T(3182)T(3183)T(3184)T(3185)T(3186)T(3187)T(3188)T(3189)T(3190)T(3191)T(3192)T(3193)T(3194)T(3195)T(3196)T(3197)T(3198)T(3199) \
    T(3200)T(3201)T(3202)T(3203)T(3204)T(3205)T(3206)T(3207)T(3208)T(3209)T(3210)T(3211)T(3212)T(3213)T(3214)T(3215)T(3216)T(3217)T(3218)T(3219)T(3220)T(3221)T(3222)T(3223)T(3224)T(3225)T(3226)T(3227)T(3228)T(3229)T(3230)T(3231)T(3232)T(3233)T(3234)T(3235)T(3236)T(3237)T(3238)T(3239)T(3240)T(3241)T(3242)T(3243)T(3244)T(3245)T(3246)T(3247)T(3248)T(3249)T(3250)T(3251)T(3252)T(3253)T(3254)T(3255)T(3256)T(3257)T(3258)T(3259)T(3260)T(3261)T(3262)T(3263)T(3264)T(3265)T(3266)T(3267)T(3268)T(3269)T(3270)T(3271)T(3272)T(3273)T(3274)T(3275)T(3276)T(3277)T(3278)T(3279)T(3280)T(3281)T(3282)T(3283)T(3284)T(3285)T(3286)T(3287)T(3288)T(3289)T(3290)T(3291)T(3292)T(3293)T(3294)T(3295)T(3296)T(3297)T(3298)T(3299) \
    T(3300)T(3301)T(3302)T(3303)T(3304)T(3305)T(3306)T(3307)T(3308)T(3309)T(3310)T(3311)T(3312)T(3313)T(3314)T(3315)T(3316)T(3317)T(3318)T(3319)T(3320)T(3321)T(3322)T(3323)T(3324)T(3325)T(3326)T(3327)T(3328)T(3329)T(3330)T(3331)T(3332)T(3333)T(3334)T(3335)T(3336)T(3337)T(3338)T(3339)T(3340)T(3341)T(3342)T(3343)T(3344)T(3345)T(3346)T(3347)T(3348)T(3349)T(3350)T(3351)T(3352)T(3353)T(3354)T(3355)T(3356)T(3357)T(3358)T(3359)T(3360)T(3361)T(3362)T(3363)T(3364)T(3365)T(3366)T(3367)T(3368)T(3369)T(3370)T(3371)T(3372)T(3373)T(3374)T(3375)T(3376)T(3377)T(3378)T(3379)T(3380)T(3381)T(3382)T(3383)T(3384)T(3385)T(3386)T(3387)T(3388)T(3389)T(3390)T(3391)T(3392)T(3393)T(3394)T(3395)T(3396)T(3397)T(3398)T(3399) \
    T(3400)T(3401)T(3402)T(3403)T(3404)T(3405)T(3406)T(3407)T(3408)T(3409)T(3410)T(3411)T(3412)T(3413)T(3414)T(3415)T(3416)T(3417)T(3418)T(3419)T(3420)T(3421)T(3422)T(3423)T(3424)T(3425)T(3426)T(3427)T(3428)T(3429)T(3430)T(3431)T(3432)T(3433)T(3434)T(3435)T(3436)T(3437)T(3438)T(3439)T(3440)T(3441)T(3442)T(3443)T(3444)T(3445)T(3446)T(3447)T(3448)T(3449)T(3450)T(3451)T(3452)T(3453)T(3454)T(3455)T(3456)T(3457)T(3458)T(3459)T(3460)T(3461)T(3462)T(3463)T(3464)T(3465)T(3466)T(3467)T(3468)T(3469)T(3470)T(3471)T(3472)T(3473)T(3474)T(3475)T(3476)T(3477)T(3478)T(3479)T(3480)T(3481)T(3482)T(3483)T(3484)T(3485)T(3486)T(3487)T(3488)T(3489)T(3490)T(3491)T(3492)T(3493)T(3494)T(3495)T(3496)T(3497)T(3498)T(3499) \
    T(3500)T(3501)T(3502)T(3503)T(3504)T(3505)T(3506)T(3507)T(3508)T(3509)T(3510)T(3511)T(3512)T(3513)T(3514)T(3515)T(3516)T(3517)T(3518)T(3519)T(3520)T(3521)T(3522)T(3523)T(3524)T(3525)T(3526)T(3527)T(3528)T(3529)T(3530)T(3531)T(3532)T(3533)T(3534)T(3535)T(3536)T(3537)T(3538)T(3539)T(3540)T(3541)T(3542)T(3543)T(3544)T(3545)T(3546)T(3547)T(3548)T(3549)T(3550)T(3551)T(3552)T(3553)T(3554)T(3555)T(3556)T(3557)T(3558)T(3559)T(3560)T(3561)T(3562)T(3563)T(3564)T(3565)T(3566)T(3567)T(3568)T(3569)T(3570)T(3571)T(3572)T(3573)T(3574)T(3575)T(3576)T(3577)T(3578)T(3579)T(3580)T(3581)T(3582)T(3583)T(3584)T(3585)T(3586)T(3587)T(3588)T(3589)T(3590)T(3591)T(3592)T(3593)T(3594)T(3595)T(3596)T(3597)T(3598)T(3599) \
    T(3600)T(3601)T(3602)T(3603)T(3604)T(3605)T(3606)T(3607)T(3608)T(3609)T(3610)T(3611)T(3612)T(3613)T(3614)T(3615)T(3616)T(3617)T(3618)T(3619)T(3620)T(3621)T(3622)T(3623)T(3624)T(3625)T(3626)T(3627)T(3628)T(3629)T(3630)T(3631)T(3632)T(3633)T(3634)T(3635)T(3636)T(3637)T(3638)T(3639)T(3640)T(3641)T(3642)T(3643)T(3644)T(3645)T(3646)T(3647)T(3648)T(3649)T(3650)T(3651)T(3652)T(3653)T(3654)T(3655)T(3656)T(3657)T(3658)T(3659)T(3660)T(3661)T(3662)T(3663)T(3664)T(3665)T(3666)T(3667)T(3668)T(3669)T(3670)T(3671)T(3672)T(3673)T(3674)T(3675)T(3676)T(3677)T(3678)T(3679)T(3680)T(3681)T(3682)T(3683)T(3684)T(3685)T(3686)T(3687)T(3688)T(3689)T(3690)T(3691)T(3692)T(3693)T(3694)T(3695)T(3696)T(3697)T(3698)T(3699) \
    T(3700)T(3701)T(3702)T(3703)T(3704)T(3705)T(3706)T(3707)T(3708)T(3709)T(3710)T(3711)T(3712)T(3713)T(3714)T(3715)T(3716)T(3717)T(3718)T(3719)T(3720)T(3721)T(3722)T(3723)T(3724)T(3725)T(3726)T(3727)T(3728)T(3729)T(3730)T(3731)T(3732)T(3733)T(3734)T(3735)T(3736)T(3737)T(3738)T(3739)T(3740)T(3741)T(3742)T(3743)T(3744)T(3745)T(3746)T(3747)T(3748)T(3749)T(3750)T(3751)T(3752)T(3753)T(3754)T(3755)T(3756)T(3757)T(3758)T(3759)T(3760)T(3761)T(3762)T(3763)T(3764)T(3765)T(3766)T(3767)T(3768)T(3769)T(3770)T(3771)T(3772)T(3773)T(3774)T(3775)T(3776)T(3777)T(3778)T(3779)T(3780)T(3781)T(3782)T(3783)T(3784)T(3785)T(3786)T(3787)T(3788)T(3789)T(3790)T(3791)T(3792)T(3793)T(3794)T(3795)T(3796)T(3797)T(3798)T(3799) \
    T(3800)T(3801)T(3802)T(3803)T(3804)T(3805)T(3806)T(3807)T(3808)T(3809)T(3810)T(3811)T(3812)T(3813)T(3814)T(3815)T(3816)T(3817)T(3818)T(3819)T(3820)T(3821)T(3822)T(3823)T(3824)T(3825)T(3826)T(3827)T(3828)T(3829)T(3830)T(3831)T(3832)T(3833)T(3834)T(3835)T(3836)T(3837)T(3838)T(3839)T(3840)T(3841)T(3842)T(3843)T(3844)T(3845)T(3846)T(3847)T(3848)T(3849)T(3850)T(3851)T(3852)T(3853)T(3854)T(3855)T(3856)T(3857)T(3858)T(3859)T(3860)T(3861)T(3862)T(3863)T(3864)T(3865)T(3866)T(3867)T(3868)T(3869)T(3870)T(3871)T(3872)T(3873)T(3874)T(3875)T(3876)T(3877)T(3878)T(3879)T(3880)T(3881)T(3882)T(3883)T(3884)T(3885)T(3886)T(3887)T(3888)T(3889)T(3890)T(3891)T(3892)T(3893)T(3894)T(3895)T(3896)T(3897)T(3898)T(3899) \
    T(3900)T(3901)T(3902)T(3903)T(3904)T(3905)T(3906)T(3907)T(3908)T(3909)T(3910)T(3911)T(3912)T(3913)T(3914)T(3915)T(3916)T(3917)T(3918)T(3919)T(3920)T(3921)T(3922)T(3923)T(3924)T(3925)T(3926)T(3927)T(3928)T(3929)T(3930)T(3931)T(3932)T(3933)T(3934)T(3935)T(3936)T(3937)T(3938)T(3939)T(3940)T(3941)T(3942)T(3943)T(3944)T(3945)T(3946)T(3947)T(3948)T(3949)T(3950)T(3951)T(3952)T(3953)T(3954)T(3955)T(3956)T(3957)T(3958)T(3959)T(3960)T(3961)T(3962)T(3963)T(3964)T(3965)T(3966)T(3967)T(3968)T(3969)T(3970)T(3971)T(3972)T(3973)T(3974)T(3975)T(3976)T(3977)T(3978)T(3979)T(3980)T(3981)T(3982)T(3983)T(3984)T(3985)T(3986)T(3987)T(3988)T(3989)T(3990)T(3991)T(3992)T(3993)T(3994)T(3995)T(3996)T(3997)T(3998)T(3999) \
    T(4000)T(4001)T(4002)T(4003)T(4004)T(4005)T(4006)T(4007)T(4008)T(4009)T(4010)T(4011)T(4012)T(4013)T(4014)T(4015)T(4016)T(4017)T(4018)T(4019)T(4020)T(4021)T(4022)T(4023)T(4024)T(4025)T(4026)T(4027)T(4028)T(4029)T(4030)T(4031)T(4032)T(4033)T(4034)T(4035)T(4036)T(4037)T(4038)T(4039)T(4040)T(4041)T(4042)T(4043)T(4044)T(4045)T(4046)T(4047)T(4048)T(4049)T(4050)T(4051)T(4052)T(4053)T(4054)T(4055)T(4056)T(4057)T(4058)T(4059)T(4060)T(4061)T(4062)T(4063)T(4064)T(4065)T(4066)T(4067)T(4068)T(4069)T(4070)T(4071)T(4072)T(4073)T(4074)T(4075)T(4076)T(4077)T(4078)T(4079)T(4080)T(4081)T(4082)T(4083)T(4084)T(4085)T(4086)T(4087)T(4088)T(4089)T(4090)T(4091)T(4092)T(4093)T(4094)T(4095)T(4096)T(4097)T(4098)T(4099) \
    T(4100)T(4101)T(4102)T(4103)T(4104)T(4105)T(4106)T(4107)T(4108)T(4109)T(4110)T(4111)T(4112)T(4113)T(4114)T(4115)T(4116)T(4117)T(4118)T(4119)T(4120)T(4121)T(4122)T(4123)T(4124)T(4125)T(4126)T(4127)T(4128)T(4129)T(4130)T(4131)T(4132)T(4133)T(4134)T(4135)T(4136)T(4137)T(4138)T(4139)T(4140)T(4141)T(4142)T(4143)T(4144)T(4145)T(4146)T(4147)T(4148)T(4149)T(4150)T(4151)T(4152)T(4153)T(4154)T(4155)T(4156)T(4157)T(4158)T(4159)T(4160)T(4161)T(4162)T(4163)T(4164)T(4165)T(4166)T(4167)T(4168)T(4169)T(4170)T(4171)T(4172)T(4173)T(4174)T(4175)T(4176)T(4177)T(4178)T(4179)T(4180)T(4181)T(4182)T(4183)T(4184)T(4185)T(4186)T(4187)T(4188)T(4189)T(4190)T(4191)T(4192)T(4193)T(4194)T(4195)T(4196)T(4197)T(4198)T(4199) \
    T(4200)T(4201)T(4202)T(4203)T(4204)T(4205)T(4206)T(4207)T(4208)T(4209)T(4210)T(4211)T(4212)T(4213)T(4214)T(4215)T(4216)T(4217)T(4218)T(4219)T(4220)T(4221)T(4222)T(4223)T(4224)T(4225)T(4226)T(4227)T(4228)T(4229)T(4230)T(4231)T(4232)T(4233)T(4234)T(4235)T(4236)T(4237)T(4238)T(4239)T(4240)T(4241)T(4242)T(4243)T(4244)T(4245)T(4246)T(4247)T(4248)T(4249)T(4250)T(4251)T(4252)T(4253)T(4254)T(4255)T(4256)T(4257)T(4258)T(4259)T(4260)T(4261)T(4262)T(4263)T(4264)T(4265)T(4266)T(4267)T(4268)T(4269)T(4270)T(4271)T(4272)T(4273)T(4274)T(4275)T(4276)T(4277)T(4278)T(4279)T(4280)T(4281)T(4282)T(4283)T(4284)T(4285)T(4286)T(4287)T(4288)T(4289)T(4290)T(4291)T(4292)T(4293)T(4294)T(4295)T(4296)T(4297)T(4298)T(4299) \
    T(4300)T(4301)T(4302)T(4303)T(4304)T(4305)T(4306)T(4307)T(4308)T(4309)T(4310)T(4311)T(4312)T(4313)T(4314)T(4315)T(4316)T(4317)T(4318)T(4319)T(4320)T(4321)T(4322)T(4323)T(4324)T(4325)T(4326)T(4327)T(4328)T(4329)T(4330)T(4331)T(4332)T(4333)T(4334)T(4335)T(4336)T(4337)T(4338)T(4339)T(4340)T(4341)T(4342)T(4343)T(4344)T(4345)T(4346)T(4347)T(4348)T(4349)T(4350)T(4351)T(4352)T(4353)T(4354)T(4355)T(4356)T(4357)T(4358)T(4359)T(4360)T(4361)T(4362)T(4363)T(4364)T(4365)T(4366)T(4367)T(4368)T(4369)T(4370)T(4371)T(4372)T(4373)T(4374)T(4375)T(4376)T(4377)T(4378)T(4379)T(4380)T(4381)T(4382)T(4383)T(4384)T(4385)T(4386)T(4387)T(4388)T(4389)T(4390)T(4391)T(4392)T(4393)T(4394)T(4395)T(4396)T(4397)T(4398)T(4399) \
    T(4400)T(4401)T(4402)T(4403)T(4404)T(4405)T(4406)T(4407)T(4408)T(4409)T(4410)T(4411)T(4412)T(4413)T(4414)T(4415)T(4416)T(4417)T(4418)T(4419)T(4420)T(4421)T(4422)T(4423)T(4424)T(4425)T(4426)T(4427)T(4428)T(4429)T(4430)T(4431)T(4432)T(4433)T(4434)T(4435)T(4436)T(4437)T(4438)T(4439)T(4440)T(4441)T(4442)T(4443)T(4444)T(4445)T(4446)T(4447)T(4448)T(4449)T(4450)T(4451)T(4452)T(4453)T(4454)T(4455)T(4456)T(4457)T(4458)T(4459)T(4460)T(4461)T(4462)T(4463)T(4464)T(4465)T(4466)T(4467)T(4468)T(4469)T(4470)T(4471)T(4472)T(4473)T(4474)T(4475)T(4476)T(4477)T(4478)T(4479)T(4480)T(4481)T(4482)T(4483)T(4484)T(4485)T(4486)T(4487)T(4488)T(4489)T(4490)T(4491)T(4492)T(4493)T(4494)T(4495)T(4496)T(4497)T(4498)T(4499) \
    T(4500)T(4501)T(4502)T(4503)T(4504)T(4505)T(4506)T(4507)T(4508)T(4509)T(4510)T(4511)T(4512)T(4513)T(4514)T(4515)T(4516)T(4517)T(4518)T(4519)T(4520)T(4521)T(4522)T(4523)T(4524)T(4525)T(4526)T(4527)T(4528)T(4529)T(4530)T(4531)T(4532)T(4533)T(4534)T(4535)T(4536)T(4537)T(4538)T(4539)T(4540)T(4541)T(4542)T(4543)T(4544)T(4545)T(4546)T(4547)T(4548)T(4549)T(4550)T(4551)T(4552)T(4553)T(4554)T(4555)T(4556)T(4557)T(4558)T(4559)T(4560)T(4561)T(4562)T(4563)T(4564)T(4565)T(4566)T(4567)T(4568)T(4569)T(4570)T(4571)T(4572)T(4573)T(4574)T(4575)T(4576)T(4577)T(4578)T(4579)T(4580)T(4581)T(4582)T(4583)T(4584)T(4585)T(4586)T(4587)T(4588)T(4589)T(4590)T(4591)T(4592)T(4593)T(4594)T(4595)T(4596)T(4597)T(4598)T(4599) \
    T(4600)T(4601)T(4602)T(4603)T(4604)T(4605)T(4606)T(4607)T(4608)T(4609)T(4610)T(4611)T(4612)T(4613)T(4614)T(4615)T(4616)T(4617)T(4618)T(4619)T(4620)T(4621)T(4622)T(4623)T(4624)T(4625)T(4626)T(4627)T(4628)T(4629)T(4630)T(4631)T(4632)T(4633)T(4634)T(4635)T(4636)T(4637)T(4638)T(4639)T(4640)T(4641)T(4642)T(4643)T(4644)T(4645)T(4646)T(4647)T(4648)T(4649)T(4650)T(4651)T(4652)T(4653)T(4654)T(4655)T(4656)T(4657)T(4658)T(4659)T(4660)T(4661)T(4662)T(4663)T(4664)T(4665)T(4666)T(4667)T(4668)T(4669)T(4670)T(4671)T(4672)T(4673)T(4674)T(4675)T(4676)T(4677)T(4678)T(4679)T(4680)T(4681)T(4682)T(4683)T(4684)T(4685)T(4686)T(4687)T(4688)T(4689)T(4690)T(4691)T(4692)T(4693)T(4694)T(4695)T(4696)T(4697)T(4698)T(4699) \
    T(4700)T(4701)T(4702)T(4703)T(4704)T(4705)T(4706)T(4707)T(4708)T(4709)T(4710)T(4711)T(4712)T(4713)T(4714)T(4715)T(4716)T(4717)T(4718)T(4719)T(4720)T(4721)T(4722)T(4723)T(4724)T(4725)T(4726)T(4727)T(4728)T(4729)T(4730)T(4731)T(4732)T(4733)T(4734)T(4735)T(4736)T(4737)T(4738)T(4739)T(4740)T(4741)T(4742)T(4743)T(4744)T(4745)T(4746)T(4747)T(4748)T(4749)T(4750)T(4751)T(4752)T(4753)T(4754)T(4755)T(4756)T(4757)T(4758)T(4759)T(4760)T(4761)T(4762)T(4763)T(4764)T(4765)T(4766)T(4767)T(4768)T(4769)T(4770)T(4771)T(4772)T(4773)T(4774)T(4775)T(4776)T(4777)T(4778)T(4779)T(4780)T(4781)T(4782)T(4783)T(4784)T(4785)T(4786)T(4787)T(4788)T(4789)T(4790)T(4791)T(4792)T(4793)T(4794)T(4795)T(4796)T(4797)T(4798)T(4799) \
    T(4800)T(4801)T(4802)T(4803)T(4804)T(4805)T(4806)T(4807)T(4808)T(4809)T(4810)T(4811)T(4812)T(4813)T(4814)T(4815)T(4816)T(4817)T(4818)T(4819)T(4820)T(4821)T(4822)T(4823)T(4824)T(4825)T(4826)T(4827)T(4828)T(4829)T(4830)T(4831)T(4832)T(4833)T(4834)T(4835)T(4836)T(4837)T(4838)T(4839)T(4840)T(4841)T(4842)T(4843)T(4844)T(4845)T(4846)T(4847)T(4848)T(4849)T(4850)T(4851)T(4852)T(4853)T(4854)T(4855)T(4856)T(4857)T(4858)T(4859)T(4860)T(4861)T(4862)T(4863)T(4864)T(4865)T(4866)T(4867)T(4868)T(4869)T(4870)T(4871)T(4872)T(4873)T(4874)T(4875)T(4876)T(4877)T(4878)T(4879)T(4880)T(4881)T(4882)T(4883)T(4884)T(4885)T(4886)T(4887)T(4888)T(4889)T(4890)T(4891)T(4892)T(4893)T(4894)T(4895)T(4896)T(4897)T(4898)T(4899) \
    T(4900)T(4901)T(4902)T(4903)T(4904)T(4905)T(4906)T(4907)T(4908)T(4909)T(4910)T(4911)T(4912)T(4913)T(4914)T(4915)T(4916)T(4917)T(4918)T(4919)T(4920)T(4921)T(4922)T(4923)T(4924)T(4925)T(4926)T(4927)T(4928)T(4929)T(4930)T(4931)T(4932)

#define _KWARGS_GENERATE_LITERAL_LONGDOUBLE(N) 1e##N##L,
#define _KWARGS_GENERATE_LITERAL_LONGDOUBLE_NEGATIVE(N) 1e##N##L,

    constexpr long double power[] = {
        _KWARGS_GENERATE_LITERAL(_KWARGS_GENERATE_LITERAL_LONGDOUBLE)
    };

    constexpr long double power_negative[] = {
        _KWARGS_GENERATE_LITERAL(_KWARGS_GENERATE_LITERAL_LONGDOUBLE_NEGATIVE)
    };

#undef  _KWARGS_GENERATE_LITERAL_LONGDOUBLE
#undef  _KWARGS_GENERATE_LITERAL_LONGDOUBLE_NEGATIVE
#undef  _KWARGS_GENERATE
    
    if (__i >= 0)
    {
        assert(__i < static_cast<decltype(__i)>(std::size(power)));
        return power[__i];
    }
    else
    {
        assert(-__i < static_cast<decltype(__i)>(std::size(power_negative)));
        return power_negative[-__i];
    }

#endif  // _KWARGS_LONG_DOUBLE_IS_DOUBLE
}


template<typename _Tp>
[[nodiscard]] constexpr auto fast_power(_Tp __fv, _Tp __sv, _Tp __mod = std::numeric_limits<_Tp>::max()) noexcept
        -> std::enable_if_t<std::is_unsigned_v<_Tp>, _Tp>
{
    _Tp result = 1;
    
    for (__fv %= __mod; __sv; __fv = (static_cast<promote_numeric_t<_Tp>>(__fv) * __fv) % __mod, __sv >>= 1)
    {
        if (__sv & 1)
        {
            result = (static_cast<promote_numeric_t<_Tp>>(result) * __fv) % __mod;
        }
    }

    return result;
}


// Miller Rabin
template<typename _Tp>
[[nodiscard]] constexpr auto is_prime(_Tp __n) noexcept -> std::enable_if_t<std::is_integral_v<_Tp>, bool>
{
    constexpr std::array<_Tp, 25> test{
         2,  3,  5,  7, 11, 13, 17, 19, 23, 29,
        31, 37, 41, 43, 47, 53, 59, 61, 67, 71,
        73, 79, 83, 89, 97
    };

    if (__n <= 1)
    {
        return false;
    }


    _Tp t = __n - 1, k = 0;

    for (; (t & 1) == 0; ++k, t >>= 1);


    for (std::size_t i = 0; i < test.size(); ++i)
    {
        if (__n == test[i])
        {
            return true;
        }

        _Tp a = fast_power(test[i], t, __n);
        _Tp nxt = a;

        for (_Tp j = 1; j <= k; ++j)
        {
            nxt = (static_cast<promote_numeric_t<_Tp>>(a) * a) % __n;

            if (nxt == 1 && a != 1 && a != __n - 1)
            {
                return false;
            }

            a = nxt;
        }

        if (a != 1)
        {
            return false;
        }
    }

    return true;
}

template <typename _Tp, typename _Other = _Tp>
constexpr _Tp exchange(_Tp& __value, _Other&& __newValue) noexcept
{
    _Tp old_value = std::move(__value);
    __value = std::forward<_Other>(__newValue);

    return old_value;
}


#define _KWARGS_SPACE_CHARACTER_SET  " \t\r\f\n"

constexpr void string_view_left_trimmed(std::string_view& __sv) noexcept
{
    if (auto pos = __sv.find_first_not_of(_KWARGS_SPACE_CHARACTER_SET))
    {
        __sv.remove_prefix(pos != std::string_view::npos ? pos : __sv.size());
    }
}

constexpr void string_view_right_trimmed(std::string_view& __sv) noexcept
{
    if (auto pos = __sv.find_last_not_of(_KWARGS_SPACE_CHARACTER_SET); __sv.size() && pos != __sv.size() - 1)
    {
        __sv.remove_suffix(pos != std::string_view::npos ? __sv.size() - 1 - pos : __sv.size());
    }
}

constexpr void string_view_trimmed(std::string_view& __sv) noexcept
{
    string_view_left_trimmed (__sv);
    string_view_right_trimmed(__sv);
}

_KWARGS_Test_([]() constexpr noexcept -> bool { std::string_view sv = "  _ 5201314 _  "; string_view_trimmed(sv); return sv == "_ 5201314 _"; }());

#undef _KWARGS_SPACE_CHARACTER_SET


template<typename _Tp>
[[nodiscard]] constexpr auto string_to_integer(std::string_view __str) noexcept -> std::enable_if_t<std::is_unsigned_v<_Tp>, _Tp>
{
    _Tp result{};

    string_view_left_trimmed(__str);

    if (__str.empty())
    {
        return _Tp{};
    }

    if (__str.front() == '+')
    {
        __str.remove_prefix(1);
        string_view_left_trimmed(__str);

        if (__str.empty())
        {
            return _Tp{};
        }
    }
    

    if (__str.size() >= 2 && __str[0] == '0')
    {
#define _KWARGS_Convert_to_decimal_from(__radix, __start)                        \
            do                                                                   \
            {                                                                    \
                for (auto it = __start, last = __str.end(); it != last; ++it)    \
                {                                                                \
                    if (is_radix_character(*it, __radix) == false)               \
                    {                                                            \
                        return result;                                           \
                    }                                                            \
                    result = result * __radix + radix_character_to_integer(*it); \
                }                                                                \
                return result;                                                   \
            } while (false)


        switch (__str[1])
        {
            // Binary
            case 'b': case 'B':
                _KWARGS_Convert_to_decimal_from(2, __str.begin() + 2);
                
            // Octal
            case 'o': case 'O':
                _KWARGS_Convert_to_decimal_from(8, __str.begin() + 2);

            // Hexadecimal
            case 'x': case 'X':
            case 'h': case 'H':
                _KWARGS_Convert_to_decimal_from(16, __str.begin() + 2);
        }

    }

    // Decimal
    _KWARGS_Convert_to_decimal_from(10, __str.begin());

#undef _KWARGS_Convert_to_decimal_from
}

template<typename _Tp>
[[nodiscard]] constexpr auto string_to_integer(std::string_view __str) noexcept -> std::enable_if_t<std::is_signed_v<_Tp>, _Tp>
{
    string_view_left_trimmed(__str);

    if (__str.empty())
    {
        return _Tp{};
    }

    if (__str.front() == '-')
    {
        return -static_cast<_Tp>(string_to_integer<std::make_unsigned_t<_Tp>>(__str.substr(1)));
    }
    else
    {
        return static_cast<_Tp>(string_to_integer<std::make_unsigned_t<_Tp>>(__str));
    }
}

_KWARGS_Test_(string_to_integer<std::int32_t>("  "), 0);                      _KWARGS_Test_(string_to_integer<std::int32_t>("  -"), 0);
_KWARGS_Test_(string_to_integer<std::int32_t>("0"), 0);                       _KWARGS_Test_(string_to_integer<std::int32_t>("-0"), -0);
_KWARGS_Test_(string_to_integer<std::int32_t>("123"), 123);                   _KWARGS_Test_(string_to_integer<std::int32_t>("-123"), -123);
_KWARGS_Test_(string_to_integer<std::int32_t>("100"), 100);                   _KWARGS_Test_(string_to_integer<std::int32_t>("-100"), -100);
_KWARGS_Test_(string_to_integer<std::int32_t>("0b101"), 0b101);               _KWARGS_Test_(string_to_integer<std::int32_t>("-0b101"), -0b101);
_KWARGS_Test_(string_to_integer<std::int32_t>("0b1011001110"), 0b1011001110); _KWARGS_Test_(string_to_integer<std::int32_t>("-0b1011001110"), -0b1011001110);
_KWARGS_Test_(string_to_integer<std::int32_t>("0o1234567"), 01234567);        _KWARGS_Test_(string_to_integer<std::int32_t>("-0o1234567"), -01234567);
_KWARGS_Test_(string_to_integer<std::int64_t>("0xfFabceEd"), 0xfFabceEdLL);   _KWARGS_Test_(string_to_integer<std::int64_t>("-0xfFabceEd"), -0xfFabceEdLL);
_KWARGS_Test_(string_to_integer<std::int64_t>("  +0x5201314A"), 0x5201314A);  _KWARGS_Test_(string_to_integer<std::int64_t>(" -0x5201314A"), -0x5201314A);


template<typename _Tp>
[[nodiscard]] constexpr auto string_to_floating_point(std::string_view __str) noexcept -> std::enable_if_t<std::is_floating_point_v<_Tp>, _Tp>
{
    constexpr _Tp zero = static_cast<_Tp>(0);
    constexpr _Tp one  = static_cast<_Tp>(1);
    constexpr _Tp ten  = static_cast<_Tp>(10);


    if (string_view_trimmed(__str), __str.empty())
    {
        return zero;
    }


    bool negative = false;

    if (__str.front() == '-')
    {
        negative = true;
        __str.remove_prefix(1);
    }
    else if (__str.front() == '+')
    {
        __str.remove_prefix(1);
    }


    if (string_view_left_trimmed(__str), __str.empty())
    {
        return zero;
    }

    
    if (is_same_string_ignore_case(__str, "inf") || is_same_string_ignore_case(__str, "infinity"))
    {
        return negative ? -std::numeric_limits<float>::infinity() : std::numeric_limits<float>::infinity();
    }
    else if (is_same_string_ignore_case(__str, "nan"))
    {
        return negative ? -std::numeric_limits<float>::quiet_NaN() : std::numeric_limits<float>::quiet_NaN();
    }


    _Tp result = zero;

    std::size_t i = 0;

    for (; i < __str.size() && __str[i] >= '0' && __str[i] <= '9'; ++i)
    {
        result = result * ten + (__str[i] - '0');
    }


    if (i < __str.size() && __str[i] == '.' && (++i, true))
    {
        _Tp fractional_part = zero;
        
        _Tp power = one;

        for (; i < __str.size() && __str[i] >= '0' && __str[i] <= '9'; ++i, power *= 10)
        {
            fractional_part = fractional_part * ten + (__str[i] - '0');
        }
        
        result += fractional_part / power;
    }


    if (i < __str.size() && is_same_char_ignore_case(__str[i], 'e') && ++i < __str.size())
    {
        result *= power10<_Tp>(string_to_integer<std::int64_t>(__str.substr(i)));
    }

    return negative ? -result : result;
}

_KWARGS_Test_(string_to_floating_point<float>("123.123"), 123.123f);  _KWARGS_Test_(string_to_floating_point<float>("-123.123"), -123.123f);
_KWARGS_Test_(string_to_floating_point<float>("0.520"), 0.520f);      _KWARGS_Test_(string_to_floating_point<float>("-0.1314"), -0.1314f);
_KWARGS_Test_(string_to_floating_point<double>("0.520"), 0.520);      _KWARGS_Test_(string_to_floating_point<double>("-0.1314"), -0.1314);
_KWARGS_Test_(string_to_floating_point<long double>("134567890.0987654321"), 134567890.0987654321L);
_KWARGS_Test_(string_to_floating_point<long double>("-134567890.0987654321"), -134567890.0987654321L);
_KWARGS_Test_(string_to_floating_point<double>("0.520E4"), 0.520E4);   _KWARGS_Test_(string_to_floating_point<double>("-0.1314e10"), -0.1314e10);
_KWARGS_Test_(string_to_floating_point<double>("0.520E-5"), 0.520E-5); _KWARGS_Test_(string_to_floating_point<double>("-0.1314e-10"), -0.1314e-10);
_KWARGS_Test_(string_to_floating_point<double>("1e308"), 1e308);       _KWARGS_Test_(string_to_floating_point<double>("-1e308"), -1e308);
_KWARGS_Test_(string_to_floating_point<double>("1e-308"), 1e-308);     _KWARGS_Test_(string_to_floating_point<double>("-1e-308"), -1e-308);


template<typename _Tp>
[[nodiscard]] constexpr auto string_to_number(std::string_view __str) noexcept
        -> std::enable_if_t<std::disjunction_v<std::is_integral<_Tp>, std::is_floating_point<_Tp>>, _Tp>
{
    if constexpr (std::is_integral_v<_Tp>)
    {
        return string_to_integer<_Tp>(__str);
    }
    else
    {
        return string_to_floating_point<_Tp>(__str);
    }
}


#undef _KWARGS_Test_

}  // namespace detail


#undef _KWARGS_INT128
#undef _KWARGS_UINT128

#undef _KWARGS_HAS_INT128
#undef _KWARGS_HAS_UINT128

#undef _KWARGS_LONG_DOUBLE_IS_DOUBLE


/**
* @brief Define KWARGSKEY_CASE_INSENSITIVE to enable case-insensitivity for KwargsKey.
*/
#ifndef KWARGSKEY_CASE_INSENSITIVE
#   define _KWARGSKEY_To_lowercase(c) (c)
#else
#   define _KWARGSKEY_To_lowercase(c) detail::tolower(c)
#endif

class KwargsKey
{
public:

    using value_type = std::uint64_t;

    constexpr KwargsKey() = default;

    template<std::size_t _Size>
    constexpr KwargsKey(const char (&__str)[_Size]) noexcept
        : KwargsKey(__str, _Size - 1)
    { }

    /// @note It does not need to contains '\0' at the end.
    constexpr KwargsKey(const char* const __str, std::size_t __size) noexcept
        : KwargsKey()
    {
        for (std::size_t i = 0; i < __size; ++i)
        { append(__str[i]); }
    }

    constexpr explicit KwargsKey(value_type __option) noexcept
        : _M_key(__option)
    { }

    constexpr void append(char __c) noexcept
    { _M_key = ((_M_key * base) % mod + _KWARGSKEY_To_lowercase(__c)) % mod; }

    template<typename... _Args>
    constexpr void append(char __c, _Args&&... __args) noexcept
    {
        append(__c);
        append(__args...);
    }

    static_assert(std::is_same<value_type, std::uint64_t>::value);

    static constexpr value_type base = 0X1C1ULL;
    static constexpr value_type mod  = 0X91F5BCB8BB0243ULL;

    static_assert(std::less_equal<value_type>{}(mod, std::numeric_limits<value_type>::max() / base), "KwargsKey: The expression 'base * mod' overflows.");

    static_assert(detail::is_prime(base)
#if !defined(_KWARGS_HAS_UINT128)
        || base > 0x100000000ULL
#endif  
        , "KwargsKey: 'base' must be a prime number.");

    static_assert(detail::is_prime(mod)
#if !defined(_KWARGS_HAS_UINT128)
        || mod > 0x100000000ULL
#endif  
        , "KwargsKey: 'mod' must be a prime number.");

    static_assert(std::gcd(base, mod) == 1, "KwargsKey: 'base' and 'mod' must be coprime.");


    [[nodiscard]]
    constexpr operator value_type() const noexcept
    { return _M_key; }

    [[nodiscard]]
    constexpr bool operator==(KwargsKey __other) const noexcept
    { return _M_key == __other._M_key; }

    [[nodiscard]]
    constexpr bool operator<(KwargsKey __other) const noexcept
    { return _M_key < __other._M_key; }


    /// @brief Multiple KwargsKeys can be joined using the `or` operator.
    [[nodiscard]]
    friend constexpr std::array<KwargsKey, 2>
        operator||(KwargsKey __first, KwargsKey __second) noexcept
    { return {__first, __second}; }

    template<std::size_t _StringSize>
    [[nodiscard]]
    friend constexpr std::array<KwargsKey, 2>
        operator||(KwargsKey __first, const char (&__second)[_StringSize]) noexcept
    { return {__first, KwargsKey(__second)}; }

    template<std::size_t _Size>
    [[nodiscard]]
    friend constexpr std::array<KwargsKey, _Size + 1>
        operator||(const std::array<KwargsKey, _Size>& __first, KwargsKey __second) noexcept
    {
        std::array<KwargsKey, _Size + 1> res;
        std::copy(__first.begin(), __first.end(), res.begin());
        res.back() = __second;
        return res;
    }

    template<std::size_t _Size, std::size_t _StringSize>
    [[nodiscard]]
    friend constexpr std::array<KwargsKey, _Size + 1>
        operator||(const std::array<KwargsKey, _Size>& __first, const char (&__second)[_StringSize]) noexcept
    {
        std::array<KwargsKey, _Size + 1> res;
        std::copy(__first.begin(), __first.end(), res.begin());
        res.back() = KwargsKey(__second);
        return res;
    }

private:

    value_type _M_key = 0;
};

#undef _KWARGSKEY_To_lowercase

inline namespace literals
{

[[nodiscard]] constexpr KwargsKey operator""_opt(const char* const __str, std::size_t __size) noexcept
{ return KwargsKey(__str, __size); }

template<char... _String>
[[nodiscard]] constexpr KwargsKey operator""_opt() noexcept
{ KwargsKey result; return result.append(_String...), result; }

[[nodiscard]] constexpr KwargsKey operator""_opt(char __ch) noexcept
{ KwargsKey result; return result.append(__ch), result; }

}  // namespace literals


#define _KWARGSVALUE_INCORRECT_CONVERSION_()  assert(false && "KwargsValue: Incorrect conversion.")


class KwargsValue
{
protected:

    enum WorkFlags : int
    {
        DoApplyAndCopy = 0,       /// type*     , type**
        DoFree,                   /// type*     , [unused]
        DoGetTypeName,            /// [unused]  , const char**
        DoGetTypeHash,            /// [unused]  , std::size_t*
        DoGetValueTypeHash,       /// [unused]  , std::size_t*
        DoGetBeginIterator,       /// this      , iterator**
        DoIterate,                /// std::pair<this, iterator**>, value_type*
        DoIterateAny,             /// std::pair<this, iterator**>, KwargsValue*

        DoCheckInt  = 0b0001'000,  /// [unused], int*
        DoCheckReal = 0b0010'000,  /// [unused], int*
        DoCheckSign = 0b0100'000,  /// [unused], int*
        DoCheckEnum = 0b1000'000,  /// [unused], int*

        DoCheckIterable  = 0b001'0000'000, /// [unused], int*
        DoCheckValueType = 0b010'0000'000, /// [unused], int*
        DoCheckStdArray  = 0b100'0000'000  /// [unused], int*
    };

    [[nodiscard]]
    friend constexpr WorkFlags operator|(WorkFlags __first, WorkFlags __second) noexcept
    { return static_cast<WorkFlags>(static_cast<std::underlying_type_t<WorkFlags>>(__first) | static_cast<std::underlying_type_t<WorkFlags>>(__second)); }

    template<typename _Tp>
    static void _S_manage(WorkFlags __work, void* __inData, void* __outData)
    {
        using type = detail::enum_underlying_type_t<detail::remove_reference_cv_t<_Tp>>;

        switch (__work)
        {
            case DoApplyAndCopy:
            {
                type*  iptr = reinterpret_cast<type*>(__inData);
                type** optr = reinterpret_cast<type**>(__outData);

                *optr = new type(*iptr);
                break;
            }

            case DoFree:
            {
                delete reinterpret_cast<type*>(__inData);
                break;
            }

            case DoGetTypeName:
            {
                *reinterpret_cast<const char**>(__outData) = typeid(type).name();
                break;
            }

            case DoGetTypeHash:
            {
                *reinterpret_cast<std::size_t*>(__outData) = typeid(type).hash_code();
                break;
            }

            case DoGetValueTypeHash:
            {
                *reinterpret_cast<std::size_t*>(__outData) = typeid(detail::container_value_type_t<type>).hash_code();
                break;
            }

            case DoGetBeginIterator:
            {
                KwargsValue* iptr = reinterpret_cast<KwargsValue*>(__inData);
                detail::container_iterator_t<type>** optr = reinterpret_cast<detail::container_iterator_t<type>**>(__outData);

                *optr = new detail::container_iterator_t<type>(detail::container_begin_iterator<type>(iptr->_M_data._M_ptr));
                break;
            }

            case DoIterate:
            {
                if constexpr (detail::has_container_value_type_v<type>)
                {
                    auto iptr = reinterpret_cast<std::pair<KwargsValue*, detail::container_iterator_t<type>**>*>(__inData);
                    auto optr = reinterpret_cast<detail::container_value_type_t<type>*>(__outData);

                    if (**iptr->second == detail::container_end_iterator<type>(iptr->first->_M_data._M_ptr))
                    {
                        delete *iptr->second;
                        *iptr->second = nullptr;
                    }
                    else
                    {
                        *optr = static_cast<detail::container_value_type_t<type>>(***iptr->second);
                        ++**iptr->second;
                    }
                }

                break;
            }

            case DoIterateAny:
            {
                auto iptr = reinterpret_cast<std::pair<KwargsValue*, detail::container_iterator_t<type>**>*>(__inData);
                auto optr = reinterpret_cast<KwargsValue*>(__outData);

                if (**iptr->second == detail::container_end_iterator<type>(iptr->first->_M_data._M_ptr))
                {
                    delete *iptr->second;
                    *iptr->second = nullptr;
                }
                else
                {
                    *optr = ***iptr->second;
                    ++**iptr->second;
                }

                break;
            }

            default:
            {
                int result = 0;

                if ((__work & DoCheckInt) && detail::is_integral_v<type>)
                {
                    result |= DoCheckInt;
                }

                if ((__work & DoCheckReal) && std::is_floating_point_v<type>)
                {
                    result |= DoCheckReal;
                }
                
                if ((__work & DoCheckSign) && std::is_signed_v<type>)
                {
                    result |= DoCheckSign;
                }

                if ((__work & DoCheckEnum) && std::is_enum_v<detail::remove_reference_cv_t<_Tp>>)
                {
                    result |= DoCheckEnum;
                }

                
                if ((__work & DoCheckIterable) && detail::is_iterable_container_v<type>)
                {
                    result |= DoCheckIterable;
                }
                
                if ((__work & DoCheckValueType) && detail::has_container_value_type_v<type>)
                {
                    result |= DoCheckValueType;
                }

                if ((__work & DoCheckStdArray) && detail::is_std_array_v<type>)
                {
                    result |= DoCheckStdArray;
                }

                *reinterpret_cast<int*>(__outData) = result;
                break;
            }
        }
    }

public:

    constexpr KwargsValue() = default;

    template<typename _Tp>
    constexpr KwargsValue(const _Tp& __value) noexcept
        : _M_manager(&KwargsValue::_S_manage<_Tp>)
    {
        using type = std::remove_reference_t<_Tp>;

        if constexpr (detail::use_value_flag_v<type>)
        {
            _M_data._M_value = 0;

            for (std::size_t i = 0; i < sizeof(type); ++i)
            {
                _M_data._M_bytes[i] = reinterpret_cast<const std::byte*>(&__value)[i];
            }

            _M_valueTag = ValueFlag;
        }
        else
        {
            _M_data._M_ptr = const_cast<detail::remove_reference_cv_t<decltype(__value)>*>(std::addressof(__value));

            _M_valueTag = PointerFlag;
        }

        _M_size = sizeof(_Tp);
    }
    
    template<typename _Tp>
    inline KwargsValue(_Tp&& __value)
        : _M_manager(&KwargsValue::_S_manage<_Tp>)
    {
        using type = std::remove_reference_t<_Tp>;

        if constexpr (detail::use_value_flag_v<type>)
        {
            _M_data._M_value = 0;

            for (std::size_t i = 0; i < sizeof(type); ++i)
            {
                _M_data._M_bytes[i] = reinterpret_cast<std::byte*>(&__value)[i];
            }

            _M_valueTag = ValueFlag;
        }
        else
        {
            if constexpr (std::is_rvalue_reference_v<decltype(__value)>)
            {
                _M_data._M_ptr = new type(std::move(__value));
                _M_valueTag = AppliedFlag;
            }
            else if constexpr (std::is_lvalue_reference_v<decltype(__value)>)
            {
                _M_data._M_ptr = const_cast<std::add_pointer_t<detail::remove_reference_cv_t<decltype(__value)>>>(std::addressof(__value));

                _M_valueTag = PointerFlag;
            }
            else
            {
                static_assert(std::is_reference_v<decltype(__value)>);
            }
        }

        _M_size = sizeof(type);
    }
    
    template<std::size_t _Size>
    constexpr KwargsValue(const char (&__value)[_Size]) noexcept
        : _M_manager(&KwargsValue::_S_manage<const char*>)
        , _M_valueTag(StringLiteralFlag)
        , _M_size(_Size)
    {
        const char* ptr = __value;
        _M_data._M_ptr = const_cast<char*>(ptr);
    }
    
    _KWARGS_DESTRUCTOR_CONSTEXPR ~KwargsValue() noexcept
    {
        if (_M_valueTag == AppliedFlag)
        {
            _M_manager(DoFree, _M_data._M_ptr, nullptr);
        }
    }


    [[nodiscard]]
    constexpr const char* typeName() const noexcept
    {
        const char* result _KWARGS_VARIABLE_OPTIONAL_INITIALIZATION_CONSTEXPR;
        _M_manager(DoGetTypeName, nullptr, &result);

        return result;
    }

    [[nodiscard]]
    constexpr std::size_t typeHashCode() const noexcept
    {
        std::size_t hashCode _KWARGS_VARIABLE_OPTIONAL_INITIALIZATION_CONSTEXPR;
        _M_manager(DoGetTypeHash, nullptr, &hashCode);

        return hashCode;
    }

    /**
     * @return e.g. If my type is std::vector<int>, then return the hash code for int.
     */
    [[nodiscard]]
    constexpr std::size_t valueTypeHashCode() const noexcept
    {
        std::size_t hashCode _KWARGS_VARIABLE_OPTIONAL_INITIALIZATION_CONSTEXPR;
        _M_manager(DoGetValueTypeHash, nullptr, &hashCode);

        return hashCode;
    }

    [[nodiscard]]
    constexpr bool hasValueType() const noexcept
    {
        int result _KWARGS_VARIABLE_OPTIONAL_INITIALIZATION_CONSTEXPR;
        _M_manager(DoCheckValueType, nullptr, &result);

        return result;
    }

    template<typename _Tp>
    [[nodiscard]]
    constexpr bool isSameType() const noexcept
    { return typeid(detail::enum_underlying_type_t<detail::remove_reference_cv_t<_Tp>>).hash_code() == typeHashCode(); }

    [[nodiscard]]
    constexpr bool isInteger() const noexcept
    {
        int result _KWARGS_VARIABLE_OPTIONAL_INITIALIZATION_CONSTEXPR;
        _M_manager(DoCheckInt, nullptr, &result);

        return result;
    }

    [[nodiscard]]
    constexpr bool isRealNumber() const noexcept
    {
        int result _KWARGS_VARIABLE_OPTIONAL_INITIALIZATION_CONSTEXPR;
        _M_manager(DoCheckReal, nullptr, &result);

        return result;
    }

    [[nodiscard]]
    constexpr bool isStdArray() const noexcept
    {
        int result _KWARGS_VARIABLE_OPTIONAL_INITIALIZATION_CONSTEXPR;
        _M_manager(DoCheckStdArray, nullptr, &result);

        return result;
    }

    [[nodiscard]]
    constexpr bool isIterable() const noexcept
    {
        int result _KWARGS_VARIABLE_OPTIONAL_INITIALIZATION_CONSTEXPR;
        _M_manager(DoCheckIterable, nullptr, &result);

        return result;
    }

    template<typename _Tp>
    [[nodiscard]]
    constexpr _Tp* pointer() noexcept
    {
        switch (_M_valueTag)
        {
            case PointerFlag:
            case AppliedFlag:
                
                return reinterpret_cast<_Tp*>(_M_data._M_ptr);

            case StringLiteralFlag:

                return reinterpret_cast<const _Tp*>(_M_data._M_bytes);

            case ValueFlag:

                return reinterpret_cast<_Tp*>(&_M_data._M_value);

            [[unlikely]]
            default:
                
                assert(false);
                return nullptr;
        }
    }

    template<typename _Tp>
    [[nodiscard]]
    constexpr const _Tp* pointer() const noexcept
    {
        switch (_M_valueTag)
        {
            case PointerFlag:
            case AppliedFlag:
                
                return reinterpret_cast<const _Tp*>(_M_data._M_ptr);

            case StringLiteralFlag:

                return reinterpret_cast<const _Tp*>(_M_data._M_bytes);

            case ValueFlag:

                return reinterpret_cast<const _Tp*>(&_M_data._M_value);

            [[unlikely]]
            default:
                
                assert(false);
                return nullptr;
        }
    }


    template<typename _Tp>
    [[nodiscard]]
    constexpr _Tp& reference() noexcept
    { return *pointer<_Tp>(); }

    template<typename _Tp>
    [[nodiscard]]
    constexpr const _Tp& reference() const noexcept
    { return *pointer<_Tp>(); }


    template<typename _Tp>
    [[nodiscard]]
    constexpr operator _Tp() const noexcept
    { return value<_Tp>(); }

    template<typename _Tp>
    [[nodiscard]] constexpr auto value() const noexcept -> std::enable_if_t<std::is_same_v<std::remove_cv_t<_Tp>, const char*>, _Tp>
    {
        if (_M_valueTag == StringLiteralFlag)
        {
            return reference<const char*>();
        }
        
        std::size_t hashCode = typeHashCode();

        /// std::string -> const char*
        if (typeid(std::string).hash_code() == hashCode)
        {
            return pointer<std::string>()->c_str();
        } else

        /// std::string_view -> const char*
        if (typeid(std::string_view).hash_code() == hashCode)
        {
            return pointer<std::string_view>()->data();
        } else

        /// bool -> const char*
        if (typeid(bool).hash_code() == hashCode)
        {
            return static_cast<bool>(_M_data._M_bytes[0]) ? "true" : "false";
        } else

        /// char or uchar -> const char*
        if (typeid(char).hash_code() == hashCode ||
            typeid(signed char).hash_code() == hashCode ||
            typeid(unsigned char).hash_code() == hashCode)
        {
            return reinterpret_cast<const char*>(&_M_data._M_bytes[0]);
        } else

        /// std::vector<char> -> const char*
        if (typeid(std::vector<char>).hash_code() == hashCode)
        {
            return pointer<std::vector<char>>()->data();
        } else

        /// std::array<char, ...> -> const char*
        if (hashCode = valueTypeHashCode();
            isStdArray() &&
                (typeid(char).hash_code() == hashCode ||
                    typeid(signed char).hash_code() == hashCode ||
                    typeid(unsigned char).hash_code() == hashCode))
        {
            return pointer<std::array<char, 1>>()->data();
        }

        _KWARGSVALUE_INCORRECT_CONVERSION_();

        constexpr const char* nullstr = "";
        return nullstr;
    }

    template<typename _Tp>
    [[nodiscard]] constexpr auto value() const noexcept -> std::enable_if_t<std::is_same_v<std::remove_cv_t<_Tp>, std::string>, _Tp>
    {
        std::size_t hashCode = typeHashCode();

        if (typeid(std::string).hash_code() == hashCode)
        {
            return reference<_Tp>();
        } else

        /// const char* -> std::string
        if (_M_valueTag == StringLiteralFlag)
        {
            return std::string(reference<const char*>(), _M_size);
        } else

        /// std::string_view -> std::string
        if (isSameType<std::string_view>())
        {
            const std::string_view* ptr = pointer<std::string_view>();
            return std::string(ptr->data(), ptr->size());
        } else

        /// Integer or floating point -> std::string
        if (int is; _M_manager(DoCheckInt | DoCheckReal | DoCheckSign | DoCheckEnum, nullptr, &is), is)
        {
            if (is & DoCheckInt)
            {
                if (_M_size == sizeof(char) && !(is & DoCheckEnum))
                {
                    if (typeid(char).hash_code() == hashCode ||
                        typeid(signed char).hash_code() == hashCode ||
                        typeid(unsigned char).hash_code() == hashCode)
                    {
                        return std::string(1, static_cast<char>(_M_data._M_bytes[0]));
                    }
                    else // if (typeid(bool).hash_code() == hashCode)
                    {
                        return std::string(static_cast<bool>(_M_data._M_bytes[0]) ? "true" : "false");
                    }
                }

                if (is & DoCheckSign)
                {
                    if (_M_size == sizeof(std::int8_t))
                        return std::to_string(reference<std::int8_t>());
                    else if (_M_size == sizeof(std::int16_t))
                        return std::to_string(reference<std::int16_t>());
                    else if (_M_size == sizeof(std::int32_t))
                        return std::to_string(reference<std::int32_t>());
                    else // if (_M_size == sizeof(std::int64_t))
                        return std::to_string(reference<std::int64_t>());
                 // else if (_M_size == sizeof(std::int128_t))
                 //     return std::to_string(reference<std::int128_t>());
                }
                else
                {
                    return std::to_string(reference<std::uint64_t>());
                }
            }
            else
            {
                if (_M_size == sizeof(float))
                    return std::to_string(reference<float>());
                else if (_M_size == sizeof(double))
                    return std::to_string(reference<double>());
                else if (_M_size == sizeof(long double))
                    return std::to_string(reference<long double>());
            }
        }

        _KWARGSVALUE_INCORRECT_CONVERSION_();
        return std::string();
    }

    template<typename _Tp>
    [[nodiscard]] constexpr auto value() const noexcept -> std::enable_if_t<std::is_same_v<std::remove_cv_t<_Tp>, std::string_view>, _Tp>
    {
        std::size_t hashCode = typeHashCode();

        if (typeid(std::string_view).hash_code() == hashCode)
        {
            return reference<std::string_view>();
        }

        /// std::string -> std::string_view
        if (typeid(std::string).hash_code() == hashCode)
        {
            const std::string* ptr = pointer<std::string>();
            return std::string_view(ptr->data(), ptr->size());
        } else

        /// const char* -> std::string_view
        if (_M_valueTag == StringLiteralFlag)
        {
            return std::string_view(reference<const char*>(), _M_size);
        } else

        /// bool -> std::string_view
        if (typeid(bool).hash_code() == hashCode)
        {
            return std::string_view(static_cast<bool>(_M_data._M_bytes[0]) ? "true" : "false");
        } else

        /// char or uchar -> std::string_view
        if (typeid(char).hash_code() == hashCode ||
            typeid(signed char).hash_code() == hashCode ||
            typeid(unsigned char).hash_code() == hashCode)
        {
            return std::string_view(reinterpret_cast<const char*>(&_M_data._M_bytes[0]), 1);
        }

        _KWARGSVALUE_INCORRECT_CONVERSION_();
        return std::string_view();
    }

    template<typename _Tp>
    [[nodiscard]] constexpr auto value() const noexcept -> std::enable_if_t<detail::is_char_v<_Tp>, _Tp>
    {
        std::size_t hashCode = typeHashCode();

        if (typeid(_Tp).hash_code() == hashCode)
        {
            return static_cast<_Tp>(_M_data._M_bytes[0]);
        } else

        /// const char* -> char or uchar
        if (_M_valueTag == StringLiteralFlag)
        {
            if (_M_size)
                return static_cast<_Tp>(reference<const char*>()[0]);
            else
                return '\0';
        } else

        /// std::string -> char or uchar
        if (typeid(std::string).hash_code() == hashCode)
        {
            const std::string* strptr = pointer<std::string>();

            if (strptr->size())
                return static_cast<_Tp>(strptr->front());
            else
                return '\0';
        } else

        /// std::string_view -> char or uchar
        if (typeid(std::string_view).hash_code() == hashCode)
        {
            const std::string_view* strptr = pointer<std::string_view>();

            if (strptr->size())
                return static_cast<_Tp>(strptr->front());
            else
                return '\0';
        }

        return static_cast<_Tp>(value<int>());
    }

    template<typename _Tp>
    [[nodiscard]] constexpr auto value() const noexcept -> std::enable_if_t<detail::is_integral_v<_Tp> && detail::not_v<detail::is_char_v<_Tp>>, _Tp>
    {
        std::size_t hashCode = typeHashCode();

        if (typeid(detail::enum_underlying_type_t<detail::remove_reference_cv_t<_Tp>>).hash_code() == hashCode)
        {
            // return reference<_Tp>();

            if (_M_valueTag == ValueFlag) [[likely]]
                return *reinterpret_cast<const _Tp*>(&_M_data._M_value);

            else [[unlikely]] // if (_M_valueTag == PointerFlag || _M_valueTag == AppliedFlag)
                return *reinterpret_cast<const _Tp*>(_M_data._M_ptr);
        } else

        /// integer or floating point -> integer
        if (int is; _M_manager(DoCheckInt | DoCheckReal | DoCheckSign, nullptr, &is), is)
        {
            if (is & DoCheckInt)
            {
                if (typeid(bool).hash_code() == hashCode)
                {
                    return static_cast<_Tp>(static_cast<bool>(_M_data._M_bytes[0]) ? 1 : 0);
                }

                if (is & DoCheckSign)
                {
                    if (_M_size == sizeof(std::int8_t))
                        return static_cast<_Tp>(reference<std::int8_t>());

                    else if (_M_size == sizeof(std::int16_t)) [[unlikely]]
                        return static_cast<_Tp>(reference<std::int16_t>());

                    else if (_M_size == sizeof(std::int32_t)) [[likely]]
                        return static_cast<_Tp>(reference<std::int32_t>());

                    else // if (_M_size == sizeof(std::int64_t))
                        return static_cast<_Tp>(reference<std::int64_t>());
                }
                else
                {
                    return static_cast<_Tp>(reference<std::uint64_t>());
                }
            }
            else // if (is & DoCheckReal)
            {
                if (_M_size == sizeof(float))
                    return static_cast<_Tp>(reference<float>());

                else if (_M_size == sizeof(double))
                    return static_cast<_Tp>(reference<double>());

                else // if (_M_size == sizeof(long double))
                    return static_cast<_Tp>(reference<long double>());
            }

        } else

        /// const char* -> integer
        if (_M_valueTag == StringLiteralFlag)
        {
            if (reference<const char*>() == nullptr)
                return static_cast<_Tp>(0);
            else if (detail::is_same_char_ignore_case(reference<const char*>()[0], 't'))
                return static_cast<_Tp>(true);
            else if (detail::is_same_char_ignore_case(reference<const char*>()[0], 'f'))
                return static_cast<_Tp>(false);


            if constexpr (std::is_signed_v<detail::enum_underlying_type_t<_Tp>>)
                return static_cast<_Tp>(
                    detail::string_to_integer<std::int64_t>(reference<const char*>()));

            else if constexpr (std::is_unsigned_v<detail::enum_underlying_type_t<_Tp>>)
                return static_cast<_Tp>(
                    detail::string_to_integer<std::uint64_t>(reference<const char*>()));
        } else

        /// std::string -> integer
        if (typeid(std::string).hash_code() == hashCode)
        {
            if (detail::is_same_char_ignore_case((reference<std::string>()).front(), 't'))
                return static_cast<_Tp>(true);
            else if (detail::is_same_char_ignore_case((reference<std::string>()).front(), 'f'))
                return static_cast<_Tp>(false);


            if constexpr (std::is_signed_v<detail::enum_underlying_type_t<_Tp>>)
                return static_cast<_Tp>(
                    detail::string_to_integer<std::int64_t>(reference<std::string>()));

            else if constexpr (std::is_unsigned_v<detail::enum_underlying_type_t<_Tp>>)
                return static_cast<_Tp>(
                    detail::string_to_integer<std::uint64_t>(reference<std::string>()));
        } else

        /// std::string_view -> integer
        if (typeid(std::string_view).hash_code() == hashCode)
        {
            const std::string_view* sv = pointer<std::string_view>();
            
            if (sv->empty())
            {
                return static_cast<_Tp>(0);
            }
            else if (detail::is_same_char_ignore_case(sv->front(), 't'))
            {
                return static_cast<_Tp>(true);
            }
            else if (detail::is_same_char_ignore_case(sv->front(), 'f'))
            {
                return static_cast<_Tp>(false);
            }
            else
            {
                if constexpr (std::is_signed_v<detail::enum_underlying_type_t<_Tp>>)
                    return static_cast<_Tp>(detail::string_to_integer<std::int64_t>(*sv));

                else if constexpr (std::is_unsigned_v<detail::enum_underlying_type_t<_Tp>>)
                    return static_cast<_Tp>(detail::string_to_integer<std::uint64_t>(*sv));
            }
        }

        _KWARGSVALUE_INCORRECT_CONVERSION_();
        return _Tp();
    }

    template<typename _Tp>
    [[nodiscard]] constexpr auto value() const noexcept -> std::enable_if_t<std::is_floating_point_v<_Tp>, _Tp>
    {
        std::size_t hashCode = typeHashCode();

        if (typeid(detail::remove_reference_cv_t<_Tp>).hash_code() == hashCode)
        {
            // return reference<_Tp>();

            if (_M_valueTag == ValueFlag) [[likely]]
                return *reinterpret_cast<const _Tp*>(&_M_data._M_value);
            
            else [[unlikely]] // if (_M_valueTag == PointerFlag || _M_valueTag == AppliedFlag)
                return *reinterpret_cast<const _Tp*>(_M_data._M_ptr);
        }

        /// integer or floating point -> floating point
        if (int is; _M_manager(DoCheckInt | DoCheckReal | DoCheckSign, nullptr, &is), is)
        {
            if (is & DoCheckInt)
            {
                if (typeid(bool).hash_code() == hashCode)
                {
                    return static_cast<bool>(_M_data._M_bytes[0]) ? 1.00f : 0.00f;
                }

                if (is & DoCheckSign)
                {
                    if (_M_size == sizeof(std::int8_t))
                        return static_cast<_Tp>(reference<std::int8_t>());

                    else if (_M_size == sizeof(std::int16_t)) [[unlikely]]
                        return static_cast<_Tp>(reference<std::int16_t>());

                    else if (_M_size == sizeof(std::int32_t)) [[likely]]
                        return static_cast<_Tp>(reference<std::int32_t>());

                    else // if (_M_size == sizeof(std::int64_t))
                        return static_cast<_Tp>(reference<std::int64_t>());
                }
                else
                {
                    return static_cast<_Tp>(reference<std::uint64_t>());
                }
            }
            else // if (is & DoCheckReal)
            {
                if (_M_size == sizeof(float))
                    return static_cast<_Tp>(reference<float>());

                else if (_M_size == sizeof(double))
                    return static_cast<_Tp>(reference<double>());

                else // if (_M_size == sizeof(long double))
                    return static_cast<_Tp>(reference<long double>());
            }

        } else

        /// const char* -> floating point
        if (_M_valueTag == StringLiteralFlag)
        {
            return static_cast<_Tp>(detail::string_to_floating_point<long double>(reference<const char*>()));
        } else

        /// std::string -> floating point
        if (typeid(std::string).hash_code() == hashCode)
        {
            return static_cast<_Tp>(detail::string_to_floating_point<long double>(reference<std::string>()));
        } else

        /// std::string_view -> floating point
        if (typeid(std::string_view).hash_code() == hashCode)
        {
            return static_cast<_Tp>(detail::string_to_floating_point<long double>(reference<std::string_view>()));
        }

        _KWARGSVALUE_INCORRECT_CONVERSION_();
        return _Tp();
    }

    template<typename _Tp>
    [[nodiscard]] constexpr auto value() const noexcept
            -> std::enable_if_t<
                    (std::is_class_v<_Tp> || std::is_union_v<_Tp> || std::is_pointer_v<_Tp>) &&
                    detail::not_v<detail::is_any_of_v<std::remove_cv_t<_Tp>, const char*, std::string, std::string_view>>, _Tp>
    {
        if constexpr (detail::has_container_value_type_v<_Tp> && detail::is_insertable_container_v<_Tp>)
        {
            if (!isSameType<_Tp>() && hasValueType() && isIterable())
            {
                if (typeid(detail::container_value_type_t<detail::remove_reference_cv_t<_Tp>>).hash_code() == valueTypeHashCode())
                {
                    return _M_iterateAndCopy<detail::remove_reference_cv_t<_Tp>>();
                }
                else
                {
                    return _M_iterateAnyAndCopy<detail::remove_reference_cv_t<_Tp>>();
                }
            }
        }


        if (_M_valueTag == PointerFlag || _M_valueTag == AppliedFlag)
        {
            return *reinterpret_cast<_Tp*>(_M_data._M_ptr);
        }
        else if (_M_valueTag == StringLiteralFlag)
        {
            return _S_fromStringLiteral<_Tp>(reinterpret_cast<const char*>(_M_data._M_ptr));
        }
        else if (_M_valueTag == ValueFlag)
        {
            assert(_M_size <= sizeof(_M_data) && "KwargsValue: Incorrect conversion.");

            return *static_cast<const _Tp*>(reinterpret_cast<const void*>(&_M_data._M_value));
        }

        _KWARGSVALUE_INCORRECT_CONVERSION_();
        return _Tp();
    }

    [[nodiscard]]
    constexpr std::uint32_t size() const noexcept
    { return _M_size; }

    constexpr KwargsValue& operator=(const KwargsValue& __other)
    {
        if (_M_valueTag == AppliedFlag)
        {
            _M_manager(DoFree, _M_data._M_ptr, nullptr);
        }

        if (__other._M_valueTag == AppliedFlag)
            __other._M_manager(DoApplyAndCopy, __other._M_data._M_ptr, &_M_data._M_ptr);
        else
            _M_data._M_value = __other._M_data._M_value;

        _M_manager = __other._M_manager;
        _M_valueTag = __other._M_valueTag;
        _M_size = __other._M_size;

        return *this;
    }

    constexpr KwargsValue& operator=(KwargsValue&& __other) noexcept
    {
        if (_M_valueTag == AppliedFlag)
        {
            _M_manager(DoFree, _M_data._M_ptr, nullptr);
        }

        _M_data._M_value = __other._M_data._M_value;

        _M_valueTag = detail::exchange(
            __other._M_valueTag,
            __other._M_valueTag == AppliedFlag ? PointerFlag : __other._M_valueTag);

        _M_size = __other._M_size;

        _M_manager = __other._M_manager;

        return *this;
    }

protected:

    template<typename _Tp, typename = std::void_t<decltype(_Tp(std::declval<const char*>()))>>
    [[nodiscard]]
    static constexpr _Tp _S_fromStringLiteral(const char* __str) noexcept
    { return _Tp(__str); }

    template<typename _Tp>
    [[nodiscard]]
    static constexpr _Tp _S_fromStringLiteral(const char* __str) noexcept
    { assert(false); return _Tp(); }

    template<typename _Tp>
    [[nodiscard]]
    constexpr _Tp _M_iterateAndCopy() const noexcept
    {
        void* iterator;
        _M_manager(DoGetBeginIterator, const_cast<void*>(reinterpret_cast<const void*>(this)), &iterator);

        _Tp result{};
        std::pair<const KwargsValue*, void**> param;

        detail::container_value_type_t<_Tp> element{ };
        _M_manager(DoIterate, &(param = std::make_pair(this, &iterator)), &element);


        for (; iterator; _M_manager(DoIterate, &(param = std::make_pair(this, &iterator)), &element))
        {
#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 26800)  // C26800: Use of a moved from object: 'object'.
#endif
            detail::container_insert(result, std::move(element));
#if defined(_MSC_VER)
#   pragma warning(pop)
#endif
        }

        return result;
    }

    template<typename _Tp>
    [[nodiscard]]
    constexpr _Tp _M_iterateAnyAndCopy() const noexcept
    {
        void* iterator;
        _M_manager(DoGetBeginIterator, const_cast<void*>(reinterpret_cast<const void*>(this)), &iterator);

        _Tp result{};
        std::pair<const KwargsValue*, void**> param;

        KwargsValue element;

        _M_manager(DoIterateAny, &(param = std::make_pair(this, &iterator)), &element);

        for (; iterator; _M_manager(DoIterateAny, &(param = std::make_pair(this, &iterator)), &element))
        {
            detail::container_insert(result, element.value<detail::container_value_type_t<_Tp>>());
        }

        return result;
    }

private:
    
    union Data
    {
        void* _M_ptr = nullptr;
        
        std::uintptr_t _M_value;

        std::byte _M_bytes[sizeof(void*)];

    } _M_data;

    void (*_M_manager)(WorkFlags, void*, void*) = &_S_manage<int>;

    enum ValueTag : std::uint32_t
    {
        PointerFlag = 0,
        StringLiteralFlag,
        ValueFlag,
        AppliedFlag
    };

    ValueTag _M_valueTag = ValueFlag;

    std::uint32_t _M_size = 0;
};


#undef _KWARGSVALUE_INCORRECT_CONVERSION_


class Args
{
protected:

    enum WorkFlags : int
    {
        DoFree,    /// _M_data , [unused], [unused]
        DoFind,    /// _M_data , [used]  , KwargsValue**
        DoGetSize  /// [unused], [unused], std::size_t*
    };

    template<typename... _Args>
    static void _S_manage(WorkFlags __work, void* __inData, int __index, void* __outData)
    {
        switch (__work)
        {
            case DoFree:
            {
                auto iptr = reinterpret_cast<std::array<KwargsValue, sizeof...(_Args)>*>(__inData);

                delete iptr;
                break;
            }

            case DoFind:
            {
                auto iptr = reinterpret_cast<std::array<KwargsValue, sizeof...(_Args)>*>(__inData);
                auto optr = reinterpret_cast<KwargsValue**>(__outData);

                if (__index >= 0)
                {
                    assert(__index < static_cast<decltype(__index)>(sizeof...(_Args)));
                    *optr = &(*iptr)[__index];
                }
                else
                {
                    assert(-__index <= static_cast<decltype(__index)>(sizeof...(_Args)));
                    *optr = &(*iptr)[sizeof...(_Args) + __index];
                }

                break;
            }

            case DoGetSize:
            {
                auto optr = reinterpret_cast<std::size_t*>(__outData);
                *optr = sizeof...(_Args);
                break;
            }
        }
    }

public:

    constexpr Args() = default;

    template<typename... _Args>
    Args(_Args&&... __args) noexcept
        : _M_manager(&Args::_S_manage<_Args...>)
    {
        _M_data = new std::array<KwargsValue, sizeof...(_Args)>{ std::forward<_Args>(__args)... };
    }

    _KWARGS_DESTRUCTOR_CONSTEXPR ~Args() noexcept
    {
        if (_M_data)
        {
            _M_manager(DoFree, _M_data, 0, nullptr);
        }
    }

    [[nodiscard]]
    constexpr std::size_t size() const noexcept
    {
        std::size_t result _KWARGS_VARIABLE_OPTIONAL_INITIALIZATION_CONSTEXPR;
        _M_manager(DoGetSize, nullptr, 0, &result);
        return result;
    }

    [[nodiscard]]
    constexpr KwargsValue& operator[](int __i) noexcept
    {
        KwargsValue* result _KWARGS_VARIABLE_OPTIONAL_INITIALIZATION_CONSTEXPR;
        _M_manager(DoFind, _M_data, __i, &result);
        return *result;
    }

    [[nodiscard]]
    constexpr const KwargsValue& operator[](int __i) const noexcept
    {
        KwargsValue* result _KWARGS_VARIABLE_OPTIONAL_INITIALIZATION_CONSTEXPR;
        _M_manager(DoFind, _M_data, __i, &result);
        return *result;
    }

private:

    void* _M_data = nullptr;

    void (*_M_manager)(WorkFlags, void*, int, void*) = &Args::_S_manage<>;
};


template<KwargsKey::value_type... _OptionalList>
class Kwargs
{
public:

    using value_type = std::pair<KwargsKey, KwargsValue>;
    using container_type = std::initializer_list<value_type>;

    using iterator = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;

    struct DataItem
    {
        constexpr DataItem(const KwargsValue* __that = nullptr) noexcept
            : _M_that(__that)
        { }

        template<typename _ValueType, typename... _Args>
        [[nodiscard]]
        constexpr _ValueType valueOr(_Args&&... __args) const noexcept
        { return _M_that ? _M_that->value<_ValueType>() : _ValueType(std::forward<_Args>(__args)...); }

        [[nodiscard]]
        constexpr bool hasValue() const noexcept
        { return _M_that; }

        [[nodiscard]]
        constexpr const DataItem* operator->() const noexcept
        { return this; }

    private:

        const KwargsValue* _M_that;
    };

    constexpr Kwargs(container_type __list) noexcept
        : _M_data(__list)
    {
        /// If _OptionalList is not empty then check
        /// whether all keys are in _OptionalList
        if constexpr (sizeof...(_OptionalList))
        {
            for (const auto& i : __list)
            {
                if (_M_contains<_OptionalList...>(i.first) == false)
                {
                    assert(false && "This key is not in the OptionalList.");
                }
            }
        }

        /// Check for duplicate keys
        assert([&]() constexpr -> bool
        {
            std::set<KwargsKey> st;

            for (const auto& [key, unused] : __list)
            {
                st.insert(key);
            }

            return st.size() == __list.size();
        }());
    }

    [[nodiscard]]
    constexpr DataItem operator[](KwargsKey __option) noexcept
    {
        for (const auto& i : _M_data)
        {
            if (i.first == __option)
            {
                return DataItem(&i.second);
            }
        }

        return DataItem(nullptr);
    }

    template<std::size_t _Size>
    [[nodiscard]]
    constexpr DataItem operator[](const std::array<KwargsKey, _Size>& __options) noexcept
    {
        for (KwargsKey key : __options)
        {
            for (const auto& i : _M_data)
            {
                if (i.first == key)
                {
                    return DataItem(&i.second);
                }
            }
        }
        return DataItem(nullptr);
    }

    [[nodiscard]]
    constexpr const_iterator begin() const noexcept
    { return _M_data.begin(); }

    [[nodiscard]]
    constexpr const_iterator end() const noexcept
    { return _M_data.end(); }

    [[nodiscard]]
    constexpr std::size_t size() const noexcept
    { return _M_data.size(); }

protected:

    template<KwargsKey::value_type _Current>
    [[nodiscard]]
    constexpr bool _M_contains(KwargsKey __option) const noexcept
    { return __option == KwargsKey(_Current); }

    template<KwargsKey::value_type _Current, KwargsKey::value_type _Next, KwargsKey::value_type... _Args>
    [[nodiscard]]
    constexpr bool _M_contains(KwargsKey __option) const noexcept
    { return __option == KwargsKey(_Current) || _M_contains<_Next, _Args...>(__option); }

private:

    container_type _M_data;
};


#undef _KWARGS_DESTRUCTOR_CONSTEXPR
#undef _KWARGS_VARIABLE_OPTIONAL_INITIALIZATION_CONSTEXPR


#if defined(_MSC_VER)
#   pragma warning (pop)
#endif


}  // namespace kwargs


// --- Export ---

using kwargs::KwargsKey;
using kwargs::KwargsValue;

using kwargs::Kwargs;
using kwargs::Args;

using namespace kwargs::literals;


#endif  // CPP_KWARGS_H


//  _                           _                                         _           //
// ( )                         ( )                                       (_ )         //
// | |__   _   _    _ _   ___  | |__   _   _    _ _   ___     _     ___   | |  _   _  //
// |  _ `\( ) ( ) /'_` )/' _ `\|  _ `\( ) ( ) /'_` )/' _ `\ /'_`\ /' _ `\ | | ( ) ( ) //
// | | | || (_) |( (_| || ( ) || | | || (_) |( (_| || ( ) |( (_) )| ( ) | | | | (_) | //
// (_) (_)`\___/'`\__,_)(_) (_)(_) (_)`\___/'`\__,_)(_) (_)`\___/'(_) (_)(___)`\__, | //
// https://github.com/huanhuanonly                                            ( )_| | //
//                                                                            `\___/' //