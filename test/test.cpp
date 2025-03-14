#include "jsonlib.h"
#include <iostream>
using namespace Json;

int main(){
    JsonBasic json {R"_JSON_(
        {
            "numbers": [
                23,
                true,
                5,
                33,
                false,
                78
            ],
            "checked": true,
            "id": 38934.1235,
            "object": {
                "t": "json校验器",
                "w": "json检查",
                "中文": {
                    "test": null
                }
            },
            "host": "json-online.com"
        }
    )_JSON_"};

    // JsonBasic json2 {R"_JSON_(
    //     {
    //     "123" : 1234
    //     }
    // )_JSON_"};
    // json["test"] = "{}";
    
    json["test"] = "[ 12340 ]";

    // json = "{}";

    std::cout << json.serialize() << std::endl << std::endl;

    json.erase("test");

    json["numbers"][0] = "21";

    std::cout << json.serialize() << std::endl;

    std::cout << std::boolalpha;
    std::cout << json["numbers"][0].is_number() << std::endl;
    std::cout << json["checked"].is_bool() << std::endl;
}
