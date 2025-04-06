<div align="center">

# mysvac-jsonlib

<p>
   <a href="#ENGLISH"><img src="https://img.shields.io/badge/English-blue?style=for-the-badge" alt="English" /></a>
   &nbsp;&nbsp;
   <a href="#中文"><img src="https://img.shields.io/badge/中文-red?style=for-the-badge" alt="中文" /></a>
</p>
<p>
    <a href="#simple-cmp">
        <img src="https://img.shields.io/badge/Simple Comparison | 简要对比-green?style=for-the-badge" alt="Simple Comparison  | 简要对比" />
    </a>
    &nbsp;&nbsp;
    <a href="https://github.com/Mysvac/cpp-json-test">
        <img src="https://img.shields.io/badge/Comprehensive Comparison | 详细对比-yellow?style=for-the-badge" alt="Comprehensive Comparison | 详细对比" />
    </a>
</p>
</div>

---

<div id="ENGLISH">

# Modern C++JSON parsing library

## Overview
- C++17 standard
- Only standard library, cross platform
- Less than 1000 lines of code, lightweight
- Good performance
- Easy to use
- Serialization, deserialization, prettify, modification, and search
- Move and exception support
- Support installation from vcpkg


## Document
*I'm sorry, generate through document annotations, only zh_cn.*

↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓<br>
[C++Doxygen文档](https://mysvac.github.io/cpp-jsonlib/documents/html/index.html)<br>
↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

## Application examples

### 0. Import libraries and header files
You can directly download the two code files from `src` and `include` and use them in your project.


You can also install from vcpkg, with a default static library (very small).
```shell
# In classic mode
vcpkg install mysvac-jsonlib

# In manifest mode
vcpkg add port mysvac-jsonlib
```

```cmake
# CmakeLists.txt
find_package(mysvac-jsonlib CONFIG REQUIRED)
target_link_libraries(main PRIVATE mysvac::jsonlib)
```

```cpp
// C++ source file -  include header file
#include "mysvac/jsonlib.h"
```
### 1. Three operable types and six JSON data types
```cpp
// The broad "value type" includes all six JSON data types such as object arrays.
class Jsonlib::JsonValue;

// object type, essentially std::map<std::string, Jsonlib::JsonValue>
class Jsonlib::JsonObject;

// array type, essentially std::vector<Jsonlib::JsonValue>
class Jsonlib::JsonArray;

// use JsonValue.type() function, get inner date type
enum class JsonType{
    OBJECT, /**< JSON object */
    ARRAY,/**< JSON array */
    STRING, /**< JSON string */
    NUMBER, /**< JSON numver */
    BOOLEN, /**< JSON bool */
    ISNULL, /**< JSON null */
};
```

### 2. Deserialize and serialze
```cpp
// use 'Jsonlib::deserialize( std::string )' function to deserialize
Jsonlib::JsonValue json = Jsonlib::deserialize(R"__JSON__(
    {
        "语法": ["C++", "raw string", false ],
        "key": "support\t中文\\\nand\"escape",
        "na\"\\me": [ 114,514 , null ],
        "map": [ {} , [ [ "嵌套" ] , {} ] ]
    }
    )__JSON__");

// use '.serialize()' funciton to serialize and remove spaces
std::cout << json.serialize() << std::endl;
// use '.serialize_pretty()' function to serialize with line breaks and indentation
std::cout << json.serialize_pretty() << std::endl;
// You can specify the number of spaces to indent each time, which defaults to 2
std::cout << json.serialize_pretty(4) << std::endl;
```

### 3. Modification and sarch
```cpp
// this 'json' is [2. deserialize and serialze]'s json object
json.erase("na\"\\me"); // delete
json["map"][1].clear(); // clear content
json["语法"] = 114514; // change
json["add"] = Jsonlib::deserialize("[[[]]]"); // deserialze and add elements to object type
json["add"].push_back(1); // Add at the end of the array type

std::cout << json.serialize() << std::endl;
std::cout << json["key"].as_string() << std::endl; // Convert to string and escape
```
possible output:
```
{"add":[[[]],1],"key":"支持\t中文\\\n与\"转义字符","map":[{},[]],"语法":114514}
support	中文\
and"escape
```

### 4. Use 'is' and 'as' function
```cpp
Jsonlib::JsonValue value = 123456789012345ll;
// 'is_' function is noexcept
value.is_array(); // false
value.is_object(); // false
value.is_double(); // false 内部没有小数点
value.is_number(); // true int64和double都算number
// 'as_' function will throw exception if the convertion fail
value.as_int64(); // 123456789012345ll
value.as_double(); // 1.23457e+14 Could convert, but possible loss of accuracy
value.as_array(); // throw Jsonlib::JsonTypeException
```

### 5. Iterator and Move
```cpp
Jsonlib::JsonValue my_obj = Jsonlib::deserialize( R"__JSON__(
    { "key1" : [ null , 666 ] }
)__JSON__");

Jsonlib::JsonValue my_arr = Jsonlib::deserialize( R"__JSON__(
    [ null, {} ]
)__JSON__");

// string constructure， will not analyze the content
Jsonlib::JsonValue my_val {"[ {} this is string ]"};

// as_array() and as_object() return reference， other as_xxx() return copies
for(auto& it: my_arr.as_array()){ 
    // the type of 'it' is Jsonlib::JsonValue&
    // do something
}

// support move, the target being moved has changed to JsonType: ISNULL
my_arr.insert(1, std::move(my_obj["key"]));
my_arr.push_back(my_val);
// JsonArray and JsonObject and be implicit convert to JsonValue, so you can directly assigning values

std::cout << my_arr.serialize_pretty() << std::endl;
```
possible output:
```json
[
  null,
  [
    null,
    666
  ],
  { },
  "[ {} this is string ]"
]
```

### 6. Exception handle

1. `Jsonlib::JsonException` : inherits from `std::runtime_runtime_error`, is not thrown anywhere.
2. `Jsonlib::JsonTypeException` : inherits from`JsonException`, type error (e.g. type conversion failed).
3. `Jsonlib::JsonStructureException` : inherits from `JsonException`, structure error and deserialization fail
4. `std::out_of_range` : Accessing child element with `at()` but the element is out of bounds or doesn't exist

Example：
```cpp
try{
    Jsonlib::JsonValue json = Jsonlib::deserialize("[ {} } ]");
}
catch(const Jsonlib::JsonStructureException& e){
    std::cerr << "JsonStructureException: " << e.what() << std::endl;
}
catch(const Jsonlib::JsonException& e){
    std::cerr << "JsonException: " << e.what() << std::endl;
}
catch(...){ std::cerr << "other" << std::endl; }
```
possiable output:
```
JsonStructureException: Unknown Json Structure.
```

#### notes
No exceptions will be thrown by assignment/copy/move/serialization/`is_xx()`/`type()`/`size()`... operations.

Exceptions may only be thrown by `deserialize()` or failed type conversion via `as_xxx()`, or out-of-bounds access.

## Performance overview
*Time complexity is mostly theoretical - real-world gains come from constant optimizations.*
- **N** : JSON text length
- **M** : Number of child elements

Worst-case complexity:
- **Serialization** : Time O(N), Space O(N)
- **Deserialization** : Time O(N), Space O(N)
- **Key-value operations** (CRUD) : O(log m)
- **Array operations:**
    - Append/delete at end: O(1)
    - Insert/delete else where: O(m) average
    - Update/access: O(1)

</div>

---

<div id="中文">

# 现代C++ JSON解析库 

## 概述

- C++17 标准
- 仅标准库，跨平台
- 代码不到1000行，轻量
- 性能较好
- 操作非常简单
- 序列化、反序列化、美化、增删改查
- 移动语义与异常处理支持
- 支持从vcpkg安装库


## 文档
↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓<br>
[C++Doxygen文档](https://mysvac.github.io/cpp-jsonlib/documents/html/index.html)<br>
↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

## 应用示例

### 0. 导入库与头文件
你可以直接下载`src`和`include`中的两个代码文件，放到项目中使用。

也可以作为第三方库导入（默认静态库），方式如下：

```shell
# 使用vcpkg进行第三方依赖管理
# 全局模式
vcpkg install mysvac-jsonlib

# 清单模式
vcpkg add port mysvac-jsonlib
```

```cmake
# CmakeLists.txt
find_package(mysvac-jsonlib CONFIG REQUIRED)
target_link_libraries(main PRIVATE mysvac::jsonlib)
```

```cpp
// C++代码 导入头文件
#include "mysvac/jsonlib.h"
```

### 1. 三种可操作类型和六种JSON数据类型
```cpp
// 广义的“值类型”，包含 对象 数组 等全部6种JSON数据类型。
class Jsonlib::JsonValue;

// 对象类型，本质是 std::map<std::string, Jsonlib::JsonValue>
class Jsonlib::JsonObject;

// 数组类型，本质是 std::vector<Jsonlib::JsonValue>
class Jsonlib::JsonArray;


// 使用 JsonValue.type() 函数，获取内部JSON数据类型
enum class JsonType{
    OBJECT, /**< JSON 对象类型 */
    ARRAY,/**< JSON 数组类型 */
    STRING, /**< JSON 字符串类型 */
    NUMBER, /**< JSON 数值类型 */
    BOOLEN, /**< JSON 布尔类型 */
    ISNULL, /**< JSON null类型 */
};
```

### 2. 反序列化与序列化
```cpp
// 使用 Jsonlib::deserialize( str ) 函数进行反序列化
Jsonlib::JsonValue json = Jsonlib::deserialize(R"__JSON__(
    {
        "语法": ["C++", "原始字符串", false ],
        "key": "支持\t中文\\\n与\"转义字符",
        "na\"\\me": [ 114,514 , null ],
        "map": [ {} , [ [ "嵌套" ] , {} ] ]
    }
    )__JSON__");

// 对象.serialize() 序列化，不保留无效空格
std::cout << json.serialize() << std::endl;
// 对象.serialize_pretty() 序列化，带空格和换行，默认一次缩进2空格，可指定
std::cout << json.serialize_pretty() << std::endl;
```

### 3. 增删改查
```cpp
// json变量是上面【2. 反序列...】中的json变量
json.erase("na\"\\me"); // 删除
json["map"][1].clear(); // 清空
json["语法"] = 114514; // 修改
json["add"] = Jsonlib::deserialize("[[[]]]"); //增加
json["add"].push_back(1); 

std::cout << json.serialize() << std::endl;
std::cout << json["key"].as_string() << std::endl; // 获取字符串并转义
```
可能的输出：
```
{"add":[[[]],1],"key":"支持\t中文\\\n与\"转义字符","map":[{},[]],"语法":114514}
支持	中文\
与"转义字符
```

### 4.使用is检测类型，使用as获取内容
```cpp
Jsonlib::JsonValue value = 123456789012345ll;
// is保证不会抛出异常
value.is_array(); // false
value.is_object(); // false
value.is_double(); // false 内部没有小数点
value.is_number(); // true int64和double都算number
// as 转换失败时抛出异常
value.as_int64(); // 123456789012345ll
value.as_double(); // 1.23457e+14 能够转化，但可能丢失精度
value.as_array(); // 抛出异常 Jsonlib::JsonTypeException
```


### 5. 迭代器与移动语义支持
```cpp
Jsonlib::JsonValue my_obj = Jsonlib::deserialize( R"__JSON__(
    { "key1" : [ null , 666 ] }
)__JSON__");

Jsonlib::JsonValue my_arr = Jsonlib::deserialize( R"__JSON__(
    [ null, {} ]
)__JSON__");

// 字符串构造，不会解析内部数据，不会报错
Jsonlib::JsonValue my_val {"[ {} this is string ]"};

// 获取内部数据的引用
// as_array()和as_object()返回引用，其他的as_XXX()返回副本
for(auto& it: my_arr.as_array()){ 
    // it 的类型是 Jsonlib::JsonValue&
    // 具体操作...
}

// 支持移动，被移动的对象变成JsonType::ISNULL类型，不会删除
my_arr.insert(1, std::move(my_obj["key"]));
my_arr.push_back(my_val);
// JsonArray和JsonObject也能隐式转换成JsonValue，可以直接赋值或移动

std::cout << my_arr.serialize_pretty() << std::endl;
```
可能的输出：
```json
[
  null,
  [
    null,
    666
  ],
  { },
  "[ {} this is string ]"
]
```


### 6. 异常处理
本库使用了三种自定义异常和一种标准：
1. `Jsonlib::JsonException` : 继承自`std::runtime_runtime_error`，没有地方抛出此异常。
2. `Jsonlib::JsonTypeException` : 继承自`JsonException`，表示类型错误，比如`as_xxx()`函数。
3. `Jsonlib::JsonStructureException` : 继承自`JsonException`，表示JSON结构错误，导致反序列化失败。
4. `std::out_of_range` : 使用`at()`严格访问子元素，元素不存在或越界时抛出，

例：
```cpp
try{
    Jsonlib::JsonValue json = Jsonlib::deserialize("[ {}} ]");
}
catch(const Jsonlib::JsonStructureException& e){
    std::cerr << "JsonStructureException: " << e.what() << std::endl;
}
catch(const Jsonlib::JsonException& e){
    std::cerr << "JsonException: " << e.what() << std::endl;
}
catch(...){ std::cerr << "other" << std::endl; }
```
可能的输出：
```
JsonStructureException: Unknown Json Structure.
```

#### 注意
赋值/拷贝/移动/序列化/`is`/`type`/`size`...等操作保证不会抛出异常。

只有`deserialze()`反序列化函数，或者`as`类型转换失败，访问越界时可能抛出异常。


## 性能概述
*时间复杂度其实没什么用，看看就好，后面都是常数优化。*
- **N** : JSON文本长度。
- **m** : 子元素个数。
- 下面提供最坏情况的时空复杂度（虽然没什么用，后面都是常数优化。）：
- **序列化**：时间复杂度O(N)，空间复杂度O(N)。
- **反序列化**: 时间复杂度O(N)，空间复杂度O(N)。
- **键值对-增删改查**: O(log m)。
- **数组-增删**: 末尾操作O(1)，其余位置平均O(m)。
- **数组-改查**: O(1)。

</div>

---

<div id="simple-cmp">

# Simple Comparison | 简要对比
Lastest comprehensive test | 最新全面比较: <https://github.com/Mysvac/cpp-json-test>

test framework-1 | 测试框架-1 : <https://github.com/miloyip/nativejson-benchmark><br>
test framework-2 | 测试框架-2 : <https://github.com/Mysvac/cpp-json-test>

Note: The following tests were conducted earlier, `cpp-jsonlib` refers to this library.<br>
注：下面的测试进行时间较早，`cpp-jsonlib`代指本库。

## 1. Conformance testing | 一致性测试
Test the correctness, syntax rigor, floating-point precision, and other aspects of parsing JSON data using C++libraries. <br>
测试C++库解析json数据的正确性，语法严格性，浮点型精度等内容。

### Overall score | 整体分数
![整体分数](others/images/1-0-overall.png)

### Deserialize strictness test | 反序列化严格性测试
![反序列化](others/images/1-1-unserialize.png)

### Floating point precision test | 浮点数精度测试
![浮点数解析](others/images/1-2-double.png)

### String escape test | 字符串转义测试
![字符串解析](others/images/1-3-string.png)

### roundtrip | 不知道是什么
![ROUNDTRIP](others/images/1-4-roundtrip.png)

## 2. Performance testing | 性能测试

### Time for deserialize | 反序列化耗时
Unit milliseconds, the lower the better<br>
单位毫秒，越低越好
![反序列化耗时](others/images/2-1-unserialize.png)

### Memory used | 内存占用
Unit KB, the lower the better<br>
单位 KB ，越低越好
![内存占用](others/images/2-2-memory.png)

### Time for serialize | 序列化耗时
Unit milliseconds, the lower the better<br>
单位毫秒，越低越好
![序列化耗时](others/images/2-3-serialize.png)


### Time for serialize and prettify | 序列化+美化耗时
Unit KB, the lower the better<br>
单位 KB ，越低越好

Beautification refers to the output JSON text with line breaks and indentation. (The libraries not shown below do not support beautifying output directly.)<br>
美化是指输出的JSON文本带换行和缩进。（没显示的库是不支持直接美化输出。）
![美化](others/images/2-4-pretty.png)

</div>
