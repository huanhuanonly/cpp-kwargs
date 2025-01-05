# struct `DataItem`

## Description

A wrapper for [`KwargsValue`](./KwargsValue.md), similar to [`std::optional`](https://en.cppreference.com/w/cpp/utility/optional).

## Public Member Functions

### `operator->()`

```cpp
constexpr const DataItem* operator->() const noexcept
```

Returns `this`.

### `hasValue()`

```cpp
constexpr bool hasValue() const noexcept
```

Returns `true` if it is not empty (contains a value), otherwise returns `false`.

### `valueOr()`

```cpp
template<typename _ValueType, typename... _Args>
constexpr _ValueType valueOr(_Args&&... __args) const noexcept
```

- _`_ValueType`_

  The type of the value. It must be explicitly specified. If it differs from the original value's type, an attempt will be made to use the built-in type converter.

- _`_Args`_

  The type of the argument list.

- _`__args`_

  If the corresponding key does not have a value, this argument list will be used to call the constructor of `_ValueType` to perform in-place construction and return it.

  Example:

  ```cpp
  dataItem.valueOr<std::string>(5, 'c');
  ```

  If `$dataItem$` is empty (does not contain a value), it will return `std::string(5, 'c')`, which is equivalent to `std::string("ccccc")`.