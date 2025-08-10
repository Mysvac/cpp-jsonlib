#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import mysvac.json;


using namespace mysvac;


M_TEST(Value, type_constructors) {
    M_ASSERT_NO_THROW( Json value );
    M_ASSERT_NO_THROW( std::ignore = Json () );
    M_ASSERT_NO_THROW( std::ignore = Json {} );
    M_ASSERT_NO_THROW( std::ignore = Json (json::Type::eObject) );
    M_ASSERT_NO_THROW( std::ignore = Json (json::Type::eArray)  );
    M_ASSERT_NO_THROW( std::ignore = Json (json::Type::eString) );
    M_ASSERT_NO_THROW( std::ignore = Json (json::Type::eNumber) );
    M_ASSERT_NO_THROW( std::ignore = Json (json::Type::eBool)   );
    M_ASSERT_NO_THROW( std::ignore = Json (json::Type::eNull)   );
    M_ASSERT_NO_THROW( std::ignore = Json (Json::Object()) );
    M_ASSERT_NO_THROW( std::ignore = Json (Json::Array())  );
    M_ASSERT_NO_THROW( std::ignore = Json (Json::String()) );
    M_ASSERT_NO_THROW( std::ignore = Json (Json::Number()) );
    M_ASSERT_NO_THROW( std::ignore = Json (Json::Bool())   );
    M_ASSERT_NO_THROW( std::ignore = Json (Json::Null())   );

    M_ASSERT_NO_THROW( std::ignore = Json().is_nul() );
    M_ASSERT_NO_THROW( std::ignore = Json().is_obj() );
    M_ASSERT_NO_THROW( std::ignore = Json().is_arr() );
    M_ASSERT_NO_THROW( std::ignore = Json().is_str() );
    M_ASSERT_NO_THROW( std::ignore = Json().is_num() );
    M_ASSERT_NO_THROW( std::ignore = Json().is_bol() );

    M_ASSERT_EQ(Json().type(), json::Type::eNull);
    M_ASSERT_EQ(Json(json::Type::eObject).type(), json::Type::eNumber);
    M_ASSERT_EQ(Json(json::Type::eArray ).type(), json::Type::eNumber);
    M_ASSERT_EQ(Json(json::Type::eString).type(), json::Type::eNumber);
    M_ASSERT_EQ(Json(json::Type::eNumber).type(), json::Type::eNumber);
    M_ASSERT_EQ(Json(json::Type::eBool  ).type(), json::Type::eNumber);
    M_ASSERT_EQ(Json(json::Type::eNull  ).type(), json::Type::eNumber);

    M_EXPECT_STREQ( json::type_name(Json{ Json::Object{} }.type()), "Object");
    M_EXPECT_STREQ( json::type_name(Json{ Json::Array{}  }.type()), "Array");
    M_EXPECT_STREQ( json::type_name(Json{ Json::String{} }.type()), "String");
    M_EXPECT_STREQ( json::type_name(Json{ Json::Number{} }.type()), "Number");
    M_EXPECT_STREQ( json::type_name(Json{ Json::Bool{}   }.type()), "Bool");
    M_EXPECT_STREQ( json::type_name(Json{ Json::Null{}   }.type()), "Null");

    M_ASSERT_TRUE(Json().is_nul());
    M_ASSERT_FALSE(Json().is_obj());
    M_ASSERT_FALSE(Json().is_arr());
    M_ASSERT_FALSE(Json().is_str());
    M_ASSERT_FALSE(Json().is_num());
    M_ASSERT_FALSE(Json().is_bol());

    M_ASSERT_TRUE( Json(Json::Object{}).is_obj());
    M_ASSERT_FALSE(Json(Json::Object{}).is_nul());
    M_ASSERT_FALSE(Json(Json::Object{}).is_arr());
    M_ASSERT_FALSE(Json(Json::Object{}).is_str());
    M_ASSERT_FALSE(Json(Json::Object{}).is_num());
    M_ASSERT_FALSE(Json(Json::Object{}).is_bol());

    M_ASSERT_FALSE(Json(Json::Array{}).is_nul());
    M_ASSERT_FALSE(Json(Json::Array{}).is_obj());
    M_ASSERT_TRUE( Json(Json::Array{}).is_arr());
    M_ASSERT_FALSE(Json(Json::Array{}).is_str());
    M_ASSERT_FALSE(Json(Json::Array{}).is_num());
    M_ASSERT_FALSE(Json(Json::Array{}).is_bol());

    M_ASSERT_FALSE(Json(Json::String{}).is_nul());
    M_ASSERT_FALSE(Json(Json::String{}).is_obj());
    M_ASSERT_FALSE(Json(Json::String{}).is_arr());
    M_ASSERT_TRUE( Json(Json::String{}).is_str());
    M_ASSERT_FALSE(Json(Json::String{}).is_num());
    M_ASSERT_FALSE(Json(Json::String{}).is_bol());


    M_ASSERT_FALSE(Json(Json::Number{}).is_nul());
    M_ASSERT_FALSE(Json(Json::Number{}).is_obj());
    M_ASSERT_FALSE(Json(Json::Number{}).is_arr());
    M_ASSERT_FALSE(Json(Json::Number{}).is_str());
    M_ASSERT_TRUE( Json(Json::Number{}).is_num());
    M_ASSERT_FALSE(Json(Json::Number{}).is_bol());

    M_ASSERT_FALSE(Json(Json::Bool{}).is_nul());
    M_ASSERT_FALSE(Json(Json::Bool{}).is_obj());
    M_ASSERT_FALSE(Json(Json::Bool{}).is_arr());
    M_ASSERT_FALSE(Json(Json::Bool{}).is_str());
    M_ASSERT_FALSE(Json(Json::Bool{}).is_num());
    M_ASSERT_TRUE( Json(Json::Bool{}).is_bol());
}
