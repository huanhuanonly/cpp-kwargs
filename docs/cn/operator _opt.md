# `operator""_opt()`

> [!NOTE]
> 如果觉得下划线不好输入可以直接修改在 `CppKwargs.h` 中的定义，但是会受到编译器的警告。

使用 **字面量** 构造一个 KwargsKeyLiteral（[`KwargsKey`](./KwargsKey.md) 的适配器），有以下几个版本的重载：

```cpp
constexpr
KwargsKeyLiteral operator""_opt(
    const char* const __str,
    std::size_t __size) noexcept;
```

匹配 _字符串字面量_。

例如：

- `"data"_opt`
- `"123"_opt`
- `""_opt`

```cpp
template<char... _String> constexpr KwargsKeyLiteral operator""_opt() noexcept
```

匹配 _整数字面量_，_浮点数字面量_。

例如：

- `123_opt`
- `0_opt`
- `20.02_opt`
- `999999999999999999999999999999999999_opt`
- `9999999999999999.9999999999999999999_opt`

```cpp
constexpr KwargsKeyLiteral operator""_opt(char __ch) noexcept
```

匹配 _字符字面量_。

例如：

- `'a'`
- `'A'`
- `'\n'`