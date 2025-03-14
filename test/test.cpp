#include <iostream>
#include "jsonlib.h"

int main(){
    try{
        Json::JsonBasic json1 { "[1, null, {}, true , {\"key\" : \"值\" }]" };

        Json::JsonBasic json2 = Json::stojson(R"__JSON__(
        {
            "method": ["C++", "原始字符串", "语法" , 114514],
            "name": "cpp-\t-jsonlib",
            "version": "0.1.0",
            "description": "A simple JSON library using C++17.",
            "homepage": "https://github.com/Mysvac/cpp-jsonlib"
        }
        )__JSON__"
        );
        // JsonBasic json2 = stojson

        json1.erase(4);
        json1.push_back( Json::stojson("666") );
        json2["method"][0] = R"("Python")";
        json2.erase("description");
        json2["homepage"].clear();

        std::cout << json1.serialize() << std::endl;
        std::cout << json2["name"].as_string() << std::endl;
        std::cout << json2.serialize() << std::endl;
        // std::cout << json2.serialize() << std::endl;
    }
    catch(const std::exception& e){
        std::cerr << e.what() << std::endl;
    }
}
