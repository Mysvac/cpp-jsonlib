#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import mysvac.json;


using namespace mysvac;

M_TEST(Value, Bol) {
    // --- Construction tests ---
    // Default bool construction
    M_ASSERT_NO_THROW(Json bool_default{json::Type::eBol});
    M_EXPECT_EQ(Json{json::Type::eBol}.to<Json::Bol>(), true);
    M_ASSERT_EQ(Json{json::Type::eBol}.type(), json::Type::eNum);
    M_ASSERT_EQ(Json{json::Type::eBol}.is_num(), true);
    M_ASSERT_EQ(Json{json::Type::eBol}.is_bol(), false);

    // Default bool construction
    M_ASSERT_NO_THROW(Json bool_default{Json::Bol{}});
    M_ASSERT_EQ(Json{Json::Bol{}}.to<Json::Bol>(), false);
    M_ASSERT_EQ(Json{Json::Bol{}}.type(), json::Type::eBol);
    M_ASSERT_EQ(Json{Json::Bol{}}.is_num(), false);
    M_ASSERT_EQ(Json{Json::Bol{}}.is_bol(), true);

    // Explicit construction with Json::Bol
    M_ASSERT_NO_THROW(Json bool_true{Json::Bol(true)});
    M_ASSERT_NO_THROW(Json bool_false{Json::Bol(false)});

    // Direct bool construction
    M_ASSERT_NO_THROW(Json direct_true{true});
    M_ASSERT_NO_THROW(Json direct_false{false});

    // Implicit construction
    Json implicit_true = true;
    Json implicit_false = false;
    M_ASSERT_EQ(implicit_true.type(), json::Type::eBol);
    M_ASSERT_EQ(implicit_false.type(), json::Type::eBol);
    M_ASSERT_EQ(implicit_true.to<Json::Bol>(), true);
    M_ASSERT_EQ(implicit_false.to<Json::Bol>(), false);

    // --- Type checking ---
    Json true_val{true};
    Json false_val{false};
    M_ASSERT_TRUE(true_val.is_bol());
    M_ASSERT_TRUE(false_val.is_bol());
    M_ASSERT_FALSE(true_val.is_str());
    M_ASSERT_FALSE(false_val.is_num());
    M_EXPECT_STREQ( json::type_name(true_val.type()), "Bol");
    M_EXPECT_STREQ( json::type_name(false_val.type()), "Bol");

    // --- Reference access ---
    M_ASSERT_EQ(true_val.bol(), true);
    M_ASSERT_EQ(false_val.bol(), false);
    const Json const_bool{true};
    M_ASSERT_EQ(const_bool.bol(), true);
    M_ASSERT_THROW(std::ignore = Json{Json::Str("not a bool")}.bol(), std::bad_variant_access);

    // --- Assignment tests ---
    Json assign_val{Json::Bol{}};
    assign_val = Json::Bol(true);
    M_ASSERT_EQ(assign_val.to<Json::Bol>(), true);
    assign_val = false;
    M_ASSERT_EQ(assign_val.to<Json::Bol>(), false);

    // Direct assignment
    Json direct_assign{};
    direct_assign = true;
    M_ASSERT_EQ(direct_assign.type(), json::Type::eBol);
    M_ASSERT_EQ(direct_assign.to<Json::Bol>(), true);
    direct_assign = false;
    M_ASSERT_EQ(direct_assign.to<Json::Bol>(), false);

    // Reference modification
    Json mod_val{true};
    auto& bool_ref = mod_val.bol();
    bool_ref = false;
    M_ASSERT_EQ(mod_val.to<Json::Bol>(), false);
    bool_ref = true;
    M_ASSERT_EQ(mod_val.to<Json::Bol>(), true);

    // --- Comparison tests ---
    Json val_true1{true}, val_true2{true};
    Json val_false1{false}, val_false2{false};
    Json val_string{Json::Str("true")};
    M_ASSERT_TRUE(val_true1 == val_true2);
    M_ASSERT_TRUE(val_false1 == val_false2);
    M_ASSERT_FALSE(val_true1 == val_false1);
    M_ASSERT_FALSE(val_true1 == val_string);

    // Value-to-bool comparison
    M_ASSERT_TRUE(Json{true} == true);
    M_ASSERT_TRUE(Json{false} == false);
    M_ASSERT_FALSE(Json{true} == false);

    // Mixed type comparison
    M_ASSERT_FALSE(true_val == Json{Json::Str("hello")});
    M_ASSERT_FALSE(true_val == Json{42.0});
    M_ASSERT_FALSE(true_val == Json{nullptr});
    M_ASSERT_TRUE(Json{nullptr} == nullptr);

    // --- Serialization tests ---
    M_ASSERT_EQ(Json{true}.dump(), "true");
    M_ASSERT_EQ(Json{false}.dump(), "false");
    M_ASSERT_EQ(Json{true}.dumpf(), "true");
    M_ASSERT_EQ(Json{false}.dumpf(), "false");

    // --- Parsing tests ---
    auto parsed_true = Json::parse("true");
    M_ASSERT_TRUE(parsed_true.has_value());
    M_ASSERT_EQ(parsed_true->type(), json::Type::eBol);
    M_ASSERT_EQ(parsed_true->bol(), true);

    auto parsed_false = Json::parse("false");
    M_ASSERT_TRUE(parsed_false.has_value());
    M_ASSERT_EQ(parsed_false->type(), json::Type::eBol);
    M_ASSERT_EQ(parsed_false->bol(), false);

    // --- Type safety for conversions ---
    M_ASSERT_THROW(std::ignore = true_val.to<Json::Str>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = true_val.to<Json::Array>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = false_val.to<Json::Object>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = false_val.to<std::nullptr_t>(), std::runtime_error);

    // --- Arithmetic conversions ---
    M_ASSERT_EQ(true_val.to<Json::Num>(), 1.0);
    M_ASSERT_EQ(false_val.to<Json::Num>(), 0.0);
    M_ASSERT_EQ(true_val.to<int>(), 1);
    M_ASSERT_EQ(false_val.to<int>(), 0);
    M_ASSERT_EQ(true_val.to<float>(), 1.0f);
    M_ASSERT_EQ(false_val.to<double>(), 0.0);

    // --- get<T> type safety ---
    M_ASSERT_THROW(std::ignore = true_val.num(), std::bad_variant_access);
    M_ASSERT_THROW(std::ignore = false_val.str(), std::bad_variant_access);
    M_ASSERT_THROW(std::ignore = true_val.arr(), std::bad_variant_access);
    M_ASSERT_THROW(std::ignore = false_val.obj(), std::bad_variant_access);
    M_ASSERT_THROW(std::ignore = true_val.nul(), std::bad_variant_access);

    // --- Round-trip serialization/parsing ---
    auto serialized_true = Json{true}.dump();
    auto parsed_back_true = Json::parse(serialized_true);
    M_ASSERT_TRUE(parsed_back_true.has_value());
    M_ASSERT_EQ(parsed_back_true->type(), json::Type::eBol);
    M_ASSERT_EQ(parsed_back_true->bol(), true);

    auto serialized_false = Json{false}.dump();
    auto parsed_back_false = Json::parse(serialized_false);
    M_ASSERT_TRUE(parsed_back_false.has_value());
    M_ASSERT_EQ(parsed_back_false->type(), json::Type::eBol);
    M_ASSERT_EQ(parsed_back_false->bol(), false);

    // --- boolean logic consistency ---
    M_ASSERT_TRUE(Json{true}.to<Json::Bol>());
    M_ASSERT_FALSE(Json{false}.to<Json::Bol>());
    M_ASSERT_TRUE(Json{true}.to<Json::Bol>() == true);
    M_ASSERT_TRUE(Json{false}.to<Json::Bol>() == false);
    M_ASSERT_FALSE(Json{true}.to<Json::Bol>() == false);
    M_ASSERT_FALSE(Json{false}.to<Json::Bol>() == true);
}
