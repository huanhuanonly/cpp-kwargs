# `KwargsValue` class

A value wrapper capable of storing values of any type, similar to [`std::any`](https://en.cppreference.com/w/cpp/utility/any).

## Syntax

```cpp
class KwargsValue;
```

## Members

### Constructors

| Name                                                 | Description                                                                            |
| ---------------------------------------------------- | -------------------------------------------------------------------------------------- |
| [`KwargsValue()`](#kwargsvalue)                      | Default constructor.                                                                   |
| [`KwargsValue(const KwargsValue&)`](#kwargsvalue)    | Copy constructor.                                                                      |
| [`KwargsValue(KwargsValue&&)`](#kwargsvalue)         | Move constructor.                                                                      |
| [`KwargsValue(const _Tp&)`](#kwargsvalue)            | Stores a value or a pointer to a value of type `_Tp`.                                  |
| [`KwargsValue(_Tp&&)`](#kwargsvalue)                 | Stores a value of type `_Tp` with move semantics.                                      |
| [`KwargsValue(const char (&)[_Size])`](#kwargsvalue) | Partial specialization for string literals, stores the pointer to the first character. |

### Destructor

| Name                             | Description |
| -------------------------------- | ----------- |
| [`~KwargsValue`](#kwargsvalue-1) | Destructor. |

### Functions

| Name                                      | Description                                                                                                   |
| ----------------------------------------- | ------------------------------------------------------------------------------------------------------------- |
| [`typeName`](#typename)                   | Returns the string name of the last `_Tp` used in construction. Compiler-dependent.                           |
| [`typeHashCode`](#typehashcode)           | Returns the hash code of the last `_Tp` using `std::type_info::hash_code()`.                                  |
| [`valueTypeHashCode`](#valuetypehashcode) | Returns the hash code of `_Tp::value_type` if available; otherwise same as [`typeHashCode()`](#typehashcode). |
| [`hasValueType`](#hasvaluetype)           | Checks whether `_Tp::value_type` exists.                                                                      |
| [`isSameType`](#issametype)               | Checks whether the stored type `_Tp` matches a given type.                                                    |
| [`isInteger`](#isinteger)                 | Checks if `_Tp` is an integer or enum type.                                                                   |
| [`isRealNumber`](#isrealnumber)           | Checks if `_Tp` is a floating-point type.                                                                     |
| [`isEnum`](#isenum)                       | Checks if `_Tp` is an enumeration.                                                                            |
| [`isStdArray`](#isstdarray)               | Checks if `_Tp` is `std::array`.                                                                              |
| [`isIterable`](#isiterable)               | Checks if `_Tp` is an iterable container.                                                                     |
| [`pointer`](#pointer)                     | Returns a pointer to the internal value. Ensure the type layout matches before dereferencing.                 |
| [`reference`](#reference)                 | Dereferences [`pointer()`](#pointer).                                                                         |
| [`value`](#value)                         | Retrieves the stored value with optional implicit conversion.                                                 |
| [`size`](#size)                           | Returns `sizeof(_Tp)` at construction time.                                                                   |

### Operators

| Name                     | Description          |
| ------------------------ | -------------------- |
| [`operator=`](#operator) | Assignment operator. |

---

### `KwargsValue`

#### Default constructor

Initializes with a default `int` value of `0`.

```cpp
constexpr KwargsValue() = default;
```

#### Copy/Move/Template constructor

Constructs from any value. If the type is a **struct**, **union**, or **larger than 8 bytes**, a pointer to the object is stored (lifetime considerations apply). Otherwise, the value is stored via byte-level copy.

```cpp
template<typename _Tp>
constexpr KwargsValue(const _Tp& __value) noexcept;
```

For rvalue references, constructs by dynamically allocating the object and moving it.

```cpp
template<typename _Tp>
constexpr KwargsValue(_Tp&& __value);
```

Partial specialization for string literals; stores a pointer to the first character.

```cpp
template<std::size_t _Size>
constexpr KwargsValue(const char (&__value)[_Size]) noexcept;
```

---

### `~KwargsValue`

If the value was dynamically allocated during construction, it is destroyed and freed.

```cpp
_KWARGS_DESTRUCTOR_CONSTEXPR ~KwargsValue() noexcept;
```

> [!TIP]
> `_KWARGS_DESTRUCTOR_CONSTEXPR` is defined as `constexpr` in C++20 and `inline` in C++17.

---

### `typeName`

Returns the string representation of `_Tp`. The result may vary by compiler.

```cpp
constexpr std::string_view typeName() const noexcept;
```

---

### `typeHashCode`

Returns the hash code of `_Tp`.

```cpp
constexpr std::size_t typeHashCode() const noexcept;
```

---

### `valueTypeHashCode`

Returns the hash code of `_Tp::value_type`, or falls back to `typeHashCode()` if unavailable.

```cpp
constexpr std::size_t valueTypeHashCode() const noexcept;
```

---

### `hasValueType`

Checks for the existence of `_Tp::value_type`.

```cpp
constexpr bool hasValueType() const noexcept;
```

---

### `isSameType`

Checks whether the stored type `_Tp` matches the provided type.

```cpp
template<typename _Tp>
constexpr bool isSameType() const noexcept;
```

> \[!TIP]
> For enums, it compares the underlying type instead.

---

### `isInteger`

Checks if `_Tp` is an integer or enum.

```cpp
constexpr bool isInteger() const noexcept;
```

---

### `isRealNumber`

Checks if `_Tp` is a floating-point type.

```cpp
constexpr bool isRealNumber() const noexcept;
```

---

### `isEnum`

Checks if `_Tp` is an enum.

```cpp
constexpr bool isEnum() const noexcept;
```

---

### `isStdArray`

Checks if `_Tp` is a `std::array`.

```cpp
constexpr bool isStdArray() const noexcept;
```

---

### `isIterable`

Checks if `_Tp` is an iterable container type.

```cpp
constexpr bool isIterable() const noexcept;
```

> \[!TIP]
> Iterable containers are those with `.begin()` and `.end()` member functions.

---

### `pointer`

Returns a pointer to the stored value. Ensure type layout compatibility before dereferencing.

```cpp
template<typename _Tp>
constexpr _Tp* pointer() noexcept;

template<typename _Tp>
constexpr const _Tp* pointer() const noexcept;
```

---

### `reference`

Dereferences the pointer returned by [`pointer()`](#pointer).

```cpp
template<typename _Tp>
constexpr _Tp& reference() noexcept
{ return *pointer<_Tp>(); }

template<typename _Tp>
constexpr const _Tp& reference() const noexcept
{ return *pointer<_Tp>(); }
```

---

### `value`

Returns the stored value of type `_Tp`, with optional conversion if types differ.

```cpp
template<typename _Tp>
constexpr std::enable_if_t<..., _Tp> value() const noexcept;
```

> [!NOTE]
> Due to type erasure, direct copying is not supported.

> [!WARNING]
> If conversion fails, `assert()` is triggered. If `NODEBUG` is defined, it returns a default-constructed `_Tp`.

<details>
<summary>
    <h4>Why not use <code>throw</code> for errors?</h4>
</summary>

In `constexpr` functions, which may execute at compile-time, `throw` is not permitted since it's a runtime operation.

</details>

---

### `size`

Returns the size of the stored type `_Tp`. If `_Tp` is a string literal, returns the number of characters.

```cpp
constexpr std::uint32_t size() const noexcept;
```

---

### `operator=`

Performs copy or move assignment and updates the stored type to that of the `__other`'s `_Tp`.

```cpp
constexpr KwargsValue& operator=(const KwargsValue& __other);

constexpr KwargsValue& operator=(KwargsValue&& __other) noexcept;
```
