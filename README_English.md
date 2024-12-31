<div align="center">

# cpp-kwargs

[![STD-CPP](https://img.shields.io/badge/STD%20C%2B%2B-20-darkorange?style=for-the-badge&logo=C%2B%2B&logoColor=white&logoSize=auto&labelColor=darkcyan)](https://en.cppreference.com/w/cpp/20)
[![CHINESE](https://img.shields.io/badge/CHINESE-goto-lavender?style=for-the-badge&logo=googletranslate&logoColor=white&logoSize=auto&labelColor=lightskyblue)](./README.md)
[![VIEW-CODE](https://img.shields.io/badge/VIEW-CODE-greed?style=for-the-badge&logo=github&logoColor=white&logoSize=auto&labelColor=blue)](https://github.com/huanhuanonly/cpp-kwargs/blob/main/CppKwargs.h)
[![EXAMPLES-MORE](https://img.shields.io/badge/EXAMPLES-MORE-gold?style=for-the-badge&logo=openbugbounty&logoColor=white&logoSize=auto&labelColor=orange)](https://github.com/huanhuanonly/cpp-kwargs/blob/main/test.cpp)


_It is well-known that Python's `**Kwargs` is used to accept an arbitrary number of keyword arguments in function definitions. It wraps all parameters passed in the `Key=Value` form into a dictionary, which can be accessed via `kwargs` inside the function. `**kwargs` allows a function to flexibly accept an undefined number of keyword arguments, improving the extensibility of the code._

**_This repository encapsulates a [`Kwargs`](#class-kwargs) class using C++'s powerful template programming to achieve this functionality._**

</div>

- [x] Parameters in the argument list can be **optional** and **default**;

- [x] The **order of the parameters** in the argument list is **not important**;

- [x] [Built-in type auto-converters](#supported-built-in-type-automatic-conversion) automatically convert types when input and output types are inconsistent;

- [x] Low overhead, using `constexpr` as much as possible, with results determined at compile time;

> It is recommended to use C++ $20$. In C++ $20$, more of the `STL` is declared as `constexpr`, but it is best not to use anything lower than C++ $17$.

## Sample Demonstrations

### Using in a class constructor

```cpp
struct People
{
    std::string name;
    int old;
    float height;

    // Or Kwargs<> kwargs = {} without checking.
    People(Kwargs<"name"_opt, "old"_opt, "height"_opt> kwargs = {})
    {
        this->name = kwargs["name"].valueOr<std::string>("MyName");

        this->old = kwargs["old"].valueOr<int>(18);

        this->height = kwargs["height"].valueOr<float>(1.75);
    }
};
```

The following ways to construct a `People` object are all valid:

- `People()`
- `People({ })`
- `People({ {"name", "huanhuanonly"} })`
- `People({ {"name"_opt, "huanhuanonly"} })`
- `People({ {"old"_opt, 16} })`
- `People({ {"old", 16ULL}, {"name", "huanhuanonly"} })`
- `People({ {"height", 1.80} })`
- `People({ {"height", 2} })`

### Print list function in Python and C++

- In Python

    ```py
    def printList(value : list, /, *, sep = ', ', end = '\n'):

        if len(value) == 0:
            return

        for i in range(len(value) - 1):
            print(value[i], end=sep)

        print(value[-1], end=end)
    ```

- In C++

    ```cpp
    void printList(
        const std::vector<int>& value,
        Kwargs<"sep"_opt, "end"_opt> kwargs = { })
    {
        if (value.empty())
            return;

        for (std::size_t i = 0; i < value.size() - 1; ++i)
            std::cout << value[i],
            std::cout << kwargs["sep"].valueOr<std::string_view>(", ");

        std::cout << value.back();
        std::cout << kwargs["end"].valueOr<std::string_view>("\n");
    }
    ```

Calling:

- In Python
    ```py
    printList([1, 4, 3, 3, 2, 2, 3], sep=' | ', end='.')
    ```

- In C++
    ```cpp
    printList(
        {1, 4, 3, 3, 2, 2, 3},
        { {"sep", " | "}, {"end", '.'} });
    ```

## Download and Import

Download the file from [https://github.com/huanhuanonly/cpp-kwargs/blob/main/CppKwargs.h](https://github.com/huanhuanonly/cpp-kwargs/blob/main/CppKwargs.h) by clicking **Download raw file**, then move the file to your project directory. Include the following code in your project source code:
```cpp
#include "CppKwargs.h"
```

It is recommended to use C++ $20$.

## CppKwargs.h

### class `Kwargs`

The type of `**kwargs`, which internally maintains a `std::initializer_list<std::pair<KwargsKey, KwargsValue>>`.
```cpp
template<KwargsKey::value_type... _OptionalList>
class Kwargs {...}
```

* `_OptionalList`: A list of supported keys, constructed using literals with the `_opt` suffix. If a key not in the list is passed in, an `assert()` will cause a crash.

> If `_OptionalList` is empty, no checks will be made.

### class `KwargsKey`

The type of the key, which can be constructed using the key name (`const char*`):

```cpp
class KwargsKey
{
    using value_type = std::uint64_t;
}
```

Defines the user-defined literal suffix `operator""_opt()`, which returns a value of type `KwargsKey` (and `KwargsKey` can be implicitly converted to `KwargsKey::value_type`).

The following usages are valid:

* `"cpp-kwargs"_opt`
* `'c'_opt`
* `1314_opt`
* `1314.520_opt`
* `99999999999999999999999999999999_opt`

> [!NOTE]
> If typing underscores is inconvenient, you can modify the definition in `CppKwargs.h`, but it will generate compiler warnings.

### class `KwargsValue`

```cpp
class KwargsValue {...}
```

The value type, similar to `std::any`, can be constructed using any type. The difference is that it has built-in type converters, so the input and output types do not necessarily need to be consistent. If they are not, it will attempt to perform a conversion (if possible).

> [!NOTE]
> It does not use `std::any` internally, but it still ensures correct destruction of objects.

- For **non-`class`**, **non-`union`** types of size **less than or equal to** 8 bytes, a byte-level copy will be made.
- When using the `const auto&` version of the constructor for `class`, `union`, and types **larger than** 8 bytes, it will internally use a pointer to the original data, not copy it.
- When using the `auto&&` version of the constructor for `class`, `union`, and types **larger than** 8 bytes, it will use `new` to allocate dynamic memory and use `std::move` to transfer resources.

### Supported Built-in Type Automatic Conversion

- Conversion between all integer and floating point types.
- `std::string` $\longleftrightarrow$ `std::string_view`.
- `std::string` / `std::string_view` $\longleftrightarrow$ `const char*`.
- `std::vector<char>` / `std::array<char>` / `std::string_view` $\longrightarrow$ `const char*` (no guarantee of null-termination).
- `const char*` / `std::string` / `std::string_view` $\longrightarrow$ `Integer` / `Floating point`.
- `Integer` / `Floating point` $\longrightarrow$ `std::string`.
- `const char*` / `std::string` / `std::string_view` $\longleftrightarrow$ `char` / `uchar` (takes the first character, returns `\0` if empty).
- `bool` $\longrightarrow$ `const char*` / `std::string` / `std::string_view` (`true` or `false`).
- Iterable containers (with `.begin()`, `.end()`, and _forward iterators_) $\longrightarrow$ Insertable containers (with `.push_back()` / `.insert()` / `.push()` or `.append()`).

> [!NOTE]
> Both containers must contain the same `::value_type` type. The value types do not need to be the same; if they differ, conversion will follow the rules above.

---

- Copyright 2024 [Yang Huanhuan](https://github.com/huanhuanonly) (3347484963@qq.com). All rights reserved.

- Created by [Yang Huanhuan](https://github.com/huanhuanonly) on $December$ $29, 2024, 14:40:45$.

- Goodbye $2024$, ***Hello*** $2025$ !