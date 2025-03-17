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
        Json::JsonBasic json { R"__JSON__(
{
    "version": 5,
    "cmakeMinimumRequired": {
      "major": 3,
      "minor": 24,
      "patch": 0
    },
    "configurePresets": [
      {
        "name": "Ninja",
        "description": "Global config preset - base",
        "generator": "Ninja Multi-Config",
        "binaryDir": "${sourceDir}/build/",
        "installDir": "${sourceDir}/build/",
        "cacheVariables": {
            "CMAKE_TOOLCHAIN_FILE": "$env{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake",
            "CMAKE_CONFIGURATION_TYPES": "Debug;Release",
            "ENABLE_BUILD_TEST_EXE": "ON",
            "VCPKG_MANIFEST_FEATURES": "test",
            "VCPKG_MANIFEST_MODE": "ON",
            "CMAKE_CXX_COMPILER": "cl.exe"
          },
        "architecture": {
            "value": "x64",
            "strategy": "external"
        }
      }
    ],
    "buildPresets": [
        {
        "name": "build-debug",
        "displayName": "Build Debug",
        "configurePreset": "Ninja",
        "configuration": "Debug"
        }
    ]
}
  
            )__JSON__"};
        std::cout << json.serialize_pretty() << std::endl;
    }
    catch(const std::exception& e){
        std::cerr << e.what() << std::endl;
    }

}
