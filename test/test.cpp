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
        Jsonlib::JsonValue value{"1231415"};
        std::cout << value.serialize() << std::endl;
    }
    catch(const Jsonlib::JsonStructureException& e){
        std::cerr << "JsonStructureException: " << e.what() << std::endl;
    }
    catch(const Jsonlib::JsonException& e){
        std::cerr << "JsonException: " << e.what() << std::endl;
    }
    catch(...){ }
}
