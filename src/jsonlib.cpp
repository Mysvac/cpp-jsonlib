#include "jsonlib.h"

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <variant>
#include <exception>
#include <stdexcept>
#include <iostream>

// #define JSONLIB_CHECK_END_COMMA 1
// #define JSONLIB_CHECK_START_ZERO 1

namespace Json {

    /**
     * @brief 内部函数，转义\u字符
     */
    static void escapeUnicode(std::string& res, const std::string& str, size_t& index) {
        // 将十六进制字符串转换为 Unicode 码点
        if (index + 4 >= str.size()) throw JsonStructureException{ "Illegel string.\n" };
        ++index;
        std::istringstream iss(str.substr(index, 4));
        index += 3;
        unsigned int codePoint;
        iss >> std::hex >> codePoint;

        if (iss.fail() || codePoint > 0x10FFFF) {
            std::cerr << "Illegel Unicode string.\n" << std::endl;
            return;
        }

        if (codePoint >= 0xD800 && codePoint <= 0xDFFF) {
            if (codePoint >= 0xDC00) {
                std::cerr << "Illegel Unicode string.\n" << std::endl;
                return;
            }

            // 检查下一个转义序列是否是低代理
            if (index + 6 >= str.length() || str[index + 1] != '\\' || str[index + 2] != 'u') {
                std::cerr << "Illegel Unicode string.\n" << std::endl;
                return;
            }

            // 解析低代理
            index += 3;
            std::string lowHexStr = str.substr(index, 4);
            index += 3; // 将索引移动到低代理转义序列之后

            unsigned int lowCodePoint;
            std::istringstream lowIss(lowHexStr);
            lowIss >> std::hex >> lowCodePoint;

            if (lowIss.fail() || lowCodePoint < 0xDC00 || lowCodePoint > 0xDFFF) {
                std::cerr << "Illegel Unicode string.\n" << std::endl;
                return;
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
        else {
            // return;
            std::cerr << "Illegel Unicode string.\n" << std::endl;
            return;
        }
    }

    /**
     * @brief 内部函数，判断字符串是不是数值
     * @note 静止输入空字符串
     */
    static bool is_number(const std::string& str) noexcept {
        size_t it = 0;
        size_t len = str.size();
        if (str[0] == '-') {
            if (len == 1 && !std::isdigit(str[1])) return false;
            ++it;
        }
        bool have_not_point = true;
        bool have_not_e = true;
        while (it < len) {
            if (std::isdigit(str[it])) ++it;
            else if (str[it] == '.' && have_not_point && have_not_e) {
                have_not_point = false;
                ++it;
            }
            else if ((str[it] == 'e' || str[it] == 'E') && have_not_e) {
                have_not_e = false;
                ++it;
                if (it >= len) return false;
                if ((str[it] == '-' || str[it] == '+') && it < len - 1) ++it;
            }
            else return false;
        }

#ifdef JSONLIB_CHECK_START_ZERO
        if (str[0] == '0' && have_not_point) throw JsonStructureException{"Intager could not start with 0.\n"};
#endif

        return true;
    }

    /**
     * @brief 内部函数，反转义字符串
     */
    static std::string reverse_escape(const std::string& str) noexcept {
        std::string res;
        // 提前分配空间，减少扩容开销
        if (str.size() > 13) res.reserve(str.size() + 3 + (str.size() >> 4));
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
        return std::move(res);
    }

    /**
     * @brief 内部函数，转义字符串
     */
    static std::string escape(const std::string& str, const size_t& left, const size_t& right) {
        std::string res;
        // 预分配空间
        if (right - left > 14) res.reserve(right - left + 1);

        size_t index = left;
        while (index <= right) {
            if (str[index] == '\\') {
                // 转义字符处理
                ++index;
                if (index > right) throw JsonStructureException{ "String end with escape character :" + str.substr(left, right - left + 1) };
                switch (str[index])
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
                    escapeUnicode(res, str, index);
                    // res += "\\u";
                    break;
                default:
                    throw JsonStructureException{ "Illegal escape characters " };
                    break;
                }
            }
            else res += str[index];
            ++index;
        }
        return std::move(res);
    }

    /**
     * @brief 内部函数，转义字符串且移动指针
     */
    static std::string escape_next(const std::string& str, size_t& left, const size_t& right) {
        // 跳过字符串起始的双引号
        ++left;
        std::string res;

        while (left < right && str[left] != '\"') {
            switch (str[left]) {
            case '\\':
            {
                // 转义字符处理
                ++left;
                if (left >= right) throw JsonStructureException{ "String have not end char '\"'." };
                switch (str[left])
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
                    escapeUnicode(res, str, left);
                    break;
                default:
                    throw JsonStructureException{ "Illegal escape characters.\n " };
                    break;
                }
            }
            break;
            case '\t':
            case '\n':
            case '\f':
            case '\b':
            case '\r':
                throw JsonStructureException{ "Illegal characters in key.\n" };
                break;
            default:
                res += str[left];
                break;
            }
            ++left;
        }
        // left最终停在结束的双引号
        return std::move(res);
    }

