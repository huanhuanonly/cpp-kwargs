# `operator""_opt()`

User-defined literal operators for constructing a `KwargsKeyLiteral` (an adapter for [`KwargsKey`](./KwargsKey.md)). Several overloads are provided:

---

```cpp
constexpr KwargsKeyLiteral operator""_opt(const char* const __str, std::size_t __size) noexcept;
```

Matches **string literals**.

Examples:

* `"data"_opt`
* `"123"_opt`
* `""_opt`

---

```cpp
template<char... _String>
constexpr KwargsKeyLiteral operator""_opt() noexcept;
```

Matches **integer** and **floating-point literals**.

Examples:

* `123_opt`
* `0_opt`
* `20.02_opt`
* `999999999999999999999999999999999_opt`
* `9999999999999999.9999999999999999999_opt`

---

```cpp
constexpr KwargsKeyLiteral operator""_opt(char __ch) noexcept;
```

Matches **character literals**.

Examples:

* `'a'_opt`
* `'A'_opt`
* `'\n'_opt`
