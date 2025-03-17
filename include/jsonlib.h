#pragma once

#include <string>
#include <map>
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
         * @brief 序列号构造函数。
         * @param str JSON格式的文本数据。
         * @details 将输入字符串反序列化，生成对象。
         * @exception JsonException JSON格式错误异常
         */
        JsonBasic(const std::string& str);

        /**
         * @brief 字符串赋值反序列化
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
         * @brief int64类型构造
         */
        JsonBasic(const long long& tmp) noexcept;

        /**
         * @brief 浮点类型构造
         */
        JsonBasic(const double& tmp) noexcept;

        /**
         * @brief long long类型赋值
         */
        JsonBasic& operator=(const int& tmp) noexcept;

        /**
         * @brief long long类型赋值
         */
        JsonBasic& operator=(const long long& tmp) noexcept;
        /**
         * @brief long long类型赋值
         */
        JsonBasic& operator=(const double& tmp) noexcept;

        JsonBasic(const JsonObject& jsonObject) noexcept;
        JsonBasic(const JsonArray& jsonArray) noexcept;
        JsonBasic(const JsonValue& jsonValue) noexcept;
        JsonBasic(JsonObject&& jsonObject) noexcept;
        JsonBasic(JsonArray&& jsonArray) noexcept;
        JsonBasic(JsonValue&& jsonValue) noexcept;
        JsonBasic& operator=(const JsonObject& jsonObject) noexcept;
        JsonBasic& operator=(const JsonArray& jsonArray) noexcept;
        JsonBasic& operator=(const JsonValue& jsonValue) noexcept;
        JsonBasic& operator=(JsonObject&& jsonObject) noexcept;
        JsonBasic& operator=(JsonArray&& jsonArray) noexcept;
        JsonBasic& operator=(JsonValue&& jsonValue) noexcept;

    private:
        JsonBasic(const std::string& str, size_t& index, const size_t& tail);

    protected:
        /**
         * @var content_
         * @brief 存储JSON数据。
         * @details std::variant类型，存储JSON数据，需要C++17。
         * @note 注意，Map和List存储的也是JsonObject对象，含有嵌套内容。
         */
        JsonVar content_ { std::string { "null" } };

        /**
         * @var type_
         * @brief 表示当前对象数据类型，
         */
        JsonType type_ { JsonType::ISNULL };

    public:

        /**
         * @brief 获取当前对象数据类型
         */
        JsonType type() const noexcept { return type_; }

        /**
         * @brief 获取内部容器，键值对。
         * @exception JsonTypeException
         * @note 必须是OBJECT类型，否则抛出异常。
         */
        const Map& getMapConst() const;

        /**
         * @brief 获取内部容器，键列表。
         * @exception JsonTypeException
         * @note 必须是ARRAY类型，否则抛出异常。
         */
        const List& getListConst() const;

        /**
         * @brief 序列化对象
         * @details 将对象序列号，生成JSON文本，去除无效空白字符。
         * @note 注意，序列化包含转义符号，不推荐此函数查询string数据内容。
         */
        std::string serialize() const noexcept;

        /**
         * @brief 序列化对象且美化
         * @details 将对象序列号，生成JSON文本，包含空格和换行。
         * @note 注意，序列化包含转义符号，不推荐此函数查询string数据内容。
         */
        std::string serialize_pretty(const size_t& space_num = 2,const size_t& depth = 0) const noexcept;

        /**
         * @brief 重置
         * @note 变为 null 值
         */
        void reset() noexcept;

        /**
         * @brief 按类型清除
         * @note 对象变为空自动 数组变空数组 数值变0 布尔变false 字符串变空串
         */
        void clear() noexcept;

        /**
         * @brief 获取子元素数量
         * @details 对象和列表返回子元素个数，字符串和数值会返回长度。
         * @note 布尔和null 返回值待定，请勿使用
         */
        size_t size() const noexcept;

        /**
         * @brief at，带越界检查
         * @note 不会创建新元素
         */
        JsonBasic& at(const size_t& index);
        /**
         * @brief at，带越界检查
         * @note 不会创建新元素
         */
        JsonBasic& at(const std::string& key);
        /**
         * @brief []，带越界检查
         * @note 可能会创建新元素，如果指向末尾
         */
        JsonBasic& operator[](const size_t& index);
        /**
         * @brief []，带越界检查
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

        /**
         * @brief 检查是否存在某个key
         * @exception JsonTypeException 非对象类型抛出异常
         * @note 必须是OBJECT类型
         */
        bool hasKey(const std::string& key) const;

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
         * @brief 删除指定位置的元素
         * @note 必须是ARRAY类型
         */
        void erase(const size_t& index);
        /**
         * @brief 删除key的元素
         * @note 必须是OBJECT类型
         */
        void erase(const std::string& key);

        bool is_object() const noexcept { return type_ == JsonType::OBJECT; }
        bool is_array() const noexcept { return type_ == JsonType::ARRAY; }
        bool is_string() const noexcept { return type_ == JsonType::STRING; }
        bool is_bool() const noexcept { return type_ == JsonType::BOOLEN; }
        bool is_number() const noexcept { return type_ == JsonType::NUMBER; }
        bool is_int64() const noexcept { return type_ == JsonType::NUMBER && std::get<std::string>(content_).find('.') == std::string::npos; }
        bool is_double() const noexcept { return type_ == JsonType::NUMBER && std::get<std::string>(content_).find('.') != std::string::npos; }
        bool is_null() const noexcept { return type_ == JsonType::ISNULL; }
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

        JsonObject as_object() const;
        JsonArray as_array() const ;
        JsonValue as_value() const;
    };


    /**
     * @class JsonObject
     * @brief Json对象类型对象。
     * @details JSON反序列化后的，可操作对象。
     * @note 可以直接使用此类。
     */
    class JSONLIB_EXPORT JsonObject : public JsonBasic{
    public:
        const List& getListConst() const = delete;
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

        JsonObject(): JsonBasic( "{}" ){ }

        explicit JsonObject(const JsonBasic& jsonBasic): JsonBasic(jsonBasic){
            if(!jsonBasic.is_object()) throw JsonTypeException { "JsonObject's type must be JsonType::OBJECT.\n" };
        }

        /**
         * 不再进行类型检查
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
        const Map& getMapConst() const = delete;
        void reset() noexcept = delete;
        JsonBasic& at(const std::string& key) = delete;
        bool hasKey(const std::string& key) const = delete;
        void insert(const std::string& key,const JsonBasic& jsonBasic) = delete;
        void insert(const std::string& key,JsonBasic&& jsonBasic) = delete;
        void erase(const std::string& key) = delete;
        long long as_int64() const = delete;
        double as_double() const = delete;
        bool as_bool() const = delete;
        std::string as_string() const = delete;

        JsonArray(): JsonBasic( "[]" ){ }
        explicit JsonArray(const JsonBasic& jsonBasic): JsonBasic(jsonBasic){
            if(!jsonBasic.is_array()) throw JsonTypeException { "JsonArray's type must be JsonType::ARRAY.\n" };
        }

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
        const List& getListConst() const = delete;
        const Map& getMapConst() const = delete;
        JsonBasic& at(const size_t& index) = delete;
        JsonBasic& at(const std::string& key) = delete;
        bool hasKey(const std::string& key) const = delete;
        void push_back(const JsonBasic& jsonBasic) = delete;
        void push_back(JsonBasic&& jsonBasic) = delete;
        void insert(const size_t& index, const JsonBasic& jsonBasic) = delete;
        void insert(const size_t& index, JsonBasic&& jsonBasic) = delete;
        void insert(const std::string& key,const JsonBasic& jsonBasic) = delete;
        void insert(const std::string& key,JsonBasic&& jsonBasic) = delete;
        void erase(const size_t& index) = delete;
        void erase(const std::string& key) = delete;

        JsonValue(): JsonBasic(){ }

        

        explicit JsonValue(const JsonBasic& jsonBasic): JsonBasic(jsonBasic){
            if(jsonBasic.is_array() || jsonBasic.is_object()) throw JsonTypeException { "JsonValue's type must be not JsonType::ARRAY and JsonType::OBJECT.\n" };
        }

    };


}
