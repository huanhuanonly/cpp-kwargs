<div align="center">

# cpp-kwargs

[![STD-CPP](https://img.shields.io/badge/STD%20C%2B%2B-20-darkorange?style=for-the-badge&logo=C%2B%2B&logoColor=white&logoSize=auto&labelColor=darkcyan)](https://en.cppreference.com/w/cpp/20)
[![ENGLISH](https://img.shields.io/badge/English-goto-lavender?style=for-the-badge&logo=googletranslate&logoColor=white&logoSize=auto&labelColor=lightskyblue)](./README_English.md)
[![VIEW-CODE](https://img.shields.io/badge/VIEW-CODE-greed?style=for-the-badge&logo=github&logoColor=white&logoSize=auto&labelColor=blue)](https://github.com/huanhuanonly/cpp-kwargs/blob/main/CppKwargs.h)
[![EXAMPLES-MORE](https://img.shields.io/badge/EXAMPLES-MORE-gold?style=for-the-badge&logo=openbugbounty&logoColor=white&logoSize=auto&labelColor=orange)](https://github.com/huanhuanonly/cpp-kwargs/blob/main/test.cpp)
[![DOCS](https://img.shields.io/badge/Detailed-Documentation-darkcyan?style=for-the-badge&logo=googledocs&logoColor=white&labelColor=cornflowerblue
)](https://github.com/huanhuanonly/cpp-kwargs/tree/main/docs)


**_cpp-kwargs 是一个能在 C++ 中实现类似于 Python `**kwargs` 传参的库。_**

**_它通过 C++ 强大的模板编程封装了一个 [`Kwargs`](./docs/Kwargs.md) 类以此来实现了该功能。_**

<details>
    <summary>
            Python 中的 <code>**kwargs</code>
    </summary>

_在 Python 中， `**Kwargs` 用于函数定义时接受任意数量的关键字参数。它将所有通过 `Key=Value` 形式传递的参数封装成一个字典，在函数内部可以通过 `kwargs` 访问这些参数，`**kwargs` 使得函数能够灵活地接受不定数量的关键字参数，提升了代码的可扩展性。[官方文档](https://docs.python.org/3/tutorial/controlflow.html#keyword-arguments)。_

</details>

</div>

## 文档

- [Kwargs](./docs/Kwargs.md) | [Kwargs::DataItem](./docs/Kwargs_DataItem.md)
- [KwargsKey](./docs/KwargsKey.md)
- [KwargsValue](./docs/KwargsValue.md)
- [operator""_opt](./docs/operator%20_opt.md)

## [功能 & 示例](https://github.com/huanhuanonly/cpp-kwargs/blob/main/test.cpp)

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

> [!TIP]
> 推荐使用 C++ $20$，在 C++ $20$ 中，`STL` 更多的被声明为 `constexpr`，代码的 _编写_ 和 _测试_ 皆在 C++ $20$ 中完成。

* 仅 _Python (**kwargs)_ 支持的：

  - [ ] 动态的返回值类型；

> [!TIP]
> C++ 的返回值类型必须在编译时确定。

### 应用在函数中

<details open>
<summary>函数原型</summary>

- In _**Python**_:
  ```py
  # 任意的键名
  def func(**kwargs): ...
  
  # 限定键名（带默认值）
  def func(*, name='empty_name', old=0): ...
  ```

- In _**C++**_:
  ```cpp
  // 任意的键名
  auto func(Kwargs<> kwargs = {})
  {...}

  // 限定键名（无需带默认值）
  auto func(Kwargs<"name"_opt, "old"_opt> kwargs = {})
  {...}
  ```

</details>

<details open>
<summary>外部调用</summary>

- In _**Python**_:
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

- In _**C++**_:
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

<details open>
<summary>内部获取值</summary>

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
  // 等效于
  // if 'name' in kwargs
  ```

</details>


### 应用在类的构造函数中

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

以下构造 `Font` 的方式都是有效的：

- `Font()`
  - 同等于： `Font{ std::string(), 9, 0.00f, false }`

- `Font({ })`
  - 同等于： `Font{ std::string(), 9, 0.00f, false }`

- `Font({ {"name", "Arial"}, {"italic", true} })`
  - 同等于： `Font{ std::string("Arial"), 9, 0.00f, true }`

- `Font({ {"italic", "true"}, {"name", "Arial"} })`
  - 同等于： `Font{ std::string("Arial"), 9, 0.00f, true }`

- `Font({ {"i", "True"}, {"faceName", "Arial"} })`
  - 同等于： `Font{ std::string("Arial"), 9, 0.00f, true }`

- `Font({ {"size", 18}, {"escapement", 45} })`
  - 同等于： `Font{ std::string(), 18, 45.00f, false }`

- `Font({ {"size", "18"}, {"escapement", "49.2"} })`
  - 同等于： `Font{ std::string(), 18, 49.20f, false }`

- `Font({ {"size", 18.8}, {"escapement", 49.2}, {"i", 't'} })`
  - 同等于： `Font{ std::string(), 18, 49.20f, true }`

<details>

<summary>
    <h3>
        简单示例：Python 和 C++ 中的 <code>printList()</code>
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

调用：

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

在 [![EXAMPLES-MORE](https://img.shields.io/badge/EXAMPLES-MORE-gold?style=plastic&logo=openbugbounty&logoColor=white&logoSize=auto&labelColor=orange)](https://github.com/huanhuanonly/cpp-kwargs/blob/main/test.cpp) 中可以看到更多的使用方式。

## 导入到自己的项目中

### 克隆该仓库
```git
git clone https://github.com/huanhuanonly/cpp-kwargs.git
```

### 在 _CMakeList.txt_ 中配置
- CMakeList.txt
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

include_directories (${CPP_KWARGS_PATH})
```

- main.cpp
```cpp
#include <CppKwargs.h>
```

> [!TIP]
> 该项目只需要一个头文件即可运行。

### 设置 `KwargsKey` 不区分大小写

* 在 `#include "CppKwargs.h"` 前定义 `KWARGSKEY_CASE_INSENSITIVE`：
  ```cpp
  #ifndef KWARGSKEY_CASE_INSENSITIVE
  #  define KWARGSKEY_CASE_INSENSITIVE
  #endif
  
  #include "CppKwargs.h"
  ```

* 或者,在你的项目中的 _CMakeList.txt_ 文件中添加以下行：
  ```cmake
  target_compile_definitions (YourExecutable PRIVATE KWARGSKEY_CASE_INSENSITIVE)
  ```

## 支持的内置类型自动转换

- 所有的整型和浮点型之间的互相转换。

- 对于所有枚举类型 `enum` 视为其底层类型（整型）。

- `std::string` $\longleftrightarrow$ `std::string_view`。

- `std::string` / `std::string_view` $\longleftrightarrow$ `const char*`。

- `std::vector<char>` / `std::array<char>` / `std::string_view` $\longrightarrow$ `const char*`（并不保证有 `\0` 结束符）。

- `const char*` / `std::string` / `std::string_view` $\longrightarrow$ `Integer` / `Floating point`。

- `Integer` / `Floating point` $\longrightarrow$ `std::string`。

- `const char*` / `std::string` / `std::string_view` $\longleftrightarrow$ `char` / `uchar`（取首字符，空则返回 `\0`）。

- `bool` $\longrightarrow$ `const char*` / `std::string` / `std::string_view`（`"true"` or `"false"`）。

- `"true"` / `"True"` / `"TRUE"` / `'t'` / `'T'` $\longrightarrow$ `true`。

- `"false"` / `"False"` / `"FALSE"` / `'f'` / `'F'` $\longrightarrow$ `false`。

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

---

- Copyright $2024\text{-}2025$ [Yang Huanhuan](https://github.com/huanhuanonly) (3347484963@qq.com). All rights reserved.

- Created by [Yang Huanhuan](https://github.com/huanhuanonly) on $December$ $29, 2024, 14:40:45$.

- Goodbye $2024$, ***Hello*** $2025$ !