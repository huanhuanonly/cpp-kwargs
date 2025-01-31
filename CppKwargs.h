/**
* Copyright (c) 2024-2025 Yang Huanhuan (3347484963@qq.com).
* 
* Before using this file, please read its license:
*
*     https://github.com/huanhuanonly/cpp-kwargs/blob/main/LICENSE
*
* This software is provided "as is", without warranty of any kind, express or implied.
* 
* The author reserves the right to modify the license of this file at any time.
*/

/**
* CppKwargs.h In the cpp-kwargs (https://github.com/huanhuanonly/cpp-kwargs)
* 
* Created by Yang Huanhuan on December 29, 2024, 14:40:45
* 
* --- This file is the main header for cpp-kwargs ---
* 
* @brief Implement Python's **kwargs style parameter passing in C++.
* 
* This file consists of the following parts:
*
* @class KwargsKey
* @fn    operator""_opt
* @class KwargsValue
* @class Args
* @class Kwargs
*/

#pragma once

#ifndef CPP_KWARGS_H
#define CPP_KWARGS_H

#include <type_traits>
#include <typeinfo>

#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <set>

#include <stdexcept>

#include <cassert>
#include <cstdlib>

/**
* @brief Define KWARGSKEY_CASE_INSENSITIVE to enable case-insensitivity for KwargsKey.
*/
#ifndef KWARGSKEY_CASE_INSENSITIVE
#   define KWARGSKEY_TO_LOWER_CASE(c) (c)
#else
#   define KWARGSKEY_TO_LOWER_CASE(c) _S_tolower(c)
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
        { pushBack(__str[i]); }
    }

    constexpr explicit KwargsKey(value_type __option) noexcept
        : _M_key(__option)
    { }

    constexpr void pushBack(char __c) noexcept
    { _M_key = ((_M_key * base) % mod + KWARGSKEY_TO_LOWER_CASE(__c)) % mod; }

    template<typename... _Args>
    constexpr void pushBack(char __c, _Args&&... __args) noexcept
    {
        pushBack(__c);
        pushBack(__args...);
    }

    static constexpr value_type base = 0X1C1ULL;
    static constexpr value_type mod  = 0X91F5BCB8BB0243ULL;

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

public:

    static constexpr char _S_tolower(char __c) noexcept
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

private:

    value_type _M_key = 0;
};


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


class KwargsValue
{
protected:

    enum WorkFlags : int
    {
        DoApplyAndCopy,           /// type*     , type**
        DoCopy,                   /// type*     , type*
        DoFree,                   /// type*     , [unused]
        DoGetTypeHash,            /// [unused]  , std::size_t*
        DoGetValueTypeHash,       /// [unused]  , std::size_t*
        DoGetBeginIterator,       /// this      , iterator**
        DoIterate,                /// std::pair<this, iterator**>, value_type*
        DoIterateAny,             /// std::pair<this, iterator**>, KwargsValue*

        DoCheckInt  = 0b0001'000,  /// [unused], int*
        DoCheckReal = 0b0010'000,  /// [unused], int*
        DoCheckSign = 0b0100'000,  /// [unused], int*
        DoCheckEnum = 0b1000'000,

        DoCheckIterable  = 0b001'0000'000, /// [unused], int*
        DoCheckValueType = 0b010'0000'000, /// [unused], int*
        DoCheckStdArray  = 0b100'0000'000  /// [unused], int*
    };

    [[nodiscard]]
    friend static constexpr WorkFlags operator|(WorkFlags __first, WorkFlags __second) noexcept
    { return static_cast<WorkFlags>(static_cast<int>(__first) | static_cast<int>(__second)); }

