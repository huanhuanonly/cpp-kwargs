# class `KwargsValue`

## Description

值的包装器，可以存放任意类型的值，类似于 [`std::any`](https://en.cppreference.com/w/cpp/utility/any)。

## Public member functions

### `KwargsValue()`

```cpp
constexpr KwargsValue() = default;
```

空的构造函数，默认包含一个 `int` 类型的值 $0$。

```cpp
template<typename _Tp>
constexpr KwargsValue(const _Tp& __value) noexcept
```

使用任意类型的值进行构造，对于结构体或共用体或 大于 $8$ 个字节的类型，将存储数据的地址（请注意值的生命周期），否则将直接对值进行字节级的复制。

```cpp
template<typename _Tp>
inline KwargsValue(_Tp&& __value)
```

使用任意类型的值进行构造，如果 $value$ 是一个 **右值** 且 `_Tp` 是一个 **结构体** 或 **共用体** 或 _大于_ $8$ 个字节的类型，将使用 `new` 申请一个新对象，并使用 `std::move()` 进行资源的转移，否则构造方式同 `KwargsValue(const _Tp&)`。

```cpp
template<std::size_t _Size>
constexpr KwargsValue(const char (&__value)[_Size]) noexcept
```

使用字符串字面量进行构造，直接存储字符串字面量的地址。

### `~KwargsValue()`

```cpp
constexpr ~KwargsValue() noexcept
```

如果值是在构造函数中被 `new` 申请的新对象，则调用值的 **析构函数** 并 `delete` 它。

### `typeHashCode()`

```cpp
constexpr std::size_t typeHashCode() const noexcept
```

返回值的类型的哈希值，哈希值由 `std::type_info::hash_code()` 获取。

### `valueTypeHashCode()`

```cpp
constexpr std::size_t valueTypeHashCode() const noexcept
```

假设当前的值的类型是 `_Tp`，如果 `_Tp` 是一个类且类中定义了 `value_type` 类型，则返回 `typeid(_Tp::value_type).hash_code()`，否则返回 `typeid(_Tp).hash_code()`。

### `hasValueType()`

```cpp
constexpr bool hasValueType() const noexcept
```

假设当前的值的类型是 `_Tp`，如果 `_Tp` 是一个类且类中定义了 `value_type` 类型，则返回 `true`，否则返回 `false`。

### `isSameType()`

```cpp
template<typename _Tp>
constexpr bool isSameType() const noexcept
```

如果当前值的类型是 `_Tp` 则返回 `true`，否则返回 `false`。

### `isInteger()`

```cpp
constexpr bool isInteger() const noexcept
```

如果当前值的类型是整数类型则返回 `true`，否则返回 `false`。

### `isRealNumber()`

```cpp
constexpr bool isRealNumber() const noexcept
```

如果当前值的类型是浮点类型则返回 `true`，否则返回 `false`。

### `isStdArray()`

```cpp
constexpr bool isStdArray() const noexcept
```

如果当前值的类型是 `std::array` 则返回 `true`，否则返回 `false`。

### `isIterable()`

```cpp
constexpr bool isIterable() const noexcept
```

如果当前值的类型是可迭代的则返回 `true`，否则返回 `false`。

> [!TIP]
> 这里的可迭代的类型需要具有 `.begin()` 和 `.end()` 成员函数。

### `value()`

```cpp
template<typename _Tp>
constexpr std::enable_if_t<..., _Tp> value() const noexcept
```

将值转为 `_Tp` 类型并返回，使用内置类型转换器进行转换。

> [!WARNING]
> 如果转换失败则会调用 `assert()` 引发崩溃，如果定义了 `NODEBUG` 宏，则会返回 `_Tp()`。

<details>
    <summary>
        <h4>
            为什么不使用 <code>throw</code> 抛出异常？
        </h4>
    </summary>

在 `constexpr` 修饰的函数中，在满足条件时会在编译期执行，而 `throw` 是运行期的指令。

</details>

### `size()`

```cpp
constexpr std::uint32_t size() const noexcept
```

假设当前的值的类型是 `_Tp`，则返回 `sizeof(_Tp)`。

特别的，如果 `_Tp` 是一个字符串字面量类型，则返回字符个数。

## Public operator overloading

### `operator=()`

```cpp
inline KwargsValue& operator=(const KwargsValue& __other)
```

复制。

```cpp
inline KwargsValue& operator=(KwargsValue&& __other) noexcept
```

移动。