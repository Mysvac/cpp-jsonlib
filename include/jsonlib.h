#pragma once

#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <variant>
#include <exception>

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
     * @class JsonException
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
    class JSONLIB_EXPORT JsonIndexException : public JsonException {
        public:
            /**
             * @brief 默认构造函数。
             */
            JsonIndexException() : JsonException("Unknown Json Index Exception.\n") {}
    
            /**
             * @brief 带异常描述文本的构造函数。
             */
            JsonIndexException(const std::string& message) : JsonException(message) {}
    };


    /**
     * @class JsonReader
     * @brief Json文本分割工具。
     * @details 此类对象不能完全解析JSON数据，每次仅解析一层，实现数据分割。
     * @note 这是JsonObject的工具类，不可独立使用。
     */
    class JSONLIB_EXPORT JsonReader{
    public:
        friend class JsonBasic;
        /**
         * @brief 默认构造，暂时删除。
         */
        JsonReader() = delete;

        /**
         * @brief 拷贝构造，暂时删除。
         */
        JsonReader(const JsonReader& jsonReader) = delete;

        /**
         * @brief 移动构造，暂时删除。
         */
        JsonReader(JsonReader&& jsonReader) = delete;

        /**
         * @brief 拷贝赋值，暂时删除。
         */
        JsonReader& operator=(const JsonReader& jsonReader) = delete;

        /**
         * @brief 移动赋值，暂时删除。
         */
        JsonReader& operator=(JsonReader&& jsonReader) = delete;
        
    private:
        /**
         * @brief 带文本输入的构造函数。
         * @param str JSON格式的纯文本。
         * @note 注意输入str是引用类型，创建过程中外部不允许修改str字符串。注意，私有函数，仅友元JsonObject可使用此类。
         * @throw JsonException JSON格式错误
         */
        JsonReader(const std::string& str);

        using Map = std::unordered_map<std::string,std::string>;
        using List = std::vector<std::string>;
        using JsonVar = std::variant<std::string, Map, List>;
        /**
         * @var content_
         * @brief 存储JSON数据。
         * @details std::variant类型，存储JSON数据，需要C++17。
         * @note 注意，Map和List存储的都是纯字符串格式。
         */
        JsonVar content_ { Map{} };

        /**
         * @var content_
         * @brief 存储JSON数据类型。
         */
        JsonType type_ { JsonType::OBJECT };

    public:
        /**
         * @brief 获取当前对象的JSON数据类型
         * @return JsonType 枚举值
         */
        JsonType getType() const noexcept{ return type_; }

        /**
         * @brief 获取当前对象的字符串信息。
         * @note 要求当前JSON数据，不是对象或列表类型。
         */
        std::string& getString(){ return std::get<std::string>(content_); }

        /**
         * @brief 获取当前对象的键值对信息。
         * @note 要求当前JSON数据是对象类型。
         */
        const Map& getMap(){ return std::get<Map>(content_); }

        /**
         * @brief 获取当前对象的列表信息。
         * @note 要求当前JSON数据是列表类型。
         */
        const List& getList(){ return std::get<List>(content_); }
    };


    /**
     * @class JsonObject
     * @brief Json数据对象。
     * @details JSON反序列化后的，可操作对象。
     * @note 请直接使用此类。
     */
    class JSONLIB_EXPORT JsonBasic{
    public:
        using Map = std::unordered_map<std::string,JsonBasic>;
        using List = std::vector<JsonBasic>;
        using JsonVar = std::variant<std::string, Map, List>;
        /**
         * @brief 默认构造函数。
         * @details 默认构造，生成OBJECT类型的空内容JSON对象
         * @note 生成的是 { } 这样一个JSON对象
         */
        JsonBasic() = default;

        /**
         * @brief 带文本输入的构造函数。
         * @param str JSON格式的文本数据。
         * @details 将输入字符串反序列化，生成对象。
         * @exception JsonException JSON格式错误异常
         * @note 内部调用JsonReader类对象，辅助反序列化
         */
        JsonBasic(const std::string& str);

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
         * @brief 字符串赋值
         */
        JsonBasic& operator=(const std::string& str);

    protected:
        /**
         * @var content_
         * @brief 存储JSON数据。
         * @details std::variant类型，存储JSON数据，需要C++17。
         * @note 注意，Map和List存储的也是JsonObject对象，含有嵌套内容。
         */
        JsonVar content_ { Map{} };

        /**
         * @var type_
         * @brief 表示当前对象数据类型，
         */
        JsonType type_ { JsonType::OBJECT };

    public:

        /**
         * @brief 获取当前对象数据类型
         */
        JsonType getType()const noexcept{ return type_; }

        /**
         * @brief 获取当前对象信息。
         * @note 要求当前JSON数据，不是对象或列表类型。
         */
        std::string& getString(){ return std::get<std::string>(content_); }

        /**
         * @brief 获取当前对象的键值对信息。
         * @note 要求当前JSON数据是对象类型。
         */
        const Map& getMap() const { return std::get<Map>(content_); }

        /**
         * @brief 获取当前对象的列表信息。
         * @note 要求当前JSON数据是列表类型。
         */
        const List& getList() const { return std::get<List>(content_); }

        /**
         * @brief 获取当前数据内容
         */
        JsonVar getVariant() const noexcept {return content_; }

        /**
         * @brief 序列化对象
         * @details 将对象序列号，生成JSON文本。
         * @note 注意，序列化包含转义符号，不推荐此函数查询string数据内容。
         */
        std::string serialize() const noexcept;

        /**
         * @brief 清空内容
         * @note 变为 { } 
         */
        virtual void clear() noexcept;

        /**
         * @brief at，带越界检查
         */
        JsonBasic& at(const size_t& index);
        /**
         * @brief at，带越界检查
         */
        JsonBasic& at(const std::string& key);
        /**
         * @brief []，不带越界检查
         */
        JsonBasic& operator[](const size_t& index);
        /**
         * @brief []，不带越界检查
         */
        JsonBasic& operator[](const std::string& key);

        /**
         * @brief 检查是否存在某个key
         * @note 必须是OBJECT类型
         */
        bool hasKey(const std::string& key) const;

        /**
         * @brief 末尾插入元素
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
         * @brief 在指定位置插入元素
         * @note 必须是ARRAY类型
         */
        void insert(const size_t& index, JsonBasic&& jsonBasic);

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
         * @brief 获取当前层次子元素个数。
         * @note 如果不是object和array类型，返回字符串长度。
         */
        size_t size()const noexcept;

        bool is_object() const noexcept { return type_ == JsonType::OBJECT; }
        bool is_array() const noexcept { return type_ == JsonType::ARRAY; }
        bool is_string() const noexcept { return type_ == JsonType::STRING; }
        bool is_bool() const noexcept { return type_ == JsonType::BOOLEN; }
        bool is_number() const noexcept { return type_ == JsonType::NUMBER; }
        bool is_int64() const noexcept { return type_ == JsonType::NUMBER && std::get<std::string>(content_).find('.') == std::string::npos; }
        bool is_double() const noexcept { return type_ == JsonType::NUMBER && std::get<std::string>(content_).find('.') != std::string::npos; }
        bool is_null() const noexcept { return type_ == JsonType::ISNULL; }
        bool is_value() const noexcept { return type_ != JsonType::OBJECT && type_ != JsonType::ARRAY; }

        long as_int64() const;
        double as_double() const;
        bool as_bool() const;
        bool as_null() const noexcept;
        std::string as_string() const;
        // JsonObject as_object() const;
        // JsonArray as_array() const;
        // JsonValue as_value() const;
    };


    class JSONLIB_EXPORT JsonObject : public JsonBasic{
    public:
        JsonObject(): JsonBasic("{}"){}
        JsonObject(const std::string& str);
        // explicit JsonObject(const JsonBasic& jsonBasic);

        void clear() noexcept override;
    };

    
    class JSONLIB_EXPORT JsonArray : public JsonBasic{
    public:
        JsonArray(): JsonBasic("[]"){}
        JsonArray(const std::string& str);
        // explicit JsonArray(const JsonBasic& jsonBasic);

        void clear() noexcept override;
    };
    
    class JSONLIB_EXPORT JsonValue : public JsonBasic{
    public:
        JsonValue(): JsonBasic("null"){}

        JsonValue(const std::string& str);

        // explicit JsonValue(const JsonBasic& jsonBasic);

        void clear() noexcept override;
    };

}
