/**
* Copyright 2024 Yang Huanhuan (3347484963@qq.com). All rights reserved.
* 
* This software is provided "as is", without warranty of any kind, express or implied.
*/

/**
* CppKwargs.h In the cpp-kwargs (https://github.com/huanhuanonly/cpp-kwargs)
* 
* Created by Yang Huanhuan on December 29, 2024, 14:40:45
* 
* --- This file is the main header for cpp-kwargs ---
* 
* @brief Implement Python's kwargs in C++ and extend it.
* 
* @class Kwargs
* @class KwargsKey
* @class KwargsValue
* 
* @fn    operator""_opt
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

#include <stdexcept>

#include <cassert>
#include <cstdlib>

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
    { _M_key = ((_M_key * base) % mod + __c) % mod; }

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

        DoCheckInt  = 0b001'000,  /// [unused], int*
        DoCheckReal = 0b010'000,  /// [unused], int*
        DoCheckSign = 0b100'000,  /// [unused], int*

        DoCheckIterable  = 0b001000'000, /// [unused], int*
        DoCheckValueType = 0b010000'000, /// [unused], int*
        DoCheckStdArray  = 0b100000'000  /// [unused], int*
    };

    [[nodiscard]]
    friend static constexpr WorkFlags operator|(WorkFlags __first, WorkFlags __second) noexcept
    { return static_cast<WorkFlags>(static_cast<int>(__first) | static_cast<int>(__second)); }

    template<typename _Tp>
    static void _S_manage(WorkFlags __work, void* __inData, void* __outData)
    {
        using type = std::remove_reference_t<_Tp>;

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
                        *optr = ***iptr->second;
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

                if ((__work & DoCheckInt) && std::is_integral_v<type>)
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

    template<typename _Tp>
    static constexpr inline bool _S_useValueFlag_v =
        std::is_integral_v<_Tp> ||
        std::is_pointer_v<_Tp> ||
        std::is_floating_point_v<_Tp> &&
        sizeof(_Tp) <= sizeof(void*);

    template<typename _Tp, typename = std::void_t<>>
    struct _S_valueTypeOf
    { using type = _Tp; };

    template<typename _Tp>
    struct _S_valueTypeOf<_Tp, std::void_t<typename _Tp::value_type>>
    { using type =  typename _Tp::value_type; };

    template<typename _Tp>
    using _S_valueTypeOf_t = _S_valueTypeOf<_Tp>::type;


    template<typename _Tp, typename = std::void_t<>>
    struct _S_hasValueType : std::false_type { };

    template<typename _Tp>
    struct _S_hasValueType<_Tp, std::void_t<typename _Tp::value_type>>
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
    constexpr inline [[nodiscard]]
        typename std::enable_if_t<_S_isIterable_v<_Tp>, typename _Tp::iterator>
            _M_getBeginIterator() const noexcept
    { return reinterpret_cast<_Tp*>(_M_data._M_ptr)->begin(); }

    template<typename _Tp>
    constexpr inline [[nodiscard]]
        typename typename std::enable_if_t<not _S_isIterable_v<_Tp>, typename std::byte*>
            _M_getBeginIterator() const noexcept
    { return nullptr; }

    template<typename _Tp>
    constexpr inline [[nodiscard]]
        typename std::enable_if_t<_S_isIterable_v<_Tp>, typename _Tp::iterator>
            _M_getEndIterator() const noexcept
    { return reinterpret_cast<_Tp*>(_M_data._M_ptr)->end(); }

    template<typename _Tp>
    constexpr inline [[nodiscard]]
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
    { return typeid(_Tp).hash_code() == typeHashCode(); }

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
    constexpr operator _Tp() const noexcept
    { return value<_Tp>(); }

    template<typename _Tp>
    [[nodiscard]]
    constexpr std::enable_if_t<
            std::is_same_v<std::remove_volatile_t<_Tp>, const char*>, _Tp>
        value() const noexcept
    {
        if (_M_flags == StringLiteralFlag)
        {
            return reinterpret_cast<const char*>(_M_data._M_ptr);
        }
        
        std::size_t hashCode = typeHashCode();

        /// std::string -> const char*
        if (typeid(std::string).hash_code() == hashCode)
        {
            return reinterpret_cast<std::string*>(_M_data._M_ptr)->c_str();
        } else

        /// std::string_view -> const char*
        if (typeid(std::string_view).hash_code() == hashCode)
        {
            return reinterpret_cast<std::string_view*>(_M_data._M_ptr)->data();
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
            return reinterpret_cast<std::vector<char>*>(_M_data._M_ptr)->data();
        } else

        /// std::array<char, ...> -> const char*
        if (hashCode = valueTypeHashCode();
            isStdArray() &&
                (typeid(char).hash_code() == hashCode ||
                    typeid(signed char).hash_code() == hashCode ||
                    typeid(unsigned char).hash_code() == hashCode))
        {
            return reinterpret_cast<std::array<char, 1>*>(_M_data._M_ptr)->data();
        }

        assert(false && "Incorrect conversion.");

        const char* nullstr = "";
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
            return *reinterpret_cast<_Tp*>(_M_data._M_ptr);
        } else

        /// const char* -> std::string
        if (_M_flags == StringLiteralFlag)
        {
            return std::string(reinterpret_cast<const char*>(_M_data._M_ptr), _M_size);
        } else

        /// std::string_view -> std::string
        if (isSameType<std::string_view>())
        {
            std::string_view* ptr = reinterpret_cast<std::string_view*>(_M_data._M_ptr);
            return std::string(ptr->data(), ptr->size());
        } else

        /// Integer or floating point -> std::string
        if (int is; _M_manager(DoCheckInt | DoCheckReal | DoCheckSign, nullptr, &is), is)
        {
            if (_M_flags == ValueFlag)
            {
                if (is & DoCheckInt)
                {
                    if (_M_size == sizeof(char))
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
                        if (_M_size == sizeof(std::int16_t))
                            return std::to_string(*reinterpret_cast<const std::int16_t*>(&_M_data._M_value));
                        else if (_M_size == sizeof(std::int32_t))
                            return std::to_string(*reinterpret_cast<const std::int32_t*>(&_M_data._M_value));
                        else if (_M_size == sizeof(std::int64_t))
                            return std::to_string(*reinterpret_cast<const std::int64_t*>(&_M_data._M_value));
                    }
                    else
                    {
                        return std::to_string(*reinterpret_cast<const std::uint64_t*>(&_M_data._M_value));
                    }
                }
                else
                {
                    if (_M_size == sizeof(float))
                        return std::to_string(*reinterpret_cast<const float*>(&_M_data._M_value));
                    else if (_M_size == sizeof(double))
                        return std::to_string(*reinterpret_cast<const double*>(&_M_data._M_value));
                }
            }
            /// Types longer than 8 bytes
            else // if (_M_flag == PointerFlag || _M_flag == AppliedFlag)
            {
                /// @todo int128 or uint128.
                if constexpr (sizeof(long double) > sizeof(void*))
                    return std::to_string(*reinterpret_cast<long double*>(_M_data._M_ptr));
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
            return *reinterpret_cast<std::string_view*>(_M_data._M_ptr);
        }

        /// std::string -> std::string_view
        if (typeid(std::string).hash_code() == hashCode)
        {
            const std::string* ptr = reinterpret_cast<const std::string*>(_M_data._M_ptr);
            return std::string_view(ptr->data(), ptr->size());
        } else

        /// const char* -> std::string_view
        if (_M_flags == StringLiteralFlag)
        {
            return std::string_view(reinterpret_cast<const char*>(_M_data._M_ptr), _M_size);
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
            std::is_same_v<std::remove_cv_t<_Tp>, signed char> ||
            std::is_same_v<std::remove_cv_t<_Tp>, unsigned char>, _Tp>
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
                return static_cast<_Tp>(reinterpret_cast<const char*>(_M_data._M_ptr)[0]);
            else
                return '\0';
        } else

        /// std::string -> char or uchar
        if (typeid(std::string).hash_code() == hashCode)
        {
            const std::string* strptr = reinterpret_cast<const std::string*>(_M_data._M_ptr);

            if (strptr->size())
                return static_cast<_Tp>(strptr->front());
            else
                return '\0';
        } else

        /// std::string_view -> char or uchar
        if (typeid(std::string_view).hash_code() == hashCode)
        {
            const std::string_view* strptr = reinterpret_cast<const std::string_view*>(_M_data._M_ptr);

            if (strptr->size())
                return static_cast<_Tp>(strptr->front());
            else
                return '\0';
        }

        assert(false && "Incorrect conversion.");
        return '\0';
    }

    template<typename _Tp>
    [[nodiscard]]
    constexpr std::enable_if_t<
            std::is_integral_v<std::remove_cv_t<_Tp>> &&
                !(std::is_same_v<std::remove_cv_t<_Tp>, signed char> ||
                std::is_same_v<std::remove_cv_t<_Tp>, unsigned char>), _Tp>
        value() const noexcept
    {
        std::size_t hashCode = typeHashCode();

        if (typeid(_Tp).hash_code() == hashCode)
        {
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
                    return static_cast<bool>(_M_data._M_bytes[0]) ? 1 : 0;
                }

                if (is & DoCheckSign)
                {
                    if (_M_size == sizeof(std::int8_t))
                        return static_cast<_Tp>(*reinterpret_cast<const std::int8_t*>(&_M_data._M_value));

                    else if (_M_size == sizeof(std::int16_t)) [[unlikely]]
                        return static_cast<_Tp>(*reinterpret_cast<const std::int16_t*>(&_M_data._M_value));

                    else if (_M_size == sizeof(std::int32_t)) [[likely]]
                        return static_cast<_Tp>(*reinterpret_cast<const std::int32_t*>(&_M_data._M_value));

                    else // if (_M_size == sizeof(std::int64_t))
                        return static_cast<_Tp>(*reinterpret_cast<const std::int64_t*>(&_M_data._M_value));
                }
                else
                {
                    return static_cast<_Tp>(*reinterpret_cast<const std::uint64_t*>(&_M_data._M_value));
                }
            }
            else // if (is & DoCheckReal)
            {
                if (_M_size == sizeof(float))
                    return static_cast<_Tp>(*reinterpret_cast<const float*>(&_M_data._M_value));

                else if (_M_size == sizeof(double))
                    return static_cast<_Tp>(*reinterpret_cast<const double*>(&_M_data._M_value));

                else // if (_M_size == sizeof(long double))
                    if constexpr (sizeof(long double) <= 8)
                        return static_cast<_Tp>(*reinterpret_cast<const long double*>(&_M_data._M_value));
                    else
                        return static_cast<_Tp>(*reinterpret_cast<const long double*>(_M_data._M_ptr));
            }

        } else

        /// const char* -> integer
        if (_M_flags == StringLiteralFlag)
        {
            char* endptr;

            if constexpr (std::is_signed_v<_Tp>)
                return static_cast<_Tp>(
                    std::strtoll(reinterpret_cast<const char*>(_M_data._M_ptr), &endptr, 10));

            else if constexpr (std::is_unsigned_v<_Tp>)
                return static_cast<_Tp>(
                    std::strtoull(reinterpret_cast<const char*>(_M_data._M_ptr), &endptr, 10));
        } else

        /// std::string -> integer
        if (typeid(std::string).hash_code() == hashCode)
        {
            try
            {
                if constexpr (std::is_signed_v<_Tp>)
                    return static_cast<_Tp>(
                        std::stoll(*reinterpret_cast<const std::string*>(_M_data._M_ptr)));

                else if constexpr (std::is_unsigned_v<_Tp>)
                    return static_cast<_Tp>(
                        std::stoull(*reinterpret_cast<const std::string*>(_M_data._M_ptr)));
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

            const std::string_view* sv = reinterpret_cast<const std::string_view*>(_M_data._M_ptr);
            
            if (sv->empty())
            {
                return 0;
            }
            else if (sv->back() == '\0')
            {
                if constexpr (std::is_signed_v<_Tp>)
                    return static_cast<_Tp>(std::strtoll(sv->data(), &endptr, 10));

                else if constexpr (std::is_unsigned_v<_Tp>)
                    return static_cast<_Tp>(std::strtoull(sv->data(), &endptr, 10));
            }
            else
            {
                if constexpr (std::is_signed_v<_Tp>)
                    return static_cast<_Tp>(std::stoll(std::string(sv->data(), sv->size())));

                else if constexpr (std::is_unsigned_v<_Tp>)
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

        if (typeid(_Tp).hash_code() == hashCode)
        {
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
                    return static_cast<bool>(_M_data._M_bytes[0]) ? 1.00 : 0.00;
                }

                if (is & DoCheckSign)
                {
                    if (_M_size == sizeof(std::int8_t))
                        return static_cast<_Tp>(*reinterpret_cast<const std::int8_t*>(&_M_data._M_value));

                    else if (_M_size == sizeof(std::int16_t)) [[unlikely]]
                        return static_cast<_Tp>(*reinterpret_cast<const std::int16_t*>(&_M_data._M_value));

                    else if (_M_size == sizeof(std::int32_t)) [[likely]]
                        return static_cast<_Tp>(*reinterpret_cast<const std::int32_t*>(&_M_data._M_value));

                    else // if (_M_size == sizeof(std::int64_t))
                        return static_cast<_Tp>(*reinterpret_cast<const std::int64_t*>(&_M_data._M_value));
                }
                else
                {
                    return static_cast<_Tp>(*reinterpret_cast<const std::uint64_t*>(&_M_data._M_value));
                }
            }
            else // if (is & DoCheckReal)
            {
                if (_M_size == sizeof(float))
                    return static_cast<_Tp>(*reinterpret_cast<const float*>(&_M_data._M_value));

                else if (_M_size == sizeof(double))
                    return static_cast<_Tp>(*reinterpret_cast<const double*>(&_M_data._M_value));

                else // if (_M_size == sizeof(long double))
                    if constexpr (sizeof(long double) <= 8)
                        return static_cast<_Tp>(*reinterpret_cast<const long double*>(&_M_data._M_value));
                    else
                        return static_cast<_Tp>(*reinterpret_cast<const long double*>(_M_data._M_ptr));
            }

        } else

        /// const char* -> floating point
        if (_M_flags == StringLiteralFlag)
        {
            char* endptr;

            return static_cast<_Tp>(
                std::strtold(reinterpret_cast<const char*>(_M_data._M_ptr), &endptr));
        } else

        /// std::string -> floating point
        if (typeid(std::string).hash_code() == hashCode)
        {
            try
            {
                return static_cast<_Tp>(
                    std::stold(*reinterpret_cast<const std::string*>(_M_data._M_ptr)));
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

            const std::string_view* sv = reinterpret_cast<const std::string_view*>(_M_data._M_ptr);
            
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
                !(std::is_same_v<std::remove_volatile_t<_Tp>, const char*> ||
                std::is_same_v<std::remove_cv_t<_Tp>, std::string> ||
                std::is_same_v<std::remove_cv_t<_Tp>, std::string_view>), _Tp>
        value() const noexcept
    {
        if constexpr (_S_hasValueType_v<_Tp>/* && _S_isIterable_v<_Tp>*/)
        {
            if (not isSameType<_Tp>() && hasValueType() && isIterable())
            {
                if (typeid(_S_valueTypeOf_t<_Tp>).hash_code() == valueTypeHashCode())
                {
                    return _M_iterateAndCopy<_Tp>();
                }
                else
                {
                    return _M_iterateAnyAndCopy<_Tp>();
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

    [[nodiscard]]
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

    [[nodiscard]]
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

    
    template<typename _Container, typename _ValueType, typename = std::void_t<>>
    struct _S_hasPushBackMFunction : std::false_type { };

    template<typename _Container, typename _ValueType>
    struct _S_hasPushBackMFunction<
        _Container,
        _ValueType,
        std::void_t<decltype(std::declval<_Container>().push_back(std::declval<_ValueType>()))>>
            : std::true_type { };


    template<typename _Container, typename _ValueType, typename = std::void_t<>>
    struct _S_hasInsertMFunction : std::false_type { };

    template<typename _Container, typename _ValueType>
    struct _S_hasInsertMFunction<
        _Container,
        _ValueType,
        std::void_t<decltype(std::declval<_Container>().insert(std::declval<_ValueType>()))>>
            : std::true_type { };


    template<typename _Container, typename _ValueType, typename = std::void_t<>>
    struct _S_hasPushMFunction : std::false_type { };

    template<typename _Container, typename _ValueType>
    struct _S_hasPushMFunction<
        _Container,
        _ValueType,
        std::void_t<decltype(std::declval<_Container>().push(std::declval<_ValueType>()))>>
            : std::true_type { };


    template<typename _Container, typename _ValueType, typename = std::void_t<>>
    struct _S_hasAppendMFunction : std::false_type { };

    template<typename _Container, typename _ValueType>
    struct _S_hasAppendMFunction<
        _Container,
        _ValueType,
        std::void_t<decltype(std::declval<_Container>().append(std::declval<_ValueType>()))>>
            : std::true_type { };


    template<
        typename _Container,
        typename _ValueType,
        std::enable_if_t<_S_hasPushBackMFunction<_Container, _ValueType>::value, int> = 0>
    static constexpr inline
        void _S_insertToEnd(_Container& __container, _ValueType&& __element) noexcept
    { __container.push_back(std::forward<_ValueType>(__element)); }

    template<
        typename _Container,
        typename _ValueType,
        std::enable_if_t<
            _S_hasInsertMFunction<_Container, _ValueType>::value &&
        not _S_hasPushBackMFunction<_Container, _ValueType>::value, int> = 0>
    static constexpr inline
        void _S_insertToEnd(_Container& __container, _ValueType&& __element) noexcept
    { __container.insert(std::forward<_ValueType>(__element)); }

    template<
        typename _Container,
        typename _ValueType,
        std::enable_if_t<
            _S_hasPushMFunction<_Container, _ValueType>::value &&
        not _S_hasInsertMFunction<_Container, _ValueType>::value &&
        not _S_hasPushBackMFunction<_Container, _ValueType>::value, int> = 0>
    static constexpr inline
        void _S_insertToEnd(_Container& __container, _ValueType&& __element) noexcept
    { __container.push(std::forward<_ValueType>(__element)); }

    template<
        typename _Container,
        typename _ValueType,
        std::enable_if_t<
            _S_hasAppendMFunction<_Container, _ValueType>::value &&
        not _S_hasPushMFunction<_Container, _ValueType>::value &&
        not _S_hasInsertMFunction<_Container, _ValueType>::value &&
        not _S_hasPushBackMFunction<_Container, _ValueType>::value, int> = 0>
    static constexpr inline
        void _S_insertToEnd(_Container& __container, _ValueType&& __element) noexcept
    { __container.append(std::forward<_ValueType>(__element)); }

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

    constexpr Kwargs(
        container_type __list)
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