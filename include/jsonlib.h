#pragma once

#include <string>
#include <map>
#include <set>
#include <vector>
#include <variant>
#include <exception>
#include <stdexcept>

#ifndef JSONLIB_EXPORT
    #define JSONLIB_EXPORT
#endif


namespace Jsonlib{
    /**
     * @enum JsonType
     * @brief 表示Json对象内部数据类型
     */
    enum class JsonType{
        OBJECT, /**< JSON 对象类型 */
        ARRAY,/**< JSON 数组类型 */
        STRING, /**< JSON 字符串类型，值类型 */
        NUMBER, /**< JSON 数值类型，值类型 */
        BOOLEN, /**< JSON 布尔类型，值类型 */
        ISNULL, /**< JSON null类型，值类型 */
    };


    /**
     * @class JsonException
     * @brief Json专用异常基类类
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
     * @brief Json类型错误异常类
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
     * @class JsonStructureException
     * @brief Json结构错误异常类
     */
    class JSONLIB_EXPORT JsonStructureException : public JsonException {
    public:
        /**
         * @brief 默认构造函数。
         */
        JsonStructureException() : JsonException("Unknown Json Structure.\n") {}

        /**
         * @brief 带异常描述文本的构造函数。
         */
        JsonStructureException(const std::string& message) : JsonException(message) {}
    };


    class JsonValue;

    /**
     * @brief JsonObject类型
     * @note 本质是 std::map<std::string,JsonValue>
     */
    using JsonObject = std::map<std::string,JsonValue>;

    /**
     * @brief JsonArray类型
     * @note 本质是 std::vector<JsonValue>
     */
    using JsonArray = std::vector<JsonValue>;

    /**
     * @class JsonValue
     * @brief Json数据通用类
     * @details JSON反序列化后的，可操作对象。
     * @note 可以直接使用此类。
     */
    class JSONLIB_EXPORT JsonValue{
    private:
        using JsonVar = std::variant<bool, std::string, JsonObject, JsonArray>;
        /**
         * @brief 存储JSON数据。
         * @details std::variant类型，存储JSON数据，需要C++17。
         * @note 注意，Map和List存储的也是JsonObject对象，含有嵌套内容。
         */
        JsonVar content_ { false };

        /**
         * @brief 表示当前对象存储的数据类型，
         */
        JsonType type_ { JsonType::ISNULL };

    public:
        /**
         * @brief 从str的it位置开始解析
         * @note 私有，外部不可访问
         */
        JsonValue(const std::string& str, std::string::const_iterator& it);

        /**
         * @brief 获取当前对象数据类型
         * @return Json::JsonType类型，表示当前对象数据类型
         */
        inline JsonType type() const noexcept { return type_; }

        /**
         * @brief 重置，设为null值
         * @note 不会调用其他函数
         */
        inline void reset() noexcept {
            type_ = JsonType::ISNULL;
            content_ = false;
        }

        /**
         * @brief 按类型清除
         * @details 对象变为空字典 数组变空数组 数值变0 布尔变false 字符串变空串
         * @note 不会调用其他函数
         */
        void clear() noexcept;

        /**
         * @brief 获取子元素数量
         * @details 对象和列表返回子元素个数，字符串返回长度。
         * @note 数值，布尔和null 返回值待定为1
         */
        size_t size() const noexcept;

        /**
         * @brief 默认构造函数
         * @details 默认构造，生成ISNULL类型的JSON数据。
         */
        JsonValue() = default;

        /**
         * @brief 指定类型的构造函数
         * @details 根据类型进行初始化。
         */
        JsonValue(const JsonType& jsonType);

        /**
         * @brief 解析构造函数
         * @param str JSON格式的文本数据。
         * @details 将输入字符串反序列化，生成对象。
         * @exception JsonStructureException JSON格式错误异常
         * @note 字符串将视作JSON数据进行解析，而非视作JsonType::STRING类型。
         */
        JsonValue(const std::string& str, int);

