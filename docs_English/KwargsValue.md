# class `KwargsValue`

## Description

A wrapper for values, capable of storing values of any type, similar to [`std::any`](https://en.cppreference.com/w/cpp/utility/any).

## Public Member Functions

### `KwargsValue()`

```cpp
constexpr KwargsValue() = default;
```

Default constructor, which includes an `int` type value of $0$ by default.

```cpp
template<typename _Tp>
constexpr KwargsValue(const _Tp& __value) noexcept
```

Constructs using a value of any type. For structures, unions, or types larger than $8$ bytes, it stores the address of the data (note the value's lifetime). Otherwise, it directly copies the value at the byte level.

```cpp
template<typename _Tp>
inline KwargsValue(_Tp&& __value)
```

Constructs using a value of any type. If the value is an **rvalue** and `_Tp` is a **structure**, **union**, or a type larger than $8$ bytes, it will allocate a new object using `new` and use `std::move()` to transfer the resource. Otherwise, it constructs the same as `KwargsValue(const _Tp&)`.

```cpp
template<std::size_t _Size>
constexpr KwargsValue(const char (&__value)[_Size]) noexcept
```

Constructs using a string literal, directly storing the address of the string literal.

### `~KwargsValue()`

```cpp
constexpr ~KwargsValue() noexcept
```

If the value was allocated with `new` in the constructor, it calls the value's **destructor** and `delete` the allocated object.

### `typeHashCode()`

```cpp
constexpr std::size_t typeHashCode() const noexcept
```

Returns the hash value of the type of the stored value, obtained using `std::type_info::hash_code()`.

### `valueTypeHashCode()`

```cpp
constexpr std::size_t valueTypeHashCode() const noexcept
```

Assuming the current value's type is `_Tp`, if `_Tp` is a class with a `value_type` defined, it returns `typeid(_Tp::value_type).hash_code()`. Otherwise, it returns `typeid(_Tp).hash_code()`.

### `hasValueType()`

```cpp
constexpr bool hasValueType() const noexcept
```

Assuming the current value's type is `_Tp`, if `_Tp` is a class with a `value_type` defined, it returns `true`; otherwise, it returns `false`.

### `isSameType()`

```cpp
template<typename _Tp>
constexpr bool isSameType() const noexcept
```

Returns `true` if the current value's type is `_Tp`; otherwise, returns `false`.

### `isInteger()`

```cpp
constexpr bool isInteger() const noexcept
```

Returns `true` if the current value's type is an integer type; otherwise, returns `false`.

### `isRealNumber()`

```cpp
constexpr bool isRealNumber() const noexcept
```

Returns `true` if the current value's type is a floating-point type; otherwise, returns `false`.

### `isStdArray()`

```cpp
constexpr bool isStdArray() const noexcept
```

Returns `true` if the current value's type is a `std::array`; otherwise, returns `false`.

### `isIterable()`

```cpp
constexpr bool isIterable() const noexcept
```

Returns `true` if the current value's type is iterable; otherwise, returns `false`.

> [!TIP]
> The iterable type here needs to have `.begin()` and `.end()` member functions.

### `value()`

```cpp
template<typename _Tp>
constexpr std::enable_if_t<..., _Tp> value() const noexcept
```

Converts the stored value to type `_Tp` and returns it, using the built-in type converter for the conversion.

> [!WARNING]
> If the conversion fails, `assert()` will be called, causing a crash. If the `NODEBUG` macro is defined, it will return `_Tp()`.

<details>
    <summary>
        <h4>
            Why not throw an exception using <code>throw</code>?
        </h4>
    </summary>

In `constexpr` functions, conditions are evaluated at compile time, and `throw` is a runtime operation.
</details>

### `size()`

```cpp
constexpr std::uint32_t size() const noexcept
```

Assuming the current value's type is `_Tp`, returns `sizeof(_Tp)`.

Specially, if `_Tp` is a string literal type, it returns the number of characters in the string.

## Public Operator Overloading

### `operator=()`

```cpp
inline KwargsValue& operator=(const KwargsValue& __other)
```

Copy assignment.

```cpp
inline KwargsValue& operator=(KwargsValue&& __other) noexcept
```

Move assignment.