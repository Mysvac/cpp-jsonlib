#include "jsonlib.h"

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <variant>
#include <exception>
#include <stdexcept>

namespace Json{

    template<typename T, typename = std::enable_if_t<std::is_base_of_v<JsonBasic, T>>>
    static bool is_number(const T& t) noexcept {
        return t.getType() == JsonType::NUMBER;
    }

    static bool is_number(const std::string& str) noexcept{
        size_t it=0;
        size_t len = str.size();
        // if(len==0) return false;
        if(str[0] == '-' || str[0] == '+'){
            if(len==1) return false;
            ++it;
            if(str[it]=='.') return false;
        }
        bool have_point = false;
        for(;it<len;++it){
            if(str[it] == '.'){
                if(have_point) return false;
                have_point = true;
                continue;
            }
            else if(!std::isdigit(str[it])) return false;
        }
        return true;
    }

    // 反转义字符串
    static std::string reverse_escape(const std::string& str) noexcept{
        std::string res;
        // 提前分配空间，减少扩容开销
        if(str.size() > 15) res.reserve(str.size() + 2 + (str.size() >> 4));
        res += "\"";
        for(const char& it: str){
            switch(it){
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
                case '\v':
                    res += "\\v";
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
        res+="\"";
        return std::move(res);
    }

    // 转义字符串，两端位置固定
    static std::string escape(const std::string& str,const size_t& left,const size_t& right){
        if(left < 0 || right >= str.size() || left > right) 
            throw std::out_of_range { std::string { "left:" } + std::to_string(left) + " right:" + std::to_string(right) + " size:" + std::to_string(str.size()) };
        
        std::string res;
        // 预分配空间
        if(right - left > 14) res.reserve(right - left + 1);

        size_t index = left;
        while(index <= right){
            if(str[index] == '\\'){
                // 转义字符处理
                ++index;
                if(index > right) throw JsonStructureException { "String end with escape character :" + str.substr(left, right - left + 1) };
                switch (str[index])
                {
                case 'n':
                    res += '\n';
                    break;
                case 'r':
                    res += '\r';
                    break;
                case 't':
                    res += '\t';
                    break;
                case 'v':
                    res += '\b';
                    break;
                case 'f':
                    res += '\f';
                    break;
                case 'b':
                    res += '\b';
                    break;
                default:
                    res += str[index];
                    break;
                }
            }
            else{
                res += str[index];
            }
            ++index;
        }
        return std::move(res);
    }

    // 转义字符，最终位置不确定但小于等于right，且修改外部参数left
    static std::string escape_next(const std::string& str, size_t& left, const size_t& right){
        if(left < 0 || right >= str.size() || left > right) 
            throw std::out_of_range { std::string { "left:" } + std::to_string(left) + " right:" + std::to_string(right) + " size:" + std::to_string(str.size()) };
        
        // 跳过字符串起始的双引号
        ++left;
        std::string res;

        while(left <= right && str[left] != '\"'){
            if(str[left] == '\\'){
                // 转义字符处理
                ++left;
                if(left > right) throw JsonStructureException { "String have not end char '\"'." };
                switch (str[left])
                {
                case 'n':
                    res += '\n';
                    break;
                case 'r':
                    res += '\r';
                    break;
                case 't':
                    res += '\t';
                    break;
                case 'v':
                    res += '\b';
                    break;
                case 'f':
                    res += '\f';
                    break;
                case 'b':
                    res += '\b';
                    break;
                default:
                    res += str[left];
                    break;
                }
            }
            else{
                res += str[left];
            }
            ++left;
        }
        // left最终停在结束的双引号
        return std::move(res);
    }


    // 辅助递归构造函数
    JsonBasic::JsonBasic(const std::string& str, size_t& index, const size_t& tail){
        switch(str[index]){
        case '{':
            {
                type_ = JsonType::OBJECT;
                content_ = Map {};
                Map& map = std::get<Map>(content_);
                std::string key;
                ++index;
                while(str[index] != '}'){
                    while(std::isspace(str[index])) ++index;
                    // 空集合，结束
                    if(str[index] == '}') break;
                    if(str[index] != '\"') throw JsonStructureException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + "\n" + std::to_string(index) + ':' + str[index] + "\n" };
                    // 获取键
                    key = Json::escape_next(str, index, tail);
                    ++index;
                    // 分隔符
                    while(index < tail && std::isspace(str[index])) ++index;
                    if(str[index] == ':') ++index;
                    else throw JsonStructureException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + "\n" + std::to_string(index) + ':' + str[index] + "\n" };

                    // 获取值
                    while(std::isspace(str[index])) ++index;
                    map.insert(std::make_pair(key, JsonBasic { str, index, tail }));
                    // index默认停在子元素的最后一个字符，所以需要先自增
                    ++index;
                    while(index < tail && std::isspace(str[index])) ++index;
                    // 判断合法性，结束后下个字符要么逗号，要么终止
                    if(str[index] == ',') ++index;
                    else if(str[index] != '}') throw JsonStructureException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + "\n" + std::to_string(index) + ':' + str[index] + "\n" };
                }
                // 大于，说明大括号未闭合
                if(index > tail) throw JsonStructureException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + "\n" + std::to_string(index) + ':' + str[index] + "\n" };
            }
            break;
        case '[':
            {
                type_ = JsonType::ARRAY;
                content_ = List {};
                List& list = std::get<List>(content_);
                ++index;
                while(str[index] != ']'){
                    while(std::isspace(str[index])) ++index;
                    // 空列表，结束
                    if(str[index] == ']') break;
                    // 添加子类并移动index到子类末尾
                    list.push_back(JsonBasic { str, index, tail });
                    // index默认停在子元素的最后一个字符，所以需要先自增
                    ++index;
                    while(index < tail && std::isspace(str[index])) ++index;
                    // 判断合法性，结束后下个字符要么逗号，要么终止
                    if(str[index] == ',') ++index;
                    else if(str[index] != ']') throw JsonStructureException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + "\n" + std::to_string(index) + ':' + str[index] + "\n" };
                }
                // 大于，说明方括号未闭合
                if(index > tail) throw JsonStructureException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + "\n" + std::to_string(index) + ':' + str[index] + "\n" };
            }
            break;
        case '\"':
            type_ = JsonType::STRING;
            content_ = Json::escape_next(str, index, tail);
            break;
        case 't':
            // str.compare return 0 if equal
            if(tail - index <= 3 || str.compare(index, 4, "true")) throw JsonStructureException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + "\n" + std::to_string(index) + ':' + str[index] + "\n" };
            type_ = JsonType::BOOLEN;
            content_ = std::string { "true" };
            index += 3;
            break;
        case 'f':
            if(tail - index <= 4 || str.compare(index, 5, "false")) throw JsonStructureException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + "\n" + std::to_string(index) + ':' + str[index] + "\n" };
            type_ = JsonType::BOOLEN;
            content_ = std::string { "false" };
            index += 4;
            break;
        case 'n':
            if(tail - index <= 4 || str.compare(index, 4, "null")) throw JsonStructureException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + "\n" + std::to_string(index) + ':' + str[index] + "\n" };
            type_ = JsonType::ISNULL;
            content_ = std::string { "null" };
            index += 3;
            break;
        default:
            if(!std::isdigit(str[index]) && str[index]!='-' && str[index]!='+') throw JsonStructureException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + "\n" + std::to_string(index) + ':' + str[index] + "\n" };
            {
                type_ = JsonType::NUMBER;
                bool have_point = false;
                std::string tmp;
                tmp += str[index];
                // 正负号开头，且紧跟小数点
                if(index >= tail && (str[index]!='-' || str[index]!='+')) throw JsonStructureException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + "\n" + std::to_string(index) + ':' + str[index] + "\n" };
                if(str[index+1]=='.' && (str[index]=='-' || str[index]=='+')) throw JsonStructureException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + "\n" + std::to_string(index) + ':' + str[index] + "\n" };

                while(std::isdigit(str[index + 1]) || (!have_point && str[index + 1] == '.')){
                    ++index;
                    tmp += str[index];
                    if(str[index] == '.') have_point = true;
                }
                content_ = std::move(tmp);
            }
            break;
        }
    }

    // 字符串反序列化构造函数
    JsonBasic::JsonBasic(const std::string& str){
        // 起始位置
        size_t index = 0;
        // 字符串长度
        const size_t len = str.size();
        // 末尾位置
        size_t tail = len - 1;

        while(index < len && std::isspace(str[index])) ++index;
        if(index == len) throw JsonStructureException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + "\n" + std::to_string(index) + ':' + str[index] + "\n" };
        while(std::isspace(str[tail])) --tail;
        
        switch(str[index]){
        case '{':
            if(str[tail] != '}') throw JsonStructureException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + "\n" + std::to_string(index) + ':' + str[index] + "\n" };
            {
                type_ = JsonType::OBJECT;
                content_ = Map {};
                Map& map = std::get<Map>(content_);
                std::string key;
                ++index;
                while(index < tail){
                    while(std::isspace(str[index])) ++index;
                    // 空集合，结束
                    if(index == tail) break;
                    if(str[index] != '\"') throw JsonStructureException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + "\n" + std::to_string(index) + ':' + str[index] + "\n" };
                    // 获取键
                    key = Json::escape_next(str, index, tail);
                    ++index;
                    // 分隔符
                    while(index <= tail && std::isspace(str[index])) ++index;
                    if(str[index] == ':') ++index;
                    else throw JsonStructureException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + "\n" + std::to_string(index) + ':' + str[index] + "\n" };

                    // 获取值
                    while(std::isspace(str[index])) ++index;
                    map.insert(std::make_pair(key, JsonBasic { str, index, tail }));

                    // index默认停在子元素的最后一个字符，所以需要先自增
                    ++index;
                    while(index < tail && std::isspace(str[index])) ++index;
                    // 判断合法性，结束后下个字符要么逗号，要么终止
                    if(str[index] == ',') ++index;
                    else if(index != tail) throw JsonStructureException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + "\n" + std::to_string(index) + ':' + str[index] + "\n" };
                }
                // 大于，说明大括号未闭合
                if(index > tail) throw JsonStructureException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + "\n" + std::to_string(index) + ':' + str[index] + "\n" };
            }
            break;
        case '[':
            if(str[tail] != ']') throw JsonStructureException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + "\n" + std::to_string(index) + ':' + str[index] + "\n" };
            {
                type_ = JsonType::ARRAY;
                content_ = List {};
                List& list = std::get<List>(content_);
                ++index;
                while(index < tail){
                    while(std::isspace(str[index])) ++index;
                    // 空列表，结束
                    if(index == tail) break;
                    // 添加子类并移动index到子类末尾
                    list.push_back(JsonBasic { str, index, tail });
                    // index默认停在子元素的最后一个字符，所以需要先自增
                    ++index;
                    while(index < tail && std::isspace(str[index])) ++index;
                    // 判断合法性，结束后下个字符要么逗号，要么终止
                    if(str[index] == ',') ++index;
                    else if(index != tail) throw JsonStructureException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + "\n" + std::to_string(index) + ':' + str[index] + "\n" };
                }
                // 大于，说明方括号未闭合
                if(index > tail) throw JsonStructureException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + "\n" + std::to_string(index) + ':' + str[index] + "\n" };
            }
            break;
        case '\"':
            if(str[tail] != '\"') throw JsonStructureException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + "\n" + std::to_string(index) + ':' + str[index] + "\n" };
            type_ = JsonType::STRING;
            content_ = Json::escape(str, index + 1, tail - 1);
            break;
        case 't':
            // str.compare return 0 if equal
            if(tail - index <= 3 || str.compare(index, 4, "true")) throw JsonStructureException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + "\n" + std::to_string(index) + ':' + str[index] + "\n" };
            type_ = JsonType::BOOLEN;
            content_ = std::string { "true" };
            break;
        case 'f':
            if(tail - index <= 4 || str.compare(index, 5, "false")) throw JsonStructureException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + "\n" + std::to_string(index) + ':' + str[index] + "\n" };
            type_ = JsonType::BOOLEN;
            content_ = std::string { "false" };
            break;
        case 'n':
            if(tail - index <= 4 || str.compare(index, 4, "null")) throw JsonStructureException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + "\n" + std::to_string(index) + ':' + str[index] + "\n" };
            type_ = JsonType::ISNULL;
            content_ = std::string { "null" };
            break;
        default:
            if(!std::isdigit(str[index])  && str[index]!='-' && str[index]!='+') throw JsonStructureException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + "\n" + std::to_string(index) + ':' + str[index] + "\n" };
            {
                std::string tmp = str.substr(index, tail-index+1);
                if(!Json::is_number(tmp)) throw JsonStructureException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + "\n" + std::to_string(index) + ':' + str[index] + "\n" };
                type_ = JsonType::NUMBER;
                content_ = std::move(tmp);
            }
            break;
        }
    }
    
    // 反序列化赋值运算符
    JsonBasic& JsonBasic::operator=(const std::string& str){
        *this = JsonBasic { str };
        return *this;
    }

    // 拷贝构造函数
    JsonBasic::JsonBasic(const JsonBasic& jsonBasic) noexcept{
        type_ = jsonBasic.type_;
        content_ = jsonBasic.content_;
    }
    // 移动构造函数
    JsonBasic::JsonBasic(JsonBasic&& jsonBasic) noexcept{
        type_ = jsonBasic.type_;
        content_ = std::move(jsonBasic.content_);
        jsonBasic.type_ = JsonType::ISNULL;
        jsonBasic.content_ = std::string { "null" };
    }

    // 拷贝赋值函数
    JsonBasic& JsonBasic::operator=(const JsonBasic& jsonBasic) noexcept{
        if(this == &jsonBasic) return *this;
        type_ = jsonBasic.type_;
        content_ = jsonBasic.content_;
        return *this;
    }
    // 移动赋值
    JsonBasic& JsonBasic::operator=(JsonBasic&& jsonBasic) noexcept{
        if(this == &jsonBasic) return *this;
        type_ = jsonBasic.type_;
        content_ = std::move(jsonBasic.content_);
        jsonBasic.type_ = JsonType::ISNULL;
        jsonBasic.content_ = std::string { "null" };
        return *this;
    }

    // 整数 浮点
    JsonBasic::JsonBasic(const int& tmp) noexcept{
        *this = JsonBasic { std::to_string(tmp) };
        type_ = JsonType::NUMBER;
    }
    JsonBasic::JsonBasic(const long long& tmp) noexcept{
        *this = JsonBasic { std::to_string(tmp) };
        type_ = JsonType::NUMBER;
    }
    JsonBasic::JsonBasic(const double& tmp) noexcept {
        *this = JsonBasic { std::to_string(tmp) };
        type_ = JsonType::NUMBER;
    }
    JsonBasic& JsonBasic::operator=(const int& tmp) noexcept{
        *this = JsonBasic { std::to_string(tmp) };
        type_ = JsonType::NUMBER;
        return *this;
    }
    JsonBasic& JsonBasic::operator=(const long long& tmp) noexcept{
        *this = JsonBasic { std::to_string(tmp) };
        type_ = JsonType::NUMBER;
        return *this;
    }
    JsonBasic& JsonBasic::operator=(const double& tmp) noexcept{
        *this = JsonBasic { std::to_string(tmp) };
        type_ = JsonType::NUMBER;
        return *this;
    }

    // 序列号JSON对象
    std::string JsonBasic::serialize() const noexcept{ 
        switch(type_)
        {
        case JsonType::OBJECT:
            {
                // 对象类型
                // 是否在开头加上逗号
                bool add_comma = false;
                std::string res = "{";
                const Map& map = std::get<Map>(content_);
                for(const auto& it : map){
                    if(add_comma) res += ',';
                    res += ' ';
                    // 键是字符串，需要反转义
                    res += reverse_escape(it.first);
                    res += ": ";
                    // 递归序列号
                    res += it.second.serialize();
                    add_comma = true;
                }
                res += " }";
                return std::move(res);
            }
            break;
        case JsonType::ARRAY:
            {
                // 数组类型
                // 是否在开头加上逗号
                std::string res = "[";
                const List& list = std::get<List>(content_);
                bool add_comma = false;
                for(const JsonBasic& it : list){
                    if(add_comma) res += ',';
                    res += ' ';
                    // 递归序列号
                    res += it.serialize();
                    add_comma = true;
                }
                res += " ]";
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

    // 重置为null
    void JsonBasic::reset() noexcept {
        type_ = JsonType::ISNULL;
        content_ = std::string { "null" };
    }
    // 清空内容
    void JsonBasic::clear() noexcept {
        switch (type_)
        {
        case JsonType::OBJECT:
            content_ = Map {};
            break;
        case JsonType::ARRAY:
            content_ = List {};
            break;
        case JsonType::STRING:
            content_ = std::string {};
            break;
        case JsonType::NUMBER:
            content_ = std::string {"0"};
            break;
        case JsonType::BOOLEN:
            content_ = std::string {"false"};
            break;
        }
    }

    // 检测长度
    size_t JsonBasic::size()const noexcept{
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
        case JsonType::NUMBER:
            return std::get<std::string>(content_).size();
            break;
        default:
            return 0;
            break;
        }
    }

    // 列表访问，拒绝新元素
    JsonBasic& JsonBasic::at(const size_t& index){
        if(type_ != JsonType::ARRAY) throw JsonTypeException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + " Is not array.\n"};
        List& list = std::get<List>(content_);
        if(index < 0 || index >= list.size()) throw std::out_of_range { std::string{__FILE__} + ":" + std::to_string(__LINE__) + " out of range.\n"};
        return list.at(index);
    }
    // 对象访问，拒绝新元素
    JsonBasic& JsonBasic::at(const std::string& key){
        if(type_ != JsonType::OBJECT) throw JsonTypeException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + " Is not Object.\n"};
        Map& map = std::get<Map>(content_);
        if(map.find(key) == map.end()) throw std::out_of_range { std::string{__FILE__} + ":" + std::to_string(__LINE__) + " Key not find.\n"};
        return map.at(key);
    }
    // 列表访问，可能创建新元素
    JsonBasic& JsonBasic::operator[](const size_t& index){
        if(type_ != JsonType::ARRAY) throw JsonTypeException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + " Is not array.\n"};
        List& list = std::get<List>(content_);
        if(index == list.size()) list.push_back( JsonBasic {} );
        if(index < 0 || index > list.size()) throw std::out_of_range { std::string{__FILE__} + ":" + std::to_string(__LINE__) + " out of range.\n"};
        return list[index];
    }
    // 对象访问，可能创建新元素
    JsonBasic& JsonBasic::operator[](const std::string& key){
        if(type_ != JsonType::OBJECT) throw JsonTypeException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + " Is not Object.\n"};
        Map& map = std::get<Map>(content_);
        if(map.find(key) == map.end()) map.insert(std::make_pair(key, JsonBasic {}));
        return map[key];
    }

    // 检查是否包含某个key
    bool JsonBasic::hasKey(const std::string& key) const{
        if(type_ != JsonType::OBJECT) throw JsonTypeException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + " Is not Object.\n" };
        const Map& map = std::get<Map>(content_);
        return map.find(key) != map.end();
    }
    // 数组末尾插入元素
    void JsonBasic::push_back(const JsonBasic& jsonBasic){
        if(type_ != JsonType::ARRAY) throw JsonTypeException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + " Is not Array.\n" };
        List& list = std::get<List>(content_);
        list.push_back(jsonBasic);
    }
    // 数组末尾移动进入元素
    void JsonBasic::push_back(JsonBasic&& jsonBasic){
        if(type_ != JsonType::ARRAY) throw JsonTypeException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + " Is not Array.\n" };
        List& list = std::get<List>(content_);
        list.push_back(std::move(jsonBasic));
    }
    // 数值指定位置插入元素
    void JsonBasic::insert(const size_t& index, const JsonBasic& jsonBasic){
        if(type_ != JsonType::ARRAY) throw JsonTypeException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + " Is not Array.\n" };
        List& list = std::get<List>(content_);
        if(index < 0 || index > list.size()) throw std::out_of_range { std::string{__FILE__} + ":" + std::to_string(__LINE__) + " out of range.\n"};
        list.insert(list.begin()+index, jsonBasic);
    }
    // 数组指定位置移入元素
    void JsonBasic::insert(const size_t& index, JsonBasic&& jsonBasic){
        if(type_ != JsonType::ARRAY) throw JsonTypeException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + " Is not Array.\n" };
        List& list = std::get<List>(content_);
        if(index < 0 || index > list.size()) throw std::out_of_range { std::string{__FILE__} + ":" + std::to_string(__LINE__) + " out of range.\n"};
        list.insert(list.begin()+index, std::move(jsonBasic));
    }
    // 对象指定位置插入键值对
    void JsonBasic::insert(const std::string& key,const JsonBasic& jsonBasic){
        if(type_ != JsonType::OBJECT) throw JsonTypeException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + " Is not Object.\n"};
        Map& map = std::get<Map>(content_);
        map[key] = jsonBasic;
    }
    // 对象指定位置移动插入键值对
    void JsonBasic::insert(const std::string& key,JsonBasic&& jsonBasic){
        if(type_ != JsonType::OBJECT) throw JsonTypeException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + " Is not Object.\n"};
        Map& map = std::get<Map>(content_);
        map[key] = std::move(jsonBasic);
    }
    // 数值删除指定位置的元素
    void JsonBasic::erase(const size_t& index){
        if(type_ != JsonType::ARRAY) throw JsonTypeException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + " Is not array.\n"};
        List& list = std::get<List>(content_);
        if(index < 0 || index >= list.size()) throw std::out_of_range { std::string{__FILE__} + ":" + std::to_string(__LINE__) + " out of range.\n"};
        list.erase(list.begin()+index);
    }
    // 对象删除指定key的元素
    void JsonBasic::erase(const std::string& key){
        if(type_ != JsonType::OBJECT) throw JsonTypeException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + " Is not Object.\n"};
        Map& map = std::get<Map>(content_);
        if(map.find(key) == map.end()) throw std::out_of_range { std::string{__FILE__} + ":" + std::to_string(__LINE__) + " Key not find.\n"};
        map.erase(key);
    }

    long long JsonBasic::as_int64() const{
        if(type_ != JsonType::NUMBER) throw JsonTypeException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + " Is not Number.\n"};
        return std::stol(std::get<std::string>(content_));
    }
    double JsonBasic::as_double() const{
        if(type_ != JsonType::NUMBER) throw JsonTypeException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + " Is not Number.\n"};
        return std::stod(std::get<std::string>(content_));
    }
    bool JsonBasic::as_bool() const{
        if(type_ != JsonType::BOOLEN) throw JsonTypeException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + " Is not Number.\n"};
        return std::get<std::string>(content_).at(0) == 't';
    }
    std::string JsonBasic::as_string() const{
        if(type_ != JsonType::STRING) throw JsonTypeException { std::string{__FILE__} + ":" + std::to_string(__LINE__) + " Is not Number.\n"};
        return std::get<std::string>(content_);
    }
    std::string JsonBasic::to_string() const noexcept{
        if(type_ != JsonType::STRING) return std::get<std::string>(content_);
        return serialize();
    }

}
