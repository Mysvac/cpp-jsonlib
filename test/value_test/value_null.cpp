#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import mysvac.json;

using namespace mysvac;

// --- Construction tests ---
M_TEST(Value, Null) {
    // Default construction
    M_ASSERT_NO_THROW(Json null_default);
    M_ASSERT_EQ(Json{}.type(), json::Type::eNull);

    // Explicit null construction
    M_ASSERT_NO_THROW(Json null_jsonlicit{Json::Null{}});
    M_ASSERT_EQ(Json{Json::Null{}}.type(), json::Type::eNull);

    // nullptr construction
    M_ASSERT_NO_THROW(Json null_ptr{nullptr});
    M_ASSERT_EQ(Json{nullptr}.type(), json::Type::eNull);

    // Implicit construction
    Json implicit_null = nullptr;
    M_ASSERT_EQ(implicit_null.type(), json::Type::eNull);

    // --- Type checking ---
    Json null_val{nullptr};
    M_ASSERT_TRUE(null_val.is_nul());
    M_ASSERT_FALSE(null_val.is_str());
    M_EXPECT_STREQ(json::type_name(null_val.type()), "Null");

    // --- Reference access ---
    M_ASSERT_NO_THROW(std::ignore = null_val.nul());
    M_ASSERT_EQ(null_val.nul(), nullptr);

    // Const reference access
    const Json const_null{nullptr};
    M_ASSERT_EQ(const_null.nul(), nullptr);

    // Type safety: wrong type throws
    M_ASSERT_THROW(std::ignore = Json{"not null"}.nul(), std::bad_variant_access);
    M_ASSERT_THROW(std::ignore = Json{42.0}.nul(), std::bad_variant_access);

    // --- Assignment tests ---
    Json assign_val{ Json::Bool{} };
    assign_val = nullptr;
    M_ASSERT_EQ(assign_val.type(), json::Type::eNull);

    // --- Comparison tests ---
    Json null_cmp1{nullptr}, null_cmp2{nullptr}, null_cmp3{};
    Json bool_val{true}, num_val{42};
    M_ASSERT_TRUE(null_cmp1 == null_cmp2);
    M_ASSERT_TRUE(null_cmp1 == null_cmp3);
    M_ASSERT_FALSE(null_cmp1 == bool_val);
    M_ASSERT_FALSE(null_cmp1 == num_val);

    // Value-to-nullptr comparison
    M_ASSERT_TRUE(null_cmp1 == nullptr);
    M_ASSERT_FALSE(bool_val == nullptr);

    // Mixed type comparison
    M_ASSERT_FALSE(null_cmp1 == Json{"hello"});
    M_ASSERT_FALSE(null_cmp1 == Json{Json::Array{}});
    M_ASSERT_FALSE(null_cmp1 == Json{Json::Object{}});

    // --- Serialization tests ---
    M_ASSERT_EQ(Json{nullptr}.dump(), "null");
    M_ASSERT_EQ(Json{}.dump(), "null");
    M_ASSERT_EQ(Json{nullptr}.dumpf(), "null");

    // --- Parsing tests ---
    auto parsed_null = Json::parse("null");
    if (parsed_null.has_value()) {
        M_ASSERT_EQ(parsed_null->type(), json::Type::eNull);
        M_ASSERT_EQ(parsed_null->nul(), nullptr);
    } else {
        M_ASSERT_FAIL("Failed to parse 'null'");
    }

    // Round-trip serialization/parsing
    auto serialized_null = Json{nullptr}.dump();
    auto parsed_back_null = Json::parse(serialized_null);
    if (parsed_back_null.has_value()) {
        M_ASSERT_EQ(parsed_back_null->type(), json::Type::eNull);
        M_ASSERT_EQ(parsed_back_null->nul(), nullptr);
    } else {
        M_ASSERT_FAIL("Failed to parse back serialized null");
    }

    // --- Pointer conversion tests ---
    Json null_for_pointers{nullptr};
    M_ASSERT_EQ(null_for_pointers.to<int*>(), nullptr);
    M_ASSERT_EQ(null_for_pointers.to<char*>(), nullptr);
    M_ASSERT_EQ(null_for_pointers.to<void*>(), nullptr);
    M_ASSERT_EQ(null_for_pointers.to<double*>(), nullptr);

    // Non-null values cannot be converted to pointer types
    M_ASSERT_THROW(std::ignore = Json{42}.to<int*>(), std::runtime_error);

    // --- Type safety for conversions ---
    M_ASSERT_THROW(std::ignore = null_val.to<Json::String>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = null_val.to<Json::Number>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = null_val.to<Json::Bool>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = null_val.to<Json::Array>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = null_val.to<Json::Object>(), std::runtime_error);

    // Null to non-pointer types should throw
    M_ASSERT_THROW(std::ignore = null_val.to<int>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = null_val.to<float>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = null_val.to<double>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = null_val.to<bool>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = null_val.to<std::string>(), std::runtime_error);

    // --- Null consistency ---
    Json consistency_null1{}, consistency_null2{nullptr}, consistency_null3{Json::Null{}};
    M_ASSERT_TRUE(consistency_null1 == consistency_null2);
    M_ASSERT_TRUE(consistency_null2 == consistency_null3);
    M_ASSERT_TRUE(consistency_null1 == consistency_null3);
    M_ASSERT_TRUE(consistency_null1 == nullptr);

    // All null values serialize to the same string
    M_ASSERT_EQ(consistency_null1.dump(), "null");
    M_ASSERT_EQ(consistency_null2.dump(), "null");
    M_ASSERT_EQ(consistency_null3.dump(), "null");
}
