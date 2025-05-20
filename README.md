<div align="center">

# cpp-kwargs

[![STD-CPP](https://img.shields.io/badge/STD%20C%2B%2B-17-darkorange?style=for-the-badge&logo=C%2B%2B&logoColor=white&logoSize=auto&labelColor=darkcyan)](https://en.cppreference.com/w/cpp/17)
[![STD-CPP](https://img.shields.io/badge/STD%20C%2B%2B-20-darkorange?style=for-the-badge&logo=C%2B%2B&logoColor=white&logoSize=auto&labelColor=darkcyan)](https://en.cppreference.com/w/cpp/20)
[![ENGLISH](https://img.shields.io/badge/English-SWITCH-lavender?style=for-the-badge&logo=googletranslate&logoColor=white&logoSize=auto&labelColor=lightskyblue)](./README_EN.md)

[![HEADER-ONLY](https://img.shields.io/badge/HEADER-ONLY-greed?style=for-the-badge&logo=github&logoColor=white&logoSize=auto&labelColor=blue)](https://github.com/huanhuanonly/cpp-kwargs/blob/main/include/kwargs.h)
[![TEST-PASSING](https://img.shields.io/badge/TEST-PASSING-gold?style=for-the-badge&logo=openbugbounty&logoColor=white&logoSize=auto&labelColor=orange)](https://github.com/huanhuanonly/cpp-kwargs/blob/main/tests/test.cpp)
[![DOCS](https://img.shields.io/badge/Detailed-Documentation-darkcyan?style=for-the-badge&logo=googledocs&logoColor=white&labelColor=cornflowerblue)](https://github.com/huanhuanonly/cpp-kwargs/tree/main/docs/cn)

[![License](https://img.shields.io/badge/License-MIT-blue?style=for-the-badge)](./LICENSE)

<p align="center">
  <a href="./README_EN.md">English</a>
</p>

_**仅包含头文件的 C++17/20 库可以在没有任何宏或样板代码的情况下使用类似于 Python 的 `**kwargs` 风格传参，并提供一些自动类型转换。**_

<details>
    <summary>
            <u> 关于 Python 中的 <code>**kwargs</code> </u>
    </summary>

_在 Python 中， `**Kwargs` 用于函数定义时接受任意数量的关键字参数。它将所有通过 `Key=Value` 形式传递的参数封装成一个字典，在函数内部可以通过 `kwargs` 访问这些参数，`**kwargs` 使得函数能够灵活地接受不定数量的关键字参数，提升了代码的可扩展性。[官方文档](https://docs.python.org/3/tutorial/controlflow.html#keyword-arguments)。_

</details>

</div>

## 文档

- [Kwargs](./docs/cn/Kwargs.md) | [Kwargs::DataItem](./docs/cn/Kwargs_DataItem.md)
- [KwargsKey](./docs/cn/KwargsKey.md)
- [KwargsValue](./docs/cn/KwargsValue.md)
- [operator""_opt](./docs/cn/operator%20_opt.md)

## 功能 & 示例

### 函数传参

<details open> <summary>函数原型</summary>

- In Python:
  ```py
  # 任意的键名
  def func(**kwargs): ...
  
  # 限定键名（带默认值）
  def func(*, name='empty_name', old=0): ...
  ```

- In C++:
  ```cpp
  // 任意的键名
  auto func(Kwargs<> kwargs = {});

  // 限定键名（无需带默认值）
  auto func(Kwargs<"name"_opt, "old"_opt> kwargs = {});
  ```

</details>

<details open> <summary>外部调用</summary>

- In Python:
  ```py
  # 正常
  func(name='huanhuanonly', old=18)

  # 非预期的类型
  func(name='huanhuanonly', old='18')

  # 相反的顺序
  func(old=18, name='huanhuanonly')

  # 空的参数
  func()
  ```

- In C++ (版本一):
  ```cpp
  // 正常
  func({ "name"_opt="huanhuanonly", "old"_opt=18 });

  // 非预期的类型
  func({ "name"_opt="huanhuanonly", "old"_opt="18" });

  // 相反的顺序
  func({ "old"_opt=18, "name"_opt="huanhuanonly" });

  // 空的参数
  func()
  ```

- In C++ (版本二):
  ```cpp
  // 正常
  func({ {"name", "huanhuanonly"}, {"old", 18} });

  // 非预期的类型
  func({ {"name", "huanhuanonly"}, {"old", "18"} });

  // 相反的顺序
  func({ {"old", 18}, {"name", "huanhuanonly"} });

  // 空的参数
  func()
  ```

</details>

<details open> <summary>内部取值</summary>

- In Python:
  ```py
  str(kwargs['name']) if 'name' in kwargs else 'empty_name'

  int(kwargs['old']) if 'old' in kwargs else 0
  ```

- In C++:
  ```cpp
  kwargs["name"].valueOr<std::string>("empty_name");

  kwargs["old"].valueOr<int>(0);

  // kwargs["name"].hasValue()
  // 等效于
  // if 'name' in kwargs
  ```

</details>


#### 为键名取别名：
- "lastname" 和 "surname" 是 "name" 的别名
  ```cpp
  auto func(Kwargs<"name"_opt, "lastname"_opt, "surname"_opt> kwargs = {})
  {
    // 使用 `or` 连接，第一个名字必须带 `_opt` 后缀
    kwargs["name"_opt or "lastname" or "surname"].valueOr<std::string>();
  }
  ```

#### 自动的类型转换：
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

#### 获取类型名：
- ```cpp
  auto func(Kwargs<"size"_opt> kwargs = {})
  {
    std::cout << kwargs["size"]->typeName() << std::endl;
  }

  func({ "size"_opt = 520 });  // Output: int
  func({ "size"_opt = 520f }); // Output: float
  func({ "size"_opt = "hi" }); // Output: const char*
  ```
  
> [!TIP]
> 不同的编译器可能会有不同的结果。

#### 根据不同的类型作不同的处理
- ```cpp
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

### 类的构造函数传参

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
> `name` 是 `faceName` 的别名，`i` 是 `italic` 的别名。

以下构造 `Font` 的方式都是有效的：

- `Font()`
  - 构造： `Font{ std::string(), 9, 0.00f, false }`

- `Font({ })`
  - 构造： `Font{ std::string(), 9, 0.00f, false }`

- `Font({ {"name", "Arial"}, {"italic", true} })`
  - 构造： `Font{ std::string("Arial"), 9, 0.00f, true }`

- `Font({ {"italic", "true"}, {"name", "Arial"} })`
  - 构造： `Font{ std::string("Arial"), 9, 0.00f, true }`

- `Font({ "i"_opt="True", "faceName"_opt="Arial" })`
  - 构造： `Font{ std::string("Arial"), 9, 0.00f, true }`

- `Font({ {"size", 18}, {"escapement", 45} })`
  - 构造： `Font{ std::string(), 18, 45.00f, false }`

- `Font({ "size"_opt="18", "escapement"_opt="49.2" })`
  - 构造： `Font{ std::string(), 18, 49.20f, false }`

- `Font({ "size"_opt=18.8, {"escapement", 49.2}, "i"_opt='t' })`
  - 构造： `Font{ std::string(), 18, 49.20f, true }`

在 [![TEST-PASSING](https://img.shields.io/badge/TEST-PASSING-gold?style=flat&logo=openbugbounty&logoColor=white&logoSize=auto&labelColor=orange)](https://github.com/huanhuanonly/cpp-kwargs/blob/main/tests/test.cpp) 中可以看到更多的使用方式。

### 功能对比

* _Python (**kwargs)_ 和 _cpp-kwargs_ 都支持的：

  - [x] 支持 **按任意顺序排列的** 键；
  - [x] 支持 **缺少的** 或 **多余的** 键；
  - [x] 支持 **限定键名**；
  - [x] 支持 **任意类型的** 值；
  - [x] 保留 **原始值的类型信息**；

* _cpp-kwargs_ 额外支持的：

  - [x] 自动的 [类型转换](#支持的内置类型自动转换)（传入类型和传出的类型不一致时）；
  - [x] 较小的开销，`Kwargs` 的内部会尽可能地使用 `constexpr`，将在 **编译期得到结果** （如果满足条件的话）；
  - [x] 键名不区别大小写（_可选的_）；
  - [x] 键名可以拥有别名；

> [!TIP]
> 推荐使用 C++ $20$，在 C++ $20$ 中，`constexpr` 拥有更进一步的支持。

* 仅 _Python (**kwargs)_ 支持的：

  - [ ] 动态的返回值类型；

> [!TIP]
> C++ 的返回值类型必须在编译时确定。

## 导入到自己的项目中

### 通用方法

- 克隆该仓库：
```bash
git clone https://github.com/huanhuanonly/cpp-kwargs.git
```

- 在 _.cpp/.h_ 文件中：
```cpp
#include "cpp-kwargs/include/kwargs.h"
```

### 对于 CMake 项目

- 在你的 CMakeList.txt 中加入以下代码片段，他将会自动从 Github 上克隆该仓库到 `${CPP_KWARGS_PATH}`：
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

- 在 _.cpp/.h_ 文件中：
```cpp
#include <kwargs.h>
```

## 可选的设置

- **KWARGSKEY_CASE_INSENSITIVE**
  - 定义以启用 `KwargsKey` 大小写不敏感。

- **KWARGSKEY_LITERAL_SUFFIX_WITHOUT_LEADING_UNDERSCORE**
  - 定义以启用用于生成 `KwargsKey` 的字面量后缀名不带前导下划线，`_opt` $\longrightarrow$ `opt`，或 `_o` $\longrightarrow$ `o`。

- **KWARGSKEY_LITERAL_SHORT_SUFFIX**
  - 定义以启用用于生成 `KwargsKey` 的字面量后缀名 `_opt` 替换为 `_o`。

<details> <summary><b>如何定义？</b></summary>

- 在你的源文件中，在 `#include` 前 `#define`：
  ```cpp
  #if !defined(KWARGSKEY_CASE_INSENSITIVE)
  #  define KWARGSKEY_CASE_INSENSITIVE
  #endif
  
  #include "kwargs.h"
  ```

- 或者，在你的 CMakeList.txt 文件中，在 `add_subdirectory` 前 `set`：
  ```cmake
  set (KWARGSKEY_CASE_INSENSITIVE)

  add_subdirectory (${CPP_KWARGS_PATH})
  ```
</details>

## 支持的内置类型自动转换

- 所有的整型和浮点型之间都可以互相转换。

- 对于所有枚举类型 `enum` 视为其底层类型（整型）。

> [!TIP]
> 即使 `enum` 的底层类型是 `char` / `uchar`，它也会作为一个整型（`std::int8_t` / `std::uint8_t`）。

- `std::string` $\longleftrightarrow$ `std::string_view`。

- `std::string` / `std::string_view` $\longleftrightarrow$ `const char*`。

- `std::vector<char>` / `std::array<char>` / `std::string_view` $\longrightarrow$ `const char*`（并不保证有 `\0` 结束符）。

- `const char*` / `std::string` / `std::string_view` $\longrightarrow$ `Integer`。
  - 支持以 `0b`/`0o`/`0x`/`0h` 前缀表示不同的进制数（不区分大小写），且可以有前导符号 `-` 或 `+`。
  - `max` 和 `min` 分别表示最大值和最小值（不区分大小写），且可以有前导符号 `-` 或 `+`。
  - 转为无符号类型时，仅允许前导符号 `+`。

- `const char*` / `std::string` / `std::string_view` $\longrightarrow$ `Floating point`。
  - 支持科学计数法。
  - `max` 和 `min` 分别表示最大值和最小值。
  - `inf`/`infinity` 表示无穷。
  - `nan` 表示空值。
  - `pi` 和 `e` 分别表示 $\pi$ 和自然常数 $e$ 。
  - 均不区分大小写，且可以有前导符号 `-` 或 `+`。

- `Integer` / `Floating point` $\longrightarrow$ `std::string`。

- `const char*` / `std::string` / `std::string_view` $\longleftrightarrow$ `char` / `uchar`（取首字符，空则返回 `\0`）。

- `bool` $\longrightarrow$ `const char*` / `std::string` / `std::string_view`（`"true"` or `"false"`）。

- `"true"` / `'t'` （不区分大小写） $\longrightarrow$ `true`。

- `"false"` / `'f'` （不区分大小写） $\longrightarrow$ `false`。

- 可迭代的容器（拥有 `.begin()`、`.end()` 和 _前向迭代器_） $\longrightarrow$ 可插入的容器。

> [!NOTE]
> 两个容器必须都要包含 `::value_type` 类型，值类型不需要一致，不一致时将按照以上规则进行转换。

<details open>
  <summary><b><i>可插入的容器</i></b></summary>
  拥有以下成员函数之一（按顺序）：

  1. `.append()`
  2. `.push_back()`
  3. `.push()`
  4. `.insert()`
  5. `.push_front()`

</details>

> [!IMPORTANT]
> 编译器最低版本要求：
> - $\texttt{MSVC}\geq\text{v}{19.25}\left(\text{VS}{16.5}\right)$  
> - $\texttt{GCC}\geq{10.1}$  
> - $\texttt{Clang}\geq{11.0.0}$
> 
> [发现问题？](https://github.com/huanhuanonly/cpp-kwargs/issues/new)

---

- Copyright $2024\text{-}2025$ [Yang Huanhuan](https://github.com/huanhuanonly) (3347484963@qq.com).

- Created by [Yang Huanhuan](https://github.com/huanhuanonly) on $December$ $29, 2024, 14:40:45$.

- Goodbye $2024$, Hello $2025$ !