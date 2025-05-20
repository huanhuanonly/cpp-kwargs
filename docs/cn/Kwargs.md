# `Kwargs` class

用于封装键值对的集合列表，底层维护了一个 
`std::initializer_list<std::pair<const KwargsKey, KwargsValue>>`。

## 语法

```cpp
template<KwargsKey::value_type... _OptionalList>
class Kwargs;
```

### 参数

- _`_OptionalList`_

    支持的键的列表，使用以 [`_opt`](./operator%20_opt.md) 后缀的字面量进行构造。

> [!TIP]
> 字面量后缀名可以通过定义宏来更改为 `_opt` / `_o` / `opt` / `o` 中的任意一种。
> - `KWARGSKEY_LITERAL_SUFFIX_WITHOUT_LEADING_UNDERSCORE`：`_opt`；
> - `KWARGSKEY_LITERAL_SHORT_SUFFIX`：`_o`；
> - `KWARGSKEY_LITERAL_SUFFIX_WITHOUT_LEADING_UNDERSCORE` | `KWARGSKEY_LITERAL_SHORT_SUFFIX`：`o`；

> [!WARNING]
> 如果传入不在列表中的键名或者出现重复的键名，将会通过 `assert()` 引发崩溃。  
> 如果 `_OptionalList` 为空，则仅检查是否出现重复的键名。

## 成员

### 构造函数

|名称|描述|
|-|-|
|[`Kwargs`](#kwargs)|构造函数。|

### 类型

|名称|描述|
|-|-|
|[`value_type`](#typedef)|键值对类型。|
|[`container_type`](#typedef)|底层容器类型。|
|[`iterator`](#typedef)|迭代器类型。|
|[`const_iterator`](#typedef)|迭代器类型。|
|[`DataItem`](./Kwargs_DataItem.md)|值的包装器类型。|

### 函数

|名称|描述|
|-|-|
|[`begin`](#begin)|获取键值对的起始迭代器。|
|[`end`](#end)|获取键值对的末尾迭代器。|
|[`size`](#size)|获取键值对的数量。|

### 运算符

|名称|描述|
|-|-|
|[`operator[]`](#operator)|获取键对应的值的包装器。|

### Typedef

```cpp
using value_type = std::pair<const KwargsKey, KwargsValue>;
using container_type = std::initializer_list<value_type>;

using iterator = typename container_type::iterator;
using const_iterator = typename container_type::const_iterator;
```

- struct [`DataItem`](./Kwargs_DataItem.md)

### `Kwargs`

使用 `std::initializer_list<std::pair<const KwargsKey, KwargsValue>>` 进行构造。

```cpp
constexpr Kwargs(container_type __list) noexcept
```
  
### `operator[]`

根据键名返回对应的值的包装器 [`DataItem`](./Kwargs_DataItem.md)。

```cpp
constexpr DataItem operator[](KwargsKey __key) noexcept;

template<std::size_t _Size>
constexpr DataItem operator[](const std::array<KwargsKey, _Size>& __options) noexcept;
```

第二个重载版本适用于，[`KwargsKey`](./KwargsKey.md) 或 `KwargsKeyLiteral` 使用 [`or`](./KwargsKey.md#operator-1) 运算符连接的多个键，将返回第一个匹配到的键对应的值。

### `begin`

获取键值对的起始迭代器。

```cpp
constexpr const_iterator begin() const noexcept;
```

### `end`

获取键值对的末尾迭代器。

```cpp
constexpr const_iterator end() const noexcept;
```

### `size`

获取键值对的数量。

```cpp
constexpr std::size_t size() const noexcept;
```
