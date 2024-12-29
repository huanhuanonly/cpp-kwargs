#pragma once

#ifndef CPP_KWARGS_H
#define CPP_KWARGS_H

#include <type_traits>
#include <typeinfo>

#include <string>
#include <string_view>
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

    constexpr KwargsKey(value_type __option) noexcept
        : _M_option(__option)
    { }

    constexpr void pushBack(char __c) noexcept
    { _M_option = ((_M_option * base) % mod + __c) % mod; }

    template<typename... _Args>
    constexpr void pushBack(char __c, _Args&&... __args) noexcept
    {
        pushBack(__c);
        pushBack(__args...);
    }

    static constexpr value_type base = 0X1C1ULL;
    static constexpr value_type mod  = 0X91F5BCB8BB0243ULL;

    constexpr operator value_type() noexcept
    { return _M_option; }

    constexpr bool operator==(KwargsKey __other) const noexcept
    { return _M_option == __other._M_option; }

private:

    value_type _M_option = 0;
};

constexpr KwargsKey operator""_opt(const char* const __str, std::size_t __size) noexcept
{ return KwargsKey(__str, __size); }

template<char... _String>
constexpr KwargsKey operator""_opt() noexcept
{ KwargsKey result; return result.pushBack(_String...), result; }

constexpr KwargsKey operator""_opt(char __ch) noexcept
{ KwargsKey result; return result.pushBack(__ch), result; }

class KwargsValue
{
protected:

    enum WorkFlags : int
    {
        DoApplyAndCopy, /// type*   , type**
        DoCopy,         /// type*   , type*
        DoFree,         /// type*   , [unused]
        DoGetTypeHash,  /// [unused], std::size_t*

        DoCheckInt  = 0b001'00,  /// [unused], int*
        DoCheckReal = 0b010'00,  /// [unused], int*
        DoCheckSign = 0b100'00   /// [unused], int*
    };

    friend static constexpr WorkFlags operator|(WorkFlags __first, WorkFlags __second) noexcept
    { return static_cast<WorkFlags>(static_cast<int>(__first) | static_cast<int>(__second)); }

    template<typename _Tp>
    static void _S_manage(WorkFlags __work, void* __inData, void* __outData)
    {
        using type = _Tp;

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

                *reinterpret_cast<int*>(__outData) = result;
                break;
            }
        }
    }

    template<typename _Tp>
    static constexpr inline bool use_value_flag_v =
        std::is_integral_v<_Tp> ||
        std::is_pointer_v<_Tp> ||
        std::is_floating_point_v<_Tp> &&
        sizeof(_Tp) <= sizeof(void*);

