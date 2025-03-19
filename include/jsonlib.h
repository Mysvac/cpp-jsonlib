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

namespace Json{

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
     * @brief Json数据基类
     * @details JSON反序列化后的，可操作对象。
     * @note 可以直接使用此类。
     */
    class JSONLIB_EXPORT JsonBasic{
    public:
        using Map = std::map<std::string,JsonBasic>;
        using List = std::vector<JsonBasic>;
        using JsonVar = std::variant<std::string, Map, List>;
        /**
         * @brief 默认构造函数
         * @details 默认构造，生成ISNULL类型的JSON数据。
         */
        JsonBasic() = default;

        /**
         * @brief 指定类型的构造函数
         * @details 根据类型进行初始化。
         */
        JsonBasic(const JsonType& jsonType);

        /**
         * @brief 字符串字面量构造函数
         * @param str JSON格式的文本数据。
         * @details 将输入字符串反序列化，生成对象。
         * @exception JsonStructureException JSON格式错误异常
         * @note 字符串将视作JSON数据进行解析，而非视作JsonType::STRING类型。
         */
        inline JsonBasic(const char* str){
            *this = JsonBasic { std::string {str} };
        }

        /**
         * @brief 字符串字面量赋值函数
         * @param str JSON格式的文本数据。
         * @details 将输入字符串反序列化，生成对象。
         * @exception JsonStructureException JSON格式错误异常
         * @note 字符串将视作JSON数据进行解析，而非视作JsonType::STRING类型。
         */
        inline JsonBasic& operator=(const char* str){
            *this = JsonBasic { std::string {str} };
            return *this;
        }
        
        /**
         * @brief 字符串构造函数
         * @param str JSON格式的文本数据。
         * @details 将输入字符串反序列化，生成对象。
         * @exception JsonStructureException JSON格式错误异常
         * @note 字符串将视作JSON数据进行解析，而非视作JsonType::STRING类型。
         */
        JsonBasic(const std::string& str);

        /**
         * @brief 字符串赋值函数
         * @param str JSON格式的文本数据
         * @details 将输入字符串反序列化，生成对象。
         * @exception JsonStructureException JSON格式错误异常
         * @note 字符串将视作JSON数据进行解析，而非视作JsonType::STRING类型。
         */
        JsonBasic& operator=(const std::string& str);

        /**
         * @brief 布尔类型构造
         */
        inline JsonBasic(const bool& bl) noexcept: type_(JsonType::BOOLEN) {
            content_ = bl ? std::string {"true"} : std::string {"false"};
        }
        /**
         * @brief 整数类型构造
         */
        inline JsonBasic(const int& num) noexcept: type_(JsonType::NUMBER) {
            content_ = std::to_string(num);
        }
        /**
         * @brief 长整数类型构造
         */
        inline JsonBasic(const long long& num) noexcept: type_(JsonType::NUMBER) {
            content_ = std::to_string(num);
        }
        /**
         * @brief 浮点数类型构造
         */
        inline JsonBasic(const double& num) noexcept: type_(JsonType::NUMBER) {
            content_ = std::to_string(num);
        }
        /**
         * @brief 浮点数类型构造
         */
        inline JsonBasic(const long double& num) noexcept: type_(JsonType::NUMBER) {
            content_ = std::to_string(num);
        }
        /**
         * @brief nullptr_t类型构造
         */
        inline JsonBasic(const std::nullptr_t& n_ptr) noexcept: type_(JsonType::ISNULL) {
            content_ = std::string {"null"};
        }

