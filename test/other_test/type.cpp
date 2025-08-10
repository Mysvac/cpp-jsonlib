#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import mysvac.json;


using namespace mysvac;

// Test the Type enum values
M_TEST(Type, enum) {
    M_ASSERT_NO_THROW( M_EXPECT_EQ( static_cast<int>(json::Type::eNul),   0 ) );
    M_ASSERT_NO_THROW( M_EXPECT_EQ( static_cast<int>(json::Type::eBol), 1 ) );
    M_ASSERT_NO_THROW( M_EXPECT_EQ( static_cast<int>(json::Type::eNum),  2 ) );
    M_ASSERT_NO_THROW( M_EXPECT_EQ( static_cast<int>(json::Type::eStr), 3 ) );
    M_ASSERT_NO_THROW( M_EXPECT_EQ( static_cast<int>(json::Type::eArray), 4 ) );
    M_ASSERT_NO_THROW( M_EXPECT_EQ( static_cast<int>(json::Type::eObject),   5 ) );

}

// Test the Object type
M_TEST(Type, Object) {

    M_ASSERT_TRUE( (std::is_same_v<Json::Object, std::map<Json::Str, Json>>) );
    M_ASSERT_EQ( Json::Object{}, (std::map<Json::Str, Json> {} ));
    M_ASSERT_EQ( Json::Object(), (std::map<Json::Str, Json> {} ));
}

// Test the Num type
M_TEST(Type, Num) {
    M_ASSERT_TRUE( (std::is_same_v<Json::Num, double>) );
    M_ASSERT_EQ( 0.0 , Json::Num{} );
    M_ASSERT_EQ( 0.0 , Json::Num() );
}

// Test the Str type
M_TEST(Type, Array) {
    M_ASSERT_TRUE( (std::is_same_v<Json::Array, std::vector<Json>>) );
    M_ASSERT_EQ( std::vector<Json>{} , Json::Array{} );
    M_ASSERT_EQ( std::vector<Json>{} , Json::Array() );
}

// Test the Str type
M_TEST(Type, Str) {
    M_ASSERT_TRUE( (std::is_same_v<Json::Str, std::string>) );
    M_ASSERT_EQ( "" , Json::Str{} );
    M_ASSERT_EQ( "" , Json::Str() );
}

// Test the Bol type
M_TEST(Type, Bol) {
    M_ASSERT_TRUE( (std::is_same_v<Json::Bol, bool>) );
    M_ASSERT_EQ( false , Json::Bol{} );
    M_ASSERT_EQ( false , Json::Bol() );
}

// Test the Nul type
M_TEST(Type, Nul) {
    M_ASSERT_TRUE( (std::is_same_v<Json::Nul, std::nullptr_t>) );
    M_ASSERT_EQ( nullptr , Json::Nul{} );
    M_ASSERT_EQ( nullptr , Json::Nul() );
}

// Test the json_types concept
M_TEST(Type, concept) {
    M_ASSERT_TRUE( (requires { requires json::json_type<Json, Json::Object>; } ));
    M_ASSERT_TRUE( (requires { requires json::json_type<Json, Json::Array>; }  ));
    M_ASSERT_TRUE( (requires { requires json::json_type<Json, Json::Num>; } ));
    M_ASSERT_TRUE( (requires { requires json::json_type<Json, Json::Str>; } ));
    M_ASSERT_TRUE( (requires { requires json::json_type<Json, Json::Bol>; }   ));
    M_ASSERT_TRUE( (requires { requires json::json_type<Json, Json::Nul>; }   ));
}

M_TEST(Type, value) {
    M_ASSERT_NO_THROW( std::ignore = Json{} );
}

