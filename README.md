# 一个现代CPP-JSON库

## 功能概述
提供如下功能：
- 序列化，JSON控制对象->JSON文本。
- 反序列化，JSON文本 -> JSON控制对象。
- 便捷的 增、删、改、查。
- 移动语义支持。
- 异常处理支持。

## 库内容文档
↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓<br>
[C++Doxygen文档](https://mysvac.github.io/cpp-jsonlib/documents/html/index.html)<br>
↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

## 效率概述
*时间复杂度其实没什么用，看看就好，后面都是常数优化。*
- **N** : JSON文本长度。
- **m** : 子元素个数。
- 下面提供最坏情况的时空复杂度（虽然没什么用，后面都是常数优化。）：
- **序列化**：时间复杂度O(N)，空间复杂度O(N)。
- **反序列化**: 时间复杂度O(N)，空间复杂度O(N)。
- **键值对-增删改查**: O(log m)。
- **数组-增删**: 末尾操作O(1)，其余位置平均O(m)。
- **数组-改查**: O(1)。

## 对比其他库
测试框架-1（不推荐使用）: <https://github.com/miloyip/nativejson-benchmark><br>
测试框架-2 : <https://github.com/Mysvac/cpp-json-test>


### 一致性测试
测试C++库解析json数据的正确性，语法严格性，浮点型精度等内容。

#### 整体分数
![整体分数](others/images/1-0-overall.png)

#### 反序列化测试
json字符串->可操作对象。<br>
反序列化错误格式能否识别，正确格式能否解析准确等。
![反序列化](others/images/1-1-unserialize.png)

#### 浮点数解析
解析后的精度差，各自格式的解析能力。
![浮点数解析](others/images/1-2-double.png)

#### 字符串解析
非法字符串能否检测，字符转义是否正确。
![字符串解析](others/images/1-3-string.png)

#### roundtrip不知道是什么
![ROUNDTRIP](others/images/1-4-roundtrip.png)

### 性能测试

#### 反序列化耗时（越低越好）
![反序列化耗时](others/images/2-1-unserialize.png)

#### 内存占用（越低越好）
![内存占用](others/images/2-2-memory.png)

#### 序列化耗时
可操作对象->json字符串。<br>
![序列化耗时](others/images/2-3-serialize.png)

#### 序列化+美化耗时
美化是指输出的JSON文本带换行和缩进。（下面没显示的库就是不支持美化输出。）
![美化](others/images/2-4-pretty.png)

## 应用示例

### 0. 导入库与头文件
（暂未合并vcpkg官方仓库，提供源码，自行编译。）
```cmake
# CmakeLists.txt
find_package(cpp-jsonlib CONFIG REQUIRED)
target_link_libraries(main PRIVATE jsonlib::jsonlib)
```
```cpp
#include "jsonlib.h"
```

### 1. 三种可操作类型和六种JSON数据类型
```cpp
// 广义的“值类型”，实际上包含 对象 数组 等全部6种JSON数据类型。
class Jsonlib::JsonValue;
// 对象类型，可以转换成JsonValue，本质是 std::map<std::string, Jsonlib::JsonValue>
class Jsonlib::JsonObject;
// 数组类型，可以转换成JsonValue，本质是 std::vector<Jsonlib::JsonValue>
class Jsonlib::JsonArray;


// 使用 JsonValue.type() 函数，获取内部JSON数据类型
enum class JsonType{
    OBJECT, /**< JSON 对象类型 */
    ARRAY,/**< JSON 数组类型 */
    STRING, /**< JSON 字符串类型，值类型 */
    NUMBER, /**< JSON 数值类型，值类型 */
    BOOLEN, /**< JSON 布尔类型，值类型 */
    ISNULL, /**< JSON null类型，值类型 */
};
```

### 2. 反序列化与序列化
```cpp
Jsonlib::JsonValue json { R"__JSON__(
{
    "语法": ["C++", "原始字符串", false ],
    "key": "支持\t中文\t 与\"转义字符",
    "na\"\\me": [ 114,514 , null ],
    "map": [ {} , [ [ "嵌套" ] , {} ] ]
}
)__JSON__"};

std::cout << json.serialize() << std::endl;
```

特别注意，`std::string`类型的构造和赋值，会被认为是JSON文本输入，会解析内容。<br>
如果希望生成 纯字符串类型 的可操作对象，参考下方代码：
```cpp
// 0. 字符串构造，将被认为是json数据并解析
Jsonlib::JsonValue json0 { "123 456" }; // 错误，解析内部数据失败，抛出异常.


// 1. 双重分号，检测到双引号，则作为字符串解析。 内部不能出现特殊符号，否则行为未定义。
Jsonlib::JsonValue json1 { R"__( "123 456")__" };

Jsonlib::JsonValue json2{ R"__( "123
345" )__" }; // json2错误，字符串内部出现了换行符，行为未定义。 必须写成 R"__( "123\n456" )__"

// 2. 使用set_string()，会自动反转义字符，内部可以出现转义字符。
Jsonlib::JsonValue json3;
json3.set_string("123
456"); // 正确，自动反转义，然后两端加上双引号，变成 "123\n345"

```

### 3. 增删改查
```cpp
// json变量是上面【2. 反序列...】中的json变量

json.erase("na\"\\me"); // 删除
json["map"][1].clear(); // 清空
json["语法"] = 114514; // 修改
json["add"] = "[[[]]]"; //增加
json["add"].push_back(1); 

std::cout << json.serialize() << std::endl;
std::cout << json["key"].as_string() << std::endl; // 获取字符串并转义
```
可能的输出：
```
{"add":[[[]],1],"key":"支持\t中文\\\n与\"转义字符","map":[{},[]],"语法":114514}
支持	中文\与"转义字符
```

### 4.使用is检测类型，使用as获取内容
```cpp
Jsonlib::JsonValue value{"123456789012345"};
// is保证不会抛出异常
value.is_array(); // false
value.is_object(); // false
value.is_double(); // false 内部没有小数点
value.is_number(); // true int64和double都算number
// as 转换失败时抛出异常
std::cout << value.as_int64(); // 123456789012345
std::cout << value.as_double(); // 123456789012345.0 能够转化，但可能丢失精度
std::cout << value.as_array(); // 抛出异常 Jsonlib::JsonTypeException
```


### 5. 无损获取array或object类型的引用
```cpp
Jsonlib::JsonValue my_obj { R"__JSON__(
    {
        "key1" : "value1",
        "key2" : [ null , 666 ]
    }
)__JSON__"};

Jsonlib::JsonValue my_arr { R"__JSON__(
    [
        null,
        {}
    ]
)__JSON__"};

// 纯文本类型，需要调用set_string创建，不能直接构造或赋值
Jsonlib::JsonValue my_val;
my_val.set_string("[ {} this is string ]");

// as_array() 和 as_object() 返回引用，其他的 as_XXX() 返回副本
for(auto& it: my_arr.as_array()){ 
    // 操作...
}

// 支持移动语义，移动后初始成null值，不会删除
my_arr.insert(1, std::move(my_obj["key2"]));
my_arr.push_back(my_val);
// JsonArray和JsonObject也能隐式转换成JsonValue，可以直接赋值或移动

// 带美化（缩进和空格）的序列化
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
本库定义了三种异常：
1. **Jsonlib::JsonException** : 继承自`std::runtime_runtime_error`，没有地方抛出此异常。
2. **Jsonlib::JsonTypeException** : 继承自`JsonException`，表示类型错误。<br>
（比如对值类型使用了`[]`运算符，或`as_XXX()`类型转换失败。）
3. **Jsonlib::JsonStructureException** : 继承自`JsonException`，表示JSON结构错误。<br>
（将JSON格式的文本 反序列化失败时 抛出。）

本库还可能抛出一个异常: `std::out_of_range`。<br>
会出现在以下函数中：
- **.at( index/key )** : 严格的的子元素访问，禁止创建新元素，访问无效位置抛出异常。
- **\[ index/key \]** : 不严格的子元素访问，ARRAY对象越界时抛出异常。<br>
OBJECT对象可以用此方式创建新键值对，ARRAY对象可以在末尾添加元素。

例：
```cpp
try{
    Jsonlib::JsonValue json = "[ {}} ]";
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
JsonStructureException: Unknow element.
```

#### 注意
同类赋值/拷贝/移动/序列化/is类型检查/type()/size()...等操作保证不会抛出异常。

只有文本解析构造/赋值函数，或者类型转换失败，访问越界时可能抛出异常。


