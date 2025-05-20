# `KwargsKey` class

A wrapper for keyword names (string literals) that internally stores a 64-bit hash value based on the **BKDRHash** algorithm.

## Syntax

```cpp
class KwargsKey;
```

## Members

### Constructors

| Name                      | Description  |
| ------------------------- | ------------ |
| [`KwargsKey`](#kwargskey) | Constructor. |

### Types

| Name                        | Description                                                    |
| --------------------------- | -------------------------------------------------------------- |
| [`value_type`](#value_type) | The type of the string hash value; an unsigned 64-bit integer. |

### Functions

| Name              | Description             |
| ----------------- | ----------------------- |
| [`value`](#value) | Returns the hash value. |

### Operators

| Name                                            | Description                              |
| ----------------------------------------------- | ---------------------------------------- |
| [`operator value_type()`](#operator-value_type) | Implicit conversion to `value_type`.     |
| [`operator==`](#operator)                       | Hash value comparison.                   |
| [`operator!=`](#operator)                       | Hash value comparison.                   |
| [`operator<`](#operator)                        | Hash value comparison.                   |
| [`operator>`](#operator)                        | Hash value comparison.                   |
| [`operator<=`](#operator)                       | Hash value comparison.                   |
| [`operator>=`](#operator)                       | Hash value comparison.                   |
| [`operator\|\|`](#operator-1)                   | Joins two or more `KwargsKey` instances. |

---

### `value_type`

The type of the stored hash value. A 64-bit unsigned integer:

```cpp
using value_type = detail::string_hash_type; // std::uint64_t
```

---

### `KwargsKey`

#### Construct from a string literal:

```cpp
template<std::size_t _Size>
constexpr KwargsKey(const char (&__str)[_Size]) noexcept;
```

#### Construct from a string pointer and length:

```cpp
constexpr KwargsKey(const char* const __str, std::size_t __size) noexcept;
```

> [!TIP]
> The `__size` does not need to include the null terminator `\0`. If it does, the `\0` will also be hashed.

#### Construct directly from a precomputed hash value (not recommended):

```cpp
constexpr explicit KwargsKey(value_type __option) noexcept;
```

---

### `value`

Returns the internal hash value.

```cpp
constexpr value_type value() const noexcept;
```

---

### `operator value_type()`

Allows implicit conversion to `value_type`.

```cpp
constexpr operator value_type() const noexcept;
```

---

### `operator<=>` (Comparison Operators)

Compare based on hash values.

```cpp
constexpr bool operator==(KwargsKey __other) const noexcept;
constexpr bool operator!=(KwargsKey __other) const noexcept;
constexpr bool operator<(KwargsKey __other) const noexcept;
constexpr bool operator>(KwargsKey __other) const noexcept;
constexpr bool operator<=(KwargsKey __other) const noexcept;
constexpr bool operator>=(KwargsKey __other) const noexcept;
```

> [!WARNING]
> **Hash collisions** may occur. Two different strings can have the same hash in extremely rare cases. However, identical strings will always produce the same hash.

---

### `operator||`

Combines multiple `KwargsKey` instances using the `or` operator.

Supports combining keys using logical OR. See also [Kwargs](./Kwargs.md) for usage.

```cpp
friend constexpr std::array<KwargsKey, 2>
    operator||(KwargsKey __first, KwargsKey __second) noexcept;

template<std::size_t _StringSize>
friend constexpr std::array<KwargsKey, 2>
    operator||(KwargsKey __first, const char (&__second)[_StringSize]) noexcept;

template<std::size_t _Size>
friend constexpr std::array<KwargsKey, _Size + 1>
    operator||(const std::array<KwargsKey, _Size>& __first, KwargsKey __second) noexcept;

template<std::size_t _Size, std::size_t _StringSize>
friend constexpr std::array<KwargsKey, _Size + 1>
    operator||(const std::array<KwargsKey, _Size>& __first, const char (&__second)[_StringSize]) noexcept;
```
