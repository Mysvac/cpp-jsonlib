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

M_TEST(OS, medium_1) {
    std::ifstream ifs( CURRENT_PATH "/files/medium_1_plain.json" );
    M_EXPECT_TRUE( ifs.is_open() );
    Json value = Json::parse(ifs).value_or( nullptr );
    M_ASSERT_NE( value.type(), json::Type::eNul );
    std::string pretty_str = read_file("files/medium_1.json");
    Json pretty_value = Json::parse(pretty_str).value_or( nullptr );
    M_ASSERT_NE( pretty_value.type(), json::Type::eNul );
    M_ASSERT_EQ( value, pretty_value );
}

M_TEST(OS, simple_1) {
    std::ifstream ifs( CURRENT_PATH "/files/simple_1_plain.json" );
    M_EXPECT_TRUE( ifs.is_open() );
    Json value = Json::parse(ifs).value_or( nullptr );
    M_ASSERT_NE( value.type(), json::Type::eNul );
    std::string pretty_str = read_file("files/simple_1.json");
    Json pretty_value = Json::parse(pretty_str).value_or( nullptr );
    M_ASSERT_NE( pretty_value.type(), json::Type::eNul );
    M_ASSERT_EQ( value, pretty_value );
}

M_TEST(OS, simple_2) {
    std::ifstream ifs( CURRENT_PATH "/files/simple_2_plain.json" );
    M_EXPECT_TRUE( ifs.is_open() );
    Json value = Json::parse(ifs).value_or( nullptr );
    M_ASSERT_NE( value.type(), json::Type::eNul );
    std::string pretty_str = read_file("files/simple_2.json");
    Json pretty_value = Json::parse(pretty_str).value_or( nullptr );
    M_ASSERT_NE( pretty_value.type(), json::Type::eNul );
    M_ASSERT_EQ( value, pretty_value );
}

M_TEST(OS, simple_3) {
    std::ifstream ifs( CURRENT_PATH "/files/simple_3_plain.json" );
    M_EXPECT_TRUE( ifs.is_open() );
    Json value = Json::parse(ifs).value_or( nullptr );
    M_ASSERT_NE( value.type(), json::Type::eNul );
    std::string pretty_str = read_file("files/simple_3.json");
    Json pretty_value = Json::parse(pretty_str).value_or( nullptr );
    M_ASSERT_NE( pretty_value.type(), json::Type::eNul );
    M_ASSERT_EQ( value, pretty_value );
}

M_TEST(OS, many_number) {
    std::ifstream ifs( CURRENT_PATH "/files/many_number_plain.json" );
    M_EXPECT_TRUE( ifs.is_open() );
    Json value = Json::parse(ifs).value_or( nullptr );
    M_ASSERT_NE( value.type(), json::Type::eNul );
    std::string pretty_str = read_file("files/many_number.json");
    Json pretty_value = Json::parse(pretty_str).value_or( nullptr );
    M_ASSERT_NE( pretty_value.type(), json::Type::eNul );
    M_ASSERT_EQ( value, pretty_value );
}

M_TEST(OS, many_complex) {
    std::ifstream ifs( CURRENT_PATH "/files/many_complex_plain.json" );
    M_EXPECT_TRUE( ifs.is_open() );
    const Json value = Json::parse(ifs).value_or( nullptr );
    M_ASSERT_NE( value.type(), json::Type::eNul );
    const std::string pretty_str = read_file("files/many_complex.json");
    const Json pretty_value = Json::parse(pretty_str).value_or( nullptr );
    M_ASSERT_NE( pretty_value.type(), json::Type::eNul );
    M_ASSERT_EQ( value, pretty_value );
}


M_TEST(OS, ordered_medium_1) {
    std::ifstream ifs( CURRENT_PATH "/files/medium_1.json" );
    M_EXPECT_TRUE( ifs.is_open() );
    const Json value = Json::parse(ifs).value_or( nullptr );
    M_ASSERT_NE( value.type(), json::Type::eNul );
    const std::string plain = read_file("files/medium_1_plain.json");
    M_EXPECT_EQ(  value.dump(), plain );
}

M_TEST(OS, ordered_simple_1) {
    std::ifstream ifs( CURRENT_PATH "/files/simple_1.json" );
    M_EXPECT_TRUE( ifs.is_open() );
    const Json value = Json::parse(ifs).value_or( nullptr );
    M_ASSERT_NE( value.type(), json::Type::eNul );
    const std::string plain = read_file("files/simple_1_plain.json");
    M_EXPECT_EQ(  value.dump(), plain );
}

M_TEST(OS, ordered_simple_2) {
    std::ifstream ifs( CURRENT_PATH "/files/simple_2.json" );
    M_EXPECT_TRUE( ifs.is_open() );
    const Json value = Json::parse(ifs).value_or( nullptr );
    M_ASSERT_NE( value.type(), json::Type::eNul );
    const std::string plain = read_file("files/simple_2_plain.json");
    M_EXPECT_EQ(  value.dump(), plain );
}

M_TEST(OS, ordered_simple_3) {
    std::ifstream ifs( CURRENT_PATH "/files/simple_3.json" );
    M_EXPECT_TRUE( ifs.is_open() );
    const Json value = Json::parse(ifs).value_or( nullptr );
    M_ASSERT_NE( value.type(), json::Type::eNul );
    const std::string plain = read_file("files/simple_3_plain.json");
    M_EXPECT_EQ(  value.dump(), plain );
}

M_TEST(OS, ordered_many_number) {
    std::ifstream ifs( CURRENT_PATH "/files/many_number.json" );
    M_EXPECT_TRUE( ifs.is_open() );
    const Json value = Json::parse(ifs).value_or( nullptr );
    M_ASSERT_NE( value.type(), json::Type::eNul );
    const std::string plain = read_file("files/many_number_plain.json");
    M_EXPECT_EQ(  value.dump(), plain );
}

M_TEST(OS, ordered_many_complex) {
    std::ifstream ifs( CURRENT_PATH "/files/many_complex.json" );
    M_EXPECT_TRUE( ifs.is_open() );
    const Json value = Json::parse(ifs).value_or( nullptr );
    M_ASSERT_NE( value.type(), json::Type::eNul );
    const std::string plain = read_file("files/many_complex_plain.json");
    M_EXPECT_EQ(  value.dump(), plain );
}