    // 指定类型的构造函数
    JsonBasic::JsonBasic(const JsonType& jsonType) {
        type_ = jsonType;
        switch (jsonType)
        {
        case JsonType::OBJECT:
            content_ = Map{};
            break;
        case JsonType::ARRAY:
            content_ = List{};
            break;
        case JsonType::STRING:
            content_ = std::string{};
            break;
        case JsonType::NUMBER:
            content_ = std::string{ "0" };
            break;
        case JsonType::BOOLEN:
            content_ = std::string{ "false" };
            break;
        }
    }

    // 辅助递归构造函数
    JsonBasic::JsonBasic(const std::string& str, size_t& index, const size_t& tail) {
        switch (str[index]) {
        case '{':
        {
            type_ = JsonType::OBJECT;
            content_ = Map{};
            Map& map = std::get<Map>(content_);
            std::string key;

#ifdef JSONLIB_CHECK_END_COMMA
            unsigned int num_comma = 0;
#endif
            ++index;
            while (str[index] != '}') {
                while (std::isspace(str[index])) ++index;
                // 空集合，结束
                if (str[index] == '}') break;
                if (str[index] != '\"') throw JsonStructureException{ "Key is not string.\n" };
                // 获取键
                key = Json::escape_next(str, index, tail);
                ++index;
                // 分隔符 子元素 不可能以 '\"' 结尾，无需检测越界
                while (std::isspace(str[index])) ++index;
                if (str[index] == ':') ++index;
                else throw JsonStructureException{ "Unknow element.\n" };

                // 获取值 子元素 不可能以':'结尾，此处不检测
                while (std::isspace(str[index])) ++index;
                map[std::move(key)] = JsonBasic{ str, index, tail };

                // index默认停在子元素的最后一个字符，所以需要先自增
                ++index;
                while (index < tail && std::isspace(str[index])) ++index;
                // 判断合法性，结束后下个字符要么逗号，要么终止
                if (str[index] == ',') 
#ifdef JSONLIB_CHECK_END_COMMA
                    ++index, ++num_comma;
#else
                    ++index;
#endif
                else if (str[index] != '}') throw JsonStructureException{ "Unknow element.\n" };
            }

            
#ifdef JSONLIB_CHECK_END_COMMA
            if (num_comma == map.size() && num_comma) throw JsonStructureException{ "Redundant comma.\n" };
#endif

            // 大于等于，说明内部括号未闭合
            if (index >= tail) throw JsonStructureException{ "The { } are not closed.\n" };
        }
        break;
        case '[':
        {
            type_ = JsonType::ARRAY;
            content_ = List{};
            List& list = std::get<List>(content_);

#ifdef JSONLIB_CHECK_END_COMMA
            unsigned int num_comma = 0;
#endif
        
            ++index;
            while (str[index] != ']') {
                while (std::isspace(str[index])) ++index;
                // 空列表，结束
                if (str[index] == ']') break;
                // 添加子类并移动index到子类末尾
                list.push_back(JsonBasic{ str, index, tail });
                // index默认停在子元素的最后一个字符，所以需要先自增
                ++index;
                while (index < tail && std::isspace(str[index])) ++index;
                // 判断合法性，结束后下个字符要么逗号，要么终止
                if (str[index] == ',') 
#ifdef JSONLIB_CHECK_END_COMMA
                    ++index, ++num_comma;
#else
                    ++index;
#endif
                else if (str[index] != ']') throw JsonStructureException{ "Unknow element.\n" };
            }
#ifdef JSONLIB_CHECK_END_COMMA
            if (num_comma == list.size() && num_comma) throw JsonStructureException{ "Redundant comma.\n" };
#endif
            // 大于等于，说明内部括号未闭合
            if (index >= tail) throw JsonStructureException{ "The [ ] are not closed.\n" };
        }
        break;
        case '\"':
            type_ = JsonType::STRING;
            content_ = Json::escape_next(str, index, tail);
            break;
        case 't':
            // str.compare return 0 if equal
            if (tail - index <= 3 || str.compare(index, 4, "true")) throw JsonStructureException{ "Unknow element.\n" };
            type_ = JsonType::BOOLEN;
            content_ = std::string{ "true" };
            index += 3;
            break;
        case 'f':
            if (tail - index <= 4 || str.compare(index, 5, "false")) throw JsonStructureException{ "Unknow element.\n" };
            type_ = JsonType::BOOLEN;
            content_ = std::string{ "false" };
            index += 4;
            break;
        case 'n':
            if (tail - index <= 3 || str.compare(index, 4, "null")) throw JsonStructureException{ "Unknow element.\n" };
            // type_ = JsonType::ISNULL;
            index += 3;
            break;
        default:
            if (!std::isdigit(str[index]) && str[index] != '-') throw JsonStructureException{ "Unknow element.\n" };
            if (!std::isdigit(str[index + 1]) && str[index] == '-') throw JsonStructureException{ "Unknow element.\n" };
            {
                type_ = JsonType::NUMBER;
                bool have_not_point = true;
                std::string tmp;
                tmp += str[index];

#ifdef JSONLIB_CHECK_START_ZERO
                const char head_char= str[index];
#endif
                while (index < tail)
                {
                    ++index;
                    while (std::isdigit(str[index])) {
                        tmp += str[index];
                        ++index;
                    }
                    if (str[index] == '.') {
                        tmp += str[index];
                        ++index;
                        while (std::isdigit(str[index])) {
                            tmp += str[index];
                            ++index;
                        }
                    }
                    if (str[index] == 'e' || str[index] == 'E') {
                        tmp += str[index];
                        ++index;
                        if (str[index] == '-' || str[index] == '+') {
                            tmp += str[index];
                            ++index;
                        }
                        while (std::isdigit(str[index])) {
                            tmp += str[index];
                            ++index;
                        }
                    }
                    --index;
                    break;
                }

#ifdef JSONLIB_CHECK_START_ZERO
                if (head_char == '0' && have_not_point) throw JsonStructureException{ "Intager could not start with 0.\n" };
#endif
                content_ = std::move(tmp);
            }
            break;
        }
    }

