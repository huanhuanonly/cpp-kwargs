# `DataItem` structure

[`KwargsValue`](./KwargsValue.md) 的包装器，类似于 [`std::optional`](https://en.cppreference.com/w/cpp/utility/optional)。

## 语法

```cpp
struct DataItem;
```

## 成员

### 构造函数

|名称|描述|
|-|-|
|[`DateItem`](#dataitem)|构造函数。|

### 函数

|名称|描述|
|-|-|
|[`valueOr`](#valueor)|构造函数。|
|[`hasValue`](#hasvalue)|构造函数。|

## 运算符

|名称|描述|
|-|-|
|[`operator->`](#operator-)|构造函数。|

### `DataItem`

构造函数。

```cpp
constexpr DataItem(const KwargsValue* __that = nullptr) noexcept;
```

### `operator->`

返回指向数据的指针。

```cpp
constexpr const KwargsValue* operator->() const noexcept;
```

### `hasValue`

检查是否有键对应的值，如果有，返回 `true`，否则，返回 `false`。

```cpp
constexpr bool hasValue() const noexcept;
```

### `valueOr`

```cpp
template<typename _ValueType, typename... _Args>
constexpr _ValueType valueOr(_Args&&... __args) const noexcept;
```

- _`_ValueType`_

  值的类型，需要显式指定，如果和原始值的类型不一致时将尝试使用内置类型转换器进行转换。

- _`_Args`_

  参数列表的类型。

- _`__args`_
  
  当对应的键没有值时，将使用该参数列表来调用 `_ValueType` 的构造函数进行原地构造并返回。

#### 示例

```cpp
dataItem.valueOr<std::string>(5, 'c');
```

如果 $dataItem$ 为空（不包含值），则返回 `std::string(5, 'c')`，同等于 `std::string("ccccc")`。
