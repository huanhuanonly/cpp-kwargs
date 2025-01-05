# class `KwargsKey`

## Description

A wrapper for key names (strings), internally storing a 64-bit hash value based on the **_BKDRHash_** algorithm.

## Public Member Types

### `value_type`

```cpp
using value_type = std::uint64_t;
```

The type of the string hash value, which is a 64-bit unsigned integer.

## Public Member Variables

### $base$

```cpp
static constexpr value_type base;
```

The base value required by the **_BKDRHash_** algorithm.

### $mod$

```cpp
static constexpr value_type mod;
```

The modulus value required by the **_BKDRHash_** algorithm.

## Public Member Functions

### `KwargsKey()`

```cpp
template<std::size_t _Size>
constexpr KwargsKey(const char (&__str)[_Size]) noexcept
```

Constructs using a string literal.

```cpp
constexpr KwargsKey(const char* const __str, std::size_t __size) noexcept
```

Constructs using a string pointer.

> [!WARNING]
> The size specified by `$size$` should not include the trailing `\0` terminator; otherwise, the `\0` will be included in the hash calculation.

```cpp
constexpr explicit KwargsKey(value_type __option) noexcept
```

Constructs directly using the precomputed hash value (not recommended).

### `pushBack()`

Extends the existing string hash value by adding one more character to the end and recalculating the hash value.

```cpp
constexpr void pushBack(char __c) noexcept
```

Pushes a single character.

```cpp
template<typename... _Args>
constexpr void pushBack(char __c, _Args&&... __args) noexcept
```

Pushes multiple characters at once.

Example:

```cpp
KwargsKey key("Hel");
key.pushBack('l', 'o');

// At this point, the hash value of `key` is equivalent to the hash value of the string "Hello".
```

## Public Operator Overloading

### `operator value_type()`

```cpp
constexpr operator value_type() const noexcept
```

Returns the hash value.

### `operator==()`

```cpp
constexpr bool operator==(KwargsKey __other) const noexcept
```

Compares whether the hash values of two strings are equal.

> [!WARNING]
> **Hash collisions** may occur, meaning that two different strings might have the same hash value with a very small probability. However, two identical strings will always have the same hash value.

### `operator<()`

```cpp
constexpr bool operator<(KwargsKey __other) const noexcept
```