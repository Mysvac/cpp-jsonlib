#include <gtest/gtest.h>
#include "jsonlib.h"
#include <fstream>
#include <sstream>
#include <chrono>

void read_json(const std::string& json_path, std::string& res) { 
    std::ifstream ifs { json_path };
    // 其他解析逻辑...
    if (!ifs.is_open()) {
        throw std::runtime_error("Could not open file: " + json_path);
        return;
    }
    std::stringstream buffer;
    buffer << ifs.rdbuf();
    res = buffer.str();
    ifs.close();
}

void unserialize_json(const std::string& str, Json::JsonBasic& jsonBasic){
    auto start = std::chrono::system_clock::now();
    jsonBasic = str;
    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "unserialize time: " << duration.count() << " ms"  << std::endl;
}

void serialize_json(std::string& str,const Json::JsonBasic& jsonBasic){
    auto start = std::chrono::system_clock::now();
    str = jsonBasic.serialize();
    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "serialize time: " << duration.count() << " ms"  << std::endl;
}

void model_JsonAnalysis(const std::string& fileName) {
    std::string json;
    std::string json_plain;
    Json::JsonBasic jsonBasic;
    ASSERT_NO_THROW(read_json("files/" + fileName + "_plain.json", json));
    ASSERT_NO_THROW(unserialize_json(json, jsonBasic));
    ASSERT_NO_THROW(read_json("files/" + fileName + "_plain.json", json_plain));
    ASSERT_NO_THROW(serialize_json(json, jsonBasic));
    ASSERT_EQ(json, json_plain);
}


// 测试用例
TEST(JsonAnalysis, simple_1) {
    model_JsonAnalysis("simple_1");
}
// 测试用例
TEST(JsonAnalysis, medium_1) {
    model_JsonAnalysis("medium_1");
}

// 测试用例
TEST(JsonAnalysis, senior_1) {
    model_JsonAnalysis("senior_1");
}

// 测试用例
TEST(JsonAnalysis, senior_2) {
    model_JsonAnalysis("senior_2");
}