    // 字符串反序列化构造函数
    JsonBasic::JsonBasic(const std::string& str) {
        // 起始位置
        size_t index = 0;
        // 字符串长度
        const size_t len = str.size();
        // 末尾位置
        size_t tail = len - 1;

        while (index < len && std::isspace(str[index])) ++index;
        if (index == len) throw JsonStructureException{ "Empty JSON text.\n" };
        while (std::isspace(str[tail])) --tail;

        switch (str[index]) {
        case '{':
            if (str[tail] != '}') throw JsonStructureException{ "The { } are not closed.\n" };
            {
                type_ = JsonType::OBJECT;
                content_ = Map{};
                Map& map = std::get<Map>(content_);
                std::string key;
                
#ifdef JSONLIB_CHECK_END_COMMA
                unsigned int num_comma = 0;
#endif
                ++index;
                while (index < tail) {
                    while (std::isspace(str[index])) ++index;
                    // 空集合，结束
                    if (index == tail) break;
                    if (str[index] != '\"') throw JsonStructureException{ "Unknow element.\n" };
                    // 获取键
                    key = Json::escape_next(str, index, tail);
                    ++index;
                    // 分隔符
                    while (std::isspace(str[index])) ++index;
                    if (str[index] == ':') ++index;
                    else throw JsonStructureException{ "Unknow element.\n" };

                    // 获取值
                    while (std::isspace(str[index])) ++index;
                    map[std::move(key)] = JsonBasic{ str, index, tail };

                    // index默认停在子元素的最后一个字符，所以需要先自增
                    ++index;
                    while (std::isspace(str[index])) ++index;
                    // 判断合法性，结束后下个字符要么逗号，要么终止
                    if (str[index] == ',') 
#ifdef JSONLIB_CHECK_END_COMMA
                        ++index, ++num_comma;
#else
                        ++index;
#endif
                    else if (index != tail) throw JsonStructureException{ "Unknow element.\n" };
                }

#ifdef JSONLIB_CHECK_END_COMMA
                if (num_comma == map.size() && num_comma) throw JsonStructureException{ "Redundant comma.\n" };
#endif
                // 大于，说明大括号未闭合
                if (index > tail) throw JsonStructureException{ "The { } are not closed.\n" };
            }
            break;
        case '[':
            if (str[tail] != ']') throw JsonStructureException{ "The [ ] are not closed.\n" };
            {
                type_ = JsonType::ARRAY;
                content_ = List{};
                List& list = std::get<List>(content_);

#ifdef JSONLIB_CHECK_END_COMMA
                unsigned int num_comma = 0;
#endif

                ++index;
                while (index < tail) {
                    while (std::isspace(str[index])) ++index;
                    // 空列表，结束
                    if (index == tail) break;
                    // 添加子类并移动index到子类末尾
                    list.push_back(JsonBasic{ str, index, tail });
                    // index默认停在子元素的最后一个字符，所以需要先自增
                    ++index;
                    while (std::isspace(str[index])) ++index;
                    // 判断合法性，结束后下个字符要么逗号，要么终止
                    if (str[index] == ',')
#ifdef JSONLIB_CHECK_END_COMMA
                        ++index, ++num_comma;
#else
                        ++index;
#endif
                    else if (index != tail) throw JsonStructureException{ "Unknow element.\n" };
                }
#ifdef JSONLIB_CHECK_END_COMMA
                if (num_comma == list.size() && num_comma) throw JsonStructureException{ "Redundant comma.\n" };
#endif
                // 大于，说明方括号未闭合
                if (index > tail) throw JsonStructureException{ "The [ ] are not closed.\n" };
            }
            break;
        case '\"':
            if (str[tail] != '\"') throw JsonStructureException{ "The \" \" are not closed.\n" };
            type_ = JsonType::STRING;
            content_ = Json::escape(str, index + 1, tail - 1);
            break;
        case 't':
            // str.compare return 0 if equal
            if (tail - index < 3 || str.compare(index, 4, "true")) throw JsonStructureException{ "Unknow element.\n" };
            type_ = JsonType::BOOLEN;
            content_ = std::string{ "true" };
            break;
        case 'f':
            if (tail - index < 4 || str.compare(index, 5, "false")) throw JsonStructureException{ "Unknow element.\n" };
            type_ = JsonType::BOOLEN;
            content_ = std::string{ "false" };
            break;
        case 'n':
            if (tail - index < 3 || str.compare(index, 4, "null")) throw JsonStructureException{ "Unknow element.\n" };
            // type_ = JsonType::ISNULL;
            break;
        default:
            if (!std::isdigit(str[index]) && str[index] != '-' && str[index] != '+') throw JsonStructureException{ "Unknow element.\n" };
            if (!std::isdigit(str[index + 1]) && (str[index] == '-' || str[index] == '+')) throw JsonStructureException{ "Unknow element.\n" };
            {
                std::string tmp = str.substr(index, tail - index + 1);
                if (!Json::is_number(tmp)) throw JsonStructureException{ "Unknow element.\n" };
                type_ = JsonType::NUMBER;
                content_ = std::move(tmp);
            }
            break;
        }
    }