        /**
         * @brief 布尔类型赋值
         */
        inline JsonBasic& operator=(const bool& bl) noexcept { 
            type_ = JsonType::BOOLEN;
            content_ = bl ? std::string {"true"} : std::string {"false"};
            return *this;
        }
        /**
         * @brief 整数类型赋值
         */
        inline JsonBasic& operator=(const int& num) noexcept {
            type_ = JsonType::NUMBER;
            content_ = std::to_string(num);
            return *this;
        }
        /**
         * @brief 长整数类型赋值
         */
        inline JsonBasic& operator=(const long long& num) noexcept{
            type_ = JsonType::NUMBER;
            content_ = std::to_string(num);
            return *this;
        }
        /**
         * @brief 浮点数类型赋值
         */
        inline JsonBasic& operator=(const double& num) noexcept {
            type_ = JsonType::NUMBER;
            content_ = std::to_string(num);
            return *this;
        }
        /**
         * @brief 浮点数类型赋值
         */
        inline JsonBasic& operator=(const long double& num) noexcept{
            type_ = JsonType::NUMBER;
            content_ = std::to_string(num);
            return *this;
        }
        /**
         * @brief null类型赋值
         */
        inline JsonBasic& operator=(const std::nullptr_t& n_ptr) noexcept{
            type_ = JsonType::ISNULL;
            content_ = std::string {"null"};
            return *this;
        }

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
         * @brief 私有-递归辅助构造函数
         * @note 私有，外部不可访问
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
         * @brief 保护-否定类型的拷贝构造函数
         * @details 含类型要求的拷贝构造函数。保证事务性，要么成功，要么不修改任何内容。仅供子类使用。
         */
        JsonBasic(const JsonBasic& jsonBasic, const JsonType& not_jsonType1 ,const JsonType& not_jsonType2);
        /**
         * @brief 保护-否定类型的移动构造函数
         * @details 含类型要求的移动构造函数。保证事务性，要么成功，要么不修改任何内容。仅供子类使用。
         */
        JsonBasic(JsonBasic&& jsonBasic , const JsonType& not_jsonType1, const JsonType& not_jsonType2);
        /**
         * @brief 保护-指定类型的拷贝构造函数
         * @details 含类型要求的移动构造函数。保证事务性，要么成功，要么不修改任何内容。仅供子类使用。
         */
        JsonBasic(const JsonBasic& jsonBasic, const JsonType& jsonType);
        /**
         * @brief 保护-指定类型的移动构造函数
         * @details 含类型要求的移动构造函数。保证事务性，要么成功，要么不修改任何内容。仅供子类使用。
         */
        JsonBasic(JsonBasic&& jsonBasic , const JsonType& jsonType);

    public:
        /**
         * @brief 获取当前对象数据类型
         * @return Json::JsonType类型，表示当前对象数据类型
         */
        inline JsonType type() const noexcept { return type_; }

        /**
         * @brief 获取内部容器只读对象，键值对。
         * @exception JsonTypeException 类型错误
         * @return Map&类型，当前对象内部字典的应用。
         * @note 必须是OBJECT类型，否则抛出异常。
         */
        const Map& getMapConst() const;

        /**
         * @brief 获取内部容器只读对象，键列表。
         * @exception JsonTypeException 类型错误
         * @return List&类型，当前对象内部列表的应用。
         * @note 必须是ARRAY类型，否则抛出异常。
         */
        const List& getListConst() const;

        /**
         * @brief 获取内部容器只读对象，值类型。
         * @exception JsonTypeException
         * @return std::string&类型，当前对象内部字符串值的应用。
         * @note 必须是值类型，否则抛出异常。
         */
        const std::string& getStringConst() const;

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
         * @brief 重置，设为null值
         * @note 不会调用其他函数
         */
        inline void reset() noexcept {
            type_ = JsonType::ISNULL;
            content_ = std::string{ "null" };
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
         * @brief at元素访问
         * @details at访问，带越界检查，必须是ARRAY类型
         * @exception JsonTypeException 类型错误异常
         * @note 不会创建新元素
         */
        JsonBasic& at(const size_t& index);
        /**
         * @brief at元素访问
         * @details at访问，带越界检查，必须是OBJECT类型
         * @exception JsonTypeException 类型错误异常
         * @note 不会创建新元素
         */
        JsonBasic& at(const std::string& key);
        /**
         * @brief []元素访问
         * @details []访问，带越界检查，必须是ARRAY类型
         * @exception JsonTypeException 类型错误异常
         * @note 如果指向末尾，会创建新元素，
         */
        JsonBasic& operator[](const size_t& index);
        /**
         * @brief []元素访问
         * @details []访问，带越界检查，必须是OBJECT类型
         * @exception JsonTypeException 类型错误异常
         * @note 如果key不存在，会创建新元素，所以可以直接赋值。
         */
        JsonBasic& operator[](const std::string& key);


        /**
         * @brief 字符串比较运算符
         * @details 先反序列化，再序列号比较字符串，Object内部顺序无影响，ARRAY内部顺序有影响
         * @exception JsonStructure json结构错误异常
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
         * @brief 与子类的比较运算
         * @note 序列化后再比较，耗时较长，不应频繁使用
         */
        bool operator==(const JsonObject& jsonObject) const noexcept;
        /**
         * @brief 与子类的比较运算
         * @note 序列化后再比较，耗时较长，不应频繁使用
         */
        bool operator==(const JsonArray& jsonArray) const noexcept;
        /**
         * @brief 与子类的比较运算
         * @note 序列化后再比较，耗时较长，不应频繁使用
         */
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
         * @brief 转换成long long类型
         * @exception JsonTypeException 转换失败，类型错误
         * @note 必须是NUMBER
         */
        long long as_int64() const;
        /**
         * @brief 转换成double类型
         * @exception JsonTypeException 转换失败，类型错误
         * @note 必须是NUMBER
         */
        double as_double() const;
        /**
         * @brief 转换成bool类型
         * @exception JsonTypeException 转换失败，类型错误
         * @note 必须是BOOL
         */
        bool as_bool() const;
        /**
         * @brief 转换成string类型
         * @exception JsonTypeException 转换失败，类型错误
         * @note 必须是STRING
         */
        std::string as_string() const;

        /**
         * @brief 转换成字符串
         * @details STRING类型则转义输出，非STRING则序列化
         * @note 非字符串直接调用序列化，所以必然成功，无异常
         */
        std::string to_string() const noexcept;

        /**
         * @brief 拷贝一份Object对象
         * @exception JsonTypeException 转换失败，类型错误
         * @note 类型必须是OBJECT
         */
        JsonObject as_object() const;

        /**
         * @brief 拷贝一份Array对象
         * @exception JsonTypeException 转换失败，类型错误
         * @note 类型必须是ARRAY
         */
        JsonArray as_array() const ;

        /**
         * @brief 拷贝一份值对象
         * @exception JsonTypeException 转换失败，类型错误
         * @note 必须是值类型
         */
        JsonValue as_value() const;
    };


