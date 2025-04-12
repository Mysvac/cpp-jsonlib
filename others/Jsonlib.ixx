export module Jsonlib;

/**
* 警告： 此模块文件未经测试
* Warning： This module file has not been tested
*/

import std;

#ifndef JSONLIB_EXPORT
    #define JSONLIB_EXPORT
#endif

export namespace Jsonlib {
    /**
     * @enum JsonType
     * @brief 表示Json对象内部数据类型
     */
    enum class JSONLIB_EXPORT JsonType{
        OBJECT, /**< JSON 对象类型 */
        ARRAY,/**< JSON 数组类型 */
        STRING, /**< JSON 字符串类型，值类型 */
        NUMBER, /**< JSON 数值类型，值类型 */
        BOOL, /**< JSON 布尔类型，值类型 */
        ISNULL, /**< JSON null类型，值类型 */
    };


    /**
     * @class JsonException
     * @brief Json专用异常基类类
     * @details 继承自std::runtime_error。
     * @note 本异常不会直接抛出。
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
        explicit JsonException(const std::string& message) : std::runtime_error(message) {}
    };

    /**
     * @class JsonTypeException
     * @brief Json类型错误异常类
     * @details as类型转换，或at,[],insert等操作发现类型不正确时抛出
     */
    class JSONLIB_EXPORT JsonTypeException final : public JsonException {
    public:
        /**
         * @brief 默认构造函数。
         */
        JsonTypeException() : JsonException("Unknown JSON Type Exception.\n") {}

        /**
         * @brief 带异常描述文本的构造函数。
         */
        explicit JsonTypeException(const std::string& message) : JsonException(message) {}
    };

    /**
     * @class JsonStructureException
     * @brief Json结构错误异常类
     * @details 结构错误，只在调用deserialize()函数时可能抛出
     */
    class JSONLIB_EXPORT JsonStructureException final : public JsonException {
    public:
        /**
         * @brief 默认构造函数。
         */
        JsonStructureException() : JsonException("Unknown Json Structure.\n") {}

        /**
         * @brief 带异常描述文本的构造函数。
         */
        explicit JsonStructureException(const std::string& message) : JsonException(message) {}
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
     * @brief 反序列化函数
     * @param str 需要反转义的原JSON字符串
     * @note 解析JSON数据统一使用此函数
     */
    JSONLIB_EXPORT
    JsonValue deserialize(std::string_view str);

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
         * @brief 反序列化构造函数
         * @details 从str的it位置开始解析，请勿使用此构造，请使用deserialize()函数。
         * @note 虽然是公共函数，能够正常工作，但不推荐直接使用。
         */
        JsonValue(std::string_view str, std::string_view::const_iterator& it);

        /**
         * @brief 列表初始化器
         * @param init_list 初始化列表
         * @details
         * 参数数量为0时，被认为是null类型
         * 参数数量为2且第一个参数是字符串类型时，被认为是object
         * 其他时候都会被认为是array
         * @note 注意区分{}初始化器和()初始化器，二者效果不同
         */
        JsonValue(const std::initializer_list<JsonValue>& init_list);


        /**
         * @brief 获取当前对象数据类型
         * @return Json::JsonType类型，表示当前对象数据类型
         */
        [[nodiscard]]
        JsonType type() const noexcept { return type_; }

