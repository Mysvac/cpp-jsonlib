#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import mysvac.json;


using namespace mysvac;

// Test the Type enum values
M_TEST(Type, enum) {
    M_ASSERT_NO_THROW( M_EXPECT_EQ( static_cast<int>(json::Type::eNull),   0 ) );
    M_ASSERT_NO_THROW( M_EXPECT_EQ( static_cast<int>(json::Type::eBool), 1 ) );
    M_ASSERT_NO_THROW( M_EXPECT_EQ( static_cast<int>(json::Type::eNumber),  2 ) );
    M_ASSERT_NO_THROW( M_EXPECT_EQ( static_cast<int>(json::Type::eString), 3 ) );
    M_ASSERT_NO_THROW( M_EXPECT_EQ( static_cast<int>(json::Type::eArray), 4 ) );
    M_ASSERT_NO_THROW( M_EXPECT_EQ( static_cast<int>(json::Type::eObject),   5 ) );

}

// Test the Object type
M_TEST(Type, Object) {

    M_ASSERT_TRUE( (std::is_same_v<Json::Object, std::map<Json::String, Json>>) );
    M_ASSERT_EQ( Json::Object{}, (std::map<Json::String, Json> {} ));
    M_ASSERT_EQ( Json::Object(), (std::map<Json::String, Json> {} ));
}

// Test the Number type
M_TEST(Type, Number) {
    M_ASSERT_TRUE( (std::is_same_v<Json::Number, double>) );
    M_ASSERT_EQ( 0.0 , Json::Number{} );
    M_ASSERT_EQ( 0.0 , Json::Number() );
}

// Test the String type
M_TEST(Type, Array) {
    M_ASSERT_TRUE( (std::is_same_v<Json::Array, std::vector<Json>>) );
    M_ASSERT_EQ( std::vector<Json>{} , Json::Array{} );
    M_ASSERT_EQ( std::vector<Json>{} , Json::Array() );
}

// Test the String type
M_TEST(Type, String) {
    M_ASSERT_TRUE( (std::is_same_v<Json::String, std::string>) );
    M_ASSERT_EQ( "" , Json::String{} );
    M_ASSERT_EQ( "" , Json::String() );
}

// Test the Bool type
M_TEST(Type, Bool) {
    M_ASSERT_TRUE( (std::is_same_v<Json::Bool, bool>) );
    M_ASSERT_EQ( false , Json::Bool{} );
    M_ASSERT_EQ( false , Json::Bool() );
}

// Test the Null type
M_TEST(Type, Null) {
    M_ASSERT_TRUE( (std::is_same_v<Json::Null, std::nullptr_t>) );
    M_ASSERT_EQ( nullptr , Json::Null{} );
    M_ASSERT_EQ( nullptr , Json::Null() );
}

// Test the json_types concept
M_TEST(Type, concept) {
    M_ASSERT_TRUE( (requires { requires json::json_type<Json, Json::Object>; } ));
    M_ASSERT_TRUE( (requires { requires json::json_type<Json, Json::Array>; }  ));
    M_ASSERT_TRUE( (requires { requires json::json_type<Json, Json::Number>; } ));
    M_ASSERT_TRUE( (requires { requires json::json_type<Json, Json::String>; } ));
    M_ASSERT_TRUE( (requires { requires json::json_type<Json, Json::Bool>; }   ));
    M_ASSERT_TRUE( (requires { requires json::json_type<Json, Json::Null>; }   ));
}

M_TEST(Type, value) {
    M_ASSERT_NO_THROW( std::ignore = Json{} );
}

