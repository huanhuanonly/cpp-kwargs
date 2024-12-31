<div align="center">

# cpp-kwargs

[![STD-CPP](https://img.shields.io/badge/STD%20C%2B%2B-20-darkorange?style=for-the-badge&logo=C%2B%2B&logoColor=white&logoSize=auto&labelColor=darkcyan)](https://en.cppreference.com/w/cpp/20)
[![ENGLISH](https://img.shields.io/badge/English-goto-lavender?style=for-the-badge&logo=googletranslate&logoColor=white&logoSize=auto&labelColor=lightskyblue)](./README_English.md)
[![VIEW-CODE](https://img.shields.io/badge/VIEW-CODE-greed?style=for-the-badge&logo=github&logoColor=white&logoSize=auto&labelColor=blue)](https://github.com/huanhuanonly/cpp-kwargs/blob/main/CppKwargs.h)
[![EXAMPLES-MORE](https://img.shields.io/badge/EXAMPLES-MORE-gold?style=for-the-badge&logo=openbugbounty&logoColor=white&logoSize=auto&labelColor=orange)](https://github.com/huanhuanonly/cpp-kwargs/blob/main/test.cpp)


_众所周知，Python 中的 `**Kwargs` 用于函数定义时接受任意数量的关键字参数。它将所有通过 `Key=Value` 形式传递的参数封装成一个字典，在函数内部可以通过 `kwargs` 访问这些参数，`**kwargs` 使得函数能够灵活地接受不定数量的关键字参数，提升了代码的可扩展性。_

**_该仓库通过 C++ 强大的模板编程封装了一个 [`Kwargs`](#class-kwargs) 类以此来实现了该功能。_**

</div>

- [x] 参数列表中的参数可以 **选填** 和 **缺省**;

- [x] 参数列表中的参数 **填入顺序不重要**;

- [x] [内置类型自动转换器](#支持的内置类型自动转换)，传入类型和传出类型不一致时自动做类型转换；

- [x] 较小的开销，尽可能地使用 `constexpr`，将在编译期得到结果；

> 推荐使用 C++ $20$，在 C++ $20$ 中，`STL` 更多的被声明为 `constexpr`，但最好不要低于 C++ $17$。

## 部分演示

### 在类的构造函数中使用

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

以下构造 `People` 的方式都是有效的：

- `People()`
- `People({ })`
- `People({ {"name", "huanhuanonly"} })`
- `People({ {"name"_opt, "huanhuanonly"} })`
- `People({ {"old"_opt, 16} })`
- `People({ {"old", 16ULL}, {"name", "huanhuanonly"} })`
- `People({ {"height", 1.80} })`
- `People({ {"height", 2} })`


### Python 和 C++ 中的打印列表函数

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

## 下载和导入

在 [https://github.com/huanhuanonly/cpp-kwargs/blob/main/CppKwargs.h](https://github.com/huanhuanonly/cpp-kwargs/blob/main/CppKwargs.h) 中点击 **Download raw file** 下载，并将该文件移动到项目目录，在项目源代码中包含以下代码：
```cpp
#include "CppKwargs.h"
```

推荐使用 C++ $20$。

## CppKwargs.h

### class `Kwargs`

`**kwargs` 的类型，底层维护了一个 `std::initializer_list<std::pair<KwargsKey, KwargsValue>>`。
```cpp
template<KwargsKey::value_type... _OptionalList>
class Kwargs {...}
```

* `_OptionalList`: 支持的键的列表，使用以 `_opt` 后缀的字面量进行构造，如果传入不在列表中的键，将会通过 `assert()` 引发崩溃。

> 如果 `_OptionalList` 为空，则不做检查。

### class `KwargsKey`

键的类型，可以通过键名（`const char*`）进行构造，

```cpp
class KwargsKey
{
    using value_type = std::uint64_t;
}
```

定义了 用户自定义字面量后缀 `operator""_opt()`，它的返回值类型为 `KwargsKey`（`KwargsKey` 可以隐式转换为 `KwargsKey::value_type`。

以下都是可行的使用：

* `"cpp-kwargs"_opt`
* `'c'_opt`
* `1314_opt`
* `1314.520_opt`
* `99999999999999999999999999999999_opt`

> [!NOTE]
> 如果觉得下划线不好输入可以直接修改在 `CppKwargs.h` 中的定义，但是会受到编译器的警告。

### class `KwargsValue`

```cpp
class KwargsValue {...}
```

值的类型，类似 `std::any`，它可以使用任意类型进行构造，不同的是，它内置了类型转换器，传入类型和传出类型不一定需要一致，不一致时将尝试进行转换（如果可以的话）。

> [!NOTE]
> 它内部并不使用 `std::any`，但是它同样会保证 析构函数 的正确调用。

- 对于 **非** `class`、`union` 且 **小于等于** $8$ 个字节的类型，将会进行字节级别的拷贝。
- 当使用 `const auto&` 版本的构造函数时，对于 `class`、`union` 以及 **大于** $8$ 个字节的类型，它内部会使用一个指针指向原始数据，而不是复制它。
- 当使用 `auto&&` 版本的构造函数时，对于 `class`、`union` 以及 **大于** $8$ 个字节的类型，它会调用 `new` 来申请一块动态内存，并使用 `std::move` 进行资源的转移。

### 支持的内置类型自动转换

- 所有的整型和浮点型之间的互相转换。
- `std::string` $\longleftrightarrow$ `std::string_view`。
- `std::string` / `std::string_view` $\longleftrightarrow$ `const char*`。
- `std::vector<char>` / `std::array<char>` / `std::string_view` $\longrightarrow$ `const char*`（并不保证有 `\0` 结束符）。
- `const char*` / `std::string` / `std::string_view` $\longrightarrow$ `Integer` / `Floating point`。
- `Integer` / `Floating point` $\longrightarrow$ `std::string`。
- `const char*` / `std::string` / `std::string_view` $\longleftrightarrow$ `char` / `uchar`（取首字符，空则返回 `\0`）。
- `bool` $\longrightarrow$ `const char*` / `std::string` / `std::string_view`（`true` or `false`）.
- 可迭代的容器（拥有 `.begin()`、`.end()` 和 _前向迭代器_）$\longrightarrow$ 可插入的容器（拥有 `.push_back()` / `.insert()` / `.push()` 或 `.append()`）。

> [!NOTE]
> 两个容器必须都要包含 `::value_type` 类型，值类型不需要一致，不一致时将按照以上规则进行转换。

---

- Copyright 2024 [Yang Huanhuan](https://github.com/huanhuanonly) (3347484963@qq.com). All rights reserved.

- Created by [Yang Huanhuan](https://github.com/huanhuanonly) on $December$ $29, 2024, 14:40:45$.

- Goodbye $2024$, ***Hello*** $2025$ !