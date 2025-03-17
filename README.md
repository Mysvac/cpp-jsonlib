# 一个现代CPP-JSON库（效率有待提升）

## 概要介绍

### 功能概述
提供如下功能：
- 序列化，JSON控制对象->JSON文本。
- 反序列化，JSON文本 -> JSON控制对象。
- 便捷的 增、删、改、查。
- 移动语义支持。
- 异常处理支持（完善中）。

### 效率概述
- **N** : JSON文本长度。
- **m** : 子元素个数。
- 下面提供最坏情况的时空复杂度（虽然没什么用，后面都是常数优化。）：
- **序列化**：时间复杂度O(N)，空间复杂度O(N)。
- **反序列化**: 时间复杂度O(N)，空间复杂度O(N)。
- **键值对-增删改查**: O(log m)。
- **数组-增删**: 末尾操作O(1)，其余位置平均O(m)。
- **数组-改查**: O(1)。

## 与其他C++JSON解析库的对比
测试框架:<https://github.com/miloyip/nativejson-benchmark>

### 一致性测试

#### 整体分数
![整体分数](others/1-0-overall.png)

#### 反序列化测试
json字符串->可操作对象。<br>
反序列化错误格式能否识别，正确格式能否解析准确等。
![反序列化](others/1-1-unserialize.png)

#### 浮点数解析
解析后的精度差，各自格式的解析能力。
![浮点数解析](others/1-2-double.png)

#### 字符串解析
非法字符串能否检测，字符转义是否正确。
![字符串解析](others/1-3-string.png)

#### roundtrip不知道是什么
非法字符串能否检测，字符转义是否正确。
![ROUNDTRIP](others/1-4-roundtrip.png)

### 性能测试
*我怀疑最快的几个库是多线程在跑...*

#### 反序列化耗时（越低越好）
![反序列化耗时](others/2-1-unserialize.png)

#### 内存占用（越低越好）
此处框架自带的QT代码内存泄漏，检测数据错误。
![内存占用](others/2-2-memory.png)

#### 序列化耗时
可操作对象->json字符串。<br>
![序列化耗时](others/2-3-serialize.png)

#### 序列化+美化耗时
美化是指输出的字符串带换行和缩进。
![美化](others/2-4-pretty.png)

## 简单示例

### 导入库
（暂未合并vcpkg官方仓库，提供源码，自行编译。）
```cmake
find_package(cpp-jsonlib CONFIG REQUIRED)
get_target_property(LIB_TYPE jsonlib::jsonlib TYPE)
```

### 1.使用JsonBasic类型
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

### 3. 增删改查
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
{"add":[[[]],1],"key":"支持\t中文\\\n与\"转义字符","map":[{},[]],"语法":114514}
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

禁止其他类型的操作。

- 提供范围for支持。
- 提供迭代器支持。
- 提供加法运算，合并两个对象内容。

完善中...

### JsonArray 类型
**Json的数组类型。**<br>
JsonBasic的子类，内部必然存放`JsonType::ARRAY`类型的数据。<br>
（即内部必然是数组。）

禁止其他类型的操作。

- 提供范围for支持。
- 提供迭代器支持。
- 提供加法运算，合并两个对象内容。

完善中...

### JsonValue 类型
**Json的值类型。**<br>
JsonBasic的子类，内部存放`JsonType::ARRAY`和`JsonType::OBJECT`以外的数据。


提供额外的操作函数，并禁止其他类型的操作。

提供更加简单的赋值，拼接，字符串化等操作。

完善中...

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

## 函数表

### JsonBasic类成员函数

- JsonBasic() : 默认构造函数，生成 null 类型的json数据对象。
- JsonBasic(const JsonType& jsonType) : 带类型的构造函数，按照类型默认初始化
- JsonBasic(const char* str) : 文本字面量构造函数，解析文件生成对象。
- JsonBasic(const std::string& str) : 字符串构造函数，解析字符串生成对象。
- JsonBasic(const int& tmp) : 整数构造函数，生成NUMBER类型对象。
- JsonBasic(const long long& tmp) : 整数构造函数，生成NUMBER类型对象。
- JsonBasic(const double& tmp) : 浮点数构造函数，生成NUMBER类型对象。
- 上述函数的赋值运算符版本。
- 子类和自身类型的 **拷贝** 与 **移动** 构造函数和赋值函数。

---

- JsonType type() : 获取内部数据类型。
- std::string serialize() : 序列化，去除所有无效空格。
- std::string serialize_pretty() : 序列化，含美化（含空格，自动控制缩进）。
- void reset() : 重置内部数据为 null。
- void clear() : 根据当前类型，初始化内部值。
- size_t size() : 获取子元素个数。（字符串和数值返回长度，布尔和null返回1）。
- JsonBasic& at(index / key) ： 访问子元素，带越界检查，不会创建新元素。
- JsonBasic& operator\[index / key\] ： 访问子元素，带越界检查，可能创建新元素。
- bool operator== : 序列化后比较，耗时较长。
- bool hasKey(const std::string& key) : 检测是否包含某个key。
- std::set\<std::string\> getKeys() : 获取key的集合。
- void push_back : 数组尾部插入
- void insert : 数组指定位置插入，或键值对插入。
- void erase : 删除数组指定位置，或某个key的元素。
- bool is_xxx : 判断是不是xxx类型。
- xxx as_xxx : 尝试转换成xxx类型。

### JsonObject类成员函数

- JsonBasic类中可对OBJECT类型使用的全部函数。
- begin(),end() : 迭代器支持，范围for支持。
- \+ 和 \+= 运算符重载 ： 合并两个JsonObject类型。
- 拷贝与移动支持。

### JsonArray类成员函数

- JsonBasic类中可对ARRAY类型使用的全部函数。
- begin(),end() : 迭代器支持，范围for支持。
- \+ 和 \+= 运算符重载 ： 合并两个JsonArray类型。
- 拷贝与移动支持。

### JsonValue类成员函数

- JsonBasic类中可对ARRAY类型使用的全部函数。
- 更多更直接的构造函数。制作中...
- 更多更直接的比较运算符。制作中...
- 更多更直接的操作运算符。制作中...