        /**
         * @brief 字符串构造函数
         * @param str 需要转义的字符串
         * @details 将输入字符串反转义，生成对象。
         * @note 字符串将视作值进行解析，而非视为JSON数据
         */
        JsonValue(const std::string& str) noexcept;

        /**
         * @brief 字符串赋值函数
         * @param str JSON格式的文本数据
         * @details 直接视为文本数据，反转义后生成对象
         * @exception JsonStructureException JSON格式错误异常
         * @note 不会视为JSON结构解析，而是直接当做JSON字符串
         */
        JsonValue& operator=(const std::string& str);


        /**
         * @brief 字符串字面量构造函数
         * @param str JSON格式的文本数据。
         * @details 将输入字符串反序列化，生成对象。
         * @exception JsonStructureException JSON格式错误异常
         * @note 字符串将视作JSON数据进行解析，而非视作JsonType::STRING类型。
         */
        inline JsonValue(const char* str){
            *this = JsonValue { std::string {str} };
        }

        /**
         * @brief 字符串字面量赋值函数
         * @param str JSON格式的文本数据。
         * @details 将输入字符串反序列化，生成对象。
         * @exception JsonStructureException JSON格式错误异常
         * @note 字符串将视作JSON数据进行解析，而非视作JsonType::STRING类型。
         */
        inline JsonValue& operator=(const char* str){
            *this = JsonValue { std::string {str} };
            return *this;
        }
        
        /**
         * @brief 布尔类型构造
         */
        inline JsonValue(const bool& bl) noexcept: type_(JsonType::BOOLEN) {
            content_ = bl;
        }
        /**
         * @brief 整数类型构造
         */
        inline JsonValue(const int& num) noexcept: type_(JsonType::NUMBER) {
            content_ = std::to_string(num);
        }
        /**
         * @brief 长整数类型构造
         */
        inline JsonValue(const long long& num) noexcept: type_(JsonType::NUMBER) {
            content_ = std::to_string(num);
        }
        /**
         * @brief 浮点数类型构造
         */
        inline JsonValue(const double& num) noexcept: type_(JsonType::NUMBER) {
            content_ = std::to_string(num);
        }
        /**
         * @brief 浮点数类型构造
         */
        inline JsonValue(const long double& num) noexcept: type_(JsonType::NUMBER) {
            content_ = std::to_string(num);
        }
        /**
         * @brief nullptr_t类型构造
         */
        inline JsonValue(const std::nullptr_t& n_ptr) noexcept{}

        /**
         * @brief JsonArray拷贝构造
         */
        JsonValue(const JsonArray& jsonArray) noexcept;
        /**
         * @brief JsonArray移动构造
         */
        JsonValue(JsonArray&& jsonArray) noexcept;
        /**
         * @brief JsonObject拷贝构造
         */
        JsonValue(const JsonObject& jsonObject) noexcept;
        /**
         * @brief JsonObject移动构造
         */
        JsonValue(JsonObject&& jsonObject) noexcept;

        /**
         * @brief JsonArray拷贝赋值
         */
        JsonValue& operator=(const JsonArray& jsonArray) noexcept;
        /**
         * @brief JsonArray移动赋值
         */
        JsonValue& operator=(JsonArray&& jsonArray) noexcept;
        /**
         * @brief JsonObject拷贝赋值
         */
        JsonValue& operator=(const JsonObject& jsonObject) noexcept;
        /**
         * @brief JsonObject移动赋值
         */
        JsonValue& operator=(JsonObject&& jsonObject) noexcept;

