# 一个简陋的CPP-JSON库（效率不高）

## 概要介绍

### 功能概述
提供如下功能：
- 序列化，JSON控制对象->JSON文本。
- 反序列化，JSON文本 -> JSON控制对象。
- 增、删、改、查。
- 移动语义支持。
- 异常处理支持（完善中）。

### 效率概述
- **N** : JSON文本长度。
- **m** : 子元素个数。
- 下面提供最坏情况的时空复杂度：
- **序列化**：时间复杂度O(N)，空间复杂度O(N)。
- **反序列化**: 时间复杂度O(N)，空间复杂度O(N)。
- **键值对-增删改查**: O(log m)。
- **数组-增删**: 末尾操作O(1)，其余位置平均O(m)。
- **数组-改查**: O(1)。


### 简单示例

#### 导入库
（暂未合并vcpkg官方仓库，提供源码，自行编译。）
```cmake
find_package(cpp-jsonlib CONFIG REQUIRED)
get_target_property(LIB_TYPE jsonlib::jsonlib TYPE)
```

#### 1.使用JsonBasic类型
```cpp
Json::JsonBasic json {};
json = "{ \"key\": [ true, 12, \"val\" ] }";
Json::JsonBasic json2 { R"__JSON__(
{
    "语法": ["C++", "原始字符串", false ],
    "key": "支持\t中文\t 与\"转义字符",
    "na\"\\me": [ 114,514 , null ],
    "map": [ {} , [ [ "嵌套" ] , {} ] ]
}
)__JSON__"};
```

### 2.反序列化与序列化
```cpp
Json::JsonBasic json {};
json = "{ \"key\": [ true, 12, \"val\" ] }";
Json::JsonBasic json2 { R"__JSON__(
{
    "语法": ["C++", "原始字符串", false ],
    "key": "支持\t中文\t 与\"转义字符",
    "na\"\\me": [ 114,514 , null ],
    "map": [ {} , [ [ "嵌套" ] , {} ] ]
}
)__JSON__"};

std::cout << json2.serialize() << std::endl;
```

3. 增删改查
```cpp
Json::JsonBasic json { R"__JSON__(
{
    "语法": ["C++", "原始字符串", false ],
    "key": "支持\t中文\\与\"转义字符",
    "na\"\\me": [ 114,514 , null ],
    "map": [ {} , [ [ "嵌套" ] , {} ] ]
}
)__JSON__"};

json.erase("na\"\\me"); // 删除
json["map"][1].clear(); // 清空
json["语法"] = 114514; // 修改
json["add"] = "[[[]]]"; //增加
json["add"].push_back(Json::JsonBasic { 1 }); // 此处不完善，只能push对象
// 支持移动语义

std::cout << json.serialize() << std::endl;
std::cout << json["key"].as_string() << std::endl; // 获取字符串并转义
```
输出：
```
{ "add": [ [ [ ] ], 1 ], "key": "支持\t中文\\\n与\"转义字符", "map": [ { }, [ ] ], "语法": 114514 }
支持	中文\与"转义字符
```

## 类型列表

### JsonType 枚举
表示当前JSON对象存放的数据类型：
```cpp
enum class JsonType{
    /** 对象，内部存放键值对 */
    OBJECT,
    /** 列表，内部多个任意类型 */
    ARRAY,
    /** 字符串 */
    STRING,
    /** 数值 */
    NUMBER,
    /** true or false */
    BOOLEN,
    /** null */
    ISNULL,
};
```

### JsonBasic 类型
**本库中所有JSON数据类型的基类。**<br>
内部封装了大部分算法，可以直接使用。

其他高级类型的直接内部元素访问，会退化成此类型。

### JsonObject 类型
**Json的对象类型。**<br>
JsonBasic的子类，内部必然存放`JsonType::OBJECT`类型的数据。<br>
（即内部必然是键值对。）

提供额外的操作函数，并禁止其他类型的操作。

制作中...

### JsonArray 类型
**Json的数组类型。**<br>
JsonBasic的子类，内部必然存放`JsonType::ARRAY`类型的数据。<br>
（即内部必然是数组。）

提供额外的操作函数，并禁止其他类型的操作。

制作中...

### JsonValue 类型
**Json的值类型。**<br>
JsonBasic的子类，内部存放`JsonType::ARRAY`和`JsonType::OBJECT`以外的数据。


提供额外的操作函数，并禁止其他类型的操作。

提供更加简单的赋值，拼接，字符串化等操作。

制作中...

### JsonException 异常
**本库中异常的基类。**<br>
`std::runtime_error`的子类。

### JsonStructureException 异常
**表示JSON文本存在格式错误。**

`JsonException`的子类。<br>
通常在反序列化，生成对象时出现。

### JsonTypeException 异常
**表示JSON对象内部数据类型错误。**

`JsonException`的子类。<br>

通常在操作对象时出现，比如对`OBJECT`对象使用了`push_back()`函数。<br>
（`push_back()`函数用在`ARRAY`末尾插入元素，不能用在`OBJECT`。）


