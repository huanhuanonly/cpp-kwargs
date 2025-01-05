# class `Kwargs`

## Description

用于封装键值对的集合列表，底层维护了一个 
`std::initializer_list<std::pair<KwargsKey, KwargsValue>>`。

## Syntax

```cpp
template<KwargsKey::value_type... _OptionalList>
class Kwargs {...}
```

## Parameters

- _`_OptionalList`_

    支持的键的列表，使用以 [`_opt`](./operator%20_opt.md) 后缀的字面量进行构造。

> [!WARNING]
> 如果传入不在列表中的键名或者出现重复的键名，将会通过 `assert()` 引发崩溃。  
> 如果 `_OptionalList` 为空，则仅检查是否出现重复的键名。

## Public member types

```cpp
using value_type = std::pair<KwargsKey, KwargsValue>;
using container_type = std::initializer_list<value_type>;

using iterator = typename container_type::iterator;
using const_iterator = typename container_type::const_iterator;
```

- struct [`DataItem`](./Kwargs_DataItem.md)

## Public member functions

### `Kwargs()`

```cpp
constexpr Kwargs(container_type __list) noexcept
```
  
使用 `std::initializer_list<std::pair<KwargsKey, KwargsValue>>` 进行构造。

### `operator[]()`

```cpp
constexpr DataItem operator[](KwargsKey __key) noexcept
```

根据键名返回对应的值的包装器 [`DataItem`](./Kwargs_DataItem.md)。

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

返回键的数量。