        /**
         * @brief 布尔类型赋值
         */
        inline JsonValue& operator=(const bool& bl) noexcept { 
            type_ = JsonType::BOOLEN;
            content_ = bl;
            return *this;
        }
        /**
         * @brief 整数类型赋值
         */
        inline JsonValue& operator=(const int& num) noexcept {
            type_ = JsonType::NUMBER;
            content_ = std::to_string(num);
            return *this;
        }
        /**
         * @brief 长整数类型赋值
         */
        inline JsonValue& operator=(const long long& num) noexcept{
            type_ = JsonType::NUMBER;
            content_ = std::to_string(num);
            return *this;
        }
        /**
         * @brief 浮点数类型赋值
         */
        inline JsonValue& operator=(const double& num) noexcept {
            type_ = JsonType::NUMBER;
            content_ = std::to_string(num);
            return *this;
        }
        /**
         * @brief 浮点数类型赋值
         */
        inline JsonValue& operator=(const long double& num) noexcept{
            type_ = JsonType::NUMBER;
            content_ = std::to_string(num);
            return *this;
        }
        /**
         * @brief null类型赋值
         */
        inline JsonValue& operator=(const std::nullptr_t& n_ptr) noexcept{
            type_ = JsonType::ISNULL;
            content_ = false;
            return *this;
        }

        /**
         * @brief 拷贝构造
         */
        JsonValue(const JsonValue& jsonValue) noexcept;

        /**
         * @brief 移动构造
         */
        JsonValue(JsonValue&& jsonValue) noexcept;

        /**
         * @brief 拷贝赋值
         */
        JsonValue& operator=(const JsonValue& jsonValue) noexcept;

        /**
         * @brief 移动赋值
         */
        JsonValue& operator=(JsonValue&& jsonValue) noexcept;

        /**
         * @brief 序列化对象
         * @details 将对象序列化，生成JSON结构的字符串，去除无效空白字符。
         * @return std::string&类型，当前对象内部字符串值的应用。
         * @note 注意，序列化包含反转义，注意区分使用。
         */
        std::string serialize() const noexcept;

        /**
         * @brief 序列化对象且美化
         * @details 对象序列化，生成JSON结构的字符串，自动生成换行和空格缩进。
         * @param space_num 每次缩进使用的空格数量 默认 2
         * @param depth 当前对象缩进次数，默认 0
         * @note 注意，序列化包含反转义，注意区分使用。
         */
        std::string serialize_pretty(const size_t& space_num = 2,const size_t& depth = 0) const noexcept;

        /**
         * @brief 判断是不是对象
         */
        inline bool is_object() const noexcept { return type_ == JsonType::OBJECT; }

        /**
         * @brief 判断是不是数组
         */
        inline bool is_array() const noexcept { return type_ == JsonType::ARRAY; }

        /**
         * @brief 判断是不是字符串
         */
        inline bool is_string() const noexcept { return type_ == JsonType::STRING; }

        /**
         * @brief 判断是不是布尔
         */
        inline bool is_bool() const noexcept { return type_ == JsonType::BOOLEN; }

        /**
         * @brief 判断是不是数值
         */
        inline bool is_number() const noexcept { return type_ == JsonType::NUMBER; }

        /**
         * @brief 判断是不是整数
         */
        inline bool is_int64() const noexcept { return type_ == JsonType::NUMBER && std::get<std::string>(content_).find('.') == std::string::npos; }

        /**
         * @brief 判断是不是浮点
         */
        inline bool is_double() const noexcept { return type_ == JsonType::NUMBER && std::get<std::string>(content_).find('.') != std::string::npos; }

        /**
         * @brief 判断是不是null
         */
        inline bool is_null() const noexcept { return type_ == JsonType::ISNULL; }

        /**
         * @brief 判断是不是值类型
         */
        inline bool is_value() const noexcept { return type_ != JsonType::OBJECT && type_ != JsonType::ARRAY; }

