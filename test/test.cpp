#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include "jsonlib.h"


// void read_json(const std::string& json_path, std::string& res) {
//     std::ifstream ifs { json_path , std::ios::in | std::ios::binary };
//     // 其他解析逻辑...
//     if (!ifs.is_open()) {
//         throw std::runtime_error("Could not open file: " + json_path);
//         return;
//     }
//     std::stringstream buffer;
//     buffer << ifs.rdbuf();
//     res = buffer.str();
//     ifs.close();
// }

// void write_json(const std::string& json_path,const std::string& res) {
//     std::ofstream ofs { json_path , std::ios::out | std::ios::trunc  };
//     // 其他解析逻辑...
//     if (!ofs.is_open()) {
//         throw std::runtime_error("Could not open file: " + json_path);
//         return;
//     }
//     ofs << res;
//     ofs.close();
// }

// void unserialize_json(const std::string& str, Json::JsonBasic& jsonBasic){
//     jsonBasic = str;
// }

// void serialize_json(std::string& str,const Json::JsonBasic& jsonBasic){
//     str = jsonBasic.serialize();
// }

    // 禁止输入空字符串
static bool is_number(const std::string& str) noexcept{
    size_t it=0;
    size_t len = str.size();
    if(str[0] == '-' || str[0] == '+'){
        if(len == 1 && !std::isdigit(str[1])) return false;
        ++it;
    }
    bool have_point = false;
    for(;it<len;++it){
        if(std::isdigit(str[it])) continue;
        else if(str[it] == '.'){
            if(have_point) return false;
            have_point = true;
        }
        else return false;
    }
    return true;
}


int main(){
    try{
        Json::JsonBasic json { R"({"123":"123","321":["321"]})"};
        const auto& map = json.getMapConst();
        for(const auto& it : map){
            std::cout << it.first << ' ' << it.second.serialize() << std::endl;
        }
    }
    catch(const std::exception& e){
        std::cerr << e.what() << std::endl;
    }

}
