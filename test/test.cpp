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
        Json::JsonBasic json { R"__JSON__([

    "JSON Test Pattern pass1",

    {"object with 1 member":["array with 1 element"]},

    {},

    [],

    -42,

    true,

    false,

    null,

    {

        "integer": 1234567890,

        "real": -9876.543210,

        "e": 0.123456789e-12,

        "E": 1.234567890E+34,

        "":  23456789012E66,

        "zero": 0,

        "one": 1,

        "space": " ",

        "quote": "\"",

        "backslash": "\\",

        "controls": "\b\f\n\r\t",

        "slash": "/ & \/",

        "alpha": "abcdefghijklmnopqrstuvwyz",

        "ALPHA": "ABCDEFGHIJKLMNOPQRSTUVWYZ",

        "digit": "0123456789",

        "0123456789": "digit",

        "special": "`1~!@#$%^&*()_+-={':[,]}|;.</>?",

        "hex": "\u0123\u4567\u89AB\uCDEF\uabcd\uef4A",

        "true": true,

        "false": false,

        "null": null,

        "array":[  ],

        "object":{  },

        "address": "50 St. James Street",

        "url": "http://www.JSON.org/",

        "comment": "// /* <!-- --",

        "# -- --> */": " ",

        " s p a c e d " :[1,2 , 3



,



4 , 5        ,          6           ,7        ],"compact":[1,2,3,4,5,6,7],

        "jsontext": "{\"object with 1 member\":[\"array with 1 element\"]}",

        "quotes": "&#34; \u0022 %22 0x22 034 &#x22;",

        "\/\\\"\uCAFE\uBABE\uAB98\uFCDE\ubcda\uef4A\b\f\n\r\t`1~!@#$%^&*()_+-=[]{}|;:',./<>?"

: "A key can be any string"

    },

    0.5 ,98.6

,

99.44

,



1066,

1e1,

0.1e1,

1e-1,

1e00,2e+00,2e-00

,"rosebud"])__JSON__" };
        std::cout << json.serialize_pretty() << std::endl;
    }
    catch(const std::exception& e){
        std::cerr << e.what() << std::endl;
    }

}