    /**
     * @class JsonObject
     * @brief Json对象类型对象。
     * @details JSON反序列化后的，可操作对象，内部必然是键值对。
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
        inline JsonObject(std::initializer_list<std::pair<const std::string, JsonBasic>> map){
            type_ = JsonType::OBJECT;
            content_ = Map (map); 
        }

        /**
         * @brief 默认构造函数
         */
        inline JsonObject(): JsonBasic( JsonType::OBJECT ){ }

        /**
         * @brief 拷贝构造
         */
        JsonObject(const JsonObject& jsonObject) noexcept;

        /**
         * @brief 移动构造
         */
        JsonObject(JsonObject&& jsonObject) noexcept;

        /**
         * @brief 拷贝赋值
         */
        JsonObject& operator=(const JsonObject& jsonObject) noexcept;

        /**
         * @brief 移动赋值
         */
        JsonObject& operator=(JsonObject&& jsonObject) noexcept;


        /**
         * @brief 显式父类转子类拷贝构造函数
         * @note 保证事务行，转换失败时不影响父类对象
         */
        inline explicit JsonObject(JsonBasic&& jsonBasic): JsonBasic(std::move(jsonBasic), JsonType::OBJECT){}
        /**
         * @brief 显式父类转子类移动构造函数
         */
        inline explicit JsonObject(const JsonBasic& jsonBasic): JsonBasic(jsonBasic, JsonType::OBJECT){}
        
        /**
         * @brief 字符串赋值
         */
        JsonObject& operator=(const std::string& str){
            *this = JsonBasic{ str };
            return *this;
        }

        /**
         * @brief 父类转子类拷贝赋值运算符
         */
        JsonObject& operator=(const JsonBasic& jsonBasic);
        /**
         * @brief 父类转子类移动赋值运算符
         * @note 保证事务行，转换失败时不影响父类对象
         */
        JsonObject& operator=(JsonBasic&& jsonBasic);


        /**
         * @brief 开始迭代器
         */
        inline auto begin() { return std::get<Map>(content_).begin(); }
        /**
         * @brief 末尾迭代器
         */
        inline auto end() { return std::get<Map>(content_).end(); }
        /**
         * @brief 只读开始迭代器
         */
        inline auto begin() const { return std::get<Map>(content_).begin(); }
        /**
         * @brief 只读末尾迭代器
         */
        inline auto end() const { return std::get<Map>(content_).end(); }

        /**
         * @brief 字符串比较运算符
         * @details 先反序列化后比较，耗时较长
         * @note 不抛出异常，错误格式直接返回false
         */
        bool operator==(const std::string& str) const noexcept;
        /**
         * @brief 父类比较运算符
         */
        bool operator==(const JsonBasic& jsonBasic) const noexcept;
        /**
         * @brief 比较运算符
         */
        bool operator==(const JsonObject& jsonObject) const noexcept;
        /**
         * @brief 其他子类比较运算符
         */
        inline bool operator==(const JsonArray&) const noexcept { return false; }
        /**
         * @brief 其他子类比较运算符
         */
        inline bool operator==(const JsonValue&) const noexcept { return false; }