    // 反序列化赋值运算符
    JsonBasic& JsonBasic::operator=(const std::string& str) {
        *this = JsonBasic{ str };
        return *this;
    }

    // 拷贝构造函数
    JsonBasic::JsonBasic(const JsonBasic& jsonBasic) noexcept {
        type_ = jsonBasic.type_;
        content_ = jsonBasic.content_;
    }
    // 移动构造函数
    JsonBasic::JsonBasic(JsonBasic&& jsonBasic) noexcept {
        type_ = jsonBasic.type_;
        content_ = std::move(jsonBasic.content_);
        jsonBasic.reset();
    }
    // 拷贝赋值函数
    JsonBasic& JsonBasic::operator=(const JsonBasic& jsonBasic) noexcept {
        if (this == &jsonBasic) return *this;
        type_ = jsonBasic.type_;
        content_ = jsonBasic.content_;
        return *this;
    }
    // 移动赋值
    JsonBasic& JsonBasic::operator=(JsonBasic&& jsonBasic) noexcept {
        if (this == &jsonBasic) return *this;
        type_ = jsonBasic.type_;
        content_ = std::move(jsonBasic.content_);
        jsonBasic.reset();
        return *this;
    }

    // 强制类型构造函数
    JsonBasic::JsonBasic(const JsonBasic& jsonBasic, const JsonType& jsonType) {
        if (jsonBasic.type_ != jsonType) throw JsonTypeException{ "Illegel jsonType in constructure function." };
        type_ = jsonBasic.type_;
        content_ = jsonBasic.content_;
    }
    JsonBasic::JsonBasic(JsonBasic&& jsonBasic, const JsonType& jsonType) {
        if (jsonBasic.type_ != jsonType) throw JsonTypeException{ "Illegel jsonType in constructure function." };
        type_ = jsonBasic.type_;
        content_ = std::move(jsonBasic.content_);
        jsonBasic.reset(); // 重置状态
    }
    // 强制否定类型的构造函数
    JsonBasic::JsonBasic(const JsonBasic& jsonBasic, const JsonType& not_jsonType1, const JsonType& not_jsonType2) {
        if (jsonBasic.type_ == not_jsonType1 || jsonBasic.type_ == not_jsonType2) throw JsonTypeException{ "Illegel jsonType in constructure function." };
        type_ = jsonBasic.type_;
        content_ = jsonBasic.content_;
    }
    JsonBasic::JsonBasic(JsonBasic&& jsonBasic, const JsonType& not_jsonType1, const JsonType& not_jsonType2) {
        if (jsonBasic.type_ == not_jsonType1 || jsonBasic.type_ == not_jsonType2) throw JsonTypeException{ "Illegel jsonType in constructure function." };
        type_ = jsonBasic.type_;
        content_ = std::move(jsonBasic.content_);
        jsonBasic.reset(); // 重置状态
    }

    // 获取内部容器只读对象
    const JsonBasic::Map& JsonBasic::getMapConst() const {
        if (type_ != JsonType::OBJECT) throw JsonTypeException{ "Is not Object.\n" };
        return std::get<Map>(content_);
    }
    // 获取内部容器只读对象
    const JsonBasic::List& JsonBasic::getListConst() const {
        if (type_ != JsonType::ARRAY) throw JsonTypeException{ "Is not array.\n" };
        return std::get<List>(content_);
    }
    // 获取内部容器只读对象
    const std::string& JsonBasic::getStringConst() const {
        if (type_ == JsonType::ARRAY || type_ == JsonType::OBJECT) throw JsonTypeException{ "Is not Value Type.\n" };
        return std::get<std::string>(content_);
    }

    // 序列化JSON对象
    std::string JsonBasic::serialize() const noexcept {
        switch (type_)
        {
        case JsonType::OBJECT:
        {
            // 对象类型
            // 是否在开头加上逗号
            std::string res{ "{" };
            const Map& map = std::get<Map>(content_);
            for (const std::pair<std::string, JsonBasic>& it : map) {
                // 键是字符串，需要反转义
                res += reverse_escape(it.first);
                res += ':';
                // 递归序列号
                res += it.second.serialize();
                res += ',';
            }
            if (*res.rbegin() == ',') *res.rbegin() = '}';
            else res += '}';
            return std::move(res);
        }
        break;
        case JsonType::ARRAY:
        {
            // 数组类型
            // 是否在开头加上逗号
            std::string res{ "[" };
            const List& list = std::get<List>(content_);
            for (const JsonBasic& it : list) {
                // 递归序列号
                res += it.serialize();
                res += ',';
            }
            if (*res.rbegin() == ',') *res.rbegin() = ']';
            else res += ']';
            return std::move(res);
        }
        break;
        case JsonType::STRING:
            // 字符串类型，直接反转义，然后输出
            return reverse_escape(std::get<std::string>(content_));
            break;
        default:
            // 非对象 非列表 非字符串，可以直接返回内容
            return std::get<std::string>(content_);
            break;
        }
    }

