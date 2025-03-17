#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include "jsonlib.h"



int main(){

    std::map<std::string, std::string> mp;
    try{
        Json::JsonBasic json { R"__JSON__(
            {
                "语法": ["C++", "原始字符串", false ],
                "key": "支持\t中文\\与\"转义字符",
                "na\"\\me": [ 114,514 , null ],
                "map": [ {} , [ [ "嵌套" ] , {} ] ]
            }
            )__JSON__"};
            
            json.erase("na\"\\me"); // 删除
            json["map"][1].clear(); // 清空
            json["语法"] = 114514; // 修改
            json["add"] = "[[[]]]"; //增加
            json["add"].push_back(Json::JsonBasic { 1 }); // 此处不完善，只能push对象
            // 支持移动语义
            
            std::cout << json.serialize() << std::endl;
            std::cout << json["key"].as_string() << std::endl; // 获取字符串并转义

    }
    catch(const std::exception& e){
        std::cerr << e.what() << std::endl;
    }

}
