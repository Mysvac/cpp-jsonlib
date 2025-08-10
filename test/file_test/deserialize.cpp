#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import mysvac.json;


using namespace mysvac;

static std::string read_file(const std::string& path) {
    std::ifstream file( CURRENT_PATH "/" + path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    return { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
}

M_TEST(File, Deser_Unicode) {
    const std::string pretty_str = read_file("files/all_unicode.json");

    const auto bein = std::chrono::system_clock::now();

    const auto json = Json::parse(pretty_str).value_or( nullptr );

    const auto end = std::chrono::system_clock::now();
    const auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - bein).count();
    std::println("-----------------------------------------------Deser_Unicode: {} us", time);

    M_ASSERT_NE( json.type(), json::Type::eNul );
}

M_TEST(File, Deser_Num) {
    const std::string pretty_str = read_file("files/many_number.json");

    const auto bein = std::chrono::system_clock::now();

    const auto json = Json::parse(pretty_str).value_or( nullptr );

    const auto end = std::chrono::system_clock::now();
    const auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - bein).count();
    std::println("-----------------------------------------------Deser_Num: {} us", time);

    M_ASSERT_NE( json.type(), json::Type::eNul );
}

M_TEST(File, Deser_Complex) {
    const std::string pretty_str = read_file("files/many_complex.json");

    const auto bein = std::chrono::system_clock::now();

    const auto json = Json::parse(pretty_str).value_or( nullptr );

    const auto end = std::chrono::system_clock::now();
    const auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - bein).count();
    std::println("-----------------------------------------------Deser_Complex: {} us", time);

    M_ASSERT_NE( json.type(), json::Type::eNul );
}

M_TEST(File, Deser_All) {
    const std::string pretty_str = read_file("files/many_all.json");

    const auto bein = std::chrono::system_clock::now();

    const auto json = Json::parse(pretty_str, 1024).value_or( nullptr );

    const auto end = std::chrono::system_clock::now();
    const auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - bein).count();
    std::println("-----------------------------------------------Deser_All: {} us", time);

    M_ASSERT_NE( json.type(), json::Type::eNul );
}

M_TEST(File, Deser_Str) {
    const std::string pretty_str = read_file("files/many_string.json");

    const auto bein = std::chrono::system_clock::now();


    const auto json = Json::parse(pretty_str).value_or( nullptr );

    const auto end = std::chrono::system_clock::now();
    const auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - bein).count();
    std::println("-----------------------------------------------Deser_Str: {} us", time);

    M_ASSERT_NE( json.type(), json::Type::eNul );
}