        /**
         * @brief 加法合并运算
         */
        JsonObject operator+(const JsonObject& jsonObject) const noexcept;
        /**
         * @brief 加法合并运算
         */
        JsonObject& operator+=(const JsonObject& jsonObject) noexcept;

        /**
         * @brief 内部元素访问
         * @note 不再进行类型检测，不存在异常。
         */
        JsonBasic& operator[](const std::string& key) noexcept;
    };


    /**
     * @class JsonArray
     * @brief Json列表类型对象。
     * @details JSON反序列化后的，可操作对象，内部必然是数组。
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
        inline JsonArray(std::initializer_list<JsonBasic> list){
            type_ = JsonType::ARRAY;
            content_ = List (list); 
        }

        /**
         * @brief 默认构造函数
         */
        inline JsonArray(): JsonBasic( JsonType::ARRAY ){ }
/**
         * @brief 拷贝构造函数
         */
        JsonArray(const JsonArray& jsonArray) noexcept;
        /**
         * @brief 移动构造函数
         */
        JsonArray(JsonArray&& jsonArray) noexcept;
        /**
         * @brief 拷贝赋值函数
         */
        JsonArray& operator=(const JsonArray& jsonArray) noexcept;
        /**
         * @brief 移动赋值函数
         */
        JsonArray& operator=(JsonArray&& jsonArray) noexcept;

        /**
         * @brief 显式父类转子类拷贝构造函数
         */
        inline explicit JsonArray(JsonBasic&& jsonBasic): JsonBasic(std::move(jsonBasic), JsonType::ARRAY){}
        /**
         * @brief 显式父类转子类移动构造函数
         */
        inline explicit JsonArray(const JsonBasic& jsonBasic): JsonBasic(jsonBasic, JsonType::ARRAY){}

        /**
         * @brief 字符串赋值
         */
        JsonArray& operator=(const std::string& str){
            *this = JsonBasic{ str };
            return *this;
        }

        /**
         * @brief 父类转子类拷贝赋值
         */
        JsonArray& operator=(const JsonBasic& jsonBasic);
        /**
         * @brief 父类转子类移动赋值
         */
        JsonArray& operator=(JsonBasic&& jsonBasic);

        /**
         * @brief 开始迭代器
         */
        inline auto begin() { return std::get<List>(content_).begin(); }
        /**
         * @brief 结束迭代器
         */
        inline auto end() { return std::get<List>(content_).end(); }
        /**
         * @brief 只读开始迭代器
         */        
        inline auto begin() const { return std::get<List>(content_).begin(); }
        /**
         * @brief 只读结束迭代器
         */        
        inline auto end() const { return std::get<List>(content_).end(); }

        /**
         * @brief 字符串比较运算符
         * @details 先反序列化后比较，耗时较长
         * @note 不抛出异常，错误格式直接返回false
         */
        bool operator==(const std::string& str) const noexcept;
        /**
         * @brief 父类比较运算符
         */
        bool operator==(const JsonBasic& jsonBasic) const noexcept;
        /**
         * @brief 比较运算符
         */
        inline bool operator==(const JsonObject&) const noexcept { return false; }
        /**
         * @brief 其他子类比较运算符
         */
        bool operator==(const JsonArray& jsonArray) const noexcept;
        /**
         * @brief 其他子类比较运算符
         */
        inline bool operator==(const JsonValue&) const noexcept { return false; }

        /**
         * @brief 合并运算
         */
        JsonArray operator+(const JsonArray& jsonArray) const noexcept;
        /**
         * @brief 合并运算
         */
        JsonArray& operator+=(const JsonArray& jsonArray) noexcept;

