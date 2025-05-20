# `DataItem` structure

A wrapper for [`KwargsValue`](./KwargsValue.md), similar to [`std::optional`](https://en.cppreference.com/w/cpp/utility/optional).

## Syntax

```cpp
struct DataItem;
```

---

## Members

### Constructors

| Name                    | Description  |
| ----------------------- | ------------ |
| [`DataItem`](#dataitem) | Constructor. |

### Functions

| Name                    | Description                            |
| ----------------------- | -------------------------------------- |
| [`valueOr`](#valueor)   | Returns the stored value or a default. |
| [`hasValue`](#hasvalue) | Checks whether a value is present.     |

### Operators

| Name                       | Description                        |
| -------------------------- | ---------------------------------- |
| [`operator->`](#operator-) | Accesses the stored `KwargsValue`. |

---

### `DataItem`

Constructor.

```cpp
constexpr DataItem(const KwargsValue* __that = nullptr) noexcept;
```

---

### `operator->`

Returns a pointer to the stored data.

```cpp
constexpr const KwargsValue* operator->() const noexcept;
```

---

### `hasValue`

Checks if a value is present for the corresponding key. Returns `true` if there is a value; otherwise, returns `false`.

```cpp
constexpr bool hasValue() const noexcept;
```

---

### `valueOr`

Returns the stored value if available; otherwise, constructs and returns a default value.

```cpp
template<typename _ValueType, typename... _Args>
constexpr _ValueType valueOr(_Args&&... __args) const noexcept;
```

* ***`_ValueType`***
  The desired return type. Must be specified explicitly. If the actual value type is different, the built-in type converter will attempt conversion.

* ***`_Args`***
  Types of the arguments to be forwarded.

* ***`__args`***
  Arguments forwarded to construct the default value if no stored value is present.

#### Example

```cpp
dataItem.valueOr<std::string>(5, 'c');
```

If `dataItem` has no stored value, this will return `std::string(5, 'c')`, which is equivalent to `"ccccc"`.
