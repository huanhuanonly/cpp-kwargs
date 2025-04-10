# class `KwargsKey`

## Description

键名（字符串）包装器，内部存储着一个 $64$ 位哈希值，基于 **_BKDRHash_** 算法。

## Public member types

### `value_type`

```cpp
using value_type = detail::string_hash_type; // std::uint64_t
```

字符串哈希值的类型，是一个占 $64$ 位的无符号整型；

## Public member functions

### `KwargsKey()`

```cpp
template<std::size_t _Size>
constexpr KwargsKey(const char (&__str)[_Size]) noexcept;
```

使用字符串字面量构造。

```cpp
constexpr KwargsKey(const char* const __str, std::size_t __size) noexcept;
```

使用字符串指针构造。

> [!WARNING]
> $size$ 指定的大小不需要包含末尾的 `\0` 结束符，否则 `\0` 也将一起参与哈希计算。

```cpp
constexpr explicit KwargsKey(value_type __option) noexcept;
```

直接使用计算后的哈希值进行构造（不推荐使用）。

### `value()`

```cpp
[[nodiscard]] constexpr value_type value() const noexcept;
```

返回哈希值。

## Public operator overloading

### `operator value_type()`

```cpp
constexpr operator value_type() const noexcept;
```

返回哈希值。

### `operator<=>()`

```cpp
constexpr bool operator==(KwargsKey __other) const noexcept;
constexpr bool operator!=(KwargsKey __other) const noexcept;
constexpr bool operator<(KwargsKey __other) const noexcept;
constexpr bool operator>(KwargsKey __other) const noexcept;
constexpr bool operator<=(KwargsKey __other) const noexcept;
constexpr bool operator>=(KwargsKey __other) const noexcept;
```

比较字符串的哈希值是否相等。

> [!WARNING]
> 哈希算法可能会发生 **哈希碰撞**，两个不相等的字符串的哈希值在极小概率的情况下也有可能会相同，但是两个相同的字符串的哈希值一定相同。

### `operator||`

```cpp
[[nodiscard]] friend constexpr std::array<KwargsKey, 2>
    operator||(KwargsKey __first, KwargsKey __second) noexcept;

template<std::size_t _StringSize>
[[nodiscard]] friend constexpr std::array<KwargsKey, 2>
    operator||(KwargsKey __first, const char (&__second)[_StringSize]) noexcept;

template<std::size_t _Size>
[[nodiscard]] friend constexpr std::array<KwargsKey, _Size + 1>
    operator||(const std::array<KwargsKey, _Size>& __first, KwargsKey __second) noexcept;

template<std::size_t _Size, std::size_t _StringSize>
[[nodiscard]] friend constexpr std::array<KwargsKey, _Size + 1>
    operator||(const std::array<KwargsKey, _Size>& __first, const char (&__second)[_StringSize]) noexcept;
```

支持使用 `or` 运算连接两个键，详见 [Kwargs](./Kwargs.md)。