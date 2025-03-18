# 一个现代CPP-JSON库（效率有待提升）

## 功能概述
提供如下功能：
- 序列化，JSON控制对象->JSON文本。
- 反序列化，JSON文本 -> JSON控制对象。
- 便捷的 增、删、改、查。
- 移动语义支持。
- 异常处理支持（完善中）。

## 库内容文档
↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓<br>
[C++Doxygen文档](https://mysvac.github.io/cpp-jsonlib/documents/html/index.html)<br>
↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

## 效率概述
- **N** : JSON文本长度。
- **m** : 子元素个数。
- 下面提供最坏情况的时空复杂度（虽然没什么用，后面都是常数优化。）：
- **序列化**：时间复杂度O(N)，空间复杂度O(N)。
- **反序列化**: 时间复杂度O(N)，空间复杂度O(N)。
- **键值对-增删改查**: O(log m)。
- **数组-增删**: 末尾操作O(1)，其余位置平均O(m)。
- **数组-改查**: O(1)。

## 对比其他库
测试框架: <https://github.com/miloyip/nativejson-benchmark><br>
（不推荐尝试使用此框架，稀烂。正考虑后续自己做一个基准测试框架。）

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
非法字符串能否检测，字符转义是否正确。
![ROUNDTRIP](others/images/1-4-roundtrip.png)

### 性能测试
*我怀疑最快的几个库是多线程在跑...*

#### 反序列化耗时（越低越好）
![反序列化耗时](others/images/2-1-unserialize.png)

#### 内存占用（越低越好）
此处框架自带的QT代码内存泄漏，检测数据错误。
![内存占用](others/images/2-2-memory.png)

#### 序列化耗时
可操作对象->json字符串。<br>
![序列化耗时](others/images/2-3-serialize.png)

#### 序列化+美化耗时
美化是指输出的字符串带换行和缩进。
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

### 1. 三种类型和六中JSON类型
```cpp
// 基类 可直接使用 提供了大部分函数
class Json::JsonBasic;
// 子类，内部是JSON对象类型，提供额外函数
class Json::JsonObject;
// 子类，内部是JSON数组类型，提供额外函数
class Json::JsonArray;
// 子类，内部是值类型，提供额外函数
class Json::JsonValue;

// 使用 对象.type() 获取内部JSON数据类型
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
Json::JsonBasic json { R"__JSON__(
{
    "语法": ["C++", "原始字符串", false ],
    "key": "支持\t中文\t 与\"转义字符",
    "na\"\\me": [ 114,514 , null ],
    "map": [ {} , [ [ "嵌套" ] , {} ] ]
}
)__JSON__"};

std::cout << json.serialize() << std::endl;
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
输出：
```
{"add":[[[]],1],"key":"支持\t中文\\\n与\"转义字符","map":[{},[]],"语法":114514}
支持	中文\与"转义字符
```
### 4. 使用类型更加明确的子类
```cpp
Json::JsonObject my_obj { R"__JSON__(
    {
        "key1" : "value1",
        "key2" : [ null , 666 ]
    }
)__JSON__"};

Json::JsonArray my_arr { R"__JSON__(
    [
        null,
        {}
    ]
)__JSON__"};

// 注意，JsonValue将字符串直接视为JSON的STRING，不再解析内容
// 其他类型将字符串视为JSON数据，会解析内部内容
Json::JsonValue my_val { "[ {} string ]" };

// JsonObject 和 JsonArray类型明确，所以支持迭代器和范围for
// 注意，任何对JsonObject和JsonArray子类的访问，都会退化成JsonBasic类型
for(auto& it: my_arr){ 
    // it的类型是 JsonBasic&
    // ...
}

// 支持移动语义，移动后初始成null值，不会删除
// 三种子类都能隐式转换成父类JsonBasic，所以可以直接赋值给JsonBasic对象
my_arr.insert(1, std::move(my_obj["key2"]));
my_arr.push_back(my_val);

// 带美化（带缩进和空格）的序列化，四个类型都支持
std::cout << my_arr.serialize_pretty() << std::endl;
```
输出：
```json
[
  [
    null,
    { }
  ],
  [
    null,
    666
  ],
  "[ {} string ]"
]
```

### 5. 列表和对象的合并
JsonObject和JsonArray重载了加法运算，可以轻松合并同类对象。
```cpp
// 完全支持JSON标准的数值类型要求，可以是各种奇怪的格式
Json::JsonBasic json1 = "[ 1, 2. , 3, 4.e+ ]";
std::cout << std::boolalpha << json1[1].is_double() << std::endl;

Json::JsonArray json2 { " [ 5,6,7,8 ]" };
// 注意 .as_array() 是拷贝一份对象，C++17难以支持父类直接调用子类代码
json2 += json1.as_array();
// JsonType::STRING的to_string()输出字符串内容，其他类型等于serialize()序列号函数。
std::cout << json2.serialize() << std::endl; 

json1 = R"( { "1":"11", "2": [2 , 2] } )";
Json::JsonObject json3 { R"( {"3": false } )" };
std::cout << (json3 + json1.as_object()).serialize() << std::endl;
```
结果：
```
true
[[5,6,7,8],[1,2.,3,4.e+]]
{"1":"11","2":[2,2],"3":false}
```

### 6. 异常处理
本库定义了三种异常：
1. **Json::JsonException** : 基础自`std::runtime_runtime_error`，没有地方抛出此异常。
2. **Json::JsonTypeException** : 基础自`JsonException`，表示类型错误。<br>
（比如对值类型对象使用了`[]`运算符查找，`as_XXX()`函数类型转换失败）。
3. **Json::JsonStructureException** : 基础自`JsonException`，表示JSON结构错误。<br>
（一般只在将JSON格式的文本数据，反序列化成对象时抛出。）

本库还可能抛出一个异常: `std::out_of_range`。<br>
会出现在以下函数中：
- **.at( index/key )** : 严格的的子元素访问，禁止创建新元素，访问无效位置抛出异常。
- **\[ index/key \]** : 不严格的子元素访问，ARRAY对象越界时抛出异常。<br>
OBJECT对象可以用此方式创建新键值对，ARRAY对象可以在末尾添加元素。

例：
```cpp
try{
    Json::JsonBasic json1 = "[ {}} ]";
}
catch(const Json::JsonStructureException& e){
    std::cerr << "JsonStructureException: " << e.what() << std::endl;
}
catch(const Json::JsonException& e){
    std::cerr << "JsonException: " << e.what() << std::endl;
}
catch(...){ std::cerr << "other" << std::endl; }
```
输出：
```
JsonStructureException: Unknow element.
```
