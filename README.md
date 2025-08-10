<div align="center">

<p><img src="/docs/logo.png" width="400"/></p>

<p>
   <a href="#ENGLISH"><img src="https://img.shields.io/badge/English-blue?style=for-the-badge" alt="English" /></a>
   &nbsp;&nbsp;
   <a href="#中文"><img src="https://img.shields.io/badge/中文-red?style=for-the-badge" alt="中文" /></a>
</p>
</div>

---

<div id="中文">

## **介绍**

mysvac-jsonlib 是一个 C++20 的 JSON 库，它提供简洁、高效的 JSON 解析、操作和序列化功能。

你可以在本仓库的 [github-pages](https://mysvac.github.io/cpp-jsonlib/) 页面找到详细的文档。

## **导入库**

```cpp
import std; // 使用标准库头文件或标准库模块
import mysvac.json; // 导入 mysvac-jsonlib 库
using namespace mysvac; // 使用命名空间，简化代码书写
```

## **基础类型**

JSON 存在六种基本类型，本库使用了一个枚举来表示它们：

```cpp
enum class Type{
    eNul = 0,  ///< Nul type
    eBol,      ///< boolean type
    eNumber,    ///< Number type
    eString,    ///< String type
    eArray,     ///< Array type
    eObject     ///< Object type
};
```

此枚举位于 `mysvac::json` 命名空间中，这里面还有一个重要的类模板 `Json` ，它是一个容器，可以存储任意一种 JSON 数据结构。

```cpp
template<
    bool UseOrderedMap = true,
    template<typename U> class VecAllocator = std::allocator,
    template<typename U> class MapAllocator = std::allocator,
    template<typename U> class StrAllocator = std::allocator
>
class Json;
```

虽然你无法指定对象（映射）、数组和字符串的具体实现，但可以为它们指定内存分配器。

1. `UseOrderedMap` : 使用有序映射`std::map`，还是无序的`std::unordered_map`。
2. `VecAllocator` : 数组类型`std::vector`使用的分配器模板。
3. `MapAllocator` : 对象类型（映射）使用的分配器模板。
4. `StrAllocator` : 字符串类型`std::basic_string`使用的分配器模板。

为了方便操作，我们在 `mysvac` 命名空间中提供了一个默认的 `Json` 别名，本文档后续内容仅使用这个默认类型：

```cpp
namespace mysvac {
    using Json = ::mysvac::json::Json<>;
}
```

类内部提供了六种 JSON 子类型的别名：

| 别名             | 默认类型                          | 实际类型                                                                        |
|----------------|-------------------------------|-----------------------------------------------------------------------------|
| `Json::Nul`   | `std::nullptr_t`              | `std::nullptr_t`                                                            |
| `Json::Bol`   | `bool`                        | `bool`                                                                      |
| `Json::Number` | `double`                      | `double`                                                                    |
| `Json::String` | `std::string`                 | `std::basic_string<...,StrAllocator<char>>`                                 |
| `Json::Array`  | `std::vector<Json>`           | `std::vector<Json, VecAllocator<Json>>`                                     |
| `Json::Object` | `std::map<std::string, Json>` | `std::map<..,MapAllocator<..>>` 或 `std::unordered_map<..,MapAllocator<..>>` |

这些类型的具体定义与类模板形参有关，因此别名只能放在类内，而非命名空间中。

## **基础操作**

### 1. 初始化

`Value` 的默认构造函数会创建 `Nul` 类型的值，但你可以直接通过上述六种类型进行初始化：

```cpp
Json null_val;                  // 默认构造，类型为 Nul
Json bool_val(3.3);             // 浮点初始化，类型为 Number
Json obj_val = Json::Object{};  // 直接使用 Object 初始化
```

除了上述六种 JSON 类型，我们还支持使用基本算术类型、枚举类型和 `const char*` 进行隐式构造。
**枚举会被视为整数**，不要试图使用 `json::Type` 枚举值进行指定初始化，这只会生成 `Number` 类型的值：

```cpp
Json enum_val{ json::Type::eObject }; // 危险
// 这会生成一个 Number 类型的值，具体值取决于枚举值的整数表示。
```

虽然 `Json` 不支持初始化列表，但由于隐式构造的存在，可以通过 `Array` 和 `Object` 的初始化列表快速创建对象：

```cpp
Json smp_val = Json::Object{
    { "key1", 42 },
    {"key2", "value2"},
    {"key3", true },
    {"arr", Json::Array{ { 2, 3.14, nullptr } } },
    {"obj", Json::Object{ {"nested_k", "nested_v"} } }
};
```

空数组初始化请使用 `Array{}` ，非空初始化必须使用 `Array{ { ... } }` 双重**大括号**，否则在特定情况下会被认为是拷贝构造或扩容构造而非初始化列表。

> 请不要使用 `( {} )` ，小括号依然可能识别错误。

你可以用 `type()` 或 `is_xxx()` 函数检查类型，或者使用 `type_name()` 获取字符串形式的类型名：

```cpp
smp_val.type();        // 返回 json::Type::eObject
json::type_name(smp_val.type());  // 返回 "Object"
smp_val.is_arr();     // 返回 false
```

`is` 共有六个，分别是 `arr`、`obj`、`str`、`num`、`bol` 和 `nul` ，对应六种 JSON 类型。

你可以通过赋值语句重置内容，也可以使用 `reset()` 成员函数。
这是个模板函数，默认重置回 `Nul` 类型，但你可以显示指定重置类型，比如使用 `reset<Json::Object>()` 将内容重置为一个空的 `Object` 。

`reset` 的模板形参只能是六种 JSON 类型之一，否则无法通过编译。

### 2. 访问和修改

本库提供了 `xxx()` 成员函数以获取内部数据的**引用**，`xxx` 和上面的 `is_xxx` 相同。

```cpp
 // 注意 Object 的 mapped 依然是 Value 类型
Json& vi_42 = smp_val.obj()["key1"];

// 虽然返回引用，但也可以用于赋值
double i_42 = vi_42.num(); 

 // vi_42.str(); // 类型不匹配，抛出 std::bad_varient_access 异常
```

`Json` 还提供了 `[]` 和 `at` 运算符，区别在于 `at` 禁止索引越界（抛出异常），而 `[]` 不检查越界（所以`Object`可以用`[]`创建新键值对，但`Array`越界是未定义行为，可能直接断言崩溃）。

> `const` 的 `[]` 较为特殊，等价于 `const` 的 `at` ，越界抛出异常而不会创建新键值对或崩溃。

```cpp
smp_val["arr"][1].num(); // 返回 3.14
smp_val.at("obj").at("nested_k") = nullptr; // 修改对象，变为 Nul
smp_val["obj"].at("nested_k").is_nul(); // [] 和 at 可以混合使用
```

数组和映射类型可能需要频繁操作，因此我们提供了一系列辅助函数。这些函数返回一个布尔值表示操作是否成功，不会抛出异常。

| 函数名 | 返回值 | 说明 |
--------------------------------------------------------------
| `size()` | `size_t` | 数组或映射则返回元素数，否则返回 `0` |
| `empty()` | `bool` | 返回数组或映射是否为空，其他类型返回 `true` |
| `contains(key)` | `bool` | 判断是否含某个键，非映射返回 `false` |
| `erase(key)` | `bool` | 删除键值对，非映射或键不存在返回 `false` |
| `erase(index)` | `bool` | 删除数组元素，非数组或索引越界返回 `false` |
| `insert(key, val)` | `bool` | 插入键值对，非映射返回 `false` |
| `insert(index,val)` | `bool` | 插入数组元素，非数组或越界返回 `false` |
| `push_back(val)` | `bool` | 向数组末尾添加元素，非数组返回 `false` |
| `pop_back()` | `bool` | 删除数组末尾元素，非数组或空数组返回 `false` |

`obj()/arr()` 等函数只能获取六种引用，所以本库还提供了 `to` 和 `move` 模板来获取内部的值并强制转换类型。
前者必然是拷贝，后者是移动或拷贝（简单类型，或者无需移动时进行拷贝）。

> 调用 `move` 后难以确定原对象的内容，最好主动 `reset`。

```cpp
auto str_view = smp_val["key2"].to<std::string_view>(); // 返回内部字符串的视图
auto str = smp_val["key2"].move<std::string>(); // 将内部字符串移动了出来，现在内部变为空字符串，之前的视图不再可用
int int_42 = smp_val["key1"].to<int>(); // 返回内部整数的拷贝
```

特别注意，`Number` 数据使用 `double` 存储，因此在转换成整数时（枚举类型，或符合整数模板要求的类型），会**四舍五入**，避免精度问题。

注意，`to` 和 `move` 支持很多类型，转换失败会抛出 `std::runtime_error` 异常。
为此，我们还提供了 `xx_if` 和 `xx_or` 版本，前者返回 `optional<T>` ，后者则是在失败时返回指定的默认值。

```cpp
auto opt_str = smp_val["key1"].to_if<std::string>(); // opt_str 是 std::optional<std::string> ，转换失败为空，但不抛出异常
if(opt_str) std::cout << *opt_str << std::endl; // 如果转换成功，输出字符串
std::string or_str = smp_val["key1"].to_or<std::string>("default"); // 如果转换失败，返回 "default"
```

转换具有非常准确的规则与测试顺序，详细内容请参考 [github-pages](https://v-craft.github.io/vct-tools-json/) 文档，或源码注释。

本库提供了三个概念用于查询类型是否可能被转换：（都不满足的无法使用`to/move`模板，无法通过编译）

1. `json::convertible<J, T>` 可能**直接**转换成功的类型 `T`，这包含了满足 `json::json_type<J, T>` 的类型。
2. `json::convertible_map<J, T,D>` 可能**间接**转换成功的映射类型 `T`，键必须是字符串，值\(mapped\)是 `D` 类型且满足条件 1 。
3. `json::convertible_array<J, T,D>` 可能**间接**转换成功的数组类型 `T`，内部值是 `D` 类型且满足条件 1 。

这里的 `J` 是指 `mysvac::json::Json` 类模板的具体实例化类型，比如 `mysvac::Json` 。

只要类型满足三种概念之一，就可以使用 `to` 和 `move` 系列函数进行转换。我们会在后续的“自定义类型序列化”部分详细介绍。

> 数组或映射可能同时满足多种概念，但这不影响效果。

### 3. 序列化与反序列化

本库的序列化和反序列化非常高效且容易使用。

首先是反序列化，将字符串转换为 `Json` 对象，使用 `Json::parse()` 函数。
注意 `parse` 是类的静态成员函数，而非命名空间中的函数，因为不同类型需要不同的解析函数。

```cpp
std::string json_str1 = R"( [ 1, false, null, { "Hello": "World" } ] )";
std::string json_str2 = R"( false )"; // 允许顶层类型是任一 JSON 类型
Json val1 = Json::parse(json_str1).value_or( nullptr ); // 解析 JSON 字符串
std::cout << val1[1].to<bool>() << std::endl; // 输出 0 （没有指定 boolaplha）
```

这里还需要说明三件事：

1. JSON 文件解析失败在实际应用中很常见且难以预料，因为很容易有一些格式错误或垃圾数据。
因此本库的反序列化函数返回 `std::optional<Json>` ，从而避免使用异常机制，减小开销。后者是一个描述错误类型的枚举。

2. 此函数还具有一个可选参数 `max_depth`（默认是 256），用于限制解析的最大（嵌套）深度。
本库虽然保证总解析复杂度是 `O(n)` 的（严格单次遍历），但使用了递归来处理嵌套结构，因此需要用它避免某些垃圾数据的问题（比如过长的 `[[[[[[[[]]]]]]]]` 在递归时可能导致栈溢出）。

3. 此函数除了 `string_view` 外，还能传入 `std::istream` 进行流式解析。
流式解析文件的效率几乎等同于先将文件全部读入 string 再用 string 解析，但内存占用可能更少。

然后是序列化，使用 `Json` 对象的 `dump/write` 成员函数。

```cpp
std::string str_ser = val1.dump(); // 不含无效空白字符的高效序列化，返回 std::string
std::string str_back;
val1.write( str_back ); // 将序列化结果写入 std::string 的末尾
val1.write( std::cout ); // 将序列化结果直接输出到 `ostream` 中
```

现在 `str_ser` 和 `str_back` 的内容完全一样，因为 `dump` 就是用 `write` 实现的。

由于 `Json` 必然是有效的 JSON 数据，因此 `dump` 必然成功。
不过 `write` 的流操作不一定成功（比如文件突然关闭）、函数检测到流的状态为 `fail()` 后会立即返回，但不会抛出异常，需要你自行检查流的状态。

上面的三个序列化函数都是高效的紧凑序列化，不含无效空白字符。
但你可以使用 `dumpf/writef` 系列函数来获得更易读的格式化输出，`f` 指 `format` ，它同样包含三种形式：

```cpp
std::string pretty_str = val1.dumpf();
val1.writef( std::cout ); // 输出到 `ostream`
// 还有 writef 写入字符串末尾，此处省略
```

`f` 系列有三个可选参数，依次是 “单次缩进空格数（默认 2）”，“初始缩进次数（默认 0）”。
这些函数同样必然成功（除非写入流但流状态异常）。

> 一些特殊数据如 `[[[[]]]]` 这样的深度嵌套结构，序列化时可能导致递归过深栈溢出，且带缩进的格式化文本会非常大。
> 我们在反序列化函数中限制了嵌套深度，因此你只可能在程序中主动构造这样的特殊数据而不可能从外部输入，可以主动避免构造此类数据。

### 4. 等于运算符

`Json` 类型提供了和 `Json` 进行比较的 `==` 运算符，它首先判断类型是否相同，然后调用内部的 `==` 进行比较（ `std::map` 和 `std::vector` 的比较基于子元素内容，从而实现递归比较）。

```cpp
Json val_arr_1 = Json::Array{{ 1, 2, 3 }};
Json val_arr_2 = Json::Array{{ 1, 2, 3 }};
Json val_arr_3 = Json::Array{{ 1, true, 3 }};
val_arr_1 == val_arr_2; // true
val_arr_1 == val_arr_3; // false
```

更加特殊的是， `Json` 还通过模板函数实现了和其他任意类型的 `==` 比较。

不兼容的类型直接返回 `false` ，如果目标是六种 JSON 类型之一，则先测试类型是否匹配，然后比较具体值。
否则，尝试将对象转换为 `Json` ，或者将 `Json` 转换为目标类型然后比较。都不匹配则返回 `false` 。

`==` 操作必然成功，不会抛出异常。

## **自定义类型序列化**

任何自定义类型，只要提供针对 `Json` 的构造函数和类型转换函数，就能通过 `to/move` 或者类型转换等方式与 JSON 数据交互，从而实现快速的序列化和反序列化。

> 提供了针对 `Json` 构造函数和类型转换函数，就满足了 `json::convertible` 概念。

这些函数还有一些细节要求，它们的实现并不轻松，因此本库提供了一个仅包含宏定义的头文件，让你可以轻松实现自定义类型与 JSON 的交互，它甚至支持移动语义。

> 你可以自行浏览此头文件，它的内容很少，但可以让你了解什么类型能够满足转换条件。

```cpp
#define M_MYSVAC_JSON_SIMPLIFY_MACROS // 定义宏，以启用简化的宏函数名
#include <mysvac/json_macros.hpp>
// 建议将所有头文件放在所有 import 之前，虽然此文件仅含宏定义
import std;
import mysvac.json;
using namespace mysvac;
```

假设你现在有这样一个类型：

```cpp
struct MyData{
    int id{};
    std::string m_name{};
    bool active{};
    double m_value{};
};
```

然后你可以像下面这样，通过宏定义为其添加构造函数和转换函数，但需要显式启用默认构造：

```cpp
struct MyData{
    int id{};
    std::string m_name{};
    bool active{false};
    double m_value{};

    MyData() = default; // 必须存在默认构造，内容可以自定义

    M_JSON_CV_FUN( MyData,  // 转换函数，必须在 public 作用域
        M_JSON_CV_MEM( id );    // 注意，MyData 后面必须有 `,` 
        M_JSON_CV_MAP( name, m_name )   // 但是剩余的字段后面不能有逗号 `,` ，分号 `;` 则是可选的 
        M_JSON_CV_MEM( active )
        M_JSON_CV_MAP( value, m_value )
    )
    M_JSON_CS_FUN( MyData,  // 构造函数，必须在 public 作用域
        M_JSON_CS_MEM( id )
        M_JSON_CS_MAP( name, m_name )
        M_JSON_CS_MEM_OR( active, true, nullptr ) // 默认值是 `true`
        M_JSON_CS_MAP_OR( value, m_value, 64.0, nullptr ) // nullptr 表示此类型不需要子元素默认值
    )
};
```

`CV` 的是指 `Conversion` 转换函数，而 `CS` 是指 `Constructor` 构造函数。它们的第一个参数都是类型名，后面需要一个 `,` 分隔符。
然后通过对应的宏定义指定 JSON 转换中需要的字段，`MEM` 是指成员变量名与 JSON 键名相同，`MAP` 是指成员变量名与 JSON 键名不同（比如键是 `name` ，而成员变量名是 `m_name`）。

> 你可以选择自行定义一些简化宏，比如 `JCSM` `JCSP` 等等，高度简化书写。

转换函数是必然成功的，因为需要的数据都是成员变量。
但是构造函数中的成员赋值可能会失败，因为 `Json` 中可能不存在对应的键（甚至 `Json` 根本不是 `Object` 类型），因此需要指定成员默认值。

你会看到构造函数（`CS`）的宏，部分带有 `OR` 后者，它们多了两个参数，第一个参数就是默认值。
而没有 `OR` 的宏并非没有默认值，而是将对应类型的默认构造作为默认值，即 `decltype(name){}` 。

作者建议是，字段的默认值请和成员变量的默认值保持一致，因为我们希望从 `Json` 转换失败的结果等于默认构造函数的效果。
（上面 `active` 的默认值就和 `CS` 中指定的不一样，不推荐这种写法）

然后你就可以像下面这样，让 `Json` 和 `MyData` 互相转换了：

```cpp
Json v_null; 
MyData d_null{ v_null }; // 什么都没有，因此全部字段都是 CS 中的默认值
d_null.active; // true，因为 CS 函数指定了默认值为 true

Json v_object{ Json::Object{} };
v_object["id"] = 42;
v_object["name"] = "Test User";
v_object["active"] = false;
v_object["value"] = 128.0;
MyData d_object{ v_object };    // 必须显式转换，不能用 `=` 构造
d_object.m_name == "Test User"; // true

Json v_data{ d_object }; // 将 MyData 转换为 JSON 对象
v_data["id"] == 42; // true
```

使用这两个宏有一个非常重要的要求，即需要转换的**成员变量**必须支持与 `Json` 类型的转换。

1. 对于基本算术类型、枚举类型、六种 JSON 类型和 `Json` 自身，必然满足要求。
2. 对于其他自定义类类型，需要像上面一样提供转换函数和构造函数。
3. 对于满足条件 1 或 2 的类型构成的列表（如 `std::vector`,`std::list` 等），可以直接使用。
4. 对于满足条件 1 或 2 的类型构成的映射（如 `std::map`,`unordered_map` 等），在键为 `std::string` 时也可以直接使用。

条件 1 和 2 指的是概念 `json::convertible` ，而条件 3 和 4 指的是概念 `json::convertible_array` 和 `json::convertible_map` 。

比如，现在的 `MyData` 类型已经通过宏定义提供了转换函数和构造函数，满足条件 2 。
因此你可以在其他类型中直接使用它，然后实现嵌套的 JSON 对象：

```cpp
struct MyData2 {
    std::string name;   // std::string 等于 json::String，因此可以直接使用
    MyData my_data;     // MyData 已经有转换函数和构造函数，因此可以直接使用
    std::vector<MyData> data_list;  // 能够直接使用的类型构成的列表也能直接使用（但再套一层列表就不行了）
    MyData2() = default;
    M_JSON_CV_FUN( MyData2,
        M_JSON_CV_MEM( name )
        M_JSON_CV_MAP( data, my_data )
        M_JSON_CV_MEM( data_list )
    )
    M_JSON_CS_FUN( MyData2,
        M_JSON_CS_MEM( name )
        M_JSON_CS_MAP( data, my_data )
        M_JSON_CS_MEM_OR( data_list, std::vector<MyData>{}, MyData{} ) // 变量名，默认值，内部子元素的默认值
    )
};
```

可以看到我们用到了 `OR` 宏的第四个参数。第三个参数是字段本身的默认值，第四个参数是子元素的默认值。
第四个参数仅在目标是数组或者映射类型（且非 `Json::Array/Object` ）时才有用，其他时候可以随意填写，通常用 `nullptr` 。

比如你需要数组，但是 `Json` 内部不是数组，就会返回第三个字段的默认值。
`Json` 也是数组，但是内部只有部分元素能够转成你需要的类型，那么其他元素会用第四个参数的默认值填充，保证数组长度一致。

然后你可以像下面这样在两种类型之间来回切换：

```cpp
Json v_data2{ MyData2{} };
std::println("");
v_data2.writef( std::cout );
std::println("");
v_data2["data"]["id"] = 8848;
v_data2["data"]["name"] = "Mount Everest";
v_data2["data"]["active"] = true;
v_data2["data_list"].arr().push_back( v_data2["data"] );
v_data2["name"] = "name_name";
MyData2 d_data2{ v_data2 };
M_EXPECT_TRUE( d_data2.my_data.id == 8848 ); // true
M_EXPECT_TRUE( d_data2.my_data.m_name == "Mount Everest" ); // true
M_EXPECT_TRUE( d_data2.data_list.size() == 1 ); // true
M_EXPECT_TRUE( d_data2.data_list[0].id == 8848 ); // true
M_EXPECT_TRUE( d_data2.data_list[0].m_name == "Mount Everest" ); // true
M_EXPECT_TRUE( d_data2.name == "name_name" ); // true
```

> 这里的 `M_EXPECT_TRUE` 使用的是 vct-test-unit 库，你可以不用在意。

## **列表与映射扩展**

内容变得越来越复杂了，这里作为最后一部分，将介绍列表和映射的实现细节。

我们之前提到，原先只有六种 JSON 类型和基本算术类型是能够直接和 `Json` 转换的，而自定义类型需提供转换函数和构造函数。
也就是只有满足 `json::convertible` 概念的类型才能直接转换。

但是，像 `array<int>` 这种标准库提供的类型怎么办呢？它很常用，内部的 `int` 满足了转换条件，但整体并不满足，又无法让它提供转换函数和构造函数。

因此，本库为 `Json` 提供了四个模板函数，分别对应 `数组类型->Json` 和 `Json->数组类型` 以及 `映射类型->Json` 和 `Json->映射类型` 的转换。

什么类型能够用这些模板呢？首先映射类型的键必须是 `std::string` 或者可以转换为 `std::string` 的类型。
最重要的是内部的值类型，要求是 `json::convertible` ，因为这些类型能够直接转换。

这就是为什么会有两个独立的概念 `json::convertible_map` 和 `json::convertible_array` 。

`数组/映射->Json`是不会遗漏任何元素的，因为所有元素都能被 `Json` 接受。
但是反之则不然，`Json->数组/映射` 可能会丢失一些元素，因为 `Json` 可能有各种奇怪的数据和格式。

因此，如果你的数组和映射不是基本类型里的 `Json::Array` 和 `Json::Object` ，那么在转换时必须提供两个默认值：

1. 完全不匹配时返回的默认结果。比如需要转换成数组，但是 `Json` 内部不是数组，则直接返回此默认值。

2. 能够匹配类型，但是局部元素不匹配时填充的子元素默认值。比如需要 `vector<int>` ，但是 `Json` 中是 `[1, 2, [], 3]` ，你需要指定遇到 `[]` 这些不匹配元素时填充的默认整数。

这也就是为什么 `M_JSON_CS_MEM_OR` 和 `M_JSON_CS_MAP_OR` 宏定义需要两个默认值。
不过，如果你转换的类型不是数组或者映射，最后这个子元素默认值可以任意填写，上面我们就使用过 `nullptr` 作为默认值。

此内容在代码中实际对应 `to/move` 系列函数。

对于基本类型或者自定义类型的数据，可以像之前一样直接转换：

```cpp
xxx = val.to<MyData>(); // 或者 move<MyData>()
```

但如果需要转换成数组，就需要显式指定子元素默认值：

```cpp
// 实际模板有两个参数，第一个是目标类型，第二个是填充子元素的类型
xxx = val.to<std::vector<MyData>, MyData>( MyData{} );
// 可以根据函数参数自动推导第二个模板参数
xxx = val.to<std::vector<MyData>>( MyData{} ); 
```

第二个模板参数默认是 `Json::Nul` ，即 `std:::nullptr_t` 。如果转换目标不是数组或对象，完全不需要添加它。

注意， `to/move` 在完全不匹配时直接抛出异常，所以我们只指定了子元素默认值。
而宏定义实际由 `to_or` 和 `move_or` 实现，因此需要两个默认值：

```cpp
// 第二个模板参数使用自动推导
xxx = val.to_or<std::vector<MyData>>( std::vector<MyData>{} , MyData{} ); 
```

## **最后**

以上就是本库的基本使用，虽然自定义类型序列化的部分比较复杂，但你可以自行阅读文档和源码，本库的源码的有效行数其实非常少（不足 2000 行）。
重点观察 `to` 和 `move` 的实现，以及头文件中的宏定义，你应该能很快上手。

如果你发现的本库的任何问题，或者可优化的地方，欢迎提交 issue 或 PR。

</div>

---

<div id="ENGLISH">



</div>
