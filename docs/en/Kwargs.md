# `Kwargs` class

A container for encapsulating key-value pairs. Internally, it maintains a
`std::initializer_list<std::pair<const KwargsKey, KwargsValue>>`.

## Syntax

```cpp
template<KwargsKey::value_type... _OptionalList>
class Kwargs;
```

### Parameters

* **_`_OptionalList`_**
  A list of supported keys, constructed using string literals with the [`_opt`](./operator%20_opt.md) suffix.

> [!TIP]
> The literal suffix name can be customized using the following macros:
>
> * `KWARGSKEY_LITERAL_SUFFIX_WITHOUT_LEADING_UNDERSCORE`: `_opt`
> * `KWARGSKEY_LITERAL_SHORT_SUFFIX`: `_o`
> * Both defined: `o`

> [!WARNING]
> If a key that is not in the list is used, or a duplicate key appears, the program will `assert()` and crash.
> If `_OptionalList` is empty, only key duplication will be checked.

---

## Members

### Constructors

| Name                | Description  |
| ------------------- | ------------ |
| [`Kwargs`](#kwargs) | Constructor. |

### Types

| Name                               | Description                |
| ---------------------------------- | -------------------------- |
| [`value_type`](#typedef)           | Key-value pair type.       |
| [`container_type`](#typedef)       | Underlying container type. |
| [`iterator`](#typedef)             | Iterator type.             |
| [`const_iterator`](#typedef)       | Iterator type.             |
| [`DataItem`](./Kwargs_DataItem.md) | Value wrapper type.        |

### Functions

| Name              | Description                                                   |
| ----------------- | ------------------------------------------------------------- |
| [`begin`](#begin) | Returns iterator to the beginning of the key-value pair list. |
| [`end`](#end)     | Returns iterator to the end of the key-value pair list.       |
| [`size`](#size)   | Returns the number of key-value pairs.                        |

### Operators

| Name                      | Description                                      |
| ------------------------- | ------------------------------------------------ |
| [`operator[]`](#operator) | Access the value wrapper corresponding to a key. |

---

### Typedef

```cpp
using value_type = std::pair<const KwargsKey, KwargsValue>;
using container_type = std::initializer_list<value_type>;

using iterator = typename container_type::iterator;
using const_iterator = typename container_type::const_iterator;
```

* See struct [`DataItem`](./Kwargs_DataItem.md)

---

### `Kwargs`

Constructed with an `std::initializer_list<std::pair<const KwargsKey, KwargsValue>>`.

```cpp
constexpr Kwargs(container_type __list) noexcept;
```

---

### `operator[]`

Returns a [`DataItem`](./Kwargs_DataItem.md) wrapper for the value associated with the specified key.

```cpp
constexpr DataItem operator[](KwargsKey __key) noexcept;

template<std::size_t _Size>
constexpr DataItem operator[](const std::array<KwargsKey, _Size>& __options) noexcept;
```

The second overload supports looking up the first matching key when multiple keys are joined using the [`or`](./KwargsKey.md#operator-1) operator (via `KwargsKey` or `KwargsKeyLiteral`).

---

### `begin`

Returns an iterator to the beginning of the key-value pair list.

```cpp
constexpr const_iterator begin() const noexcept;
```

---

### `end`

Returns an iterator to the end of the key-value pair list.

```cpp
constexpr const_iterator end() const noexcept;
```

---

### `size`

Returns the number of key-value pairs.

```cpp
constexpr std::size_t size() const noexcept;
```
