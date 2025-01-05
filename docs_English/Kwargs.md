# class `Kwargs`

## Description

A class used to encapsulate a collection of key-value pairs, which is internally managed by a `std::initializer_list<std::pair<KwargsKey, KwargsValue>>`.

## Syntax

```cpp
template<KwargsKey::value_type... _OptionalList>
class Kwargs {...}
```

## Parameters

- _`_OptionalList`_

    A list of supported keys, constructed using the literal suffix [`_opt`](./operator%20_opt.md).

> [!WARNING]
> If a key that is not in the list is passed, or if there are duplicate keys, a crash will occur via `assert()`.  
> If `_OptionalList` is empty, only duplicate key names will be checked.

## Public Member Types

```cpp
using value_type = std::pair<KwargsKey, KwargsValue>;
using container_type = std::initializer_list<value_type>;

using iterator = typename container_type::iterator;
using const_iterator = typename container_type::const_iterator;
```

- struct [`DataItem`](./Kwargs_DataItem.md)

## Public Member Functions

### `Kwargs()`

```cpp
constexpr Kwargs(container_type __list) noexcept
```
  
Constructs using a `std::initializer_list<std::pair<KwargsKey, KwargsValue>>`.

### `operator[]()`

```cpp
constexpr DataItem operator[](KwargsKey __key) noexcept
```

Returns the corresponding value wrapper [`DataItem`](./Kwargs_DataItem.md) for the given key.

### `begin()`

```cpp
constexpr const_iterator begin() const noexcept;
```

### `end()`

```cpp
constexpr const_iterator end() const noexcept;
```

### `size()`

```cpp
constexpr std::size_t size() const noexcept;
```

Returns the number of keys.