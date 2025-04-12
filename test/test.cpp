#include <iostream>
#include <fstream>
#include <chrono>
#include "jsonlib.h"

#define Field(name) result[#name] = name;
#define Serializable(...) \
    operator Jsonlib::JsonValue(){ \
        Jsonlib::JsonValue result(Jsonlib::JsonType::OBJECT);    \
        __VA_ARGS__    \
        return result;    \
    }


using namespace Jsonlib;

class A{
public:
    std::string name;
    int value;
    bool check;
    // 自行判断是否添加 explicit
    Serializable(
        Field(name)
        Field(value)
        Field(check)
    );
};

int main(){

    try{
        A a {"XX", 1, true};
        std::cout << JsonValue(a).serialize() << std::endl;

        JsonValue json1 = {
            { "key", "value" },
            { 1, "value" },
            true,
            false,
            nullptr,
            0,
            114.514
        };
        std::cout << json1.serialize() << std::endl;
    }
    catch(const Jsonlib::JsonStructureException& e){
        std::cerr << "JsonStructureException: " << e.what() << std::endl;
    }
    catch(const Jsonlib::JsonException& e){
        std::cerr << "JsonException: " << e.what() << std::endl;
    }
    catch(...){ std::cerr << "other" << std::endl; }
}