    // 序列化JSON对象含空格和换行
    std::string JsonBasic::serialize_pretty(const size_t& space_num, const size_t& depth) const noexcept {
        switch (type_)
        {
        case JsonType::OBJECT:
        {
            std::string tabs(depth * space_num, ' ');
            // 对象类型
            std::string res{ "{" };

            const Map& map = std::get<Map>(content_);
            for (const std::pair<std::string, JsonBasic>& it : map) {
                res += '\n' + std::string(space_num, ' ') + tabs;
                // 键是字符串，需要反转义
                res += reverse_escape(it.first);
                res += ": ";
                // 递归序列号
                res += it.second.serialize_pretty(space_num, depth + 1);
                res += ',';
            }
            if (*res.rbegin() == ',') *res.rbegin() = '\n';
            if(map.size()) res += tabs + '}';
            else res += " }";
            return std::move(res);
        }
        break;
        case JsonType::ARRAY:
        {
            std::string tabs(depth * space_num, ' ');
            // 数组类型
            std::string res{ "[" };

            const List& list = std::get<List>(content_);
            for (const JsonBasic& it : list) {
                // 递归序列号
                res += '\n' + std::string(space_num, ' ') + tabs;
                res += it.serialize_pretty(space_num, depth + 1);
                res += ',';
            }
            if (*res.rbegin() == ',') *res.rbegin() = '\n';
            if(list.size()) res += tabs + ']';
            else res += " ]";
            return std::move(res);
        }
        break;
        case JsonType::STRING:
            // 字符串类型，直接反转义，然后输出
            return reverse_escape(std::get<std::string>(content_));
            break;
        default:
            // 非对象 非列表 非字符串，可以直接返回内容
            return std::get<std::string>(content_);
            break;
        }
    }

    // 列表访问，拒绝新元素
    JsonBasic& JsonBasic::at(const size_t& index) {
        if (type_ != JsonType::ARRAY) throw JsonTypeException{ "Is not array.\n" };
        List& list = std::get<List>(content_);
        if (index < 0 || index >= list.size()) throw std::out_of_range{ "out of range.\n" };
        return list.at(index);
    }
    // 对象访问，拒绝新元素
    JsonBasic& JsonBasic::at(const std::string& key) {
        if (type_ != JsonType::OBJECT) throw JsonTypeException{ "Is not Object.\n" };
        Map& map = std::get<Map>(content_);
        if (map.find(key) == map.end()) throw std::out_of_range{ + "Key not find.\n" };
        return map.at(key);
    }
    // 列表访问，可能创建新元素
    JsonBasic& JsonBasic::operator[](const size_t& index) {
        if (type_ != JsonType::ARRAY) throw JsonTypeException{ "Is not array.\n" };
        List& list = std::get<List>(content_);
        if (index == list.size()) list.push_back(JsonBasic{});
        if (index < 0 || index > list.size()) throw std::out_of_range{ "out of range.\n" };
        return list[index];
    }
    // 对象访问，可能创建新元素
    JsonBasic& JsonBasic::operator[](const std::string& key) {
        if (type_ != JsonType::OBJECT) throw JsonTypeException{ "Is not Object.\n" };
        Map& map = std::get<Map>(content_);
        if (map.find(key) == map.end()) map[key] = JsonBasic{};
        return map[key];
    }

    // 比较
    bool JsonBasic::operator==(const std::string& str) const {
        JsonBasic tmp{ str };
        return serialize() == tmp.serialize();
    }
    // 比较
    bool JsonBasic::operator==(const JsonBasic& jsonBasic) const noexcept {
        return serialize() == jsonBasic.serialize();
    }

    // 清空内容
    void JsonBasic::clear() noexcept {
        switch (type_)
        {
        case JsonType::OBJECT:
            content_ = Map{};
            break;
        case JsonType::ARRAY:
            content_ = List{};
            break;
        case JsonType::STRING:
            content_ = std::string{};
            break;
        case JsonType::NUMBER:
            content_ = std::string{ "0" };
            break;
        case JsonType::BOOLEN:
            content_ = std::string{ "false" };
            break;
        }
    }
    // 长度获取
    size_t JsonBasic::size() const noexcept{
        switch (type_)
        {
        case JsonType::OBJECT:
            return std::get<Map>(content_).size();
            break;
        case JsonType::ARRAY:
            return std::get<List>(content_).size();
            break;
        case JsonType::STRING:
            return std::get<std::string>(content_).size();
            break;
        default:
            return 1;
            break;
        }
    }