    template<typename _Tp>
    static void _S_manage(WorkFlags __work, void* __inData, void* __outData)
    {
        using type = _S_enumBaseTypeOf_t<std::remove_cv_t<std::remove_reference_t<_Tp>>>;

        switch (__work)
        {
            case DoApplyAndCopy:
            {
                type*  iptr = reinterpret_cast<type*>(__inData);
                type** optr = reinterpret_cast<type**>(__outData);

                *optr = new type(*iptr);
                break;
            }

            case DoCopy:
            {
                type* iptr = reinterpret_cast<type*>(__inData);
                type* optr = reinterpret_cast<type*>(__outData);

                *optr = *iptr;
                break;
            }

            case DoFree:
            {
                delete reinterpret_cast<type*>(__inData);
                break;
            }

            case DoGetTypeHash:
            {
                *reinterpret_cast<std::size_t*>(__outData) = typeid(type).hash_code();
                break;
            }

            case DoGetValueTypeHash:
            {
                *reinterpret_cast<std::size_t*>(__outData) = typeid(_S_valueTypeOf_t<type>).hash_code();
                break;
            }

            case DoGetBeginIterator:
            {
                KwargsValue* iptr = reinterpret_cast<KwargsValue*>(__inData);
                _S_iterator_t<type>** optr = reinterpret_cast<_S_iterator_t<type>**>(__outData);

                *optr = new _S_iterator_t<type>(iptr->_M_getBeginIterator<type>());
                break;
            }

            case DoIterate:
            {
                if constexpr (_S_hasValueType_v<type>)
                {
                    auto iptr = reinterpret_cast<std::pair<KwargsValue*, _S_iterator_t<type>**>*>(__inData);
                    auto optr = reinterpret_cast<_S_valueTypeOf_t<type>*>(__outData);

                    if (**iptr->second == iptr->first->_M_getEndIterator<type>())
                    {
                        delete *iptr->second;
                        *iptr->second = nullptr;
                    }
                    else
                    {
                        *optr = static_cast<_S_valueTypeOf_t<type>>(***iptr->second);
                        ++**iptr->second;
                    }
                }

                break;
            }

            case DoIterateAny:
            {
                auto iptr = reinterpret_cast<std::pair<KwargsValue*, _S_iterator_t<type>**>*>(__inData);
                auto optr = reinterpret_cast<KwargsValue*>(__outData);

                if (**iptr->second == iptr->first->_M_getEndIterator<type>())
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

                if ((__work & DoCheckInt) && _S_isIntegral_v<type>)
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

                if ((__work & DoCheckEnum) && std::is_enum_v<std::remove_cv_t<std::remove_reference_t<_Tp>>>)
                {
                    result |= DoCheckEnum;
                }

                
                if ((__work & DoCheckIterable) && _S_isIterable_v<type>)
                {
                    result |= DoCheckIterable;
                }
                
                if ((__work & DoCheckValueType) && _S_hasValueType_v<type>)
                {
                    result |= DoCheckValueType;
                }

                if ((__work & DoCheckStdArray) && _S_isStdArray_v<type>)
                {
                    result |= DoCheckStdArray;
                }

                *reinterpret_cast<int*>(__outData) = result;
                break;
            }
        }
    }

    template<typename _Tp, typename... _Types>
    static constexpr inline bool _S_isAnyOf_v = (std::is_same_v<_Tp, _Types> || ...);


    template<typename _Tp>
    static constexpr inline bool _S_isIntegral_v = std::is_integral_v<_Tp> || std::is_enum_v<_Tp>;


    template<typename _Tp, typename = std::void_t<>>
    struct _S_enumBaseTypeOf
    { using type = _Tp; };
    
    template<typename _Tp>
    struct _S_enumBaseTypeOf<_Tp, std::enable_if_t<std::is_enum_v<_Tp>>>
    { using type = std::underlying_type_t<_Tp>; };
    
    template<typename _Tp>
    using _S_enumBaseTypeOf_t = typename _S_enumBaseTypeOf<_Tp>::type;


    template<typename _Tp>
    static constexpr inline bool _S_useValueFlag_v =
        (_S_isIntegral_v<_Tp> ||
        std::is_pointer_v<_Tp> ||
        std::is_floating_point_v<_Tp>) &&
        sizeof(_Tp) <= sizeof(void*);


