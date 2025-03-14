#include "jsonlib.h"


namespace Json{

    // 判断字符串类型能否转数值
    static bool strIsNumber(const std::string& str) noexcept{
        double tmp;
        std::istringstream iss(str);
        iss >> tmp;
        return iss && !iss.fail();
    }


    // 反转义字符串
    static std::string reverse_escape(const std::string& str){
        std::string res;
        // 提前分配空间，减少扩容开销
        if(str.size() > 13) res.reserve(str.size() + 2 + (str.size() >> 4));
        res += "\"";
        for(const char& it: str){
            switch(it){
                case '\"':
                    res += "\\\"";
                    break;
                case '\\':
                    res += "\\\\";
                case '\t':
                    res += "\\t";
                    break;
                case '\n':
                    res += "\\n";
                    break;
                case '\r':
                    res += "\\r";
                    break;
                case '\f':
                    res += "\\f";
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

    // JsonReader构造函数
    JsonReader::JsonReader(const std::string& str){
        // 字符串长度
        size_t len = str.size();
        // 左右端点定位
        size_t left = 0;
        size_t right = len-1;

        // 去除左侧空字符
        while(left<len && std::isspace(str[left])) ++left;
        if(left == len) throw JsonException { "JSON Content is empty.\n" };
        // 去除右侧空字符
        while(std::isspace(str[right])) --right;

        // 分析当前类型
        switch(str[left])
        {
        case '{':
            // 对象类型
            if(str[right] != '}') throw JsonException { "illegal Object at:" + std::to_string(__LINE__) + " json: " + str + "\n" };
            try{
                Map& map = std::get<Map>(content_);
                size_t l = left + 1;
                size_t r = left + 1;

                // 暂存key，初始容量15，无需reverse()
                std::string key;

                // 遍历内容，分析出全部的键值对
                while(l < right){
                    key = "";

                    // 寻找key，key一定是字符串
                    // at()提供越界检测和抛出异常；[]不检测越界，但是速度更快。
                    while(str.at(l) != '\"') ++l;
                    r = l + 1;
                    while(str.at(r) != '\"'){
                        if(str[r] == '\\'){
                            key += str.at(r+1);
                            r += 2;
                        }
                        else{
                            key += str[r];
                            ++r;
                        }
                    }
                    
                    // 键与值中间有 ':' 分割
                    l = r + 1;
                    while(str.at(l)!=':') ++l;
                    ++l;
                    
                    // 开始分析值
                    while(std::isspace(str.at(l))) ++l;
                    r=l+1;

                    // 确定值类型
                    switch(str[l])
                    {
                    case '{':
                        // 值是对象类型，则根据大括号匹配数量截取内容
                        {
                            size_t cnt = 1;
                            while(r<right){
                                while(str.at(r)!='{' && str[r]!='}' && str[r]!='\"') ++r;
                                // 任何字符串内部内容，直接跳过
                                if(str[r]=='\"'){
                                    ++r;
                                    while(str.at(r)!='\"'){
                                        if(str[r]=='\\') r+=2;
                                        else ++r;
                                    }
                                }
                                if(str[r]=='{') ++cnt;
                                if(str[r]=='}') --cnt;
                                if(cnt==0) break;
                                ++r;
                            }
                            // cnt!=0，说明没能找到匹配的大括号，错误格式
                            if(cnt != 0) throw JsonException { "illegal Object at " + std::to_string(__LINE__) + ": " + str + "\n" };
                        }
                        break;
                    case '[':
                        // 值是数组类型，则根据方括号匹配数量截取内容
                        {
                            size_t cnt = 1;
                            while(r<right){
                                while(str.at(r)!='[' && str[r]!=']' && str[r]!='\"') ++r;
                                if(str[r]=='\"'){
                                    ++r;
                                    while(str.at(r)!='\"'){
                                        if(str[r]=='\\') r+=2;
                                        else ++r;
                                    }
                                }
                                if(str[r]=='[') ++cnt;
                                if(str[r]==']') --cnt;
                                if(cnt==0) break;
                                ++r;
                            }
                            // cnt!=0，说明没能找到匹配的方括号，错误格式
                            if(cnt != 0) throw JsonException { "illegal List at " + std::to_string(__LINE__) + ": " + str + "\n" };
                        }
                        break;
                    case '\"':
                        // 字符串类型，跑一遍即可
                        while(str.at(r)!='\"'){
                            if(str[r]=='\\') r+=2;
                            else ++r;
                        }
                        break;
                    default:
                        // null，bool，number类型，则遍历到逗号、右大括号或空格
                        while(!std::isspace(str.at(r)) && str[r]!=',' && str[r]!='}') ++r;
                        // 减一，保证当前r在内容的最后一个位置
                        --r;
                        break;
                    }

                    // 截取当前子文本内容，作为value
                    map[key] = str.substr(l, r-l+1);
                    ++r;

                    // 开始寻找下一个键值对
                    while(std::isspace(str.at(r))) ++r;
                    if(str.at(r)==','){
                        // 逗号说明还有下一个
                        l = r+1;
                        continue;
                    }
                    else if(r==right){
                        // 不是逗号，那就应该结束了，r应该等于right
                        break;
                    }
                    else{
                        throw JsonException { "illegal Object at " + std::to_string(__LINE__) + ": " + str + "\n" };
                    }
                }
            }
            catch(const std::exception& e){
                throw JsonException { e.what() };
            }
            break;
        case '[':
            // [ 开头，数组类型
            if(str[right] != ']') throw JsonException { "illegal List at " + std::to_string(__LINE__) + " json: " + str + "\n" };
            try{
                type_ = JsonType::ARRAY;
                content_ = List{};
                List& list = std::get<List>(content_);
                size_t l=left+1;
                size_t r=left+1;

                // 遍历内容，分析出全部的内部类型
                // 由于是数组类型，内部分析时，等于仅查找值而没有键
                // 下方内容与对象类型的写法类似
                while(l<right){

                    // 跳过空字符
                    // at访问，会检测越界，提供异常处理功能
                    while(std::isspace(str.at(l))) ++l;
                    r=l+1;

                    // 确定内部值类型
                    switch(str[l])
                    {
                    case '{':
                        // 内部值是对象类型，则根据大括号匹配数量截取内容
                        {
                            size_t cnt = 1;
                            while(r<right){
                                while(str.at(r)!='{' && str[r]!='}' && str[r]!='\"') ++r;
                                // 跳过字符串
                                if(str[r]=='\"'){
                                    ++r;
                                    while(str.at(r)!='\"'){
                                        if(str[r]=='\\') r+=2;
                                        else ++r;
                                    }
                                }
                                if(str[r]=='{') ++cnt;
                                if(str[r]=='}') --cnt;
                                if(cnt==0) break;
                                ++r;
                            }
                            // cnt!=0，说明没能找到匹配的大括号，错误格式
                            if(cnt != 0) throw JsonException { "illegal Object at " + std::to_string(__LINE__) + ": " + str + "\n" };
                        }
                        break;
                    case '[':
                        // 值是数组类型，则根据方括号匹配数量截取内容
                        {
                            size_t cnt = 1;
                            while(r<right){
                                while(str.at(r)!='[' && str[r]!=']' && str[r]!='\"') ++r;
                                // 跳过字符串
                                if(str[r]=='\"'){
                                    ++r;
                                    while(str.at(r)!='\"'){
                                        if(str[r]=='\\') r+=2;
                                        else ++r;
                                    }
                                }
                                else if(str[r]=='[') ++cnt;
                                else if(str[r]==']') --cnt;
                                if(cnt==0) break;
                                ++r;
                            }
                            // cnt!=0，说明没能找到匹配的方括号，错误格式
                            if(cnt != 0) throw JsonException { "illegal List at " + std::to_string(__LINE__) + ": " + str + "\n" };
                        }
                        break;
                    case '\"':
                        // 字符串类型，跑一遍即可，跳过转义字符
                        while(str.at(r)!='\"'){
                            if(str[r]=='\\') r+=2;
                            else ++r;
                        }
                        break;
                    default:
                        // null，bool，number类型，则遍历到逗号、右方括号或空格
                        while(!std::isspace(str.at(r)) && str[r]!=',' && str[r]!=']') ++r;
                        // 减一，保证当前r在内容的最后一个位置
                        --r;
                        break;
                    }

                    // 截取当前子对象内容
                    list.push_back(str.substr(l, r-l+1));
                    ++r;

                    // 开始寻找下一个值对
                    while(std::isspace(str.at(r))) ++r;
                    if(str[r]==','){
                        // 逗号说明还有下一个
                        l = r+1;
                        continue;
                    }
                    else if(r==right){
                        // 不是逗号，那就应该结束了，r应该等于right
                        break;
                    }
                    else{
                        throw JsonException { "illegal List at " + std::to_string(__LINE__) + ": " + str + "\n" };
                    }
                }
            }
            catch(const std::exception& e){
                throw JsonException { e.what() };
            }
            break;
        case '\"':
            // "开头，只能是字符串类型
            if(right == left || str[right] != '\"') throw JsonException { "illegal String at " + std::to_string(__LINE__) + " json: " + str + "\n" };
            {
                type_ = JsonType::STRING;
                // 将字符串进行转义
                // 没有设置外部转义函数，减少一次字符串复制开销
                // 注：视图在C++20加入，此版本选用C++17，难以局部引用
                std::string tmp;
                // 长字符串预分配，减少扩容开销
                if(right - left > 16) tmp.reserve(right - left -1);
                size_t l=left+1;
                while(l<right){
                    if(str[l]=='\\'){
                        switch(str[l+1])
                        {
                        case '\"':
                            tmp += '\"';
                            break;
                        case '\\':
                            tmp += '\\';
                        case '/':
                            tmp += '/';
                        case 't':
                            tmp += '\t';
                            break;
                        case 'n':
                            tmp += '\n';
                            break;
                        case 'r':
                            tmp += '\r';
                            break;
                        case 'f':
                            tmp += '\f';
                            break;
                        case 'b':
                            tmp += '\b';
                            break;
                        default:
                            throw JsonException { "illegal String at " + std::to_string(__LINE__) + " json: " + str + "\n" };
                            break;
                        }
                        l+=2;
                    }
                    else{
                        tmp += str[l];
                        ++l;
                    }
                }
                content_ = std::move(tmp);
            }
            break;
        case 't':
            // t开头，只能是true
            if(right-left != 3 || str.substr(left,4) != "true") throw JsonException { "illegal bool at " + std::to_string(__LINE__) + ": " + str + "\n" };
            type_ = JsonType::BOOLEN;
            content_ = std::string { "true" };
            break;
        case 'f':
            // t开头，只能是false
            if(right-left != 4 || str.substr(left,5) != "false") throw JsonException { "illegal bool at " + std::to_string(__LINE__) + ": " + str + "\n" };
            type_ = JsonType::BOOLEN;
            content_ = std::string { "false" };
            break;
        case 'n':
            // n开头，只能是null
            if(right-left != 3 || str.substr(left,4) != "null") throw JsonException { "illegal null at " + std::to_string(__LINE__) + ": " + str + "\n" };
            type_ = JsonType::ISNULL;
            content_ = std::string { "null" };
            break;
        default:
            // 只剩下数值类型
            if(!strIsNumber(str.substr(left, right - left + 1))) throw JsonException { "illegal numver at " + std::to_string(__LINE__) + ": " + str + "\n" };
            type_ = JsonType::NUMBER;
            content_ = std::string { str.substr(left, right-left+1) };
            break;
        }
    }

    // JsonObject带参数构造函数
    JsonBasic::JsonBasic(const std::string& str){
        // 调用JsonReader初步解析数据
        JsonReader reader { str };
        // JSON数据类型同步
        type_ = reader.getType();
        switch(type_)
        {
        case JsonType::OBJECT:
            {
                // content_ = Map {};
                // 对象类型，键值对存储
                const auto& _map = reader.getMap();
                Map& content = std::get<Map>(content_);
                // 遍历，生成子对象
                for(const auto& it : _map){
                    // 通过递归的方式生成子对象
                    content[it.first] = JsonBasic{ it.second };
                }
            }
            break;
        case JsonType::ARRAY:
            {
                content_ = List {};
                // 列表类型
                const auto& _list = reader.getList();
                List& content = std::get<List>(content_);
                // 遍历，生成子对象
                for(const auto& it : _list){
                    // 通过递归的方式生成子对象
                    content.push_back(JsonBasic{ it });
                }
            }
            break;
        default:
            // 非 对象/列表 类型，直接移动reader的string内容。
            content_ = std::move(reader.getString());
            break;
        }
    }

    // JsonObject拷贝构造函数
    JsonBasic::JsonBasic(const JsonBasic& jsonBasic) noexcept{
        type_ = jsonBasic.type_;
        content_ = jsonBasic.content_;
    }

    // JsonObject移动构造函数
    JsonBasic::JsonBasic(JsonBasic&& jsonBasic) noexcept{
        type_ = jsonBasic.type_;
        content_ = std::move(jsonBasic.content_);
        jsonBasic.type_ = JsonType::OBJECT;
        jsonBasic.content_ = Map {};
    }


    // JsonObject拷贝赋值函数
    JsonBasic& JsonBasic::operator=(const JsonBasic& jsonBasic) noexcept{
        if(this == &jsonBasic) return *this;
        type_ = jsonBasic.type_;
        content_ = jsonBasic.content_;
        return *this;
    }

    // JsonObject移动赋值函数
    JsonBasic& JsonBasic::operator=(JsonBasic&& jsonBasic) noexcept{
        if(this == &jsonBasic) return *this;
        type_ = jsonBasic.type_;
        content_ = std::move(jsonBasic.content_);
        jsonBasic.type_ = JsonType::OBJECT;
        jsonBasic.content_ = Map {};
        return *this;
    }

    // JsonObject字符串赋值
    JsonBasic& JsonBasic::operator=(const std::string& str){
        *this = stojson(str);
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

    // 清空内容
    void JsonBasic::clear()noexcept{
        type_ = JsonType::OBJECT;
        content_ = Map {};
    }

    // 数组 带越界检测的at
    JsonBasic& JsonBasic::at(const size_t& index){
        if(type_ != JsonType::ARRAY) throw JsonTypeException {};
        List& list = std::get<List>(content_);
        if(index < 0 || index >= list.size()) throw JsonIndexException {};
        return list[index];
    }

    // 字典 带检查的at
    JsonBasic& JsonBasic::at(const std::string& key){
        if(type_ != JsonType::OBJECT) throw JsonTypeException { };
        Map& map = std::get<Map>(content_);
        if(map.find(key) == map.end()) throw JsonIndexException {};
        return map[key];
    }

    // 不带检查的[]
    JsonBasic& JsonBasic::operator[](const size_t& index){
        if(type_ != JsonType::ARRAY) throw JsonTypeException {};
        List& list = std::get<List>(content_);
        return list[index];
    }
    // 不带检查的[]
    JsonBasic& JsonBasic::operator[](const std::string& key){
        if(type_ != JsonType::OBJECT) throw JsonTypeException { };
        Map& list = std::get<Map>(content_);
        return list[key];
    }

    // 检测长度
    size_t JsonBasic::size()const noexcept{
        if(type_ == JsonType::OBJECT) return std::get<Map>(content_).size();
        if(type_ == JsonType::ARRAY) return std::get<List>(content_).size();
        return std::get<std::string>(content_).size();
    }

    // 检查是否存在某个key
    bool JsonBasic::hasKey(const std::string& key) const{
        if(type_ != JsonType::OBJECT) throw JsonTypeException { };
        const Map& map = std::get<Map>(content_);
        return map.find(key) != map.end();
    }

    // 末尾插入元素
    void JsonBasic::push_back(const JsonBasic& jsonBasic){
        if(type_ != JsonType::ARRAY) throw JsonTypeException {};
        List& list = std::get<List>(content_);
        list.push_back(jsonBasic);
    }
    // 末尾插入元素，移动
    void JsonBasic::push_back(JsonBasic&& jsonBasic){
        if(type_ != JsonType::ARRAY) throw JsonTypeException {};
        List& list = std::get<List>(content_);
        list.push_back(std::move(jsonBasic));
    }

    // 指定位置插入元素
    void JsonBasic::insert(const size_t& index, const JsonBasic& jsonBasic){
        if(type_ != JsonType::ARRAY) throw JsonTypeException {};
        List& list = std::get<List>(content_);
        if(index > list.size()) throw JsonIndexException {};
        list.insert(list.begin()+index, jsonBasic);
    }
    // 指定位置插入元素，移动
    void JsonBasic::insert(const size_t& index, JsonBasic&& jsonBasic){
        if(type_ != JsonType::ARRAY) throw JsonTypeException {};
        List& list = std::get<List>(content_);
        if(index < 0 || index > list.size()) throw JsonIndexException {};
        list.insert(list.begin()+index, std::move(jsonBasic));
    }

    // 删除ARRAY中指定位置的元素
    void JsonBasic::erase(const size_t& index){
        if(type_ != JsonType::ARRAY) throw JsonTypeException {};
        List& list = std::get<List>(content_);
        if(index < 0 || index >= list.size()) throw JsonIndexException {};
        list.erase(list.begin()+index);
    }
    // 删除OBJECT中指定key的元素
    void JsonBasic::erase(const std::string& key){
        if(type_ != JsonType::OBJECT) throw JsonTypeException {};
        Map& map = std::get<Map>(content_);
        if(map.find(key) == map.end()) throw JsonIndexException {};
        map.erase(key);
    }

    long JsonBasic::as_int64() const {
        if(type_ == JsonType::NUMBER) throw JsonTypeException { };
        return std::stol(std::get<std::string>(content_));
    }
    double JsonBasic::as_double() const {
        if(type_ == JsonType::NUMBER) throw JsonTypeException { };
        return std::stod(std::get<std::string>(content_));
    }
    bool JsonBasic::as_bool() const {
        if(type_ == JsonType::BOOLEN) throw JsonTypeException { };
        return std::get<std::string>(content_).at(0) == 't' ? true : false;
    }
    bool JsonBasic::as_null() const noexcept {
        return type_ == JsonType::ISNULL;
    }
    std::string JsonBasic::as_string() const {
        if(type_ == JsonType::OBJECT || type_ == JsonType::ARRAY) throw JsonTypeException { };
        return std::get<std::string>(content_);
    }

    JsonBasic stojson(const std::string& str){
        std::string tmp;
        size_t len = str.size();
        tmp.reserve(len);
        size_t idx=0;
        // 去除空格，加快JsonBasic生成速度
        while(idx < len){
            while(idx<len && std::isspace(str[idx])) ++idx;
            if(idx >= len) break;
            if(str[idx] == '\"'){
                tmp += str[idx++];
                while(str.at(idx) != '\"'){
                    if(str[idx] == '\\'){
                        tmp += str[idx];
                        tmp += str.at(idx+1);
                        idx +=2 ;
                    }
                    else tmp += str[idx++];
                }
                tmp += str[idx++];
            }
            else{
                tmp += str[idx++];
            }
        }
        return JsonBasic { tmp };
    }
    JsonBasic parseJson(const std::string& str){
        return stojson(str);
    }

    JsonObject::JsonObject(const std::string& str): JsonBasic(str) {
        if(type_ != JsonType::OBJECT) throw JsonTypeException {};
    }
    JsonArray::JsonArray(const std::string& str): JsonBasic(str) {
        if(type_ != JsonType::ARRAY) throw JsonTypeException {};
    }
    JsonValue::JsonValue(const std::string& str): JsonBasic(str) {
        if(type_ == JsonType::ARRAY || type_ == JsonType::OBJECT) throw JsonTypeException {};
    }

    void JsonObject::clear() noexcept {
        type_ = JsonType::OBJECT;
        content_ = Map {};
    }
    void JsonArray::clear() noexcept {
        type_ = JsonType::ARRAY;
        content_ = List {};
    }
    void JsonValue::clear() noexcept {
        type_ = JsonType::ISNULL;
        content_ = std::string { "null" };
    }

}