    // 检查是否包含某个key
    bool JsonBasic::hasKey(const std::string& key) const {
        if (type_ != JsonType::OBJECT) throw JsonTypeException{ "Is not Object.\n" };
        const Map& map = std::get<Map>(content_);
        return map.find(key) != map.end();
    }
    // 获取key的集合
    std::set<std::string> JsonBasic::getKeys() const {
        if (type_ != JsonType::OBJECT) throw JsonTypeException{ "Is not Object.\n" };
        const Map& map = std::get<Map>(content_);
        std::set<std::string> keys;
        for (const auto& pair : map) {
            keys.insert(pair.first);
        }
        return std::move(keys);
    }
    // 数组末尾插入元素
    void JsonBasic::push_back(const JsonBasic& jsonBasic) {
        if (type_ != JsonType::ARRAY) throw JsonTypeException{ "Is not Array.\n" };
        List& list = std::get<List>(content_);
        list.push_back(jsonBasic);
    }
    // 数组末尾移动进入元素
    void JsonBasic::push_back(JsonBasic&& jsonBasic) {
        if (type_ != JsonType::ARRAY) throw JsonTypeException{ "Is not Array.\n" };
        List& list = std::get<List>(content_);
        list.push_back(std::move(jsonBasic));
    }
    // 数值指定位置插入元素
    void JsonBasic::insert(const size_t& index, const JsonBasic& jsonBasic) {
        if (type_ != JsonType::ARRAY) throw JsonTypeException{ "Is not Array.\n" };
        List& list = std::get<List>(content_);
        if (index < 0 || index > list.size()) throw std::out_of_range{ "out of range.\n" };
        list.insert(list.begin() + index, jsonBasic);
    }
    // 数组指定位置移入元素
    void JsonBasic::insert(const size_t& index, JsonBasic&& jsonBasic) {
        if (type_ != JsonType::ARRAY) throw JsonTypeException{ "Is not Array.\n" };
        List& list = std::get<List>(content_);
        if (index < 0 || index > list.size()) throw std::out_of_range{ "out of range.\n" };
        list.insert(list.begin() + index, std::move(jsonBasic));
    }
    // 对象指定位置插入键值对
    void JsonBasic::insert(const std::string& key, const JsonBasic& jsonBasic) {
        if (type_ != JsonType::OBJECT) throw JsonTypeException{ "Is not Object.\n" };
        Map& map = std::get<Map>(content_);
        map[key] = jsonBasic;
    }
    // 对象指定位置移动插入键值对
    void JsonBasic::insert(const std::string& key, JsonBasic&& jsonBasic) {
        if (type_ != JsonType::OBJECT) throw JsonTypeException{ "Is not Object.\n" };
        Map& map = std::get<Map>(content_);
        map[key] = std::move(jsonBasic);
    }
    // 对象插入键值对
    void JsonBasic::insert(const std::pair<const std::string, JsonBasic>& p) {
        if (type_ != JsonType::OBJECT) throw JsonTypeException{ "Is not Object.\n" };
        Map& map = std::get<Map>(content_);
        map.insert(p);
    }
    // 数值删除指定位置的元素
    void JsonBasic::erase(const size_t& index) {
        if (type_ != JsonType::ARRAY) throw JsonTypeException{ "Is not array.\n" };
        List& list = std::get<List>(content_);
        if (index < 0 || index >= list.size()) throw std::out_of_range{ std::string{__FILE__} + ":" + std::to_string(__LINE__) + " out of range.\n" };
        list.erase(list.begin() + index);
    }
    // 对象删除指定key的元素
    void JsonBasic::erase(const std::string& key) {
        if (type_ != JsonType::OBJECT) throw JsonTypeException{ "Is not Object.\n" };
        Map& map = std::get<Map>(content_);
        if (map.find(key) == map.end()) throw std::out_of_range{ "Key not find.\n" };
        map.erase(key);
    }

    // as 赋值内容或对象
    long long JsonBasic::as_int64() const {
        if (type_ != JsonType::NUMBER) throw JsonTypeException{ "Is not Number.\n" };
        return std::stoll(std::get<std::string>(content_));
    }
    double JsonBasic::as_double() const {
        if (type_ != JsonType::NUMBER) throw JsonTypeException{ "Is not Number.\n" };
        return std::stod(std::get<std::string>(content_));
    }
    bool JsonBasic::as_bool() const {
        if (type_ != JsonType::BOOLEN) throw JsonTypeException{ "Is not boolen.\n" };
        return std::get<std::string>(content_).at(0) == 't';
    }
    std::string JsonBasic::as_string() const {
        if (type_ != JsonType::STRING) throw JsonTypeException{ "Is not String.\n" };
        return std::get<std::string>(content_);
    }
    std::string JsonBasic::to_string() const noexcept {
        if (type_ == JsonType::STRING) return std::get<std::string>(content_);
        return this->serialize();
    }
    JsonObject JsonBasic::as_object() const {
        if (!(this->is_object())) throw JsonTypeException{ "JsonObject's type must be JsonType::OBJECT.\n" };
        return JsonObject{ *this };
    }
    JsonArray JsonBasic::as_array() const {
        if (!(this->is_array())) throw JsonTypeException{ "JsonArray's type must be JsonType::ARRAY.\n" };
        return JsonArray{ *this };
    }
    JsonValue JsonBasic::as_value() const {
        if (this->is_array() || this->is_object()) throw JsonTypeException{ "JsonValue's type must be not JsonType::ARRAY and JsonType::OBJECT.\n" };
        return JsonValue{ *this };
    }

