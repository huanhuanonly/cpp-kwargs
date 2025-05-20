# `KwargsValue` class

值的包装器，可以存放任意类型的值，类似于 [`std::any`](https://en.cppreference.com/w/cpp/utility/any)。

## 语法

```cpp
class KwargsValue;
```

## 成员

### 构造函数

|名称|描述|
|-|-|
|[`KwargsValue()`](#kwargsvalue)|空的构造函数。|
|[`KwargsValue(const KwargsValue&)`](#kwargsvalue)|拷贝函数。|
|[`KwargsValue(KwargsValue&&)`](#kwargsvalue)|移动函数。|
|[`KwargsValue(const _Tp&)`](#kwargsvalue)|存储一个 `_Tp` 类型的值或指向他的指针。|
|[`KwargsValue(_Tp&&)`](#kwargsvalue)|存储一个 `_Tp` 类型的值，将触发移动语义。|
|[`KwargsValue(const char (&)[_Size])`](#kwargsvalue)|字符串字面量的偏特化构造函数，内部存储它的首地址。|

### 析构函数

|名称|描述|
|-|-|
|[`~KwargsValue`](#kwargsvalue-1)|析构函数。|


### 函数

|名称|描述|
|-|-|
|[`typeName`](#typename)|获取最后一次调用构造函数时 `_Tp` 的字符串形式，不同的编译器可能有不同的结果。|
|[`typeHashCode`](#typehashcode)|获取最后一次调用构造函数时 `_Tp` 的哈希值，通过 `std::type_info::hash_code()` 进行计算。|
|[`valueTypeHashCode`](#valuetypehashcode)|获取最后一次调用构造函数时 `_Tp` 的 `value_type` 类型的哈希值，如果不存在 `_Tp::value_type`，则同 [`typeHashCode()`](#typehashcode)。|
|[`hasValueType`](#hasvaluetype)|检查最后一次调用构造函数时 `_Tp` 是否存在 `value_type`，如果存在，返回 `true`，否则，返回 `false`。|
|[`isSameType`](#issametype)|检查最后一次调用构造函数时 `_Tp` 是否是指定的类型，如果和指定的类型相同，返回 `true`，否则，返回 `false`。|
|[`isInteger`](#isinteger)|检查最后一次调用构造函数时 `_Tp` 是否是整型或枚举类型，如果是，返回 `true`，否则，返回 `false`。|
|[`isRealNumber`](#isrealnumber)|检查最后一次调用构造函数时 `_Tp` 是否是浮点型，如果是，返回 `true`，否则，返回 `false`。|
|[`isEnum`](#isenum)|检查最后一次调用构造函数时 `_Tp` 是否是枚举型，如果是，返回 `true`，否则，返回 `false`。|
|[`isStdArray`](#isstdarray)|检查最后一次调用构造函数时 `_Tp` 是否是 `std::array`，如果是，返回 `true`，否则，返回 `false`。|
|[`isIterable`](#isiterable)|检查最后一次调用构造函数时 `_Tp` 是否是可迭代的容器类型，如果是，返回 `true`，否则，返回 `false`。|
|[`pointer`](#pointer)|获取指向内部存储值的指针，在解引用之前，请确保最后一次调用构造函数时 `_Tp` 类型的内存布局适用于当前的 `_Tp` 类型。|
|[`reference`](#reference)|[`pointer()`](#pointer) 的解引用。|
|[`value`](#value)|获取 `_Tp` 类型的内部存储值，允许和最后一次调用构造函数时 `_Tp` 类型不一致，不一致时尝试使用内置类型转换器进行转换。|
|[`size`](#size)|获取最后一次调用构造函数时 `_Tp` 的 `sizeof(_Tp)`。|

### 运算符

|名称|描述|
|-|-|
|[`operator=`](#operator)|析构函数。|



### `KwargsValue`

空的构造函数，默认包含一个 `int` 类型的值 $0$。

```cpp
constexpr KwargsValue() = default;
```

使用任意类型的值进行构造，对于 **结构体** 或 **共用体** 或 _大于_ $8$ 个字节的类型，内部将存储一个指向该对象的指针（请注意它的生命周期），否则，对值进行字节级的拷贝。

```cpp
template<typename _Tp>
constexpr KwargsValue(const _Tp& __value) noexcept;
```

使用任意类型的值进行构造，对于 **结构体** 或 **共用体** 或 _大于_ $8$ 个字节的类型，将使用 `new` 申请一个新对象，并使用 `std::move()` 进行资源的转移。

```cpp
template<typename _Tp>
constexpr KwargsValue(_Tp&& __value);
```

字符串字面量的偏特化构造函数，内部存储它的首地址。

```cpp
template<std::size_t _Size>
constexpr KwargsValue(const char (&__value)[_Size]) noexcept;
```


### `~KwargsValue`

如果值是在构造函数中使用 `new` 申请的新对象，则调用该对象的 **析构函数** 并 `delete` 它。

```cpp
_KWARGS_DESTRUCTOR_CONSTEXPR ~KwargsValue() noexcept;
```

> [!TIP]
> `_KWARGS_DESTRUCTOR_CONSTEXPR` 在 C++20 中被定义为 `constexpr`，在 C++17 中被定义为 `inline`。

### `typeName`

获取最后一次调用构造函数时 `_Tp` 的字符串形式，不同的编译器可能有不同的结果。

```cpp
constexpr std::string_view typeName() const noexcept;
```


### `typeHashCode`

获取最后一次调用构造函数时 `_Tp` 的哈希值，通过 `std::type_info::hash_code()` 进行计算。

```cpp
constexpr std::size_t typeHashCode() const noexcept;
```


### `valueTypeHashCode`

获取最后一次调用构造函数时 `_Tp` 的 `value_type` 类型的哈希值，如果不存在 `_Tp::value_type`，则同 [`typeHashCode()`](#typehashcode)。

```cpp
constexpr std::size_t valueTypeHashCode() const noexcept;
```


### `hasValueType`

检查最后一次调用构造函数时 `_Tp` 是否存在 `value_type`，如果存在，返回 `true`，否则，返回 `false`。

```cpp
constexpr bool hasValueType() const noexcept;
```

### `isSameType`

检查最后一次调用构造函数时 `_Tp` 是否是指定的类型，如果和指定的类型相同，返回 `true`，否则，返回 `false`。

```cpp
template<typename _Tp>
constexpr bool isSameType() const noexcept;
```

> [!TIP]
> 如果是枚举类型，将退化成底层类型，然后在进行类型比较。

### `isInteger`

检查最后一次调用构造函数时 `_Tp` 是否是整型或枚举类型，如果是，返回 `true`，否则，返回 `false`。

```cpp
constexpr bool isInteger() const noexcept;
```

### `isRealNumber`

检查最后一次调用构造函数时 `_Tp` 是否是浮点型，如果是，返回 `true`，否则，返回 `false`。

```cpp
constexpr bool isRealNumber() const noexcept;
```

### `isEnum`

检查最后一次调用构造函数时 `_Tp` 是否是枚举型，如果是，返回 `true`，否则，返回 `false`。

```cpp
constexpr bool isEnum() const noexcept;
```

### `isStdArray`

检查最后一次调用构造函数时 `_Tp` 是否是 `std::array`，如果是，返回 `true`，否则，返回 `false`。

```cpp
constexpr bool isStdArray() const noexcept;
```


### `isIterable`

检查最后一次调用构造函数时 `_Tp` 是否是可迭代的容器类型，如果是，返回 `true`，否则，返回 `false`。

```cpp
constexpr bool isIterable() const noexcept;
```

> [!TIP] 可迭代的容器类型
> 可迭代的容器类型具有成员函数 `.begin()` 和 `.end()`。

### `pointer`

获取指向内部存储值的指针，在解引用之前，请确保最后一次调用构造函数时 `_Tp` 类型的内存布局适用于当前的 `_Tp` 类型。

```cpp
template<typename _Tp>
constexpr _Tp* pointer() noexcept;

template<typename _Tp>
constexpr const _Tp* pointer() const noexcept;
```

### `reference`

[`pointer()`](#pointer) 的解引用。

```cpp
template<typename _Tp>
constexpr _Tp& reference() noexcept
{ return *pointer<_Tp>(); }

template<typename _Tp>
constexpr const _Tp& reference() const noexcept
{ return *pointer<_Tp>(); }
```

### `value`

获取 `_Tp` 类型的内部存储值，允许和最后一次调用构造函数时 `_Tp` 类型不一致，不一致时尝试使用内置类型转换器进行转换。

```cpp
template<typename _Tp>
constexpr std::enable_if_t<..., _Tp> value() const noexcept;
```

> [!NOTE]
> 由于 **类型擦除**，内部不支持直接调用拷贝函数生成对象副本。

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

### `size`

获取最后一次调用构造函数时 `_Tp` 的 `sizeof(_Tp)`。

特别的，如果 `_Tp` 是一个字符串字面量类型，则返回字符个数。

```cpp
constexpr std::uint32_t size() const noexcept;
```

### `operator=`

对内部存储值做拷贝或移动，当前最后一次调用构造函数时的 `_Tp` 类型也将被重新定义为 `__other` 最后一次调用构造函数时的 `_Tp` 类型。

```cpp
constexpr KwargsValue& operator=(const KwargsValue& __other);

constexpr KwargsValue& operator=(KwargsValue&& __other) noexcept;
```