        /**
         * @brief 转换成long long类型，复制一份
         * @exception JsonTypeException 转换失败，类型错误
         * @note 必须是NUMBER
         */
        long long as_int64() const;
        /**
         * @brief 转换成double类型，复制一份
         * @exception JsonTypeException 转换失败，类型错误
         * @note 必须是NUMBER
         */
        double as_double() const;
        /**
         * @brief 转换成bool类型，复制一份
         * @exception JsonTypeException 转换失败，类型错误
         * @note 必须是BOOL
         */
        bool as_bool() const;
        /**
         * @brief 转换成string类型，复制一份
         * @exception JsonTypeException 转换失败，类型错误
         * @note 必须是STRING
         */
        std::string as_string() const;

        /**
         * @brief 获取内部Object对象的引用
         * @details 获取内部Object对象的引用，可调用std::map相关函数，非const，可修改
         * @exception JsonTypeException 转换失败，类型错误
         * @note 类型必须是OBJECT
         */
        JsonObject& as_object();

        /**
         * @brief 获取内部JsonObject对象的const引用
         * @details 获取内部Object对象的const引用，可调用std::map相关函数，const，不可修改内容
         * @exception JsonTypeException 转换失败，类型错误
         * @note 类型必须是OBJECT
         */
        const JsonObject& as_object() const;

        /**
         * @brief 获取内部Array对象的引用
         * @details 获取内部Array对象的引用，可调用std::vector相关函数，非const，可修改
         * @exception JsonTypeException 转换失败，类型错误
         * @note 类型必须是ARRAY
         */
        JsonArray& as_array();

        /**
         * @brief 获取内部JsonArray对象的const引用
         * @details 获取内部Array对象的引用，可调用std::vector相关函数，const，不可修改内容
         * @exception JsonTypeException 转换失败，类型错误
         * @note 类型必须是ARRAY
         */
        const JsonArray& as_array() const;

        /**
         * @brief at元素访问
         * @details at访问，带越界检查，必须是ARRAY类型
         * @exception JsonTypeException 类型错误异常
         * @note 不会创建新元素
         */
        JsonValue& at(const size_t& index);
        /**
         * @brief at元素访问
         * @details at访问，带越界检查，必须是OBJECT类型
         * @exception JsonTypeException 类型错误异常
         * @note 不会创建新元素
         */
        JsonValue& at(const std::string& key);
        /**
         * @brief []元素访问
         * @details []访问，带越界检查，必须是ARRAY类型
         * @exception JsonTypeException 类型错误异常
         * @note 如果指向末尾，会创建新元素，
         */
        JsonValue& operator[](const size_t& index);
        /**
         * @brief []元素访问
         * @details []访问，带越界检查，必须是OBJECT类型
         * @exception JsonTypeException 类型错误异常
         * @note 如果key不存在，会创建新元素，所以可以直接赋值。
         */
        JsonValue& operator[](const std::string& key);

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
        void push_back(const JsonValue& jsonBasic);

        /**
         * @brief 末尾插入元素
         * @note 必须是ARRAY类型
         */
        void push_back(JsonValue&& jsonBasic);

        /**
         * @brief 在指定位置插入元素
         * @note 必须是ARRAY类型
         */
        void insert(const size_t& index, const JsonValue& jsonBasic);

        /**
         * @brief 在指定位置移入元素
         * @note 必须是ARRAY类型
         */
        void insert(const size_t& index, JsonValue&& jsonBasic);

        /**
         * @brief 插入键值对
         * @note 必须是Object类型
         */
        void insert(const std::string& key,const JsonValue& jsonBasic);

        /**
         * @brief 移动插入键值对
         * @note 必须是Object类型
         */
        void insert(const std::string& key,JsonValue&& jsonBasic);

        /**
         * @brief 移动插入键值对
         * @note 必须是Object类型
         */
        void insert(const std::pair<const std::string, JsonValue>& p);

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
    };

    /**
     * @brief 反序列化函数
     * @param str 需要反转义的原JSON字符串
     * @note 解析JSON数据统一使用此函数
     */
    JSONLIB_EXPORT
    inline JsonValue deserialize(const std::string& str){
        return JsonValue (str, 1);
    }

}