    template<typename _Tp, typename = std::void_t<>>
    struct _S_valueTypeOf
    { using type = _Tp; };

    template<typename _Tp>
    struct _S_valueTypeOf<_Tp, std::void_t<typename std::remove_reference_t<_Tp>::value_type>>
    { using type =  typename _Tp::value_type; };

    template<typename _Tp>
    using _S_valueTypeOf_t = typename _S_valueTypeOf<_Tp>::type;


    template<typename _Tp, typename = std::void_t<>>
    struct _S_hasValueType : std::false_type { };

    template<typename _Tp>
    struct _S_hasValueType<_Tp, std::void_t<typename std::remove_reference_t<_Tp>::value_type>>
        : std::true_type { };

    template<typename _Tp>
    static constexpr inline bool _S_hasValueType_v = _S_hasValueType<_Tp>::value;


    template<typename _Tp>
    static constexpr inline bool _S_isStdArray_v =
        std::is_same_v<
            _Tp, std::array<_S_valueTypeOf_t<_Tp>,
            sizeof(_Tp) / sizeof(_S_valueTypeOf_t<_Tp>)>>;

    
    template<typename _Tp, typename = std::void_t<>>
    struct _S_isIterable : std::false_type { };

    template<typename _Tp>
    struct _S_isIterable<
        _Tp,
        std::void_t<
            decltype(std::declval<_Tp>().begin()),
            decltype(std::declval<_Tp>().end())>> : std::true_type
    { };

    template<typename _Tp>
    static constexpr inline bool _S_isIterable_v = _S_isIterable<_Tp>::value;

    
    template<typename _Tp, typename = std::void_t<>>
    struct _S_iterator { using type = std::byte*; };

    template<typename _Tp>
    struct _S_iterator<_Tp, std::void_t<typename _Tp::iterator>>
    { using type = typename _Tp::iterator; };

    template<typename _Tp>
    using _S_iterator_t = _S_iterator<_Tp>::type;

    template<typename _Tp>
    [[nodiscard]] constexpr inline
        typename std::enable_if_t<_S_isIterable_v<_Tp>, typename _Tp::iterator>
            _M_getBeginIterator() const noexcept
    { return reinterpret_cast<_Tp*>(_M_data._M_ptr)->begin(); }

    template<typename _Tp>
    [[nodiscard]] constexpr inline
        typename typename std::enable_if_t<not _S_isIterable_v<_Tp>, typename std::byte*>
            _M_getBeginIterator() const noexcept
    { return nullptr; }

    template<typename _Tp>
    [[nodiscard]] constexpr inline
        typename std::enable_if_t<_S_isIterable_v<_Tp>, typename _Tp::iterator>
            _M_getEndIterator() const noexcept
    { return reinterpret_cast<_Tp*>(_M_data._M_ptr)->end(); }

    template<typename _Tp>
    [[nodiscard]] constexpr inline
        typename typename std::enable_if_t<not _S_isIterable_v<_Tp>, typename std::byte*>
            _M_getEndIterator() const noexcept
    { return nullptr; }

public:

    constexpr KwargsValue() = default;

    template<typename _Tp>
    constexpr KwargsValue(const _Tp& __value) noexcept
        : _M_manager(&KwargsValue::_S_manage<_Tp>)
    {
        using type = std::remove_reference_t<_Tp>;

        if constexpr (_S_useValueFlag_v<type>)
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
            _M_data._M_ptr = const_cast<
                std::remove_cv_t<
                    std::remove_reference_t<decltype(__value)>>*>(std::addressof(__value));

            _M_flags = PointerFlag;
        }

