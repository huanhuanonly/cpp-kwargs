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
#   define _KWARGS_VARIABLE_OPTIONAL_INITIALIZATION_CONSTEXPR { }
#endif



#if defined(_MSC_VER)
#   pragma warning (push)
#   pragma warning (disable : 5051)  // C5051: attribute [[attribute-name]] requires at least 'standard_version'; ignored
#endif


namespace detail
{

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
        -> typename std::enable_if_t<detail::is_iterable_container_v<_Container>, typename _Container::iterator>
{ return reinterpret_cast<_Container*>(__cp)->begin(); }

template<typename _Container>
[[nodiscard]] constexpr auto  container_begin_iterator([[maybe_unused]] void* __cp) noexcept
        -> typename std::enable_if_t<detail::not_v<detail::is_iterable_container_v<_Container>>, typename std::byte*>
{ return nullptr; }

template<typename _Container>
[[nodiscard]] constexpr auto container_end_iterator(void* __cp) noexcept
        -> typename std::enable_if_t<detail::is_iterable_container_v<_Container>, typename _Container::iterator>
{ return reinterpret_cast<_Container*>(__cp)->end(); }

template<typename _Container>
[[nodiscard]] constexpr auto container_end_iterator([[maybe_unused]] void* __cp) noexcept
        -> typename std::enable_if_t<detail::not_v<detail::is_iterable_container_v<_Container>>, typename std::byte*>
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

/// .push_back()

template<typename _Container, typename _ValueType, typename = std::void_t<>>
struct has_pushback_member_function : std::false_type { };

template<typename _Container, typename _ValueType>
struct has_pushback_member_function<
    _Container,
    _ValueType,
    std::void_t<decltype(std::declval<_Container>().push_back(std::declval<_ValueType>()))>>
        : std::true_type { };

/// .push()

template<typename _Container, typename _ValueType, typename = std::void_t<>>
struct has_push_member_function : std::false_type { };

template<typename _Container, typename _ValueType>
struct has_push_member_function<
    _Container,
    _ValueType,
    std::void_t<decltype(std::declval<_Container>().push(std::declval<_ValueType>()))>>
        : std::true_type { };

/// .insert()

template<typename _Container, typename _ValueType, typename = std::void_t<>>
struct has_insert_member_function : std::false_type { };

template<typename _Container, typename _ValueType>
struct has_insert_member_function<
    _Container,
    _ValueType,
    std::void_t<decltype(std::declval<_Container>().insert(std::declval<_ValueType>()))>>
        : std::true_type { };

/// .insert(.end(), element)

template<typename _Container, typename _ValueType, typename = std::void_t<>>
struct has_insert_with_end_member_function : std::false_type { };

template<typename _Container, typename _ValueType>
struct has_insert_with_end_member_function<
    _Container,
    _ValueType,
    std::void_t<decltype(std::declval<_Container>().insert(std::declval<typename _Container::iterator>(), std::declval<_ValueType>()))>>
        : std::true_type { };

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
static constexpr inline bool is_insertable_container_v =
    has_container_value_type_v<_Container> && (
    has_append_member_function<_Container, _ValueType>::value ||
    has_pushback_member_function<_Container, _ValueType>::value ||
    has_push_member_function<_Container, _ValueType>::value ||
    has_insert_member_function<_Container, _ValueType>::value ||
    has_insert_with_end_member_function<_Container, _ValueType>::value ||
    has_pushfront_member_function<_Container, _ValueType>::value);


constexpr char tolower(char __c) noexcept
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

#if defined(_MSC_VER)
#   define _KWARGS_UINT128
#elif defined(__GNUC__) || defined(__clang__)
#   define _KWARGS_UINT128  __uint128_t
#endif

template<typename _Tp, typename _Mt>
constexpr _Tp fast_power(_Tp __fv, _Tp __sv, _Mt __mod) noexcept
{
    _Tp result = 1;
    
    for (__fv %= __mod; __sv; __fv = (_KWARGS_UINT128(__fv) * __fv) % __mod, __sv >>= 1)
    {
        if (__sv & 1)
        {
            result = (_KWARGS_UINT128(result) * __fv) % __mod;
        }
    }

    return result;
}


// Miller Rabin
template<typename _Tp>
constexpr bool is_prime(_Tp __n) noexcept
{
    constexpr std::array<_Tp, 25> test{
         2,  3,  5,  7, 11, 13, 17, 19, 23, 29,
        31, 37, 41, 43, 47, 53, 59, 61, 67, 71,
        73, 79, 83, 89, 97
    };

    if (__n == 1)
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
            nxt = (_KWARGS_UINT128(a) * a) % __n;

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

#undef _KWARGS_UINT128

}  // namespace detail


/**
* @brief Define KWARGSKEY_CASE_INSENSITIVE to enable case-insensitivity for KwargsKey.
*/
#ifndef KWARGSKEY_CASE_INSENSITIVE
#   define KWARGSKEY_TO_LOWERCASE(c) (c)
#else
#   define KWARGSKEY_TO_LOWERCASE(c) detail::tolower(c)
#endif

class KwargsKey
{
public:

