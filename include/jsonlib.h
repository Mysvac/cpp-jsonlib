#pragma once

#include <string>
#include <map>
#include <set>
#include <vector>
#include <variant>
#include <exception>
#include <stdexcept>

#if !defined(JSONLIB_EXPORT)
    #define JSONLIB_EXPORT
#endif

namespace Json{

    /**
     * @enum JsonType
     * @brief 表示Json对象类型。
     * @details 这个枚举类定义了6种Json数据类型。
     */
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

    /**
     * @class JsonException
     * @brief Json专用异常类。
     * @note 继承自std::runtime_error。
     */
    class JSONLIB_EXPORT JsonException : public std::runtime_error {
    public:
        /**
         * @brief 默认构造函数。
         */
        JsonException() : std::runtime_error("Unknown JSON Exception.\n") {}

        /**
         * @brief 带异常描述文本的构造函数。
         */
        JsonException(const std::string& message) : std::runtime_error(message) {}
    };

    /**
     * @class JsonTypeException
     * @brief Json专用异常类。
     * @note 继承自std::runtime_error。
     */
    class JSONLIB_EXPORT JsonTypeException : public JsonException {
        public:
            /**
             * @brief 默认构造函数。
             */
            JsonTypeException() : JsonException("Unknown JSON Type Exception.\n") {}
    
            /**
             * @brief 带异常描述文本的构造函数。
             */
            JsonTypeException(const std::string& message) : JsonException(message) {}
    };

    /**
     * @class JsonException
     * @brief Json专用异常类。
     * @note 继承自std::runtime_error。
     */
    class JSONLIB_EXPORT JsonStructureException : public JsonException {
        public:
            /**
             * @brief 默认构造函数。
             */
            JsonStructureException() : JsonException("Unknown JSON Type Exception.\n") {}
    
            /**
             * @brief 带异常描述文本的构造函数。
             */
            JsonStructureException(const std::string& message) : JsonException(message) {}
    };

    class JsonObject;
    class JsonArray;
    class JsonValue;

    /**
     * @class JsonBasic
     * @brief Json数据对象。
     * @details JSON反序列化后的，可操作对象。
     * @note 可以直接使用此类。
     */
    class JSONLIB_EXPORT JsonBasic{
    public:
        using Map = std::map<std::string,JsonBasic>;
        using List = std::vector<JsonBasic>;
        using JsonVar = std::variant<std::string, Map, List>;
        /**
         * @brief 默认构造函数。
         * @details 默认构造，生成ISNULL类型的JSON数据。
         * @note 生成的是 null 这样一个JSON对象
         */
        JsonBasic() = default;

        /**
         * @brief 指定类型的构造函数。
         * @details 根据类型进行初始化。
         */
        JsonBasic(const JsonType& jsonType);

        /**
         * @brief 字符串字面量构造函数
         * @param str JSON格式的文本数据。
         * @details 将输入字符串反序列化，生成对象。
         * @exception JsonException JSON格式错误异常
         */
        JsonBasic(const char* str){
            *this = JsonBasic { std::string {str} };
        }
        
        /**
         * @brief 字符串构造函数。
         * @param str JSON格式的文本数据。
         * @details 将输入字符串反序列化，生成对象。
         * @exception JsonException JSON格式错误异常
         */
        JsonBasic(const std::string& str);

        /**
         * @brief 字符串赋值
         * @param str JSON格式的文本数据
         * @details 将输入字符串反序列化，生成对象。
         * @exception JsonException JSON格式错误异常
         */
        JsonBasic& operator=(const std::string& str);

        /**
         * @brief 拷贝构造
         */
        JsonBasic(const JsonBasic& jsonBasic) noexcept;

        /**
         * @brief 移动构造
         */
        JsonBasic(JsonBasic&& jsonBasic) noexcept;

        /**
         * @brief 拷贝赋值
         */
        JsonBasic& operator=(const JsonBasic& jsonBasic) noexcept;

        /**
         * @brief 移动赋值
         */
        JsonBasic& operator=(JsonBasic&& jsonBasic) noexcept;

        /**
         * @brief int类型构造
         */
        JsonBasic(const int& tmp) noexcept;
        /**
         * @brief long long类型构造
         */
        JsonBasic(const long long& tmp) noexcept;