        /**
         * @brief []访问，带区间检测
         * @exception std::out_of_range 越界
         */
        JsonBasic& operator[](const size_t& index);
    };

    /**
     * @class JsonValue
     * @brief Json值类型对象。
     * @details JSON反序列化后的，可操作对象，内部必然是值类型。
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
        inline JsonValue(): JsonBasic( JsonType::ISNULL ){ }
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
         * @brief 显式父类转子类拷贝构造函数
         */
        inline explicit JsonValue(JsonBasic&& jsonBasic): JsonBasic(std::move(jsonBasic), JsonType::OBJECT, JsonType::ARRAY){}
        /**
         * @brief 显式父类转子类移动构造函数
         */
        inline explicit JsonValue(const JsonBasic& jsonBasic): JsonBasic(jsonBasic, JsonType::OBJECT, JsonType::ARRAY){}

        /**
         * @brief 父类转子类拷贝赋值函数
         */
        JsonValue& operator=(const JsonBasic& jsonBasic);

        /**
         * @brief 父类转子类移动赋值函数
         */
        JsonValue& operator=(JsonBasic&& jsonBasic);

        /**
         * @brief 文本字面量构造
         * @note 与JsonBasic的文本初始化不同，此处将直接视作STRING类型，不再解析内容。
         */
        inline JsonValue(const char* str) noexcept: JsonBasic( JsonType::STRING ){ content_ = std::string { str }; }
        
        /**
         * @brief 文本字面量赋值
         * @note 与JsonBasic的文本初始化不同，此处将直接视作STRING类型，不再解析内容。
         */
        inline JsonValue& operator=(const char* str) noexcept{
            type_ = JsonType::STRING;
            content_ = std::string { str };
        }

        /**
         * @brief 字符串构造
         * @note 与JsonBasic的文本初始化不同，此处将直接视作STRING类型，不再解析内容。
         */
        inline JsonValue(const std::string& str) noexcept: JsonBasic( JsonType::STRING ){ content_ = str; }
        
        /**
         * @brief 字符串赋值
         * @note 与JsonBasic的文本初始化不同，此处将直接视作STRING类型，不再解析内容。
         */
        inline JsonValue& operator=(const std::string& str) noexcept{
            type_ = JsonType::STRING;
            content_ = str;
        }

        /**
         * @brief 字符串移动构造
         * @note 与JsonBasic的文本初始化不同，此处将直接视作STRING类型，不再解析内容。
         */
        inline JsonValue(std::string&& str) noexcept: JsonBasic( JsonType::STRING ){ content_ = std::move(str); }
        
        /**
         * @brief 字符串移动赋值
         * @note 与JsonBasic的文本初始化不同，此处将直接视作STRING类型，不再解析内容。
         */
        inline JsonValue& operator=(std::string&& str) noexcept{
            type_ = JsonType::STRING;
            content_ = std::move(str);
        }

        /**
         * @brief 数值构造
         */
        inline JsonValue(const int& num) noexcept:JsonBasic( num ){}
        /**
         * @brief 数值赋值
         */
        inline JsonValue& operator=(const int& num) noexcept{
            type_ = JsonType::NUMBER;
            content_ = std::to_string(num);
        }
        /**
         * @brief 数值构造
         */
        inline JsonValue(const long long& num) noexcept:JsonBasic( num ){}
        /**
         * @brief 数值赋值
         */
        inline JsonValue& operator=(const long long& num) noexcept{
            type_ = JsonType::NUMBER;
            content_ = std::to_string(num);
        }
        /**
         * @brief 数值构造
         */
        inline JsonValue(const double& num) noexcept:JsonBasic( num ){}
        /**
         * @brief 数值赋值
         */
        inline JsonValue& operator=(const double& num) noexcept{
            type_ = JsonType::NUMBER;
            content_ = std::to_string(num);
        }
        /**
         * @brief 数值构造
         */
        inline JsonValue(const long double& num) noexcept:JsonBasic( num ){}
        /**
         * @brief 数值赋值
         */
        inline JsonValue& operator=(const long double& num) noexcept{
            type_ = JsonType::NUMBER;
            content_ = std::to_string(num);
        }
        /**
         * @brief 布尔构造
         */
        inline JsonValue(const bool& bl) noexcept : JsonBasic( bl ){}
        /**
         * @brief 布尔赋值
         */
        inline JsonValue& operator=(const bool& bl) noexcept{
            type_ = JsonType::BOOLEN;
            content_ = bl ? std::string{ "true" } : std::string{ "false" };
        }

        /**
         * @brief null构造
         * @note 不能用NULL宏，使用nullptr空指针
         */
        inline JsonValue(const std::nullptr_t& ptr) noexcept : JsonBasic( ptr ){}

        /**
         * @brief null赋值
         * @note 不能用NULL宏，使用nullptr空指针
         */
        inline JsonValue& operator=(const std::nullptr_t& ptr) noexcept {
            type_ = JsonType::ISNULL;
            content_ = std::string{ "null" };
        }


        /**
         * @brief 字符串比较运算符
         * @note 与其他类型不同，此处直接比较内容
         */
        bool operator==(const std::string& str) const noexcept;
        /**
         * @brief 父类比较运算符
         */
        bool operator==(const JsonBasic& jsonBasic) const noexcept;
        /**
         * @brief 其他比较运算符
         */
        inline bool operator==(const JsonObject&) const noexcept { return false; }
        /**
         * @brief 其他比较运算符
         */
        inline bool operator==(const JsonArray&) const noexcept { return false; }
        /**
         * @brief 比较运算符
         */
        bool operator==(const JsonValue& jsonValue) const noexcept;

    };

}
