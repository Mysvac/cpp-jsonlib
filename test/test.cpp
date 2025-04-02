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
        Jsonlib::JsonValue json = Jsonlib::deserialize("[ {}} ]");
    }
    catch(const Jsonlib::JsonStructureException& e){
        std::cerr << "JsonStructureException: " << e.what() << std::endl;
    }
    catch(const Jsonlib::JsonException& e){
        std::cerr << "JsonException: " << e.what() << std::endl;
    }
    catch(...){ std::cerr << "other" << std::endl; }
}