        /**
         * @brief double类型构造
         */
        JsonBasic(const double& tmp) noexcept;

        /**
         * @brief int类型赋值
         */
        JsonBasic& operator=(const int& tmp) noexcept;

        /**
         * @brief long long类型赋值
         */
        JsonBasic& operator=(const long long& tmp) noexcept;
        /**
         * @brief double类型赋值
         */
        JsonBasic& operator=(const double& tmp) noexcept;

        /**
         * @brief 对象类型拷贝构造
         */
        JsonBasic(const JsonObject& jsonObject) noexcept;

        /**
         * @brief 数组类型拷贝构造
         */
        JsonBasic(const JsonArray& jsonArray) noexcept;

        /**
         * @brief 值类型拷贝构造
         */
        JsonBasic(const JsonValue& jsonValue) noexcept;

        /**
         * @brief 对象类型移动构造
         */
        JsonBasic(JsonObject&& jsonObject) noexcept;

        /**
         * @brief 数组类型移动构造
         */
        JsonBasic(JsonArray&& jsonArray) noexcept;

        /**
         * @brief 值类型移动构造
         */
        JsonBasic(JsonValue&& jsonValue) noexcept;

        /**
         * @brief 对象类型拷贝赋值
         */
        JsonBasic& operator=(const JsonObject& jsonObject) noexcept;

        /**
         * @brief 数组类型拷贝赋值
         */
        JsonBasic& operator=(const JsonArray& jsonArray) noexcept;

        /**
         * @brief 值类型拷贝赋值
         */
        JsonBasic& operator=(const JsonValue& jsonValue) noexcept;

        /**
         * @brief 对象类型移动赋值
         */
        JsonBasic& operator=(JsonObject&& jsonObject) noexcept;

        /**
         * @brief 数组类型移动赋值
         */
        JsonBasic& operator=(JsonArray&& jsonArray) noexcept;

        /**
         * @brief 值类型移动赋值
         */
        JsonBasic& operator=(JsonValue&& jsonValue) noexcept;

    private:
        /**
         * @brief 递归辅助构造函数
         * @note 私有
         */
        JsonBasic(const std::string& str, size_t& index, const size_t& tail);

    protected:
        /**
         * @brief 存储JSON数据。
         * @details std::variant类型，存储JSON数据，需要C++17。
         * @note 注意，Map和List存储的也是JsonObject对象，含有嵌套内容。
         */
        JsonVar content_ { std::string { "null" } };

        /**
         * @brief 表示当前对象存储的数据类型，
         */
        JsonType type_ { JsonType::ISNULL };

        /**
         * @brief 否定类型的拷贝构造函数
         * @note 保证事务性，要么成功，要么不修改任何内容。仅供子类使用。
         */
        JsonBasic(const JsonBasic& jsonBasic, const JsonType& not_jsonType1 ,const JsonType& not_jsonType2);
        /**
         * @brief 否定类型的移动构造函数
         * @note 保证事务性，要么成功，要么不修改任何内容。仅供子类使用。
         */
        JsonBasic(JsonBasic&& jsonBasic , const JsonType& not_jsonType1, const JsonType& not_jsonType2);
        /**
         * @brief 指定类型的拷贝构造函数
         * @note 保证事务性，要么成功，要么不修改任何内容。仅供子类使用。
         */
        JsonBasic(const JsonBasic& jsonBasic, const JsonType& jsonType);
        /**
         * @brief 指定类型的移动构造函数
         * @note 保证事务性，要么成功，要么不修改任何内容。仅供子类使用。
         */
        JsonBasic(JsonBasic&& jsonBasic , const JsonType& jsonType);

    public:
        // JsonVar& getVarRef() noexcept { return content_; }

        /**
         * @brief 获取当前对象数据类型
         */
        JsonType type() const noexcept { return type_; }

        /**
         * @brief 获取内部容器只读对象，键值对。
         * @exception JsonTypeException
         * @note 必须是OBJECT类型，否则抛出异常。
         */
        const Map& getMapConst() const;

        /**
         * @brief 获取内部容器只读对象，键列表。
         * @exception JsonTypeException
         * @note 必须是ARRAY类型，否则抛出异常。
         */
        const List& getListConst() const;

