# `operator""_opt()`

> [!NOTE]
> If you find it difficult to input the underscore, you can modify its definition in `CppKwargs.h`. However, be aware that doing so may trigger compiler warnings.

This user-defined literal operator constructs a [`KwargsKey`](./KwargsKey.md) from literal values. It provides several overloaded versions:

### Version 1: String Literals

```cpp
constexpr
KwargsKey operator""_opt(
    const char* const __str,
    std::size_t __size) noexcept;
```

This version matches **string literals**.

For example:

- `"data"_opt`
- `"123"_opt`
- `""_opt`

### Version 2: Integer and Floating-Point Literals

```cpp
template<char... _String> constexpr KwargsKey operator""_opt() noexcept
```

This version matches **integer literals** and **floating-point literals**.

For example:

- `123_opt`
- `0_opt`
- `20.02_opt`
- `999999999999999999999999999999999999_opt`
- `9999999999999999.9999999999999999999_opt`

### Version 3: Character Literals

```cpp
constexpr KwargsKey operator""_opt(char __ch) noexcept
```

This version matches **character literals**.

For example:

- `'a'_opt`
- `'A'_opt`
- `'\n'_opt`