        /**
         * @brief 重置，设为null值
         * @note 不会调用其他函数
         */
        void reset() noexcept {
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
        [[nodiscard]]
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
        explicit JsonValue(const JsonType& jsonType);

        /**
         * @brief 字符串构造函数
         * @param str 需要转义的字符串
         * @details 将输入字符串反转义，生成对象。
         * @note 不会视为JSON结构解析，而是直接当做JSON字符串
         */
        JsonValue(std::string_view str) noexcept;

        /**
         * @brief 字符串赋值函数
         * @param str JSON格式的文本数据
         * @details 直接视为文本数据，反转义后生成对象
         * @note 不会视为JSON结构解析，而是直接当做JSON字符串
         */
        JsonValue& operator=(std::string_view str);


        /**
         * @brief 字符串字面量构造函数
         * @param str JSON格式的文本数据。
         * @details 直接视为文本数据，反转义后生成对象
         * @note 不会视为JSON结构解析，而是直接当做JSON字符串
         */
        JsonValue(const char* str) noexcept;

        /**
         * @brief 字符串字面量赋值函数
         * @param str JSON格式的文本数据。
         * @details 直接视为文本数据，反转义后生成对象
         * @note 不会视为JSON结构解析，而是直接当做JSON字符串
         */
        JsonValue& operator=(const char* str) noexcept;

        /**
         * @brief 布尔类型构造
         */
        JsonValue(const bool& bl) noexcept: type_(JsonType::BOOL) {
            content_ = bl;
        }
        /**
         * @brief 整数类型构造
         */
        JsonValue(const int& num) noexcept: type_(JsonType::NUMBER) {
            content_ = std::to_string(num);
        }
        /**
         * @brief 长整数类型构造
         */
        JsonValue(const long long& num) noexcept: type_(JsonType::NUMBER) {
            content_ = std::to_string(num);
        }
        /**
         * @brief 浮点数类型构造
         */
        JsonValue(const double& num) noexcept: type_(JsonType::NUMBER) {
            content_ = std::to_string(num);
        }
        /**
         * @brief 浮点数类型构造
         */
        JsonValue(const long double& num) noexcept: type_(JsonType::NUMBER) {
            content_ = std::to_string(num);
        }
        /**
         * @brief nullptr_t类型构造
         */
        JsonValue(const std::nullptr_t&) noexcept{}

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
        JsonValue& operator=(const bool& bl) noexcept {
            type_ = JsonType::BOOL;
            content_ = bl;
            return *this;
        }
        /**
         * @brief 整数类型赋值
         */
        JsonValue& operator=(const int& num) noexcept {
            type_ = JsonType::NUMBER;
            content_ = std::to_string(num);
            return *this;
        }
        /**
         * @brief 长整数类型赋值
         */
        JsonValue& operator=(const long long& num) noexcept{
            type_ = JsonType::NUMBER;
            content_ = std::to_string(num);
            return *this;
        }
        /**
         * @brief 浮点数类型赋值
         */
        JsonValue& operator=(const double& num) noexcept {
            type_ = JsonType::NUMBER;
            content_ = std::to_string(num);
            return *this;
        }
        /**
         * @brief 浮点数类型赋值
         */
        JsonValue& operator=(const long double& num) noexcept{
            type_ = JsonType::NUMBER;
            content_ = std::to_string(num);
            return *this;
        }
        /**
         * @brief null类型赋值
         */
        JsonValue& operator=(const std::nullptr_t& ) noexcept{
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
        [[nodiscard]]
        std::string serialize() const noexcept;

        /**
         * @brief 序列化对象且美化
         * @details 对象序列化，生成JSON结构的字符串，自动生成换行和空格缩进。
         * @param space_num 每次缩进使用的空格数量 默认 2
         * @param depth 当前对象缩进次数，默认 0
         * @note 注意，序列化包含反转义，注意区分使用。
         */
        [[nodiscard]]
        std::string serialize_pretty(const size_t& space_num = 2,const size_t& depth = 0) const noexcept;

        /**
         * @brief 判断是不是对象
         */
        [[nodiscard]]
        bool is_object() const noexcept { return type_ == JsonType::OBJECT; }

        /**
         * @brief 判断是不是数组
         */
        [[nodiscard]]
        bool is_array() const noexcept { return type_ == JsonType::ARRAY; }

        /**
         * @brief 判断是不是字符串
         */
        [[nodiscard]]
        bool is_string() const noexcept { return type_ == JsonType::STRING; }

        /**
         * @brief 判断是不是布尔
         */
        [[nodiscard]]
        bool is_bool() const noexcept { return type_ == JsonType::BOOL; }

        /**
         * @brief 判断是不是数值
         */
        [[nodiscard]]
        bool is_number() const noexcept { return type_ == JsonType::NUMBER; }

        /**
         * @brief 判断是不是整数
         */
        [[nodiscard]]
        bool is_int64() const noexcept { return type_ == JsonType::NUMBER && std::get<std::string>(content_).find('.') == std::string::npos; }

        /**
         * @brief 判断是不是浮点
         */
        [[nodiscard]]
        bool is_double() const noexcept { return type_ == JsonType::NUMBER && std::get<std::string>(content_).find('.') != std::string::npos; }

        /**
         * @brief 判断是不是null
         */
        [[nodiscard]]
        bool is_null() const noexcept { return type_ == JsonType::ISNULL; }

        /**
         * @brief 判断是不是值类型
         */
        [[nodiscard]]
        bool is_value() const noexcept { return type_ != JsonType::OBJECT && type_ != JsonType::ARRAY; }

        /**
         * @brief 转换成long long类型，复制一份
         * @exception JsonTypeException 转换失败，类型错误
         * @note 必须是NUMBER
         */
        [[nodiscard]]
        long long as_int64() const;
        /**
         * @brief 转换成double类型，复制一份
         * @exception JsonTypeException 转换失败，类型错误
         * @note 必须是NUMBER
         */
        [[nodiscard]]
        double as_double() const;
        /**
         * @brief 转换成double类型，复制一份
         * @exception JsonTypeException 转换失败，类型错误
         * @note 必须是NUMBER
         */
        [[nodiscard]]
        long double as_ldouble() const;
        /**
         * @brief 转换成bool类型，复制一份
         * @exception JsonTypeException 转换失败，类型错误
         * @note 必须是BOOL
         */
        [[nodiscard]]
        bool as_bool() const;

        /**
         * @brief 转换成string类型，注意是拷贝一份
         * @exception JsonTypeException 转换失败，类型错误（只有ARRAY和OBJECT会转换失败）。
         * @note 只要不是ARRAY和OBJECT，都能正常返回。
         */
        [[nodiscard]]
        std::string as_string() const;

        /**
         * @brief 获取内部Object对象的引用
         * @details 获取内部Object对象的引用，可调用std::map相关函数，非const，可修改内容
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
        [[nodiscard]]
        const JsonObject& as_object() const;

        /**
         * @brief 获取内部Array对象的引用
         * @details 获取内部Array对象的引用，可调用std::vector相关函数，非const，可修改内容
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
        [[nodiscard]]
        const JsonArray& as_array() const;

        /**
         * @brief at元素访问
         * @details at访问，带越界检查，越界抛出out_of_range，必须是ARRAY类型
         * @exception JsonTypeException 类型错误异常
         * @exception std::out_of_range 越界不存在
         * @note 不会创建新元素
         */
        JsonValue& at(const size_t& index);
        /**
         * @brief at元素访问
         * @details at访问，带越界检查，key不存在时抛出out_of_range，必须是OBJECT类型
         * @exception JsonTypeException 类型错误异常
         * @exception std::out_of_range key不存在
         * @note 不会创建新元素
         */
        JsonValue& at(const std::string& key);
        /**
         * @brief []元素访问
         * @details []访问，必须是ARRAY类型，无越界检查。
         * @exception JsonTypeException 类型错误异常
         * @note 如果指向末尾，会创建新元素，
         */
        JsonValue& operator[](const size_t& index);
        /**
         * @brief []元素访问
         * @details []访问，必须是OBJECT类型，无越界检查。
         * @exception JsonTypeException 类型错误异常
         * @note 如果key不存在，会创建新元素，所以可以直接赋值。
         */
        JsonValue& operator[](const std::string& key);

        /**
         * @brief 检查是否存在某个key
         * @details 检查是否存在某个key，不会抛出异常，非object类型直接返回false
         */
        [[nodiscard]]
        bool hasKey(const std::string& key) const noexcept;


        /**
         * @brief 末尾拷贝插入元素，O(1)
         * @exception JsonTypeException 非数组类型抛出异常
         * @note 必须是ARRAY类型
         */
        void push_back(const JsonValue& jsonBasic);

        /**
         * @brief 末尾移动插入元素，O(1)
         * @exception JsonTypeException 非数组类型抛出异常
         * @note 必须是ARRAY类型
         */
        void push_back(JsonValue&& jsonBasic);

        /**
         * @brief 末尾删除元素，O(1)
         * @exception JsonTypeException 非数组类型抛出异常
         * @note 必须是ARRAY类型
         */
        void pop_back();

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
         * @brief 删除指定位置的元素
         * @details 删除指定位置的元素，当前对象必须是ARRAY类型，不能越界
         * @exception JsonTypeException 当前对象并非ARRAY类型。
         * @exception out_of_range 索引越界。
         * @note 你也可以使用 as_array().erase( x )，调用std::map的erase函数
         */
        void erase(const size_t& index);
        /**
         * @brief 删除key的元素
         * @details 删除指定key的元素，当前对象必须是OBJECT类型，key可以不存在(不会执行任何操作，不会抛出异常）。
         * @exception JsonTypeException 当前对象并非OBJECT类型。
         * @note 你也可以使用 as_object().erase( x )，调用std::map的erase函数
         */
        void erase(const std::string& key);
    };
}

modeule :private;

namespace Jsonlib{

