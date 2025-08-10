#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import mysvac.json;


using namespace mysvac;

M_TEST(File, Serial_Unicode) {
    std::ifstream file( CURRENT_PATH "/files/all_unicode.json" );
    const auto json = Json::parse(file).value_or( nullptr );
    M_EXPECT_FALSE( json.is_nul() );

    const auto bein = std::chrono::system_clock::now();

    std::string res = json.dump();


    const auto end = std::chrono::system_clock::now();
    const auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - bein).count();
    std::println("-----------------------------------------------Serial_Unicode: {} us", time);

    M_ASSERT_NE( json.type(), json::Type::eNull );
}

M_TEST(File, Serial_Number) {
    std::ifstream file( CURRENT_PATH "/files/many_number.json" );
    const auto json = Json::parse(file).value_or( nullptr );
    M_EXPECT_FALSE( json.is_nul() );

    const auto bein = std::chrono::system_clock::now();

    std::string res = json.dump();

    const auto end = std::chrono::system_clock::now();
    const auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - bein).count();
    std::println("-----------------------------------------------Serial_Number: {} us", time);

    M_ASSERT_NE( json.type(), json::Type::eNull );
}


M_TEST(File, Serial_Complex) {
    std::ifstream file( CURRENT_PATH "/files/many_complex.json" );
    const auto json = Json::parse(file).value_or( nullptr );
    M_EXPECT_FALSE( json.is_nul() );

    const auto bein = std::chrono::system_clock::now();

    std::string res = json.dump();

    const auto end = std::chrono::system_clock::now();
    const auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - bein).count();
    std::println("-----------------------------------------------Serial_Complex: {} us", time);

    M_ASSERT_NE( json.type(), json::Type::eNull );
}

M_TEST(File, Serial_All) {
    std::ifstream file( CURRENT_PATH "/files/many_all.json" );
    const auto json = Json::parse(file).value_or( nullptr );
    M_EXPECT_FALSE( json.is_nul() );

    const auto bein = std::chrono::system_clock::now();

    std::string res = json.dump();

    const auto end = std::chrono::system_clock::now();
    const auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - bein).count();
    std::println("-----------------------------------------------Serial_All: {} us", time);

    M_ASSERT_NE( json.type(), json::Type::eNull );
}

// M_TEST(File, Serial_String) {
//     std::ifstream file( CURRENT_PATH "/files/many_string.json" );
//     const auto json = Json::parse(file, 1024).value_or( nullptr );
//     M_EXPECT_FALSE( json.is_nul() );
//
//     const auto bein = std::chrono::system_clock::now();
//
//     std::string res = json.dump();
//
//     const auto end = std::chrono::system_clock::now();
//     const auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - bein).count();
//     std::println("-----------------------------------------------Serial_String: {} us", time);
//
//     M_ASSERT_NE( json.type(), json::Type::eNull );
// }



