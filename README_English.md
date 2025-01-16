<div align="center">

# cpp-kwargs

[![STD-CPP](https://img.shields.io/badge/STD%20C%2B%2B-20-darkorange?style=for-the-badge&logo=C%2B%2B&logoColor=white&logoSize=auto&labelColor=darkcyan)](https://en.cppreference.com/w/cpp/20)
[![CHINESE](https://img.shields.io/badge/Chinese-goto-lavender?style=for-the-badge&logo=googletranslate&logoColor=white&logoSize=auto&labelColor=lightskyblue)](./README.md)
[![VIEW-CODE](https://img.shields.io/badge/VIEW-CODE-greed?style=for-the-badge&logo=github&logoColor=white&logoSize=auto&labelColor=blue)](https://github.com/huanhuanonly/cpp-kwargs/blob/main/CppKwargs.h)
[![EXAMPLES-MORE](https://img.shields.io/badge/EXAMPLES-MORE-gold?style=for-the-badge&logo=openbugbounty&logoColor=white&logoSize=auto&labelColor=orange)](https://github.com/huanhuanonly/cpp-kwargs/blob/main/test.cpp)
[![DOCS](https://img.shields.io/badge/Detailed-Documentation-darkcyan?style=for-the-badge&logo=googledocs&logoColor=white&labelColor=cornflowerblue)](https://github.com/huanhuanonly/cpp-kwargs/tree/main/docs_English)

**_cpp-kwargs is a library that implements Python-like `**kwargs` parameter passing in C++._**

**_It encapsulates a [`Kwargs`](./docs_English/Kwargs.md) class using C++'s powerful template programming to achieve this functionality._**

<details>
    <summary>
            Python's <code>**kwargs</code>
    </summary>

_In Python, `**Kwargs` is used in function definitions to accept any number of keyword arguments. It wraps all arguments passed as `Key=Value` into a dictionary, which can be accessed via `kwargs` inside the function. `**kwargs` allows functions to flexibly accept a variable number of keyword arguments, enhancing code scalability. [Official Documentation](https://docs.python.org/3/tutorial/controlflow.html#keyword-arguments)._

</details>

</div>

## Application Comparison

* _Python (**kwargs)_ and _cpp-kwargs_ both support:

  - [x] **Keys** in **any order**;
  - [x] **Missing** or **extra** keys;
  - [x] **Limited key names**;
  - [x] **Values of any type**;
  - [x] Preserving **original value types**;

* _cpp-kwargs_ additionally supports:

  - [x] Automatic [type conversion](#supported-built-in-type-automatic-conversion) (when input and output types are mismatched);
  - [x] **Smaller overhead**, `Kwargs` uses `constexpr` internally as much as possible, providing results **at compile time** (if conditions are met);
  - [x] Key names are **case-insensitive** (_optional_);

> [!TIP]
> It's recommended to use C++ $20$, as the `STL` in C++ $20$ is more likely to be declared as `constexpr`, and code _writing_ and _testing_ are all done in C++ $20$.

* Only _Python (**kwargs)_ supports:

  - [ ] Dynamic return value types;

> [!TIP]
> In C++, the return type must be determined at compile time.

### Application in Functions

<details open>
<summary>Function Prototypes</summary>

- In _**Python**_:
  ```py
  # Any key name
  def func(**kwargs): ...
  
  # Limited key names (with default values)
  def func(*, name='empty_name', old=0): ...
  ```

- In _**C++**_:
  ```cpp
  // Any key name
  auto func(Kwargs<> kwargs = {})
  {...}

  // Limited key names (no need for default values)
  auto func(Kwargs<"name"_opt, "old"_opt> kwargs = {})
  {...}
  ```

</details>

<details open>
<summary>External Calls</summary>

- In _**Python**_:
  ```py
  # Normal
  func(name='huanhuanonly', old=18)

  # Unexpected type
  func(name='huanhuanonly', old='18')

  # Reversed order
  func(old=18, name='huanhuanonly')

  # Empty arguments
  func()
  ```

- In _**C++**_:
  ```cpp
  // Normal
  func({ {"name", "huanhuanonly"}, {"old", 18} });

  // Unexpected type
  func({ {"name", "huanhuanonly"}, {"old", "18"} });

  // Reversed order
  func({ {"old", 18}, {"name", "huanhuanonly"} });

  // Empty arguments
  func()
  ```

</details>

<details open>
<summary>Internal Value Access</summary>

- In _**Python**_:
  ```py
  str(kwargs['name']) if 'name' in kwargs else 'empty_name'

  int(kwargs['old']) if 'old' in kwargs else 0
  ```

- In _**C++**_:
  ```cpp
  kwargs["name"].valueOr<std::string>("empty_name")
  kwargs["old"].valueOr<int>(0)

  // kwargs["name"].hasValue()
  // Equivalent to
  // if 'name' in kwargs
  ```

</details>


### Application in Class Constructors

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

The following constructors for `People` are all valid:

- `Font()`
  - Equal to: `Font{ std::string(), 9, 0.00f, false }`

- `Font({ })`
  - Equal to: `Font{ std::string(), 9, 0.00f, false }`

- `Font({ {"name", "Arial"}, {"italic", true} })`
  - Equal to: `Font{ std::string("Arial"), 9, 0.00f, true }`

- `Font({ {"italic", "true"}, {"name", "Arial"} })`
  - Equal to: `Font{ std::string("Arial"), 9, 0.00f, true }`

- `Font({ {"i", "True"}, {"faceName", "Arial"} })`
  - Equal to: `Font{ std::string("Arial"), 9, 0.00f, true }`

- `Font({ {"size", 18}, {"escapement", 45} })`
  - Equal to: `Font{ std::string(), 18, 45.00f, false }`

- `Font({ {"size", "18"}, {"escapement", "49.2"} })`
  - Equal to: `Font{ std::string(), 18, 49.20f, false }`

- `Font({ {"size", 18.8}, {"escapement", 49.2}, {"i", 't'} })`
  - Equal to: `Font{ std::string(), 18, 49.20f, true }`

<details>

<summary>
    <h3>
        Simple Example: <code>printList()</code> in Python and C++
    </h3>
</summary>

- In Python
  ```py
  def printList(value: list, /, *, sep = ', ', end = '\n'):

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

Call:

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

</details>

For more usage examples, click [![EXAMPLES-MORE](https://img.shields.io/badge/EXAMPLES-MORE-gold?style=plastic&logo=openbugbounty&logoColor=white&logoSize=auto&labelColor=orange)](https://github.com/huanhuanonly/cpp-kwargs/blob/main/test.cpp).

## Importing into Your Project

### Clone this repository
```git
clone https://github.com/huanhuanonly/cpp-kwargs.git
```

### Configure in `CMakeList.txt`
- CMakeList.txt
```cmake
set (CPP_KWARGS_REPOS "https://github.com/huanhuanonly/cpp-kwargs.git")
set (CPP_KWARGS_PATH "${CMAKE_SOURCE_DIR}/cpp-kwargs")

include (FetchContent)

if (NOT EXISTS CPP_KWARGS_PATH)
	FetchContent_Declare (
        CppKwargs
        GIT_REPOSITORY ${CPP_KWARGS_REPOS}
        GIT_TAG main
        GIT_SHALLOW TRUE
        SOURCE_DIR ${CPP_KWARGS_PATH}
    )

    FetchContent_MakeAvailable (CppKwargs)
endif()

include_directories (${CPP_KWARGS_PATH})

target_sources (YourExecutable PUBLIC "${CPP_KWARGS_PATH}/CppKwargs.h")
```

- main.cpp
```cpp
#include <CppKwargs.h>
```

> [!TIP]
> This project only requires a single header file to run.

### Set `KwargsKey` to be case-insensitive

* Define `KWARGSKEY_CASE_INSENSITIVE` before the first `#include "CppKwargs.h"`.

* Alternatively, add the following line to your project's CMakeList.txt file:
```cmake
target_compile_definitions (YourExecutable PRIVATE KWARGSKEY_CASE_INSENSITIVE)
```

## Supported Built-in Type Automatic Conversion

- All integer and floating-point type conversions.
- `std::string` $\longleftrightarrow$ `std::string_view`.
- `std::string` / `std::string_view` $\longleftrightarrow$ `const char*`.
- `std::vector<char>` / `std::array<char>` / `std::string_view` $\longrightarrow$ `const char*` (does not guarantee `\0` terminator).
- `const char*` / `std::string` / `std::string_view` $\longrightarrow$ `Integer` / `Floating point`.
- `Integer` / `Floating point` $\longrightarrow$ `std::string`.
- `const char*` / `std::string` / `std::string_view` $\longleftrightarrow$ `char` / `uchar` (takes the first character, returns `\0` if empty).
- `bool` $\longrightarrow$ `const char*` / `std::string` / `std::string_view` (`true` or `false`).
- `"true"` $\longrightarrow$ `true`, `"false"` $\longrightarrow$ `false`.
- Iterable containers (with `.begin()`, `.end()`, and _forward-iterator_) $\longrightarrow$ Insertable containers (with `.push_back()` / `.insert()` / `.push()` or `.append()`).

> [!NOTE]
> Both containers must have `::value_type`, but the value types don't need to match. If they don't, conversion will follow the above rules.

---

- Copyright $2024\text{-}2025$ [Yang Huanhuan](https://github.com/huanhuanonly) (3347484963@qq.com). All rights reserved.

- Created by [Yang Huanhuan](https://github.com/huanhuanonly) on $December$ $29, 2024, 14:40:45$.

- Goodbye $2024$, ***Hello*** $2025$ !