public:

    template<typename _Tp>
    constexpr KwargsValue(const _Tp& __value) noexcept
        : _M_manager(&KwargsValue::_S_manage<_Tp>)
    {
        if constexpr (use_value_flag_v<_Tp>)
        {
            _M_data._M_value = *reinterpret_cast<
                decltype(_M_data._M_value)*>(&__value);

            _M_flags = ValueFlag;
        }
        else
        {
            _M_data._M_ptr = std::addressof(__value);
            _M_flags = PointerFlag;
        }

        _M_size = sizeof(_Tp);
    }
    
    template<typename _Tp>
    inline KwargsValue(_Tp&& __value)
        : _M_manager(&KwargsValue::_S_manage<_Tp>)
    {
        if constexpr (use_value_flag_v<_Tp>)
        {
            _M_data._M_value = 0;

            for (std::size_t i = 0; i < sizeof(_Tp); ++i)
            {
                _M_data._M_bytes[i] = reinterpret_cast<std::byte*>(&__value)[i];
            }

            _M_flags = ValueFlag;
        }
        else
        {
            _M_data._M_ptr = new _Tp(std::move(__value));
            _M_flags = AppliedFlag;
        }

        _M_size = sizeof(_Tp);
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


    constexpr std::size_t typeHashCode() const noexcept
    {
        std::size_t hashCode;
        _M_manager(DoGetTypeHash, nullptr, &hashCode);

        return hashCode;
    }

    template<typename _Tp>
    constexpr bool isSameType() const noexcept
    { return typeid(_Tp).hash_code() == typeHashCode(); }

    constexpr bool isInteger() const noexcept
    {
        int result;
        _M_manager(DoCheckInt, nullptr, &result);

        return result;
    }

    constexpr bool isRealNumber() const noexcept
    {
        int result;
        _M_manager(DoCheckReal, nullptr, &result);

        return result;
    }

    template<typename _Tp>
    constexpr operator _Tp() const noexcept
    { return value<_Tp>(); }

    template<typename _Tp>
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
        }

        /// std::string_view -> const char*
        else if (typeid(std::string_view).hash_code() == hashCode)
        {
            return reinterpret_cast<std::string_view*>(_M_data._M_ptr)->data();
        }

        assert(false && "Incorrect conversion.");

        const char* nullstr = "";
        return nullstr;
    }

    template<typename _Tp>
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
                        return std::string(1, static_cast<char>(_M_data._M_bytes[0]));
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
            std::string* ptr = reinterpret_cast<std::string*>(_M_data._M_ptr);
            return std::string_view(ptr->data(), ptr->size());
        }
        /// const char* -> std::string_view
        else if (_M_flags == StringLiteralFlag)
        {
            return std::string_view(reinterpret_cast<const char*>(_M_data._M_ptr), _M_size);
        }

        assert(false && "Incorrect conversion.");
        return std::string_view();
    }

    template<typename _Tp>
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
        }

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
                if (is & DoCheckSign)
                {
                    if (_M_size == sizeof(std::int16_t)) [[unlikely]]
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
                if (is & DoCheckSign)
                {
                    if (_M_size == sizeof(std::int16_t)) [[unlikely]]
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
    constexpr std::enable_if_t<
            (std::is_class_v<_Tp> || std::is_union_v<_Tp> || std::is_pointer_v<_Tp>) &&
                !(std::is_same_v<std::remove_volatile_t<_Tp>, const char*> ||
                std::is_same_v<std::remove_cv_t<_Tp>, std::string> ||
                std::is_same_v<std::remove_cv_t<_Tp>, std::string_view>), _Tp>
        value() const noexcept
    {
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
    }

protected:

    template<typename _Tp, typename = std::void_t<decltype(_Tp(std::declval<const char*>()))>>
    static constexpr _Tp _S_fromStringLiteral(const char* __str) noexcept
    { return _Tp(__str); }

    template<typename _Tp>
    static constexpr _Tp _S_fromStringLiteral(const char* __str) noexcept
    { return _Tp(); }

private:
    
    union Data
    {
        void* _M_ptr;
        
        std::uintptr_t _M_value;

        std::byte _M_bytes[sizeof(void*)];

    } _M_data;

    void (*_M_manager)(WorkFlags, void*, void*);

    enum Flags
    {
        PointerFlag = 0,
        StringLiteralFlag,
        ValueFlag,
        AppliedFlag
    };

    Flags _M_flags;

    std::uint32_t _M_size;
};

template<KwargsKey::value_type... _OptionalList>
class Kwargs
{
public:

    using pair_type = std::pair<KwargsKey, KwargsValue>;
    using init_type = std::initializer_list<pair_type>;

    struct DataItem
    {
        constexpr DataItem(const KwargsValue* __that = nullptr) noexcept
            : _M_that(__that)
        { }

        template<typename _ValueType, typename... _Args>
        constexpr _ValueType valueOr(_Args&&... __args) const noexcept
        { return _M_that ? _M_that->value<_ValueType>() : _ValueType(std::forward<_Args>(__args)...); }

        constexpr bool hasValue() const noexcept
        { return _M_that; }

        constexpr const DataItem* operator->() const noexcept
        { return this; }

    private:

        const KwargsValue* _M_that;
    };

    constexpr Kwargs(
        init_type __list)
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
    }

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

protected:

    template<KwargsKey::value_type _Current>
    constexpr bool _M_contains(KwargsKey __option) const noexcept
    { return __option == KwargsKey(_Current); }

    template<KwargsKey::value_type _Current, KwargsKey::value_type _Next, KwargsKey::value_type... _Args>
    constexpr bool _M_contains(KwargsKey __option) const noexcept
    { return __option == KwargsKey(_Current) || _M_contains<_Next, _Args...>(__option); }

private:

    std::initializer_list<std::pair<KwargsKey, KwargsValue>> _M_data;
};

#endif  // CPP_KWARGS_H