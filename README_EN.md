<div align="center">

# cpp-kwargs

[![STD-CPP](https://img.shields.io/badge/STD%20C%2B%2B-17-darkorange?style=for-the-badge&logo=C%2B%2B&logoColor=white&logoSize=auto&labelColor=darkcyan)](https://en.cppreference.com/w/cpp/17)
[![STD-CPP](https://img.shields.io/badge/STD%20C%2B%2B-20-darkorange?style=for-the-badge&logo=C%2B%2B&logoColor=white&logoSize=auto&labelColor=darkcyan)](https://en.cppreference.com/w/cpp/20)
[![CHINESE](https://img.shields.io/badge/%E7%AE%80%E4%BD%93%E4%B8%AD%E6%96%87-SWITCH-lavender?style=for-the-badge&logo=googletranslate&logoColor=white&logoSize=auto&labelColor=lightskyblue)](./README.md)

[![HEADER-ONLY](https://img.shields.io/badge/HEADER-ONLY-greed?style=for-the-badge&logo=github&logoColor=white&logoSize=auto&labelColor=blue)](https://github.com/huanhuanonly/cpp-kwargs/blob/main/include/kwargs.h)
[![TEST-PASSING](https://img.shields.io/badge/TEST-PASSING-gold?style=for-the-badge&logo=openbugbounty&logoColor=white&logoSize=auto&labelColor=orange)](https://github.com/huanhuanonly/cpp-kwargs/blob/main/tests/test.cpp)
[![DOCS](https://img.shields.io/badge/Detailed-Documentation-darkcyan?style=for-the-badge&logo=googledocs&logoColor=white&labelColor=cornflowerblue)](https://github.com/huanhuanonly/cpp-kwargs/tree/main/docs/en)

[![License](https://img.shields.io/badge/License-MIT-blue?style=for-the-badge)](./LICENSE)

<p align="center">
  <a href="./README.md">简体中文</a>
</p>

_**A header-only C++17/20 library that enables Python-like `**kwargs` style argument passing without any macros or boilerplate, with automatic type conversion support.**_

<details>
  <summary>
    <u> About <code>**kwargs</code> in Python </u>
  </summary>

_In Python, `**kwargs` allows a function to accept any number of keyword arguments. It collects all `Key=Value` pairs into a dictionary, enabling flexible and scalable function definitions. [Official Documentation](https://docs.python.org/3/tutorial/controlflow.html#keyword-arguments)._

</details>

</div>

## Documentation

- [Kwargs](./docs/en/Kwargs.md) | [Kwargs::DataItem](./docs/en/Kwargs_DataItem.md)
- [KwargsKey](./docs/en/KwargsKey.md)
- [KwargsValue](./docs/en/KwargsValue.md)
- [operator""_opt](./docs/en/operator%20_opt.md)

## Features & Examples

### Function Argument Passing

<details open><summary>Function Definition</summary>

- In Python:
  ```py
  # Arbitrary keys
  def func(**kwargs): ...
  
  # Specific keys with defaults
  def func(*, name='empty_name', old=0): ...
  ```

- In C++:
  ```cpp
  // Arbitrary keys
  auto func(Kwargs<> kwargs = {});

  // Specific keys (defaults optional)
  auto func(Kwargs<"name"_opt, "old"_opt> kwargs = {});
  ```

</details>

<details open><summary>External Call</summary>

- In Python:
  ```py
  # Regular
  func(name='huanhuanonly', old=18)

  # Unexpected type
  func(name='huanhuanonly', old='18')

  # Reverse order
  func(old=18, name='huanhuanonly')

  # Empty parameters
  func()
  ```

- In C++ (version 1):
  ```cpp
  // Regular
  func({ "name"_opt="huanhuanonly", "old"_opt=18 });

  // Unexpected type
  func({ "name"_opt="huanhuanonly", "old"_opt="18" });

  // Reverse order
  func({ "old"_opt=18, "name"_opt="huanhuanonly" });

  // Empty parameters
  func();
  ```

- In C++ (version 2):
  ```cpp
  // Regular
  func({ {"name", "huanhuanonly"}, {"old", 18} });

  // Unexpected type
  func({ {"name", "huanhuanonly"}, {"old", "18"} });

  // Reverse order
  func({ {"old", 18}, {"name", "huanhuanonly"} });

  // Empty parameters
  func();
  ```

</details>

<details open><summary>Accessing Values Internally</summary>

- In Python:
  ```py
  str(kwargs['name']) if 'name' in kwargs else 'empty_name'

  int(kwargs['old']) if 'old' in kwargs else 0
  ```

- In C++:
  ```cpp
  kwargs["name"].valueOr<std::string>("empty_name");

  kwargs["old"].valueOr<int>(0);

  // 'kwargs["name"].hasValue()'
  // is equivalent to
  // 'if "name" in kwargs'
  ```

</details>


#### Key Aliases:
- "lastname" and "surname" as aliases for "name"
  ```cpp
  auto func(Kwargs<"name"_opt, "lastname"_opt, "surname"_opt> kwargs = {})
  {
    // Use `or` to connect, the first name must be suffixed with `_opt`
    kwargs["name"_opt or "lastname" or "surname"].valueOr<std::string>();
  }
  ```

#### Automatic Type Conversion:
- `std::vector<int>` $\longrightarrow$ `std::set<int>`
  ```cpp
  auto func(Kwargs<"numbers"_opt> kwargs = {})
  {
    std::cout << kwargs["numbers"].valueOr<std::set<int>>() << std::endl;
  }

  func({ "number"_opt = std::vector<int>{1, 3, 5, 2, 3} }); // Output: [1, 2, 3, 5]
  ```

- `std::list<std::string_view>` $\longrightarrow$ `std::set<double>`
  ```cpp
  auto func(Kwargs<"numbers"_opt> kwargs = {})
  {
    std::cout << kwargs["numbers"].valueOr<std::set<double>>() << std::endl;
  }

  func({ "number"_opt = std::list<std::string_view>{"1", "inf", "infinity", "3.14", "1e3", "1e-2"} });
  // Output: [0.01, 1, 3.14, 1000, inf]
  ```

#### Get Type Name:
```cpp
auto func(Kwargs<"size"_opt> kwargs = {})
{
  std::cout << kwargs["size"]->typeName() << std::endl;
}

func({ "size"_opt = 520 });  // Output: int  
func({ "size"_opt = 520f }); // Output: float  
func({ "size"_opt = "hi" }); // Output: const char*
```

> [!TIP]  
> Results may vary between compilers.

#### Type-based Behavior:
```cpp
auto func(Kwargs<"data"_opt> kwargs = {})
{
  if (kwargs["data"]->isSameType<int>())
  {
    std::cout << "int-data " << kwargs["data"].valueOr<int>() << std::endl;
  }
  else if (kwargs["data"]->isSameType<float>())
  {
    std::cout << "float-data " << kwargs["data"].valueOr<float>() << std::endl;
  }
}

func({ "data"_opt = 996 });   // Output: int-data 996  
func({ "data"_opt = 3.14f }); // Output: float-data 3.14
```

### Class Constructor Arguments

```cpp
struct Font
{
  std::string faceName;
  int size;
  float escapement;
  bool italic;

  // Or Kwargs<> kwargs = {} without checking.
  Font(Kwargs<
    "faceName"_opt, /* Or */ "name"_opt,
    "size"_opt,
    "escapement"_opt,
    "italic"_opt, /* Or */ "i"_opt> kwargs = {})

      : faceName(kwargs["faceName"_opt or "name"].valueOr<std::string>())

      , size(kwargs["size"].valueOr<int>(9))

      , escapement(kwargs["escapement"].valueOr<float>(0.00f))
      
      , italic(kwargs["italic"_opt or "i"].valueOr<bool>(false))
  { }
};
```

> [!TIP]  
> `name` is an alias for `faceName`, and `i` for `italic`.

The following usages are valid:

- `Font()`
  - Construct: `Font{ std::string(), 9, 0.00f, false }`

- `Font({ })`
  - Construct: `Font{ std::string(), 9, 0.00f, false }`

- `Font({ {"name", "Arial"}, {"italic", true} })`
  - Construct: `Font{ std::string("Arial"), 9, 0.00f, true }`

- `Font({ {"italic", "true"}, {"name", "Arial"} })`
  - Construct: `Font{ std::string("Arial"), 9, 0.00f, true }`

- `Font({ "i"_opt="True", "faceName"_opt="Arial" })`
  - Construct: `Font{ std::string("Arial"), 9, 0.00f, true }`

- `Font({ {"size", 18}, {"escapement", 45} })`
  - Construct: `Font{ std::string(), 18, 45.00f, false }`

- `Font({ "size"_opt="18", "escapement"_opt="49.2" })`
  - Construct: `Font{ std::string(), 18, 49.20f, false }`

- `Font({ "size"_opt=18.8, {"escapement", 49.2}, "i"_opt='t' })`
  - Construct: `Font{ std::string(), 18, 49.20f, true }`

See more in [![TEST-PASSING](https://img.shields.io/badge/TEST-PASSING-gold?style=flat&logo=openbugbounty&logoColor=white&logoSize=auto&labelColor=orange)](https://github.com/huanhuanonly/cpp-kwargs/blob/main/tests/test.cpp).

### Feature Comparison

* Supported by both Python (**kwargs) and cpp-kwargs:

  - [x] Arbitrary key order
  - [x] Missing or extra keys
  - [x] Specific key names
  - [x] Arbitrary value types
  - [x] Type information preservation

* Additionally supported by cpp-kwargs:

  - [x] [Automatic type conversion](#supported-automatic-type-conversions)
  - [x] Low overhead, with `constexpr` results at compile time (when applicable)
  - [x] Case-insensitive keys (optional)
  - [x] Key aliases

> [!TIP]  
> C++20 is recommended for better `constexpr` support.

* Only supported by Python:

  - [ ] Dynamic return types

> [!TIP]  
> C++ requires return types to be compile-time determined.

## Integration

### Generic

- Clone the repository:
```bash
git clone https://github.com/huanhuanonly/cpp-kwargs.git
```

- In your `.cpp` or `.h`:
```cpp
#include "cpp-kwargs/include/kwargs.h"
```

### With CMake

- Add the following to your `CMakeLists.txt`:
```cmake
set (CPP_KWARGS_REPOS "https://github.com/huanhuanonly/cpp-kwargs.git")
set (CPP_KWARGS_PATH "${CMAKE_SOURCE_DIR}/cpp-kwargs")

include (FetchContent)

if (NOT EXISTS ${CPP_KWARGS_PATH})
  FetchContent_Declare (
    CppKwargs
    GIT_REPOSITORY ${CPP_KWARGS_REPOS}
    GIT_TAG main
    GIT_SHALLOW TRUE
    SOURCE_DIR ${CPP_KWARGS_PATH}
  )
  FetchContent_MakeAvailable (CppKwargs)
endif()

add_subdirectory (${CPP_KWARGS_PATH})
```

- Then in your `.cpp` or `.h`:
```cpp
#include <kwargs.h>
```

## Optional Macros

- **KWARGSKEY_CASE_INSENSITIVE**  
  Enable case-insensitive `KwargsKey`.

- **KWARGSKEY_LITERAL_SUFFIX_WITHOUT_LEADING_UNDERSCORE**  
  Use literal suffix without leading underscore: `_opt` → `opt`, `_o` → `o`.

- **KWARGSKEY_LITERAL_SHORT_SUFFIX**  
  Shorten suffix `_opt` to `_o`.

<details><summary><b>How to define?</b></summary>

- In your source file before `#include`:
  ```cpp
  #if !defined(KWARGSKEY_CASE_INSENSITIVE)
  #  define KWARGSKEY_CASE_INSENSITIVE
  #endif
  
  #include "kwargs.h"
  ```

- Or in your CMakeLists.txt before `add_subdirectory`:
  ```cmake
  set (KWARGSKEY_CASE_INSENSITIVE)
  add_subdirectory (${CPP_KWARGS_PATH})
  ```
</details>

## Supported Automatic Type Conversions

- All integral and floating-point types are mutually convertible.

- Enum types are treated as their underlying integer types.

> [!TIP]  
> Even if the enum's underlying type is `char`/`uchar`, it will be treated as `std::int8_t`/`std::uint8_t`.

- `std::string` ⇄ `std::string_view`.
- `std::string` / `std::string_view` ⇄ `const char*`.
- `std::vector<char>` / `std::array<char>` / `std::string_view` → `const char*` (null terminator not guaranteed).
- `const char*` / `std::string` / `std::string_view` → `Integer`.
  - Supports `0b`/`0o`/`0x`/`0h` prefixes (case-insensitive), optional `+`/`-` sign.
  - `max` and `min` (case-insensitive) as extreme values.
  - Unsigned types only accept leading `+`.
- `const char*` / `std::string` / `std::string_view` → `Floating point`.
  - Supports scientific notation.
  - `max`, `min`, `inf`, `infinity`, `nan`, `pi`, `e` (case-insensitive) with optional `+`/`-`.
- `Integer` / `Floating point` → `std::string`.
- `const char*` / `std::string` / `std::string_view` ⇄ `char` / `uchar` (uses first character, or `\0` if empty).
- `bool` → `const char*` / `std::string` / `std::string_view` (`"true"` or `"false"`).
- `"true"` / `'t'` (case-insensitive) → `true`.
- `"false"` / `'f'` (case-insensitive) → `false`.
- Iterable containers (with `.begin()`, `.end()` and forward iterators) → Insertable containers.

> [!NOTE]  
> Both containers must have `::value_type`. The value types may differ, and conversion will follow the rules above.

<details open>
  <summary><b><i>Insertable Containers</i></b></summary>
  Must have one of the following members:

  1. `.append()`
  2. `.push_back()`
  3. `.push()`
  4. `.insert()`
  5. `.push_front()`

</details>

> [!IMPORTANT]
> All code passes tests in MSVC/GCC (C++17/20).
> [Found a problem?](https://github.com/huanhuanonly/cpp-kwargs/issues/new)

---

- Copyright $2024\text{-}2025$ [Yang Huanhuan](https://github.com/huanhuanonly) (3347484963@qq.com).

- Created by [Yang Huanhuan](https://github.com/huanhuanonly) on $December$ $29, 2024, 14:40:45$.

- Goodbye $2024$, Hello $2025$ !