# `KwargsKey` class

键名（字符串字面量）包装器，内部存储着一个 $64$ 位哈希值，基于 **_BKDRHash_** 算法。

## 语法
```cpp
class KwargsKey;
```

## 成员

### 构造函数

|名称|描述|
|-|-|
|[`KwargsKey`](#kwargskey)|构造函数。|

### 类型

|名称|描述|
|-|-|
|[`value_type`](#value_type)|字符串哈希值的类型，是一个占 $64$ 位的无符号整型；|

### 函数

|名称|描述|
|-|-|
|[`value`](#value)|获取哈希值。|

### 运算符

|名称|描述|
|-|-|
|[`operator value_type()`](#operator-value_type)|可隐式转换为 `value_type`。|
|[`operator==`](#operator)|哈希值比较。|
|[`operator!=`](#operator)|哈希值比较。|
|[`operator<`](#operator)|哈希值比较。|
|[`operator>`](#operator)|哈希值比较。|
|[`operator<=`](#operator)|哈希值比较。|
|[`operator>=`](#operator)|哈希值比较。|
|[`operator\|\|`](#operator-1)|连接两个或以上的 `KwargsKey`。|


### `value_type`

字符串哈希值的类型，是一个占 $64$ 位的无符号整型；

```cpp
using value_type = detail::string_hash_type; // std::uint64_t
```

### `KwargsKey`

使用字符串字面量构造。

```cpp
template<std::size_t _Size>
constexpr KwargsKey(const char (&__str)[_Size]) noexcept;
```

使用字符串指针构造。

```cpp
constexpr KwargsKey(const char* const __str, std::size_t __size) noexcept;
```

> [!TIP]
> $size$ 指定的大小不需要包含末尾的 `\0` 结束符，否则 `\0` 也将一起参与哈希计算。

直接使用计算后的哈希值进行构造（不推荐使用）。

```cpp
constexpr explicit KwargsKey(value_type __option) noexcept;
```


### `value`

获取哈希值。

```cpp
constexpr value_type value() const noexcept;
```

### `operator value_type()`

可隐式转换为 `value_type`。

```cpp
constexpr operator value_type() const noexcept;
```


### `operator<=>()`

哈希值比较。

```cpp
constexpr bool operator==(KwargsKey __other) const noexcept;
constexpr bool operator!=(KwargsKey __other) const noexcept;
constexpr bool operator<(KwargsKey __other) const noexcept;
constexpr bool operator>(KwargsKey __other) const noexcept;
constexpr bool operator<=(KwargsKey __other) const noexcept;
constexpr bool operator>=(KwargsKey __other) const noexcept;
```

> [!WARNING]
> 哈希算法可能会发生 **哈希碰撞**，两个不相等的字符串的哈希值在极小概率的情况下也有可能会相同，但是两个相同的字符串的哈希值一定相同。

### `operator||`

连接两个或以上的 `KwargsKey`。

支持使用 `or` 运算连接两个键，详见 [Kwargs](./Kwargs.md)。

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