    // 子类向基类的转换
    JsonBasic::JsonBasic(const JsonObject& jsonObject) noexcept {
        type_ = jsonObject.type();
        content_ = jsonObject.content_;
    }
    JsonBasic::JsonBasic(const JsonArray& jsonArray) noexcept {
        type_ = jsonArray.type();
        content_ = jsonArray.content_;
    }
    JsonBasic::JsonBasic(const JsonValue& jsonValue) noexcept {
        type_ = jsonValue.type();
        content_ = jsonValue.content_;
    }
    JsonBasic::JsonBasic(JsonObject&& jsonObject) noexcept {
        type_ = jsonObject.type();
        content_ = std::move(jsonObject.content_);
        jsonObject.clear();
    }
    JsonBasic::JsonBasic(JsonArray&& jsonArray) noexcept {
        type_ = jsonArray.type();
        content_ = std::move(jsonArray.content_);
        jsonArray.clear();
    }
    JsonBasic::JsonBasic(JsonValue&& jsonValue) noexcept {
        type_ = jsonValue.type();
        content_ = std::move(jsonValue.content_);
        jsonValue.reset();
    }
    JsonBasic& JsonBasic::operator=(const JsonObject& jsonObject) noexcept {
        if (this == &jsonObject) return *this;
        type_ = jsonObject.type();
        content_ = jsonObject.content_;
        return *this;
    }
    JsonBasic& JsonBasic::operator=(const JsonArray& jsonArray) noexcept {
        if (this == &jsonArray) return *this;
        type_ = jsonArray.type();
        content_ = jsonArray.content_;
        return *this;
    }
    JsonBasic& JsonBasic::operator=(const JsonValue& jsonValue) noexcept {
        if (this == &jsonValue) return *this;
        type_ = jsonValue.type();
        content_ = jsonValue.content_;
        return *this;
    }
    JsonBasic& JsonBasic::operator=(JsonObject&& jsonObject) noexcept {
        if (this == &jsonObject) return *this;
        type_ = jsonObject.type();
        content_ = std::move(jsonObject.content_);
        jsonObject.clear();
        return *this;
    }
    JsonBasic& JsonBasic::operator=(JsonArray&& jsonArray) noexcept {
        if (this == &jsonArray) return *this;
        type_ = jsonArray.type();
        content_ = std::move(jsonArray.content_);
        jsonArray.clear();
        return *this;
    }
    JsonBasic& JsonBasic::operator=(JsonValue&& jsonValue) noexcept {
        if (this == &jsonValue) return *this;
        type_ = jsonValue.type();
        content_ = std::move(jsonValue.content_);
        jsonValue.reset();
        return *this;
    }

    // 子类的访问修饰符
    JsonBasic& JsonObject::operator[](const std::string& key) noexcept{
        Map& map = std::get<Map>(content_);
        if (map.find(key) == map.end()) map.insert(std::make_pair(key, JsonBasic{}));
        return map[key];
    }
    JsonBasic& JsonArray::operator[](const size_t& index) {
        List& list = std::get<List>(content_);
        if (index < 0 || index > list.size()) throw std::out_of_range{ "out of range.\n" };
        if (index == list.size()) list.push_back(JsonBasic{});
        if (index < 0 || index > list.size()) throw std::out_of_range{ std::string{__FILE__} + ":" + std::to_string(__LINE__) + " out of range.\n" };
        return list[index];
    }

    // 比较运算符
    bool JsonBasic::operator==(const JsonObject& jsonObject)const noexcept {
        if (this->type_ != jsonObject.type()) return false;
        return this->serialize() == jsonObject.serialize();
    }
    bool JsonBasic::operator==(const JsonArray& jsonArray)const noexcept {
        if (this->type_ != jsonArray.type()) return false;
        return this->serialize() == jsonArray.serialize();
    }
    bool JsonBasic::operator==(const JsonValue& jsonValue)const noexcept {
        if (this->type_ != jsonValue.type()) return false;
        return this->serialize() == jsonValue.serialize();
    }
    bool JsonObject::operator==(const std::string& str) const noexcept {
        try {
            JsonBasic tmp{ str };
            if (tmp.type() != this->type_) return false;
            return tmp.serialize() == this->serialize();
        }
        catch (...) {
            return false;
        }
    }
    bool JsonObject::operator==(const JsonBasic& jsonBasic) const noexcept {
        if (jsonBasic.type() != this->type_) return false;
        return jsonBasic.serialize() == this->serialize();
    }
    bool JsonObject::operator==(const JsonObject& jsonObject) const noexcept {
        return jsonObject.serialize() == this->serialize();
    }
    bool JsonArray::operator==(const std::string& str) const noexcept {
        try {
            JsonBasic tmp{ str };
            if (tmp.type() != this->type_) return false;
            return tmp.serialize() == this->serialize();
        }
        catch (...) {
            return false;
        }
    }
    bool JsonArray::operator==(const JsonBasic& jsonBasic) const noexcept {
        if (jsonBasic.type() != this->type_) return false;
        return jsonBasic.serialize() == this->serialize();
    }
    bool JsonArray::operator==(const JsonArray& jsonArray) const noexcept {
        return jsonArray.serialize() == this->serialize();
    }
    bool JsonValue::operator==(const std::string& str) const noexcept {
        if(type_ != JsonType::STRING) return false;
        return std::get<std::string>(content_) == str;
    }
    bool JsonValue::operator==(const JsonBasic& jsonBasic) const noexcept {
        if (jsonBasic.type() != this->type_) return false;
        return jsonBasic.serialize() == this->serialize();
    }
    bool JsonValue::operator==(const JsonValue& jsonValue) const noexcept {
        return jsonValue.serialize() == this->serialize();
    }

