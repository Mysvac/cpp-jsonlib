# 一个略显简陋的CPP-JSON库

## 概要介绍

### 功能概述
已完成类型`JsonBasic`，可以处理全部JSON数据类型。<br>
提供如下功能：
1. 序列化，JSON格式文本转JsonBasic对象。
2. 反序列化，JsonBasic对象转JSON格式文本。
3. 类型查询，当前对象数据类型（对象、列表、字符串、布尔、数值、null）。
4. 获取内部值。
5. 移动赋值，移动构造。
6. 增、删、改、查。
7. 异常处理（完善中）。

### 粗略性能概述
**N** : JSON格式文本长度。

**反序列化** : 时间复杂度O(N)，空间复杂度O(N)，稳定，常数项接近1。

**序列化** : 常规数据时间复杂度O(N)~O(NlogN)，空间复杂度相同。特殊极端数据达到O(N*N)。<br>
（什么是极端数据？\[\[\[\[\[\[\[\[\[\[\[\[ \]\]\]\]\]\]\]\]\]\]\]\]这种深度和长度一个数量级的就是。）

**值获取** : 键值对默认哈希字典，平均O(1)。列表默认Vector，引索访问O(1);

**值插入删除** : 列表尾部修改函数O(1)，指定位置修改O(n)。字典修改平均O(1)。

**值修改** : 移动语义支持，减少复制开销，

### 简单示例
使用`JsonBasic`数据类型。<br>
或使用`stojson()`函数，会清除无效空格，部分情况速度更快。
```cpp
Json::JsonBasic json1 { "[1, null, {}, true , {\"key\" : \"值\" }]" };

Json::JsonBasic json2 = Json::stojson(R"__JSON__(
{
    "method": ["C++", "原始字符串", "语法" , 114514],
    "name": "cpp-\t-jsonlib",
    "version": "0.1.0",
    "description": "A simple JSON library using C++17.",
    "homepage": "https://github.com/Mysvac/cpp-jsonlib"
}
)__JSON__"
);

json1.erase(4);
json1.push_back( Json::stojson("666") );
json2["method"][0] = R"("Python")";
json2.erase("description");
json2["homepage"].clear();

std::cout << json1.serialize() << std::endl;
std::cout << json2["name"].as_string() << std::endl;
std::cout << json2.serialize() << std::endl;
```

结果：
```txt
[ 1, null, { }, true, 666 ]
cpp-	-jsonlib
{ "method": [ "Python", "原始字符串", "语法", 114514 ], "name": "cpp-\t-jsonlib", "version": "0.1.0", "homepage": { } }
```