    using value_type = unsigned long long int;

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
        { pushBack(__str[i]); }
    }

    constexpr explicit KwargsKey(value_type __option) noexcept
        : _M_key(__option)
    { }

    constexpr void pushBack(char __c) noexcept
    { _M_key = ((_M_key * base) % mod + KWARGSKEY_TO_LOWERCASE(__c)) % mod; }

    template<typename... _Args>
    constexpr void pushBack(char __c, _Args&&... __args) noexcept
    {
        pushBack(__c);
        pushBack(__args...);
    }

    static_assert(std::is_same<value_type, unsigned long long int>::value);

    static constexpr value_type base = 0X1C1ULL;
    static constexpr value_type mod  = 0X91F5BCB8BB0243ULL;

    static_assert(mod <= std::numeric_limits<value_type>::max() / base, "KwargsKey: The expression 'base * mod' overflows.");

    static_assert(detail::is_prime(base)
#if defined(_MSC_VER)
        || base > 0x100000000ULL
#endif  
        , "KwargsKey: 'base' must be a prime number.");

    static_assert(detail::is_prime(mod)
#if defined(_MSC_VER)
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

#undef KWARGSKEY_TO_LOWERCASE


[[nodiscard]]
constexpr KwargsKey operator""_opt(const char* const __str, std::size_t __size) noexcept
{ return KwargsKey(__str, __size); }

template<char... _String>
[[nodiscard]]
constexpr KwargsKey operator""_opt() noexcept
{ KwargsKey result; return result.pushBack(_String...), result; }

[[nodiscard]]
constexpr KwargsKey operator""_opt(char __ch) noexcept
{ KwargsKey result; return result.pushBack(__ch), result; }


#define KWARGSVALUE_INCORRECT_CONVERSION() \
    assert(false && "KwargsValue: Incorrect conversion.")


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

            _M_flags = ValueFlag;
        }
        else
        {
            _M_data._M_ptr = const_cast<detail::remove_reference_cv_t<decltype(__value)>*>(std::addressof(__value));

            _M_flags = PointerFlag;
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

            _M_flags = ValueFlag;
        }
        else
        {
            if constexpr (std::is_rvalue_reference_v<decltype(__value)>)
            {
                _M_data._M_ptr = new type(std::move(__value));
                _M_flags = AppliedFlag;
            }
            else if constexpr (std::is_lvalue_reference_v<decltype(__value)>)
            {
                _M_data._M_ptr = const_cast<std::add_pointer_t<detail::remove_reference_cv_t<decltype(__value)>>>(std::addressof(__value));

                _M_flags = PointerFlag;
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
        , _M_flags(StringLiteralFlag)
        , _M_size(_Size)
    {
        const char* ptr = __value;
        _M_data._M_ptr = const_cast<char*>(ptr);
    }
    
    _KWARGS_DESTRUCTOR_CONSTEXPR ~KwargsValue() noexcept
    {
        if (_M_flags == AppliedFlag)
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
        switch (_M_flags)
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
        switch (_M_flags)
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
        if (_M_flags == StringLiteralFlag)
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

        KWARGSVALUE_INCORRECT_CONVERSION();

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
        if (_M_flags == StringLiteralFlag)
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

        KWARGSVALUE_INCORRECT_CONVERSION();
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
        if (_M_flags == StringLiteralFlag)
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

        KWARGSVALUE_INCORRECT_CONVERSION();
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
        if (_M_flags == StringLiteralFlag)
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

            if (_M_flags == ValueFlag) [[likely]]
                return *reinterpret_cast<const _Tp*>(&_M_data._M_value);

            else [[unlikely]] // if (_M_flags == PointerFlag || _M_flags == AppliedFlag)
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
        if (_M_flags == StringLiteralFlag)
        {
            if (reference<const char*>() == nullptr)
                return static_cast<_Tp>(0);
            else if (detail::tolower(reference<const char*>()[0]) == 't')
                return static_cast<_Tp>(true);
            else if (detail::tolower(reference<const char*>()[0]) == 'f')
                return static_cast<_Tp>(false);

            char* endptr;

            if constexpr (std::is_signed_v<detail::enum_underlying_type_t<_Tp>>)
                return static_cast<_Tp>(
                    std::strtoll(reference<const char*>(), &endptr, 10));

            else if constexpr (std::is_unsigned_v<detail::enum_underlying_type_t<_Tp>>)
                return static_cast<_Tp>(
                    std::strtoull(reference<const char*>(), &endptr, 10));
        } else

        /// std::string -> integer
        if (typeid(std::string).hash_code() == hashCode)
        {
            if (detail::tolower((reference<std::string>()).front()) == 't')
                return static_cast<_Tp>(true);
            else if (detail::tolower((reference<std::string>()).front()) == 'f')
                return static_cast<_Tp>(false);

            try
            {
                if constexpr (std::is_signed_v<detail::enum_underlying_type_t<_Tp>>)
                    return static_cast<_Tp>(
                        std::stoll(reference<std::string>()));

                else if constexpr (std::is_unsigned_v<detail::enum_underlying_type_t<_Tp>>)
                    return static_cast<_Tp>(
                        std::stoull(reference<std::string>()));
            }
            catch (...)
            {
                KWARGSVALUE_INCORRECT_CONVERSION();
                return _Tp();
            }
        } else

        /// std::string_view -> integer
        if (typeid(std::string_view).hash_code() == hashCode)
        {
            char* endptr;

            const std::string_view* sv = pointer<std::string_view>();
            
            if (sv->empty())
            {
                return static_cast<_Tp>(0);
            }
            else if (detail::tolower(sv->front()) == 't')
            {
                return static_cast<_Tp>(true);
            }
            else if (detail::tolower(sv->front()) == 'f')
            {
                return static_cast<_Tp>(false);
            }
            else if (sv->back() == '\0')
            {
                if constexpr (std::is_signed_v<detail::enum_underlying_type_t<_Tp>>)
                    return static_cast<_Tp>(std::strtoll(sv->data(), &endptr, 10));

                else if constexpr (std::is_unsigned_v<detail::enum_underlying_type_t<_Tp>>)
                    return static_cast<_Tp>(std::strtoull(sv->data(), &endptr, 10));
            }
            else
            {
                if constexpr (std::is_signed_v<detail::enum_underlying_type_t<_Tp>>)
                    return static_cast<_Tp>(std::stoll(std::string(sv->data(), sv->size())));

                else if constexpr (std::is_unsigned_v<detail::enum_underlying_type_t<_Tp>>)
                    return static_cast<_Tp>(std::stoull(std::string(sv->data(), sv->size())));
            }
        }

        KWARGSVALUE_INCORRECT_CONVERSION();
        return _Tp();
    }

    template<typename _Tp>
    [[nodiscard]] constexpr auto value() const noexcept -> std::enable_if_t<std::is_floating_point_v<_Tp>, _Tp>
    {
        std::size_t hashCode = typeHashCode();

        if (typeid(detail::remove_reference_cv_t<_Tp>).hash_code() == hashCode)
        {
            // return reference<_Tp>();

            if (_M_flags == ValueFlag) [[likely]]
                return *reinterpret_cast<const _Tp*>(&_M_data._M_value);
            
            else [[unlikely]] // if (_M_flags == PointerFlag || _M_flags == AppliedFlag)
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
        if (_M_flags == StringLiteralFlag)
        {
            char* endptr;

            return static_cast<_Tp>(
                std::strtold(reference<const char*>(), &endptr));
        } else

        /// std::string -> floating point
        if (typeid(std::string).hash_code() == hashCode)
        {
            try
            {
                return static_cast<_Tp>(std::stold(reference<std::string>()));
            }
            catch (...)
            {
                KWARGSVALUE_INCORRECT_CONVERSION();
                return _Tp();
            }
        } else

        /// std::string_view -> floating point
        if (typeid(std::string_view).hash_code() == hashCode)
        {
            char* endptr;

            const std::string_view* sv = pointer<std::string_view>();
            
            if (sv->empty())
            {
                return 0;
            }
            else if (sv->back() == '\0')
            {
                return static_cast<_Tp>(std::strtold(sv->data(), &endptr));
            }
            else
            {
                return static_cast<_Tp>(std::stold(std::string(sv->data(), sv->size())));
            }
        }

        KWARGSVALUE_INCORRECT_CONVERSION();
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


        if (_M_flags == PointerFlag || _M_flags == AppliedFlag)
        {
            return *reinterpret_cast<_Tp*>(_M_data._M_ptr);
        }
        else if (_M_flags == StringLiteralFlag)
        {
            return _S_fromStringLiteral<_Tp>(reinterpret_cast<const char*>(_M_data._M_ptr));
        }
        else if (_M_flags == ValueFlag)
        {
            assert(_M_size <= sizeof(_M_data) && "KwargsValue: Incorrect conversion.");

            return *static_cast<const _Tp*>(reinterpret_cast<const void*>(&_M_data._M_value));
        }

        KWARGSVALUE_INCORRECT_CONVERSION();
        return _Tp();
    }

    [[nodiscard]]
    constexpr std::uint32_t size() const noexcept
    { return _M_size; }

    inline KwargsValue& operator=(const KwargsValue& __other)
    {
        if (_M_flags == AppliedFlag)
        {
            _M_manager(DoFree, _M_data._M_ptr, nullptr);
        }

        if (__other._M_flags == AppliedFlag)
            __other._M_manager(DoApplyAndCopy, __other._M_data._M_ptr, &_M_data._M_ptr);
        else
            _M_data._M_value = __other._M_data._M_value;

        _M_manager = __other._M_manager;
        _M_flags = __other._M_flags;
        _M_size = __other._M_size;

        return *this;
    }

    inline KwargsValue& operator=(KwargsValue&& __other) noexcept
    {
        if (_M_flags == AppliedFlag)
        {
            _M_manager(DoFree, _M_data._M_ptr, nullptr);
        }

        _M_data._M_value = __other._M_data._M_value;

        _M_flags = std::exchange(
            __other._M_flags,
            __other._M_flags == AppliedFlag ? PointerFlag : __other._M_flags);

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
            _S_insertToEnd(result, std::move(element));
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
            _S_insertToEnd(result, element.value<detail::container_value_type_t<_Tp>>());
        }

        return result;
    }

    template<
        typename _Container,
        typename _ValueType,
        std::enable_if_t<
            detail::has_append_member_function<_Container, _ValueType>::value, int> = 0>
    static constexpr inline
        void _S_insertToEnd(_Container& __container, _ValueType&& __element) noexcept
    { __container.append(std::forward<_ValueType>(__element)); }


    template<
        typename _Container,
        typename _ValueType,
        std::enable_if_t<
            detail::has_pushback_member_function<_Container, _ValueType>::value &&
        detail::not_v<detail::has_append_member_function<_Container, _ValueType>::value>, int> = 0>
    static constexpr inline
        void _S_insertToEnd(_Container& __container, _ValueType&& __element) noexcept
    { __container.push_back(std::forward<_ValueType>(__element)); }

    template<
        typename _Container,
        typename _ValueType,
        std::enable_if_t<
            detail::has_push_member_function<_Container, _ValueType>::value &&
        detail::not_v<detail::has_append_member_function<_Container, _ValueType>::value> &&
        detail::not_v<detail::has_pushback_member_function<_Container, _ValueType>::value>, int> = 0>
    static constexpr inline
        void _S_insertToEnd(_Container& __container, _ValueType&& __element) noexcept
    { __container.push(std::forward<_ValueType>(__element)); }

    template<
        typename _Container,
        typename _ValueType,
        std::enable_if_t<
            detail::has_insert_member_function<_Container, _ValueType>::value &&
        detail::not_v<detail::has_append_member_function<_Container, _ValueType>::value> &&
        detail::not_v<detail::has_pushback_member_function<_Container, _ValueType>::value> &&
        detail::not_v<detail::has_push_member_function<_Container, _ValueType>::value>, int> = 0>
    static constexpr inline
        void _S_insertToEnd(_Container& __container, _ValueType&& __element) noexcept
    { __container.insert(std::forward<_ValueType>(__element)); }

    template<
        typename _Container,
        typename _ValueType,
        std::enable_if_t<
            detail::has_insert_with_end_member_function<_Container, _ValueType>::value &&
        detail::not_v<detail::has_append_member_function<_Container, _ValueType>::value> &&
        detail::not_v<detail::has_pushback_member_function<_Container, _ValueType>::value> &&
        detail::not_v<detail::has_push_member_function<_Container, _ValueType>::value> &&
        detail::not_v<detail::has_insert_member_function<_Container, _ValueType>::value>, int> = 0>
    static constexpr inline
        void _S_insertToEnd(_Container& __container, _ValueType&& __element) noexcept
    { __container.insert(__container.end(), std::forward<_ValueType>(__element)); }

    template<
        typename _Container,
        typename _ValueType,
        std::enable_if_t<
            detail::has_pushfront_member_function<_Container, _ValueType>::value &&
        detail::not_v<detail::has_append_member_function<_Container, _ValueType>::value> &&
        detail::not_v<detail::has_pushback_member_function<_Container, _ValueType>::value> &&
        detail::not_v<detail::has_push_member_function<_Container, _ValueType>::value> &&
        detail::not_v<detail::has_insert_member_function<_Container, _ValueType>::value> &&
        detail::not_v<detail::has_insert_with_end_member_function<_Container, _ValueType>::value>, int> = 0>
    static constexpr inline
        void _S_insertToEnd(_Container& __container, _ValueType&& __element) noexcept
    { __container.push_front(std::forward<_ValueType>(__element)); }

private:
    
    union Data
    {
        void* _M_ptr = nullptr;
        
        std::uintptr_t _M_value;

        std::byte _M_bytes[sizeof(void*)];

    } _M_data;

    void (*_M_manager)(WorkFlags, void*, void*) = &_S_manage<int>;

    enum Flags
    {
        PointerFlag = 0,
        StringLiteralFlag,
        ValueFlag,
        AppliedFlag
    };

    Flags _M_flags = ValueFlag;

    std::uint32_t _M_size = 0;
};


#undef KWARGSVALUE_INCORRECT_CONVERSION


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
        assert([&]() -> bool
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

using kwargs::operator""_opt;


#endif  // CPP_KWARGS_H


//  _                           _                                         _           //
// ( )                         ( )                                       (_ )         //
// | |__   _   _    _ _   ___  | |__   _   _    _ _   ___     _     ___   | |  _   _  //
// |  _ `\( ) ( ) /'_` )/' _ `\|  _ `\( ) ( ) /'_` )/' _ `\ /'_`\ /' _ `\ | | ( ) ( ) //
// | | | || (_) |( (_| || ( ) || | | || (_) |( (_| || ( ) |( (_) )| ( ) | | | | (_) | //
// (_) (_)`\___/'`\__,_)(_) (_)(_) (_)`\___/'`\__,_)(_) (_)`\___/'(_) (_)(___)`\__, | //
// https://github.com/huanhuanonly                                            ( )_| | //
//                                                                            `\___/' //