        /**
         * @brief 获取内部容器只读对象，值类型。
         * @exception JsonTypeException
         * @note 必须是ARRAY类型，否则抛出异常。
         */
        const std::string& getStringConst() const;

        /**
         * @brief 序列化对象
         * @details 将对象序列号，生成JSON文本，去除无效空白字符。
         * @note 注意，序列化包含转义符号，不推荐此函数查询string数据内容。
         */
        std::string serialize() const noexcept;

        /**
         * @brief 序列化对象且美化
         * @details 将对象序列号，生成JSON文本，包含空格和换行。
         * @param space_num 每次缩进使用的空格数量 默认 2
         * @param depth 当前对象缩进次数，默认 0
         * @note 注意，序列化包含转义符号，不推荐此函数查询string数据内容。
         */
        std::string serialize_pretty(const size_t& space_num = 2,const size_t& depth = 0) const noexcept;

        /**
         * @brief 重置，设为null值
         * @note 不会调用其他函数
         */
        void reset() noexcept;

        /**
         * @brief 按类型清除
         * @details 对象变为空字典 数组变空数组 数值变0 布尔变false 字符串变空串
         * @note 不会调用其他函数
         */
        void clear() noexcept;

        /**
         * @brief 获取子元素数量
         * @details 对象和列表返回子元素个数，字符串和数值会返回长度。
         * @note 布尔和null 返回值待定为1
         */
        size_t size() const noexcept;

        /**
         * @brief 列表at，带越界检查
         * @note 不会创建新元素
         */
        JsonBasic& at(const size_t& index);
        /**
         * @brief 对象at，带越界检查
         * @note 不会创建新元素
         */
        JsonBasic& at(const std::string& key);
        /**
         * @brief 列表[]，带越界检查
         * @note 可能会创建新元素，如果指向末尾
         */
        JsonBasic& operator[](const size_t& index);
        /**
         * @brief 对象[]，带越界检查
         * @note 可能会创建新元素，如果元素不存在
         */
        JsonBasic& operator[](const std::string& key);


        /**
         * @brief 比较运算
         * @details 比较内容，Object内部顺序无影响，ARRAY内部顺序有影响
         * @note 序列化后再比较，耗时较长，不应频繁使用
         */
        bool operator==(const std::string& key) const;
        /**
         * @brief 比较运算
         * @details 比较内容，Object内部顺序无影响，ARRAY内部顺序有影响
         * @note 序列化后再比较，耗时较长，不应频繁使用
         */
        bool operator==(const JsonBasic& jsonBasic) const noexcept;

        bool operator==(const JsonObject& jsonObject) const noexcept;
        bool operator==(const JsonArray& jsonArray) const noexcept;
        bool operator==(const JsonValue& JsonValue) const noexcept;

        /**
         * @brief 检查是否存在某个key
         * @exception JsonTypeException 非对象类型抛出异常
         * @note 必须是OBJECT类型
         */
        bool hasKey(const std::string& key) const;

        /**
         * @brief 获取key的集合
         * @exception JsonTypeException 非对象类型抛出异常
         * @note 必须是OBJECT类型
         */
        std::set<std::string> getKeys() const;

        /**
         * @brief 末尾插入元素
         * @exception JsonTypeException 非数组类型抛出异常
         * @note 必须是ARRAY类型
         */
        void push_back(const JsonBasic& jsonBasic);

        /**
         * @brief 末尾插入元素
         * @note 必须是ARRAY类型
         */
        void push_back(JsonBasic&& jsonBasic);

        /**
         * @brief 在指定位置插入元素
         * @note 必须是ARRAY类型
         */
        void insert(const size_t& index, const JsonBasic& jsonBasic);

        /**
         * @brief 在指定位置移入元素
         * @note 必须是ARRAY类型
         */
        void insert(const size_t& index, JsonBasic&& jsonBasic);

        /**
         * @brief 插入键值对
         * @note 必须是Object类型
         */
        void insert(const std::string& key,const JsonBasic& jsonBasic);

        /**
         * @brief 移动插入键值对
         * @note 必须是Object类型
         */
        void insert(const std::string& key,JsonBasic&& jsonBasic);

        /**
         * @brief 移动插入键值对
         * @note 必须是Object类型
         */
        void insert(const std::pair<const std::string, JsonBasic>& p);

