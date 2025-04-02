#include "jsonlib.h"
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <variant>
#include <exception>
#include <stdexcept>
#include <iostream>

namespace Jsonlib{

    /**
     * @brief 内部函数，转义\u字符
     */
    static void json_escape_unicode(std::string& res, const std::string& str, std::string::const_iterator& it) {
        // 将十六进制字符串转换为 Unicode 码点
        if (str.end() - it <= 4) throw JsonStructureException{ "Illegel unicode.\n" };
        ++it;
        std::istringstream iss(str.substr(it-str.begin(), 4));
        it += 3;
        unsigned int codePoint;
        iss >> std::hex >> codePoint;

        if (iss.fail() || codePoint > 0x10FFFF) {
            return;
        }

        if (codePoint >= 0xD800 && codePoint <= 0xDFFF) {
            if (codePoint >= 0xDC00) {
                return;
            }

            // 检查下一个转义序列是否是低代理
            if (str.end() - it <= 6 || *(it+1) != '\\' || *(it+2) != 'u') {
                return;
            }

            // 解析低代理
            it += 3;
            std::string lowHexStr = str.substr(it-str.begin(), 4);
            it += 3; // 将索引移动到低代理转义序列之后

            unsigned int lowCodePoint;
            std::istringstream lowIss(lowHexStr);
            lowIss >> std::hex >> lowCodePoint;

            if (lowIss.fail() || lowCodePoint < 0xDC00 || lowCodePoint > 0xDFFF) {
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
        else return;
    }

    /**
     * @brief 内部函数，转义字符串且移动指针
     */
    static std::string json_escape_next(const std::string& str, std::string::const_iterator& it) {
        // 跳过字符串起始的双引号
        ++it;
        std::string res;

        while (it !=str.end() && *it != '\"') {
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
                    json_escape_unicode(res, str, it);
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
                throw JsonStructureException{ "There are characters that have not been escaped.\n" };
                break;
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
    static std::string json_reverse_escape(const std::string& str) noexcept {
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
        return res;
    }

    /**
     * @brief 内部函数，转义字符串
     */
    static std::string json_escape(const std::string& str) {
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
                    json_escape_unicode(res, str, it);
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
        }
    }

    // 字符串反序列化构造函数
    JsonValue::JsonValue(const std::string& str, int) {
        auto it = str.begin();
        while(it!=str.end() && std::isspace(*it)) ++it;

        // 禁止空内容
        if(it == str.end()){
            throw JsonStructureException{ "Empty JSON data.\n" };
            return;
        }

        switch (*it){
            case '{':
            {
                type_ = JsonType::OBJECT;
                content_ = JsonObject {};
                JsonObject& jsonObject = std::get<JsonObject>(content_);
                std::string key;
                ++it;
                while(it!=str.end()){
                    while (it!=str.end() && std::isspace(*it)) ++it;
                    if(it == str.end() || *it == '}') break;
                    // 寻找key
                    if (*it != '\"') throw JsonStructureException{ "Key is not string.\n" };
                    key = json_escape_next(str, it);
                    // 寻找分号
                    while (it != str.end() && std::isspace(*it)) ++it;
                    if(it == str.end()) throw JsonStructureException{ "Illegal Json Object content.\n" };
                    if (*it == ':') ++it;
                    else throw JsonStructureException{ "Unknow element.\n" };
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
                JsonArray& jsonArray = std::get<JsonArray>(content_);
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
                auto left = it;
                ++it;
                while(it!=str.end() && *it != '\"'){
                    if(*it == '\\'){
                        ++it;
                        if(it == str.end()) break;
                    }
                    ++it;
                }
                if(it == str.end()) throw JsonStructureException {"Unclosed string.\n"};
                content_ = std::string (left, ++it);
                type_ = JsonType::STRING;
            }
            break;
            case 't':
                if(str.end() - it < 4 || str.compare(it-str.begin(), 4, "true")) throw JsonStructureException {};
                type_ = JsonType::BOOLEN;
                content_ = true;
                it += 4;
            break;
            case 'f':
                if(str.end() - it < 5 || str.compare(it-str.begin(), 5, "false")) throw JsonStructureException {};
                type_ = JsonType::BOOLEN;
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
        while( it != str.end() ){
            if(!std::isspace(*it)) throw JsonStructureException {"Unknow content at the end.\n"};
            ++it;
        }
    }

    JsonValue::JsonValue(const std::string& str, std::string::const_iterator& it){
        switch (*it){
            case '{':
            {
                type_ = JsonType::OBJECT;
                content_ = JsonObject {};
                JsonObject& jsonObject = std::get<JsonObject>(content_);
                std::string key;
                ++it;
                while(it!=str.end()){
                    while (it!=str.end() && std::isspace(*it)) ++it;
                    if(it == str.end() || *it == '}') break;
                    // 寻找key
                    if (*it != '\"') throw JsonStructureException{ "Key is not string.\n" };
                    key = json_escape_next(str, it);
                    // 寻找分号
                    while (it != str.end() && std::isspace(*it)) ++it;
                    if(it == str.end()) throw JsonStructureException{ "Illegal Json Object content.\n" };
                    if (*it == ':') ++it;
                    else throw JsonStructureException{ "Unknow element.\n" };
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
                JsonArray& jsonArray = std::get<JsonArray>(content_);
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
                auto left = it;
                ++it;
                while(it!=str.end() && *it != '\"'){
                    if(*it == '\\'){
                        ++it;
                        if(it == str.end()) break;
                    }
                    ++it;
                }
                if(it == str.end()) throw JsonStructureException {"Unclosed string.\n"};
                content_ = std::string (left, ++it);
                type_ = JsonType::STRING;
            }
            break;
            case 't':
                if(str.end() - it < 4 || str.compare(it-str.begin(), 4, "true")) throw JsonStructureException {};
                type_ = JsonType::BOOLEN;
                content_ = true;
                it += 4;
            break;
            case 'f':
                if(str.end() - it < 5 || str.compare(it-str.begin(), 5, "false")) throw JsonStructureException {};
                type_ = JsonType::BOOLEN;
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

    // 字符串类型构造，不解析
    JsonValue::JsonValue(const std::string& str) noexcept{
        type_ = JsonType::STRING;
        content_ = json_reverse_escape(str);
    }

    // 字符串类型赋值
    JsonValue& JsonValue::JsonValue::operator=(const std::string& str) {
        type_ = JsonType::STRING;
        content_ = json_reverse_escape(str);
        return *this;
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
        case JsonType::BOOLEN:
            content_ = false;
            break;
        }
    }

    // 长度获取
    size_t JsonValue::size() const noexcept{
        switch (type_)
        {
        case JsonType::OBJECT:
            return std::get<JsonObject>(content_).size();
            break;
        case JsonType::ARRAY:
            return std::get<JsonArray>(content_).size();
            break;
        case JsonType::STRING:
            return std::get<std::string>(content_).size();
            break;
        default:
            return 1;
            break;
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
        type_ = JsonType::ARRAY;
        content_ = jsonArray;
        return *this;
    }

    // JsonArray移动赋值
    JsonValue& JsonValue::operator=(JsonArray&& jsonArray) noexcept{
        type_ = JsonType::ARRAY;
        content_ = std::move(jsonArray);
        return *this;
    }

    // JsonObject拷贝赋值
    JsonValue& JsonValue::operator=(const JsonObject& jsonObject) noexcept{
        type_ = JsonType::OBJECT;
        content_ = jsonObject;
        return *this;
    }

    // JsonObject移动赋值
    JsonValue& JsonValue::operator=(JsonObject&& jsonObject) noexcept{
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
            const JsonObject& map = std::get<JsonObject>(content_);
            for (const std::pair<std::string, JsonValue>& it : map) {
                // 键是字符串，需要反转义
                res += json_reverse_escape(it.first);
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
            const JsonArray& list = std::get<JsonArray>(content_);
            for (const JsonValue& it : list) {
                // 递归序列号
                res += it.serialize();
                res += ',';
            }
            if (*res.rbegin() == ',') *res.rbegin() = ']';
            else res += ']';
            return std::move(res);
        }
        break;
        case JsonType::BOOLEN:
            return std::get<bool>(content_) ? "true" : "false";
            break;
        case JsonType::ISNULL:
            return "null";
            break;
        default:
            // 数值和字符串，可以直接返回内容
            return std::get<std::string>(content_);
            break;
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

            const JsonObject& map = std::get<JsonObject>(content_);
            for (const std::pair<std::string, JsonValue>& it : map) {
                res += '\n' + std::string(space_num, ' ') + tabs;
                // 键是字符串，需要反转义
                res += json_reverse_escape(it.first);
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

            const JsonArray& list = std::get<JsonArray>(content_);
            for (const JsonValue& it : list) {
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
        case JsonType::BOOLEN:
            return std::get<bool>(content_) ? "true" : "false";
            break;
        case JsonType::ISNULL:
            return "null";
            break;
        default:
            // 数值和字符串，可以直接返回内容
            return std::get<std::string>(content_);
            break;
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
    bool JsonValue::as_bool() const {
        if (type_ != JsonType::BOOLEN) throw JsonTypeException{ "Is not boolen.\n" };
        return std::get<bool>(content_);
    }
    std::string JsonValue::as_string() const {
        if (type_ != JsonType::STRING) throw JsonTypeException{ "Is not String.\n" };
        return json_escape(std::get<std::string>(content_));
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
        JsonArray& list = std::get<JsonArray>(content_);
        if (index < 0 || index >= list.size()) throw std::out_of_range{ "out of range.\n" };
        return list.at(index);
    }
    // 对象访问，拒绝新元素
    JsonValue& JsonValue::at(const std::string& key) {
        if (type_ != JsonType::OBJECT) throw JsonTypeException{ "Is not Object.\n" };
        JsonObject& map = std::get<JsonObject>(content_);
        if (map.find(key) == map.end()) throw std::out_of_range{ + "Key not find.\n" };
        return map.at(key);
    }
    // 列表访问，可能创建新元素
    JsonValue& JsonValue::operator[](const size_t& index) {
        if (type_ != JsonType::ARRAY) throw JsonTypeException{ "Is not array.\n" };
        JsonArray& list = std::get<JsonArray>(content_);
        if (index == list.size()) list.emplace_back();
        if (index < 0 || index > list.size()) throw std::out_of_range{ "out of range.\n" };
        return list[index];
    }
    // 对象访问，可能创建新元素
    JsonValue& JsonValue::operator[](const std::string& key) {
        if (type_ != JsonType::OBJECT) throw JsonTypeException{ "Is not Object.\n" };
        JsonObject& map = std::get<JsonObject>(content_);
        if (map.find(key) == map.end()) map[key] = JsonValue{};
        return map[key];
    }

    // 检查是否包含某个key
    bool JsonValue::hasKey(const std::string& key) const {
        if (type_ != JsonType::OBJECT) throw JsonTypeException{ "Is not Object.\n" };
        const JsonObject& map = std::get<JsonObject>(content_);
        return map.find(key) != map.end();
    }
    // 数组末尾插入元素
    void JsonValue::push_back(const JsonValue& jsonValue) {
        if (type_ != JsonType::ARRAY) throw JsonTypeException{ "Is not Array.\n" };
        JsonArray& list = std::get<JsonArray>(content_);
        list.push_back(jsonValue);
    }
    // 数组末尾移动进入元素
    void JsonValue::push_back(JsonValue&& jsonValue) {
        if (type_ != JsonType::ARRAY) throw JsonTypeException{ "Is not Array.\n" };
        JsonArray& list = std::get<JsonArray>(content_);
        list.push_back(std::move(jsonValue));
    }
    // 数值指定位置插入元素
    void JsonValue::insert(const size_t& index, const JsonValue& jsonValue) {
        if (type_ != JsonType::ARRAY) throw JsonTypeException{ "Is not Array.\n" };
        JsonArray& list = std::get<JsonArray>(content_);
        if (index < 0 || index > list.size()) throw std::out_of_range{ "out of range.\n" };
        list.insert(list.begin() + index, jsonValue);
    }
    // 数组指定位置移入元素
    void JsonValue::insert(const size_t& index, JsonValue&& jsonValue) {
        if (type_ != JsonType::ARRAY) throw JsonTypeException{ "Is not Array.\n" };
        JsonArray& list = std::get<JsonArray>(content_);
        if (index < 0 || index > list.size()) throw std::out_of_range{ "out of range.\n" };
        list.insert(list.begin() + index, std::move(jsonValue));
    }
    // 对象指定位置插入键值对
    void JsonValue::insert(const std::string& key, const JsonValue& jsonValue) {
        if (type_ != JsonType::OBJECT) throw JsonTypeException{ "Is not Object.\n" };
        JsonObject& map = std::get<JsonObject>(content_);
        map[key] = jsonValue;
    }
    // 对象指定位置移动插入键值对
    void JsonValue::insert(const std::string& key, JsonValue&& jsonValue) {
        if (type_ != JsonType::OBJECT) throw JsonTypeException{ "Is not Object.\n" };
        JsonObject& map = std::get<JsonObject>(content_);
        map[key] = std::move(jsonValue);
    }
    // 对象插入键值对
    void JsonValue::insert(const std::pair<const std::string, JsonValue>& p) {
        if (type_ != JsonType::OBJECT) throw JsonTypeException{ "Is not Object.\n" };
        JsonObject& map = std::get<JsonObject>(content_);
        map.insert(p);
    }
    // 数值删除指定位置的元素
    void JsonValue::erase(const size_t& index) {
        if (type_ != JsonType::ARRAY) throw JsonTypeException{ "Is not array.\n" };
        JsonArray& list = std::get<JsonArray>(content_);
        if (index < 0 || index >= list.size()) throw std::out_of_range{ std::string{__FILE__} + ":" + std::to_string(__LINE__) + " out of range.\n" };
        list.erase(list.begin() + index);
    }
    // 对象删除指定key的元素
    void JsonValue::erase(const std::string& key) {
        if (type_ != JsonType::OBJECT) throw JsonTypeException{ "Is not Object.\n" };
        JsonObject& map = std::get<JsonObject>(content_);
        if (map.find(key) == map.end()) throw std::out_of_range{ "Key not find.\n" };
        map.erase(key);
    }

}
