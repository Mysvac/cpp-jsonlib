#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <deque>
#include <list>
#include <chrono>
#include "jsonlib.h"



int main(){

    try{
        // Jsonlib::JsonValue json = Jsonlib::deserialize("[ {}} ]");
        Jsonlib::JsonValue json1 = Jsonlib::JsonArray{
            Jsonlib::JsonObject { {"key", nullptr} },
            Jsonlib::JsonArray{ 1, 2, 3, 4, 5 },
            "string",
            true,
            false,
            1234.5
        };
        std::cout << json1[0].serialize() << std::endl;
        std::cout << json1[0].is_object() << std::endl;
        std::cout << json1[0]["key"].is_string() << std::endl;
        std::cout << json1[0]["key"].is_null() << std::endl;
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