        /**
         * @brief 删除指定位置的元素
         * @note 必须是ARRAY类型
         */
        void erase(const size_t& index);
        /**
         * @brief 删除key的元素
         * @note 必须是OBJECT类型
         */
        void erase(const std::string& key);

        /**
         * @brief 判断是不是对象
         */
        bool is_object() const noexcept { return type_ == JsonType::OBJECT; }

        /**
         * @brief 判断是不是数组
         */
        bool is_array() const noexcept { return type_ == JsonType::ARRAY; }

        /**
         * @brief 判断是不是字符串
         */
        bool is_string() const noexcept { return type_ == JsonType::STRING; }

        /**
         * @brief 判断是不是布尔
         */
        bool is_bool() const noexcept { return type_ == JsonType::BOOLEN; }

        /**
         * @brief 判断是不是数值
         */
        bool is_number() const noexcept { return type_ == JsonType::NUMBER; }

        /**
         * @brief 判断是不是整数
         */
        bool is_int64() const noexcept { return type_ == JsonType::NUMBER && std::get<std::string>(content_).find('.') == std::string::npos; }

        /**
         * @brief 判断是不是浮点
         */
        bool is_double() const noexcept { return type_ == JsonType::NUMBER && std::get<std::string>(content_).find('.') != std::string::npos; }

        /**
         * @brief 判断是不是null
         */
        bool is_null() const noexcept { return type_ == JsonType::ISNULL; }

        /**
         * @brief 判断是不是值类型
         */
        bool is_value() const noexcept { return type_ != JsonType::OBJECT && type_ != JsonType::ARRAY; }

        /**
         * @brief 转换成long long类型
         * @exception JsonTypeException
         * @note 必须是NUMBER
         */
        long long as_int64() const;
        /**
         * @brief 转换成double类型
         * @exception JsonTypeException
         * @note 必须是NUMBER
         */
        double as_double() const;
        /**
         * @brief 转换成bool类型
         * @exception JsonTypeException
         * @note 必须是BOOL
         */
        bool as_bool() const;
        /**
         * @brief 转换成string类型
         * @exception JsonTypeException
         * @note 必须是STRING
         */
        std::string as_string() const;

        /**
         * @brief 转换成字符串
         * @note STRING类型则转义输出，非STRING则序列化
         */
        std::string to_string() const noexcept;

        /**
         * @brief 拷贝一份Object对象
         * @exception JsonTypeException
         * @note 类型必须是OBJECT
         */
        JsonObject as_object() const;

        /**
         * @brief 拷贝一份Array对象
         * @exception JsonTypeException
         * @note 类型必须是ARRAY
         */
        JsonArray as_array() const ;

        /**
         * @brief 拷贝一份值对象
         * @exception JsonTypeException
         * @note 必须是值类型
         */
        JsonValue as_value() const;
    };


    /**
     * @class JsonObject
     * @brief Json对象类型对象。
     * @details JSON反序列化后的，可操作对象。
     * @note 提供额外操作方法，可以直接使用此类。
     */
    class JSONLIB_EXPORT JsonObject : public JsonBasic{
    public:
        using JsonBasic::insert;
        using JsonBasic::erase;
        using JsonBasic::at;

        // 非对象类型函数，静止使用
        const List& getListConst() const = delete;
        const std::string& getStringConst() const = delete;
        void reset() noexcept = delete;
        JsonBasic& at(const size_t& index) = delete;
        void push_back(const JsonBasic& jsonBasic) = delete;
        void push_back(JsonBasic&& jsonBasic) = delete;
        void insert(const size_t& index, const JsonBasic& jsonBasic) = delete;
        void insert(const size_t& index, JsonBasic&& jsonBasic) = delete;
        void erase(const size_t& index) = delete;
        long long as_int64() const = delete;
        double as_double() const = delete;
        bool as_bool() const = delete;
        std::string as_string() const = delete;

        /**
         * @brief 列表初始化生成器
         */
        JsonObject(std::initializer_list<std::pair<const std::string, JsonBasic>> map){
            type_ = JsonType::OBJECT;
            content_ = Map (map); 
        }

        /**
         * @brief 默认构造函数
         */
        JsonObject(): JsonBasic( JsonType::OBJECT ){ }