    // 集合和数组的加法
    JsonObject JsonObject::operator+(const JsonObject& jsonObject) const noexcept {
        JsonObject result{ *this };
        for (const auto& it : jsonObject) {
            result.insert(it);
        }
        return std::move(result);
    }
    JsonObject& JsonObject::operator+=(const JsonObject& jsonObject) noexcept {
        for (const auto& it : jsonObject) {
            this->insert(it);
        }
        return *this;
    }
    JsonArray JsonArray::operator+(const JsonArray& jsonArray) const noexcept {
        JsonArray result{ *this };
        for (const auto& it : jsonArray) {
            result.push_back(it);
        }
        return std::move(result);
    }
    JsonArray& JsonArray::operator+=(const JsonArray& jsonArray) noexcept {
        for (const auto& it : jsonArray) {
            this->push_back(it);
        }
        return *this;
    }

    JsonObject& JsonObject::operator=(const JsonBasic& jsonBasic) {
        if (jsonBasic.is_object()) throw JsonTypeException{ "JsonBasic's type must be JsonType::OBJECT.\n" };
        *this = JsonObject{ jsonBasic };
        return *this;
    }
    JsonObject& JsonObject::operator=(JsonBasic&& jsonBasic) {
        if (jsonBasic.is_object()) throw JsonTypeException{ "JsonBasic's type must be JsonType::OBJECT.\n" };
        *this = JsonObject{ std::move(jsonBasic) };
        return *this;
    }
    JsonArray& JsonArray::operator=(const JsonBasic& jsonBasic) {
        if (jsonBasic.is_array()) throw JsonTypeException{ "JsonBasic's type must be JsonType::ARRAY.\n" };
        *this = JsonArray{ jsonBasic };
        return *this;
    }
    JsonArray& JsonArray::operator=(JsonBasic&& jsonBasic) {
        if (jsonBasic.is_array()) throw JsonTypeException{ "JsonBasic's type must be JsonType::ARRAY.\n" };
        *this = JsonArray{ std::move(jsonBasic) };
        return *this;
    }
    JsonValue& JsonValue::operator=(const JsonBasic& jsonBasic) {
        if (jsonBasic.is_value()) throw JsonTypeException{ "JsonBasic's type must be not JsonType::OBJECT and ARRAY.\n" };
        *this = JsonValue{ jsonBasic };
        return *this;
    }
    JsonValue& JsonValue::operator=(JsonBasic&& jsonBasic) {
        if (jsonBasic.is_value()) throw JsonTypeException{ "JsonBasic's type must be not JsonType::OBJECT and ARRAY.\n" };
        *this = JsonValue{ std::move(jsonBasic) };
        return *this;
    }

    JsonObject::JsonObject(const JsonObject& jsonObject) noexcept {
        type_ = jsonObject.type_;
        content_ = jsonObject.content_;
    }
    JsonObject::JsonObject(JsonObject&& jsonObject) noexcept {
        type_ = jsonObject.type_;
        content_ = std::move(jsonObject.content_);
        jsonObject.clear();
    }
    JsonObject& JsonObject::operator=(const JsonObject& jsonObject) noexcept {
        if (this == &jsonObject) return *this;
        type_ = jsonObject.type_;
        content_ = jsonObject.content_;
        return *this;
    }
    JsonObject& JsonObject::operator=(JsonObject&& jsonObject) noexcept {
        if (this == &jsonObject) return *this;
        type_ = jsonObject.type_;
        content_ = std::move(jsonObject.content_);
        jsonObject.clear();
        return *this;
    }

    JsonArray::JsonArray(const JsonArray& jsonArray) noexcept : JsonBasic() {
        type_ = jsonArray.type_;
        content_ = jsonArray.content_;
    }
    JsonArray::JsonArray(JsonArray&& jsonArray) noexcept : JsonBasic() {
        type_ = jsonArray.type_;
        content_ = std::move(jsonArray.content_);
        jsonArray.clear();
    }
    JsonArray& JsonArray::operator=(const JsonArray& jsonArray) noexcept {
        if (this == &jsonArray) return *this;
        type_ = jsonArray.type_;
        content_ = jsonArray.content_;
        return *this;
    }
    JsonArray& JsonArray::operator=(JsonArray&& jsonArray) noexcept {
        if (this == &jsonArray) return *this;
        type_ = jsonArray.type_;
        content_ = std::move(jsonArray.content_);
        jsonArray.clear();
        return *this;
    }

    JsonValue::JsonValue(const JsonValue& jsonValue) noexcept : JsonBasic() {
        type_ = jsonValue.type_;
        content_ = jsonValue.content_;
    }
    JsonValue::JsonValue(JsonValue&& jsonValue) noexcept : JsonBasic() {
        type_ = jsonValue.type_;
        content_ = std::move(jsonValue.content_);
        jsonValue.reset();
    }
    JsonValue& JsonValue::operator=(const JsonValue& jsonValue) noexcept {
        if (this == &jsonValue) return *this;
        type_ = jsonValue.type_;
        content_ = jsonValue.content_;
        return *this;
    }
    JsonValue& JsonValue::operator=(JsonValue&& jsonValue) noexcept {
        if (this == &jsonValue) return *this;
        type_ = jsonValue.type_;
        content_ = std::move(jsonValue.content_);
        jsonValue.reset();
        return *this;
    }

}