    /**
     * @brief 内部函数，转义\u字符
     */
    static void json_escape_unicode(std::string& res, std::string_view str, std::string_view::const_iterator& it) {
        // 进入时 it 在 \uXXXX 的 u 的位置。
        if (str.end() - it <= 4) throw JsonStructureException{ "Illegal unicode.\n" };
        ++it;
        std::istringstream iss( std::string(str.substr(it-str.begin(), 4)));
        // 函数返回时，it应该在\uABCD 的 D位置，所以只能 +3
        it += 3;
        unsigned int codePoint;
        iss >> std::hex >> codePoint;

        if (iss.fail() || codePoint > 0xFFFF) {
            // 错误的\u转义字符，会直接跳过，不会报错。
            throw JsonStructureException{ "Illegal unicode.\n" };
        }

        // [0xD800 , 0xE000) 范围，是代理对，是连续2波\u转码
        if (codePoint >= 0xD800 && codePoint <= 0xDFFF) {
            // 代理队，必须是 高代理 + 低代理
            // 高代理 [\uD800, \uDBFF]
            // 低代理 [\uDC00, \uDFFF]
            if (codePoint >= 0xDC00) {
                // 低代理开头，直接结束
                throw JsonStructureException{ "Illegal unicode - start with low-code.\n" };
            }

            // 检查下一个转义序列是否是低代理
            if (str.end() - it < 7 || *(it+1) != '\\' || *(it+2) != 'u') {
                // 当前是高代理，但是下个位置不是低代理，也直接返回
                throw JsonStructureException{ "Illegal unicode - only high-code.\n" };
            }

            // 解析低代理 +3 进入 \uABCD 的 A位置
            it += 3;
            std::istringstream lowIss( std::string(str.substr(it-str.begin(), 4) ));
            it += 3; // 移动到 \uABCD的D位置

            unsigned int lowCodePoint;
            lowIss >> std::hex >> lowCodePoint;

            if (lowIss.fail() || lowCodePoint < 0xDC00 || lowCodePoint > 0xDFFF) {
                // 不是低代理对，说明错误
                throw JsonStructureException{ "Illegal unicode - not end with lowcode.\n" };
            }

            // 将代理对组合为单个码点
            codePoint = 0x10000 + ((codePoint - 0xD800) << 10) + (lowCodePoint - 0xDC00);
        }

        // 将码点编码为 UTF-8
        if (codePoint <= 0x7F) {
            res += static_cast<char>(codePoint);
        }
        else if (codePoint <= 0x7FF) {
            res += static_cast<char>(0xC0 | (codePoint >> 6));
            res += static_cast<char>(0x80 | (codePoint & 0x3F));
        }
        else if (codePoint <= 0xFFFF) {
            res += static_cast<char>(0xE0 | (codePoint >> 12));
            res += static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F));
            res += static_cast<char>(0x80 | (codePoint & 0x3F));
        }
        else if (codePoint <= 0x10FFFF) {
            res += static_cast<char>(0xF0 | (codePoint >> 18));
            res += static_cast<char>(0x80 | ((codePoint >> 12) & 0x3F));
            res += static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F));
            res += static_cast<char>(0x80 | (codePoint & 0x3F));
        }
        else throw JsonStructureException{ "Illegal unicode.\n" };;
    }

    /**
     * @brief 内部函数，转义字符串且移动指针
     */
    static std::string json_escape_next(std::string_view str, std::string_view::const_iterator& it) {
        // 跳过字符串起始的双引号
        ++it;
        std::string res;

        while (it != str.end() && *it != '\"') {
            switch (*it) {
            case '\\':
            {
                // 转义字符处理
                ++it;
                if (it == str.end()) throw JsonStructureException{ "String have not end char '\"'." };
                switch (*it)
                {
                case '\"':
                    res += '\"';
                    break;
                case '\\':
                    res += '\\';
                    break;
                case '/':
                    res += '/';
                    break;
                case 'n':
                    res += '\n';
                    break;
                case 'r':
                    res += '\r';
                    break;
                case 't':
                    res += '\t';
                    break;
                case 'f':
                    res += '\f';
                    break;
                case 'b':
                    res += '\b';
                    break;
                case 'u':
                case 'U':
                    json_escape_unicode(res, str, it);
                    break;
                default:
                    throw JsonStructureException{ "Illegal escape characters.\n " };
                }
            }
            break;
            case '\t':
            case '\n':
            case '\f':
            case '\b':
            case '\r':
                throw JsonStructureException{ "There are characters that have not been escaped.\n" };
            default:
                res += *it;
                break;
            }
            ++it;
        }
        if(it == str.end()) throw JsonStructureException {"Unclosed string.\n"};
        ++it;
        return res;
    }


    /**
     * @brief 内部函数，反转义字符串
     */
    static std::string json_reverse_escape(std::string_view str) noexcept {
        std::string res;
        // 提前分配空间，减少扩容开销
        if (str.size() > 15) res.reserve(str.size() + (str.size() >> 4));
        res += "\"";
        for (const char& it : str) {
            switch (it) {
            case '\"':
                res += "\\\"";
                break;
            case '\\':
                res += "\\\\";
            case '\n':
                res += "\\n";
                break;
            case '\f':
                res += "\\f";
                break;
            case '\t':
                res += "\\t";
                break;
            case '\r':
                res += "\\r";
                break;
            case '\b':
                res += "\\b";
                break;
            default:
                res += it;
                break;
            }
        }
        res += "\"";
        return res;
    }

    /**
     * @brief 内部函数，转义字符串
     */
    static std::string json_escape(std::string_view str) {
        auto it = str.begin();
        ++it;
        std::string res;

        while (it != str.end() && *it != '\"') {
            switch (*it) {
            case '\\':
            {
                // 转义字符处理
                ++it;
                if (it == str.end()) throw JsonStructureException{ "String have not end char '\"'." };
                switch (*it)
                {
                case '\"':
                    res += '\"';
                    break;
                case '\\':
                    res += '\\';
                    break;
                case '/':
                    res += '/';
                    break;
                case 'n':
                    res += '\n';
                    break;
                case 'r':
                    res += '\r';
                    break;
                case 't':
                    res += '\t';
                    break;
                case 'f':
                    res += '\f';
                    break;
                case 'b':
                    res += '\b';
                    break;
                case 'u':
                case 'U':
                    json_escape_unicode(res, str, it);
                    break;
                default:
                    break;
                }
            }
            break;
            default:
                res += *it;
                break;
            }
            ++it;
        }
        return res;
    }

    // 指定类型的构造函数
    JsonValue::JsonValue(const JsonType& jsonType) {
        type_ = jsonType;
        switch (jsonType)
        {
        case JsonType::OBJECT:
            content_ = JsonObject{};
            break;
        case JsonType::ARRAY:
            content_ = JsonArray{};
            break;
        case JsonType::STRING:
            content_ = std::string{};
            break;
        case JsonType::NUMBER:
            content_ = std::string{ "0" };
            break;
        default:
            break;
        }
    }

    // 反序列化
    JsonValue deserialize(std::string_view str){
        auto it = str.begin();
        while(it!=str.end() && std::isspace(*it)) ++it;

        // 禁止空内容
        if(it == str.end()){
            throw JsonStructureException{ "Empty JSON data.\n" };
        }

        JsonValue jsonValue (str, it);

        while( it != str.end() ){
            if(!std::isspace(*it)) throw JsonStructureException {"Unknown content at the end.\n"};
            ++it;
        }
        return jsonValue;
    }

    // 反序列化构造
    JsonValue::JsonValue(std::string_view str, std::string_view::const_iterator& it){
        while(it != str.end() && std::isspace(*it)) ++it;
        if(it == str.end()) throw JsonStructureException{ "Empty JSON data.\n" };
        switch (*it){
            case '{':
            {
                type_ = JsonType::OBJECT;
                content_ = JsonObject {};
                auto& jsonObject = std::get<JsonObject>(content_);
                std::string key;
                ++it;
                while(it != str.end()){
                    while (it!=str.end() && std::isspace(*it)) ++it;
                    if(it == str.end() || *it == '}') break;
                    // 寻找key
                    if (*it != '\"') throw JsonStructureException{ "Key is not string.\n" };
                    key = json_escape_next(str, it);
                    // 寻找分号
                    while (it != str.end() && std::isspace(*it)) ++it;
                    if(it == str.end()) throw JsonStructureException{ "Illegal Json Object content.\n" };
                    if (*it == ':') ++it;
                    else throw JsonStructureException {};
                    // 寻找值
                    while (it != str.end() && std::isspace(*it)) ++it;
                    jsonObject[std::move(key)] = JsonValue ( str, it );
                    // 寻找分隔符或结束位
                    while (it != str.end() && std::isspace(*it)) ++it;
                    if (it != str.end() && *it == ',') ++it;
                }
                if(it == str.end()) throw JsonStructureException{ "Unclosed Json Object.\n" };
                else ++it;
            }
            break;
            case '[':
            {
                type_ = JsonType::ARRAY;
                content_ = JsonArray{};
                auto& jsonArray = std::get<JsonArray>(content_);
                ++it;
                while(it!=str.end()){
                    while (it!=str.end() && std::isspace(*it)) ++it;
                    if(it == str.end() || *it == ']') break;
                    // 寻找值
                    jsonArray.emplace_back( str, it );
                    // 寻找分隔符或结束位
                    while (it != str.end() && std::isspace(*it)) ++it;
                    if (it != str.end() && *it == ',') ++it;
                }
                if(it==str.end()) throw JsonStructureException{ "Unclosed Json Object.\n" };
                else ++it;
            }
            break;
            case '\"':
            {
                type_ = JsonType::STRING;
                auto left = it;
                ++it;
                while(it != str.end() && *it != '\"'){
                    if(*it == '\\'){
                        ++it;
                        if(it == str.end()) break;
                    }
                    ++it;
                }
                if(it == str.end()) throw JsonStructureException {"Unclosed string.\n"};
                content_ = std::string (left, ++it);
            }
            break;
            case 't':
                if(str.end() - it < 4 || str.compare(it-str.begin(), 4, "true")) throw JsonStructureException {};
                type_ = JsonType::BOOL;
                content_ = true;
                it += 4;
            break;
            case 'f':
                if(str.end() - it < 5 || str.compare(it-str.begin(), 5, "false")) throw JsonStructureException {};
                type_ = JsonType::BOOL;
                it += 5;
            break;
            case 'n':
                if(str.end() - it < 4 || str.compare(it-str.begin(), 4, "null")) throw JsonStructureException {};
                it += 4;
            break;
            default:
            {
                type_ = JsonType::NUMBER;
                bool have_not_point = true;
                bool have_not_e = true;
                auto left = it;
                if(*it == '-') ++it;
                // 必须数字开头
                if(it == str.end() || !std::isdigit(*it)) throw JsonStructureException {};
                while (it != str.end()) {
                    if (std::isdigit(*it)) ++it;
                    else if (*it == '.' && have_not_point && have_not_e) {
                        have_not_point = false;
                        ++it;
                    }
                    else if ((*it == 'e' || *it == 'E') && have_not_e) {
                        have_not_e = false;
                        ++it;
                        if (it != str.end() && (*it == '-' || *it == '+')) ++it;
                    }
                    else break;
                }
                if(it == left || (*left=='-' && it==left+1)) throw JsonStructureException {};
                content_ = std::string (left, it);
            }
            break;
        }
    }

    // 字符串类型构造
    JsonValue::JsonValue(std::string_view str) noexcept{
        type_ = JsonType::STRING;
        content_ = json_reverse_escape(str);
    }

    // 字符串类型赋值
    JsonValue& JsonValue::operator=(std::string_view str) {
        type_ = JsonType::STRING;
        content_ = json_reverse_escape(str);
        return *this;
    }

    // 字符串字面量构造
    JsonValue::JsonValue(const char* str) noexcept{
        type_ = JsonType::STRING;
        content_ = json_reverse_escape(str);
    }
    // 字符串字面量赋值
    JsonValue& JsonValue::operator=(const char* str) noexcept{
        type_ = JsonType::STRING;
        content_ = json_reverse_escape(str);
        return *this;
    }

    // 列表初始化器
    JsonValue::JsonValue(const std::initializer_list<JsonValue>& init_list) {
        if (init_list.size() == 0) return;
        if (init_list.size() == 2 && init_list.begin()->is_string()){
            type_ = JsonType::OBJECT;
            content_ = JsonObject{};
            auto& map = std::get<JsonObject>(content_);
            map[init_list.begin()->as_string()] = init_list.begin()[1];
        }
        else {
            type_ = JsonType::ARRAY;
            content_ = JsonArray{};
            auto& list = std::get<JsonArray>(content_);
            list.reserve(init_list.size());
            for (const auto& it : init_list) {
                list.push_back(it);
            }
        }
    }

    // 清空内容
    void JsonValue::clear() noexcept {
        switch (type_)
        {
        case JsonType::OBJECT:
            content_ = JsonObject{};
            break;
        case JsonType::ARRAY:
            content_ = JsonArray{};
            break;
        case JsonType::STRING:
            content_ = std::string{};
            break;
        case JsonType::NUMBER:
            content_ = std::string{ "0" };
            break;
        case JsonType::BOOL:
            content_ = false;
            break;
        default:
            break;
        }
    }

    // 长度获取
    size_t JsonValue::size() const noexcept{
        switch (type_)
        {
        case JsonType::OBJECT:
            return std::get<JsonObject>(content_).size();
        case JsonType::ARRAY:
            return std::get<JsonArray>(content_).size();
        case JsonType::STRING:
            return std::get<std::string>(content_).size();
        default:
            return 1;
        }
    }


    // 拷贝构造
    JsonValue::JsonValue(const JsonValue& jsonValue) noexcept{
        type_ = jsonValue.type_;
        content_ = jsonValue.content_;
    }

    //移动构造
    JsonValue::JsonValue(JsonValue&& jsonValue) noexcept{
        type_ = jsonValue.type_;
        content_ = std::move(jsonValue.content_);
        jsonValue.reset();
    }

    // 拷贝赋值
    JsonValue& JsonValue::operator=(const JsonValue& jsonValue) noexcept{
        if (this == &jsonValue) return *this;
        type_ = jsonValue.type_;
        content_ = jsonValue.content_;
        return *this;
    }

    // 移动赋值
    JsonValue& JsonValue::operator=(JsonValue&& jsonValue) noexcept{
        if (this == &jsonValue) return *this;
        type_ = jsonValue.type_;
        content_ = std::move(jsonValue.content_);
        jsonValue.reset();
        return *this;
    }


    // JsonArray拷贝构造
    JsonValue::JsonValue(const JsonArray& jsonArray) noexcept{
        type_ = JsonType::ARRAY;
        content_ = jsonArray;
    }

    // JsonArray移动构造
    JsonValue::JsonValue(JsonArray&& jsonArray) noexcept{
        type_ = JsonType::ARRAY;
        content_ = std::move(jsonArray);
    }

    // JsonObject拷贝构造
    JsonValue::JsonValue(const JsonObject& jsonObject) noexcept{
        type_ = JsonType::OBJECT;
        content_ = jsonObject;
    }

    // JsonObject移动构造
    JsonValue::JsonValue(JsonObject&& jsonObject) noexcept{
        type_ = JsonType::OBJECT;
        content_ = std::move(jsonObject);
    }


    // JsonArray拷贝赋值
    JsonValue& JsonValue::operator=(const JsonArray& jsonArray) noexcept{
        if (type_ == JsonType::ARRAY && &jsonArray == &std::get<JsonArray>(content_))
            return *this;

        type_ = JsonType::ARRAY;
        content_ = jsonArray;
        return *this;
    }

    // JsonArray移动赋值
    JsonValue& JsonValue::operator=(JsonArray&& jsonArray) noexcept{
        if (type_ == JsonType::ARRAY && &jsonArray == &std::get<JsonArray>(content_))
            return *this;

        type_ = JsonType::ARRAY;
        content_ = std::move(jsonArray);
        return *this;
    }

    // JsonObject拷贝赋值
    JsonValue& JsonValue::operator=(const JsonObject& jsonObject) noexcept{
        if (type_ == JsonType::OBJECT && &jsonObject == &std::get<JsonObject>(content_))
            return *this;

        type_ = JsonType::OBJECT;
        content_ = jsonObject;
        return *this;
    }

    // JsonObject移动赋值
    JsonValue& JsonValue::operator=(JsonObject&& jsonObject) noexcept{
        if (type_ == JsonType::OBJECT && &jsonObject == &std::get<JsonObject>(content_))
            return *this;

        type_ = JsonType::OBJECT;
        content_ = std::move(jsonObject);
        return *this;
    }



    // 序列化JSON对象
    std::string JsonValue::serialize() const noexcept {
        switch (type_)
        {
        case JsonType::OBJECT:
        {
            // 对象类型
            // 是否在开头加上逗号
            std::string res{ "{" };
            const auto& map = std::get<JsonObject>(content_);
            for (const auto& [fst, snd] : map) {
                // 键是字符串，需要反转义
                res += json_reverse_escape(fst);
                res += ':';
                // 递归序列号
                res += snd.serialize();
                res += ',';
            }
            if (*res.rbegin() == ',') *res.rbegin() = '}';
            else res += '}';
            return res;
        }
        case JsonType::ARRAY:
        {
            // 数组类型
            // 是否在开头加上逗号
            std::string res{ "[" };
            const auto& list = std::get<JsonArray>(content_);
            for (const JsonValue& it : list) {
                // 递归序列号
                res += it.serialize();
                res += ',';
            }
            if (*res.rbegin() == ',') *res.rbegin() = ']';
            else res += ']';
            return res;
        }
        case JsonType::BOOL:
            return std::get<bool>(content_) ? "true" : "false";
        case JsonType::ISNULL:
            return "null";
        default:
            // 数值和字符串，可以直接返回内容
            return std::get<std::string>(content_);
        }
    }

    // 序列化JSON对象含空格和换行
    std::string JsonValue::serialize_pretty(const size_t& space_num, const size_t& depth) const noexcept {
        switch (type_)
        {
        case JsonType::OBJECT:
        {
            std::string tabs(depth * space_num, ' ');
            // 对象类型
            std::string res{ "{" };

            const auto& map = std::get<JsonObject>(content_);
            for (const auto& [fst, snd] : map) {
                res += '\n' + std::string(space_num, ' ') + tabs;
                // 键是字符串，需要反转义
                res += json_reverse_escape(fst);
                res += ": ";
                // 递归序列号
                res += snd.serialize_pretty(space_num, depth + 1);
                res += ',';
            }
            if (*res.rbegin() == ',') *res.rbegin() = '\n';
            if(!map.empty()) res += tabs + '}';
            else res += " }";
            return res;
        }
        case JsonType::ARRAY:
        {
            std::string tabs(depth * space_num, ' ');
            // 数组类型
            std::string res{ "[" };

            const auto& list = std::get<JsonArray>(content_);
            for (const JsonValue& it : list) {
                // 递归序列号
                res += '\n' + std::string(space_num, ' ') + tabs;
                res += it.serialize_pretty(space_num, depth + 1);
                res += ',';
            }
            if (*res.rbegin() == ',') *res.rbegin() = '\n';
            if(!list.empty()) res += tabs + ']';
            else res += " ]";
            return res;
        }
        case JsonType::BOOL:
            return std::get<bool>(content_) ? "true" : "false";
        case JsonType::ISNULL:
            return "null";
        default:
            // 数值和字符串，可以直接返回内容
            return std::get<std::string>(content_);
        }
    }

    // as 赋值内容或对象
    long long JsonValue::as_int64() const {
        if (type_ != JsonType::NUMBER) throw JsonTypeException{ "Is not Number.\n" };
        return std::stoll(std::get<std::string>(content_));
    }
    double JsonValue::as_double() const {
        if (type_ != JsonType::NUMBER) throw JsonTypeException{ "Is not Number.\n" };
        return std::stod(std::get<std::string>(content_));
    }
    long double JsonValue::as_ldouble() const{
        if (type_ != JsonType::NUMBER) throw JsonTypeException{ "Is not Number.\n" };
        return std::stold(std::get<std::string>(content_));
    }
    bool JsonValue::as_bool() const {
        if (type_ != JsonType::BOOL) throw JsonTypeException{ "Is not bool.\n" };
        return std::get<bool>(content_);
    }
    std::string JsonValue::as_string() const {
        if (type_ == JsonType::ARRAY || type_ == JsonType::OBJECT) throw JsonTypeException{ "Is not String.\n" };
        switch (type_)
        {
        case JsonType::STRING:
            return json_escape(std::get<std::string>(content_));
        case JsonType::BOOL:
            return std::get<bool>(content_) ? "true" : "false";
        case JsonType::ISNULL:
            return "null";
        default:
            // 数值，可以直接返回内容
            return std::get<std::string>(content_);
        }

    }
    JsonObject& JsonValue::as_object(){
        if (type_ != JsonType::OBJECT) throw JsonTypeException{ "Is not object.\n" };
        return std::get<JsonObject>(content_);
    }
    const JsonObject& JsonValue::as_object() const{
        if (type_ != JsonType::OBJECT) throw JsonTypeException{ "Is not object.\n" };
        return std::get<JsonObject>(content_);
    }
    JsonArray& JsonValue::as_array(){
        if (type_ != JsonType::ARRAY) throw JsonTypeException{ "Is not object.\n" };
        return std::get<JsonArray>(content_);
    }
    const JsonArray& JsonValue::as_array() const{
        if (type_ != JsonType::ARRAY) throw JsonTypeException{ "Is not object.\n" };
        return std::get<JsonArray>(content_);
    }
    // 列表访问，拒绝新元素
    JsonValue& JsonValue::at(const size_t& index) {
        if (type_ != JsonType::ARRAY) throw JsonTypeException{ "Is not array.\n" };
        auto& list = std::get<JsonArray>(content_);
        if (index < 0 || index >= list.size()) throw std::out_of_range{ "out of range.\n" };
        return list.at(index);
    }
    // 对象访问，拒绝新元素
    JsonValue& JsonValue::at(const std::string& key) {
        if (type_ != JsonType::OBJECT) throw JsonTypeException{ "Is not Object.\n" };
        auto& map = std::get<JsonObject>(content_);
        if (map.find(key) == map.end()) throw std::out_of_range{ + "Key not find.\n" };
        return map.at(key);
    }
    // 列表访问，可能创建新元素
    JsonValue& JsonValue::operator[](const size_t& index) {
        if (type_ != JsonType::ARRAY) throw JsonTypeException{ "Is not array.\n" };
        auto& list = std::get<JsonArray>(content_);
        if (index == list.size()) list.emplace_back();
        return list[index];
    }
    // 对象访问，可能创建新元素
    JsonValue& JsonValue::operator[](const std::string& key) {
        if (type_ != JsonType::OBJECT) throw JsonTypeException{ "Is not Object.\n" };
        auto& map = std::get<JsonObject>(content_);
        return map[key];
    }

    // 检查是否包含某个key
    bool JsonValue::hasKey(const std::string& key) const noexcept{
        if (type_ != JsonType::OBJECT) return false;
        const auto& map = std::get<JsonObject>(content_);
        return map.find(key) != map.end();
    }
    // 数组末尾插入元素
    void JsonValue::push_back(const JsonValue& jsonValue) {
        if (type_ != JsonType::ARRAY) throw JsonTypeException{ "Is not Array.\n" };
        auto& list = std::get<JsonArray>(content_);
        list.push_back(jsonValue);
    }
    // 数组末尾移动进入元素
    void JsonValue::push_back(JsonValue&& jsonValue) {
        if (type_ != JsonType::ARRAY) throw JsonTypeException{ "Is not Array.\n" };
        auto& list = std::get<JsonArray>(content_);
        list.push_back(std::move(jsonValue));
    }
    // 数组尾部删除元素
    void JsonValue::pop_back() {
        if (type_ != JsonType::ARRAY) throw JsonTypeException{ "Is not Array.\n" };
        auto& list = std::get<JsonArray>(content_);
        list.pop_back();
    }

    // 数值指定位置插入元素
    void JsonValue::insert(const size_t& index, const JsonValue& jsonValue) {
        if (type_ != JsonType::ARRAY) throw JsonTypeException{ "Is not Array.\n" };
        auto& list = std::get<JsonArray>(content_);
        if (index < 0 || index > list.size()) throw std::out_of_range{ "out of range.\n" };
        list.insert(std::next(list.begin(), static_cast<std::ptrdiff_t>(index)), jsonValue);
    }
    // 数组指定位置移入元素
    void JsonValue::insert(const size_t& index, JsonValue&& jsonValue) {
        if (type_ != JsonType::ARRAY) throw JsonTypeException{ "Is not Array.\n" };
        auto& list = std::get<JsonArray>(content_);
        if (index < 0 || index > list.size()) throw std::out_of_range{ "out of range.\n" };
        list.insert(std::next(list.begin(), static_cast<std::ptrdiff_t>(index)), std::move(jsonValue));
    }
    // 对象指定位置插入键值对
    void JsonValue::insert(const std::string& key, const JsonValue& jsonValue) {
        if (type_ != JsonType::OBJECT) throw JsonTypeException{ "Is not Object.\n" };
        auto& map = std::get<JsonObject>(content_);
        map[key] = jsonValue;
    }
    // 对象指定位置移动插入键值对
    void JsonValue::insert(const std::string& key, JsonValue&& jsonValue) {
        if (type_ != JsonType::OBJECT) throw JsonTypeException{ "Is not Object.\n" };
        auto& map = std::get<JsonObject>(content_);
        map[key] = std::move(jsonValue);
    }
    // 数值删除指定位置的元素
    void JsonValue::erase(const size_t& index) {
        if (type_ != JsonType::ARRAY) throw JsonTypeException{ "Is not array.\n" };
        auto& list = std::get<JsonArray>(content_);
        if (index < 0 || index >= list.size()) throw std::out_of_range{ std::string{__FILE__} + ":" + std::to_string(__LINE__) + " out of range.\n" };
        list.erase(std::next(list.begin(),static_cast<std::ptrdiff_t>(index)));
    }
    // 对象删除指定key的元素
    void JsonValue::erase(const std::string& key) {
        if (type_ != JsonType::OBJECT) throw JsonTypeException{ "Is not Object.\n" };
        auto& map = std::get<JsonObject>(content_);
        map.erase(key);
    }

}