        /**
         * @brief 显示拷贝构造函数
         */
        explicit JsonObject(JsonBasic&& jsonBasic): JsonBasic(std::move(jsonBasic), JsonType::OBJECT){}
        /**
         * @brief 显示移动构造函数
         */
        explicit JsonObject(const JsonBasic& jsonBasic): JsonBasic(jsonBasic, JsonType::OBJECT){}
        
        JsonObject& operator=(const JsonBasic& jsonBasic);
        JsonObject& operator=(JsonBasic&& jsonBasic);

        JsonObject(const JsonObject& jsonObject) noexcept;
        JsonObject(JsonObject&& jsonObject) noexcept;
        JsonObject& operator=(const JsonObject& jsonObject) noexcept;
        JsonObject& operator=(JsonObject&& jsonObject) noexcept;

        /**
         * @brief 迭代器
         */
        auto begin() { return std::get<Map>(content_).begin(); }
        /**
         * @brief 迭代器
         */
        auto end() { return std::get<Map>(content_).end(); }
        /**
         * @brief 迭代器
         */
        auto begin() const { return std::get<Map>(content_).begin(); }
        /**
         * @brief 迭代器
         */
        auto end() const { return std::get<Map>(content_).end(); }

        /**
         * @brief 比较运算符
         */
        bool operator==(const std::string& jsonBasic) const noexcept;
        /**
         * @brief 比较运算符
         */
        bool operator==(const JsonBasic& jsonBasic) const noexcept;
        /**
         * @brief 比较运算符
         */
        bool operator==(const JsonObject& jsonBasic) const noexcept;
        /**
         * @brief 比较运算符
         */
        bool operator==(const JsonArray& jsonBasic) const noexcept { return false; }
        /**
         * @brief 比较运算符
         */
        bool operator==(const JsonValue& jsonBasic) const noexcept { return false; }

        /**
         * @brief 合并运算
         */
        JsonObject operator+(const JsonObject& jsonObject) const noexcept;
        /**
         * @brief 合并运算
         */
        JsonObject& operator+=(const JsonObject& jsonObject) noexcept;

        /**
         * @brief 内部元素访问
         * @note 不再进行类型检测。
         */
        JsonBasic& operator[](const std::string& key);
    };


    /**
     * @class JsonArray
     * @brief Json列表类型对象。
     * @details JSON反序列化后的，可操作对象。
     * @note 可以直接使用此类。
     */
    class JSONLIB_EXPORT JsonArray : public JsonBasic{
    public:
        using JsonBasic::insert;
        using JsonBasic::erase;
        using JsonBasic::at;

        // 非数组类型函数，静止使用

        const Map& getMapConst() const = delete;
        const std::string& getStringConst() const = delete;
        void reset() noexcept = delete;
        JsonBasic& at(const std::string& key) = delete;
        bool hasKey(const std::string& key) const = delete;
        std::set<std::string> getKeys() const = delete;
        void insert(const std::string& key,const JsonBasic& jsonBasic) = delete;
        void insert(const std::string& key,JsonBasic&& jsonBasic) = delete;
        void insert(const std::pair<const std::string, JsonBasic>& p) = delete;
        void erase(const std::string& key) = delete;
        long long as_int64() const = delete;
        double as_double() const = delete;
        bool as_bool() const = delete;
        std::string as_string() const = delete;

        /**
         * @brief 列表初始化生成器
         */
        JsonArray(std::initializer_list<JsonBasic> list){
            type_ = JsonType::ARRAY;
            content_ = List (list); 
        }

        /**
         * @brief 默认构造函数
         */
        JsonArray(): JsonBasic( JsonType::ARRAY ){ }

        /**
         * @brief 显示拷贝构造函数
         */
        explicit JsonArray(JsonBasic&& jsonBasic): JsonBasic(std::move(jsonBasic), JsonType::ARRAY){}
        /**
         * @brief 显示移动构造函数
         */
        explicit JsonArray(const JsonBasic& jsonBasic): JsonBasic(jsonBasic, JsonType::ARRAY){}
        JsonArray& operator=(const JsonBasic& jsonBasic);
        JsonArray& operator=(JsonBasic&& jsonBasic);

        JsonArray(const JsonArray& jsonArray) noexcept;
        JsonArray(JsonArray&& jsonArray) noexcept;
        JsonArray& operator=(const JsonArray& jsonArray) noexcept;
        JsonArray& operator=(JsonArray&& jsonArray) noexcept;