        _M_size = sizeof(_Tp);
    }
    
    template<typename _Tp>
    inline KwargsValue(_Tp&& __value)
        : _M_manager(&KwargsValue::_S_manage<_Tp>)
    {
        using type = std::remove_reference_t<_Tp>;

        if constexpr (_S_useValueFlag_v<type>)
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
                _M_data._M_ptr = const_cast<
                std::remove_cv_t<
                    std::remove_reference_t<decltype(__value)>>*>(std::addressof(__value));

                _M_flags = PointerFlag;
            }
            else
            {
                assert(false);
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

    constexpr ~KwargsValue() noexcept(noexcept(delete _M_data._M_ptr))
    {
        if (_M_flags == AppliedFlag)
        {
            _M_manager(DoFree, _M_data._M_ptr, nullptr);
        }
    }


    [[nodiscard]]
    constexpr std::size_t typeHashCode() const noexcept
    {
        std::size_t hashCode;
        _M_manager(DoGetTypeHash, nullptr, &hashCode);

        return hashCode;
    }

    /**
     * @return If my type is std::vector<int>, then return
     *         the hash code for int.
     */
    [[nodiscard]]
    constexpr std::size_t valueTypeHashCode() const noexcept
    {
        std::size_t hashCode;
        _M_manager(DoGetValueTypeHash, nullptr, &hashCode);

        return hashCode;
    }

    [[nodiscard]]
    constexpr bool hasValueType() const noexcept
    {
        int result;
        _M_manager(DoCheckValueType, nullptr, &result);

        return result;
    }

    template<typename _Tp>
    [[nodiscard]]
    constexpr bool isSameType() const noexcept
    { return typeid(_S_enumBaseTypeOf_t<std::remove_cv_t<std::remove_reference_t<_Tp>>>).hash_code() == typeHashCode(); }

    [[nodiscard]]
    constexpr bool isInteger() const noexcept
    {
        int result;
        _M_manager(DoCheckInt, nullptr, &result);

        return result;
    }

    [[nodiscard]]
    constexpr bool isRealNumber() const noexcept
    {
        int result;
        _M_manager(DoCheckReal, nullptr, &result);

        return result;
    }

    [[nodiscard]]
    constexpr bool isStdArray() const noexcept
    {
        int result;
        _M_manager(DoCheckStdArray, nullptr, &result);

        return result;
    }

    [[nodiscard]]
    constexpr bool isIterable() const noexcept
    {
        int result;
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
    [[nodiscard]]
    constexpr std::enable_if_t<
            std::is_same_v<std::remove_cv_t<_Tp>, const char*>, _Tp>
        value() const noexcept
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

        assert(false && "Incorrect conversion.");

        static const char* nullstr = "";
        return nullstr;
    }

    template<typename _Tp>
    [[nodiscard]]
    constexpr std::enable_if_t<
            std::is_same_v<std::remove_cv_t<_Tp>, std::string>, _Tp>
        value() const noexcept
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

        assert(false && "Incorrect conversion.");
        return std::string();
    }

    template<typename _Tp>
    [[nodiscard]]
    constexpr std::enable_if_t<
            std::is_same_v<std::remove_cv_t<_Tp>, std::string_view>, _Tp>
        value() const noexcept
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

        assert(false && "Incorrect conversion.");
        return std::string_view();
    }

    template<typename _Tp>
    [[nodiscard]]
    constexpr std::enable_if_t<
            _S_isAnyOf_v<std::remove_cv_t<_Tp>, char, signed char, unsigned char>, _Tp>
        value() const noexcept
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
    [[nodiscard]]
    constexpr std::enable_if_t<
            _S_isIntegral_v<std::remove_cv_t<_Tp>> &&
                !(_S_isAnyOf_v<std::remove_cv_t<_Tp>, char, signed char, unsigned char>), _Tp>
        value() const noexcept
    {
        std::size_t hashCode = typeHashCode();

        if (typeid(_S_enumBaseTypeOf_t<std::remove_cv_t<std::remove_reference_t<_Tp>>>).hash_code() == hashCode)
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
            else if (KwargsKey::_S_tolower(reference<const char*>()[0]) == 't')
                return static_cast<_Tp>(true);
            else if (KwargsKey::_S_tolower(reference<const char*>()[0]) == 'f')
                return static_cast<_Tp>(false);

            char* endptr;

            if constexpr (std::is_signed_v<_S_enumBaseTypeOf_t<_Tp>>)
                return static_cast<_Tp>(
                    std::strtoll(reference<const char*>(), &endptr, 10));

            else if constexpr (std::is_unsigned_v<_S_enumBaseTypeOf_t<_Tp>>)
                return static_cast<_Tp>(
                    std::strtoull(reference<const char*>(), &endptr, 10));
        } else

        /// std::string -> integer
        if (typeid(std::string).hash_code() == hashCode)
        {
            if (KwargsKey::_S_tolower((reference<std::string>()).front()) == 't')
                return static_cast<_Tp>(true);
            else if (KwargsKey::_S_tolower((reference<std::string>()).front()) == 'f')
                return static_cast<_Tp>(false);

            try
            {
                if constexpr (std::is_signed_v<_S_enumBaseTypeOf_t<_Tp>>)
                    return static_cast<_Tp>(
                        std::stoll(reference<std::string>()));

                else if constexpr (std::is_unsigned_v<_S_enumBaseTypeOf_t<_Tp>>)
                    return static_cast<_Tp>(
                        std::stoull(reference<std::string>()));
            }
            catch (...)
            {
                assert(false && "Incorrect conversion.");
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
            else if (KwargsKey::_S_tolower(sv->front()) == 't')
            {
                return static_cast<_Tp>(true);
            }
            else if (KwargsKey::_S_tolower(sv->front()) == 'f')
            {
                return static_cast<_Tp>(false);
            }
            else if (sv->back() == '\0')
            {
                if constexpr (std::is_signed_v<_S_enumBaseTypeOf_t<_Tp>>)
                    return static_cast<_Tp>(std::strtoll(sv->data(), &endptr, 10));

                else if constexpr (std::is_unsigned_v<_S_enumBaseTypeOf_t<_Tp>>)
                    return static_cast<_Tp>(std::strtoull(sv->data(), &endptr, 10));
            }
            else
            {
                if constexpr (std::is_signed_v<_S_enumBaseTypeOf_t<_Tp>>)
                    return static_cast<_Tp>(std::stoll(std::string(sv->data(), sv->size())));

                else if constexpr (std::is_unsigned_v<_S_enumBaseTypeOf_t<_Tp>>)
                    return static_cast<_Tp>(std::stoull(std::string(sv->data(), sv->size())));
            }
        }

        assert(false && "Incorrect conversion.");
        return _Tp();
    }

    template<typename _Tp>
    [[nodiscard]]
    constexpr std::enable_if_t<std::is_floating_point_v<std::remove_cv_t<_Tp>>, _Tp>
        value() const noexcept
    {
        std::size_t hashCode = typeHashCode();

        if (typeid(std::remove_cv_t<std::remove_reference_t<_Tp>>).hash_code() == hashCode)
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
                assert(false && "Incorrect conversion.");
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

        assert(false && "Incorrect conversion.");
        return _Tp();
    }

    template<typename _Tp>
    [[nodiscard]]
    constexpr std::enable_if_t<
            (std::is_class_v<_Tp> || std::is_union_v<_Tp> || std::is_pointer_v<_Tp>) &&
                !(_S_isAnyOf_v<std::remove_cv_t<_Tp>, const char*, std::string, std::string_view>), _Tp>
        value() const noexcept
    {
        if constexpr (_S_hasValueType_v<_Tp> && _S_isInsertableContainer_v<_Tp>)
        {
            if (not isSameType<_Tp>() && hasValueType() && isIterable())
            {
                if (typeid(_S_valueTypeOf_t<std::remove_cv_t<std::remove_reference_t<_Tp>>>).hash_code() == valueTypeHashCode())
                {
                    return _M_iterateAndCopy<std::remove_cv_t<std::remove_reference_t<_Tp>>>();
                }
                else
                {
                    return _M_iterateAnyAndCopy<std::remove_cv_t<std::remove_reference_t<_Tp>>>();
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
            assert(_M_size <= sizeof(_M_data) && "Incorrect conversion.");

            return *static_cast<const _Tp*>(reinterpret_cast<const void*>(&_M_data._M_value));
        }

        assert(false && "Incorrect conversion.");
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
    { return _Tp(); }

    template<typename _Tp>
    [[nodiscard]]
    constexpr _Tp _M_iterateAndCopy() const noexcept
    {
        void* iterator;
        _M_manager(DoGetBeginIterator, const_cast<void*>(reinterpret_cast<const void*>(this)), &iterator);

        _Tp result{};
        std::pair<const KwargsValue*, void**> param;

        _S_valueTypeOf_t<_Tp> element;
        _M_manager(DoIterate, &(param = std::make_pair(this, &iterator)), &element);


        for (; iterator; _M_manager(DoIterate, &(param = std::make_pair(this, &iterator)), &element))
        {
            _S_insertToEnd(result, std::move(element));
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
            _S_insertToEnd(result, element.value<_S_valueTypeOf_t<_Tp>>());
        }

        return result;
    }

    /// .append()

    template<typename _Container, typename _ValueType, typename = std::void_t<>>
    struct _S_hasAppendMFunction : std::false_type { };

    template<typename _Container, typename _ValueType>
    struct _S_hasAppendMFunction<
        _Container,
        _ValueType,
        std::void_t<decltype(std::declval<_Container>().append(std::declval<_ValueType>()))>>
            : std::true_type { };

    /// .push_back()

    template<typename _Container, typename _ValueType, typename = std::void_t<>>
    struct _S_hasPushBackMFunction : std::false_type { };

    template<typename _Container, typename _ValueType>
    struct _S_hasPushBackMFunction<
        _Container,
        _ValueType,
        std::void_t<decltype(std::declval<_Container>().push_back(std::declval<_ValueType>()))>>
            : std::true_type { };

    /// .push()

    template<typename _Container, typename _ValueType, typename = std::void_t<>>
    struct _S_hasPushMFunction : std::false_type { };

    template<typename _Container, typename _ValueType>
    struct _S_hasPushMFunction<
        _Container,
        _ValueType,
        std::void_t<decltype(std::declval<_Container>().push(std::declval<_ValueType>()))>>
            : std::true_type { };

    /// .insert()

    template<typename _Container, typename _ValueType, typename = std::void_t<>>
    struct _S_hasInsertMFunction : std::false_type { };

    template<typename _Container, typename _ValueType>
    struct _S_hasInsertMFunction<
        _Container,
        _ValueType,
        std::void_t<decltype(std::declval<_Container>().insert(std::declval<_ValueType>()))>>
            : std::true_type { };

    /// .insert(.end(), element)

    template<typename _Container, typename _ValueType, typename = std::void_t<>>
    struct _S_hasInsertWithEndMFunction : std::false_type { };

    template<typename _Container, typename _ValueType>
    struct _S_hasInsertWithEndMFunction<
        _Container,
        _ValueType,
        std::void_t<decltype(std::declval<_Container>().insert(std::declval<typename _Container::iterator>(), std::declval<_ValueType>()))>>
            : std::true_type { };

    /// .push_front()

    template<typename _Container, typename _ValueType, typename = std::void_t<>>
    struct _S_hasPushFrontMFunction : std::false_type { };

    template<typename _Container, typename _ValueType>
    struct _S_hasPushFrontMFunction<
        _Container,
        _ValueType,
        std::void_t<decltype(std::declval<_Container>().push_front(std::declval<_ValueType>()))>>
            : std::true_type { };

    template<typename _Container, typename _ValueType = _S_valueTypeOf_t<_Container>>
    static constexpr inline bool _S_isInsertableContainer_v =
        _S_hasValueType_v<_Container> && (
        _S_hasAppendMFunction<_Container, _ValueType>::value ||
        _S_hasPushBackMFunction<_Container, _ValueType>::value ||
        _S_hasPushMFunction<_Container, _ValueType>::value ||
        _S_hasInsertMFunction<_Container, _ValueType>::value ||
        _S_hasInsertWithEndMFunction<_Container, _ValueType>::value ||
        _S_hasPushFrontMFunction<_Container, _ValueType>::value);

    template<
        typename _Container,
        typename _ValueType,
        std::enable_if_t<
            _S_hasAppendMFunction<_Container, _ValueType>::value, int> = 0>
    static constexpr inline
        void _S_insertToEnd(_Container& __container, _ValueType&& __element) noexcept
    { __container.append(std::forward<_ValueType>(__element)); }


    template<
        typename _Container,
        typename _ValueType,
        std::enable_if_t<
            _S_hasPushBackMFunction<_Container, _ValueType>::value &&
        not _S_hasAppendMFunction<_Container, _ValueType>::value, int> = 0>
    static constexpr inline
        void _S_insertToEnd(_Container& __container, _ValueType&& __element) noexcept
    { __container.push_back(std::forward<_ValueType>(__element)); }

    template<
        typename _Container,
        typename _ValueType,
        std::enable_if_t<
            _S_hasPushMFunction<_Container, _ValueType>::value &&
        not _S_hasAppendMFunction<_Container, _ValueType>::value &&
        not _S_hasPushBackMFunction<_Container, _ValueType>::value, int> = 0>
    static constexpr inline
        void _S_insertToEnd(_Container& __container, _ValueType&& __element) noexcept
    { __container.push(std::forward<_ValueType>(__element)); }

    template<
        typename _Container,
        typename _ValueType,
        std::enable_if_t<
            _S_hasInsertMFunction<_Container, _ValueType>::value &&
        not _S_hasAppendMFunction<_Container, _ValueType>::value &&
        not _S_hasPushBackMFunction<_Container, _ValueType>::value &&
        not _S_hasPushMFunction<_Container, _ValueType>::value, int> = 0>
    static constexpr inline
        void _S_insertToEnd(_Container& __container, _ValueType&& __element) noexcept
    { __container.insert(std::forward<_ValueType>(__element)); }

    template<
        typename _Container,
        typename _ValueType,
        std::enable_if_t<
            _S_hasInsertWithEndMFunction<_Container, _ValueType>::value &&
        not _S_hasAppendMFunction<_Container, _ValueType>::value &&
        not _S_hasPushBackMFunction<_Container, _ValueType>::value &&
        not _S_hasPushMFunction<_Container, _ValueType>::value &&
        not _S_hasInsertMFunction<_Container, _ValueType>::value, int> = 0>
    static constexpr inline
        void _S_insertToEnd(_Container& __container, _ValueType&& __element) noexcept
    { __container.insert(__container.end(), std::forward<_ValueType>(__element)); }

    template<
        typename _Container,
        typename _ValueType,
        std::enable_if_t<
            _S_hasPushFrontMFunction<_Container, _ValueType>::value &&
        not _S_hasAppendMFunction<_Container, _ValueType>::value &&
        not _S_hasPushBackMFunction<_Container, _ValueType>::value &&
        not _S_hasPushMFunction<_Container, _ValueType>::value &&
        not _S_hasInsertMFunction<_Container, _ValueType>::value &&
        not _S_hasInsertWithEndMFunction<_Container, _ValueType>::value, int> = 0>
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
                    assert(__index < sizeof...(_Args));
                    *optr = &(*iptr)[__index];
                }
                else
                {
                    assert(-__index <= sizeof...(_Args));
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

    constexpr ~Args() noexcept
    {
        if (_M_data)
        {
            _M_manager(DoFree, _M_data, 0, nullptr);
        }
    }

    [[nodiscard]]
    constexpr std::size_t size() const noexcept
    {
        std::size_t result;
        _M_manager(DoGetSize, nullptr, 0, &result);
        return result;
    }

    [[nodiscard]]
    constexpr KwargsValue& operator[](int __i) noexcept
    {
        KwargsValue* result;
        _M_manager(DoFind, _M_data, __i, &result);
        return *result;
    }

    [[nodiscard]]
    constexpr const KwargsValue& operator[](int __i) const noexcept
    {
        KwargsValue* result;
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

#endif  // CPP_KWARGS_H