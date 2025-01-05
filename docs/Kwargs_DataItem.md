# struct `DataItem`

## Description

[`KwargsValue`](./KwargsValue.md) 的包装器，类似于 [`std::optional`](https://en.cppreference.com/w/cpp/utility/optional)。

## Public member functions

### `operator->()`

```cpp
constexpr const DataItem* operator->() const noexcept
```

`return this`。

### `hasValue()`

```cpp
constexpr bool hasValue() const noexcept
```

如果不为空（包含值）则返回 `true`，否则返回 `false`。


### `valueOr()`

```cpp
template<typename _ValueType, typename... _Args>
constexpr _ValueType valueOr(_Args&&... __args) const noexcept
```

- _`_ValueType`_

  值的类型，需要显式指定，如果和原始值的类型不一致时将尝试使用内置类型转换器进行转换。

- _`_Args`_

  参数列表的类型。

- _`__args`_
  
  当对应的键没有值时，将使用该参数列表来调用 `_ValueType` 的构造函数进行原地构造并返回。

  示例：

  ```cpp
  dataItem.valueOr<std::string>(5, 'c');
  ```

  如果 $dataItem$ 为空（不包含值），则返回 `std::string(5, 'c')`，同等于 `std::string("ccccc")`。