        /**
         * @brief 迭代器
         */
        auto begin() { return std::get<List>(content_).begin(); }
        /**
         * @brief 迭代器
         */
        auto end() { return std::get<List>(content_).end(); }
        /**
         * @brief 迭代器
         */        
        auto begin() const { return std::get<List>(content_).begin(); }
        /**
         * @brief 迭代器
         */        
        auto end() const { return std::get<List>(content_).end(); }

        /**
         * @brief 比较运算符
         */
        bool operator==(const std::string& jsonBasic) const noexcept;
        /**
         * @brief 比较运算符
         */
        bool operator==(const JsonBasic& jsonBasic) const noexcept;
        /**
         * @brief 比较运算符
         */
        bool operator==(const JsonObject& jsonBasic) const noexcept { return false; }
        /**
         * @brief 比较运算符
         */
        bool operator==(const JsonArray& jsonBasic) const noexcept;
        /**
         * @brief 比较运算符
         */
        bool operator==(const JsonValue& jsonBasic) const noexcept { return false; }

        /**
         * @brief 合并运算
         */
        JsonArray operator+(const JsonArray& jsonArray) const noexcept;
        /**
         * @brief 合并运算
         */
        JsonArray& operator+=(const JsonArray& jsonArray) noexcept;

        /**
         * 不再进行类型检查
         */
        JsonBasic& operator[](const size_t& index);
    };

    /**
     * @class JsonValue
     * @brief Json值类型对象。
     * @details JSON反序列化后的，可操作对象。
     * @note 可以直接使用此类。
     */
    class JSONLIB_EXPORT JsonValue : public JsonBasic{
    public:
        // 类型不匹配，已经删除的函数

        const List& getListConst() const = delete;
        const Map& getMapConst() const = delete;
        JsonBasic& at(const size_t& index) = delete;
        JsonBasic& at(const std::string& key) = delete;
        bool hasKey(const std::string& key) const = delete;
        std::set<std::string> getKeys() const = delete;
        void push_back(const JsonBasic& jsonBasic) = delete;
        void push_back(JsonBasic&& jsonBasic) = delete;
        void insert(const size_t& index, const JsonBasic& jsonBasic) = delete;
        void insert(const size_t& index, JsonBasic&& jsonBasic) = delete;
        void insert(const std::string& key,const JsonBasic& jsonBasic) = delete;
        void insert(const std::string& key,JsonBasic&& jsonBasic) = delete;
        void insert(const std::pair<const std::string, JsonBasic>& p) = delete;
        void erase(const size_t& index) = delete;
        void erase(const std::string& key) = delete;

        /**
         * @brief 默认构造函数
         */
        JsonValue(): JsonBasic( JsonType::ISNULL ){ }

        /**
         * @brief 显示拷贝构造函数
         */
        explicit JsonValue(JsonBasic&& jsonBasic): JsonBasic(std::move(jsonBasic), JsonType::OBJECT, JsonType::ARRAY){}
        /**
         * @brief 显示移动构造函数
         */
        explicit JsonValue(const JsonBasic& jsonBasic): JsonBasic(jsonBasic, JsonType::OBJECT, JsonType::ARRAY){}

        JsonValue& operator=(const JsonBasic& jsonBasic);
        JsonValue& operator=(JsonBasic&& jsonBasic);

        JsonValue(const JsonValue& jsonValue) noexcept;
        JsonValue(JsonValue&& jsonValue) noexcept;
        JsonValue& operator=(const JsonValue& jsonValue) noexcept;
        JsonValue& operator=(JsonValue&& jsonValue) noexcept;

        /**
         * @brief 比较运算符
         */
        bool operator==(const std::string& jsonBasic) const noexcept;
        /**
         * @brief 比较运算符
         */
        bool operator==(const JsonBasic& jsonBasic) const noexcept;
        /**
         * @brief 比较运算符
         */
        bool operator==(const JsonObject& jsonBasic) const noexcept { return false; }
        /**
         * @brief 比较运算符
         */
        bool operator==(const JsonArray& jsonBasic) const noexcept { return false; }
        /**
         * @brief 比较运算符
         */
        bool operator==(const JsonValue& jsonBasic) const noexcept;

    };

}
