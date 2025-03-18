#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include "jsonlib.h"



int main(){

    try{
        Json::JsonBasic json1 = "[ {}} ]";
    }
    catch(const Json::JsonStructureException& e){
        std::cerr << "JsonStructureException: " << e.what() << std::endl;
    }
    catch(const Json::JsonException& e){
        std::cerr << "JsonException: " << e.what() << std::endl;
    }
    catch(...){ }
}
