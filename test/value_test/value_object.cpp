#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import mysvac.json;


using namespace mysvac;

// --- Object Construction and Type Checks ---
M_TEST(Value, Object) {
    // Default and jsonlicit construction
    M_ASSERT_NO_THROW(Json obj_default{Json::Object{}});
    M_ASSERT_EQ(Json{Json::Object{}}.type(), json::Type::eObject);
    M_ASSERT_EQ(Json{Json::Object{}}.obj().size(), 0);
    M_ASSERT_TRUE(Json{Json::Object{}}.to<Json::Object>().empty());

    M_ASSERT_NO_THROW(Json obj_jsonlicit{Json::Object{}});
    M_ASSERT_NO_THROW(Json obj_init{Json::Object{{"key1", 1}, {"key2", 2}}});

    // Direct construction and mixed types
    Json obj_direct{Json::Object{{"name", "John"}, {"age", 30}, {"active", true}}};
    M_ASSERT_EQ(obj_direct.obj().size(), 3);

    Json mixed_obj{Json::Object{
        {"number", Json::Number(42)},
        {"string", "hello"},
        {"boolean", true},
        {"null_value", nullptr}
    }};
    M_ASSERT_EQ(mixed_obj.obj().size(), 4);

    // Type checking
    Json obj_val{Json::Object{{"a", 1}, {"b", 2}, {"c", 3}}};
    Json empty_obj{Json::Object{}};
    M_ASSERT_TRUE(obj_val.is_obj());
    M_ASSERT_FALSE(obj_val.is_str());
    M_EXPECT_STREQ(json::type_name(obj_val.type()), "Object");

    // --- Size and Empty Checks ---
    Json size_test{Json::Object{{"a", 1}, {"b", 2}, {"c", 3}, {"d", 4}, {"e", 5}}};
    M_ASSERT_EQ(size_test.obj().size(), 5);
    Json empty_size_test{Json::Object{}};
    M_ASSERT_EQ(empty_size_test.obj().size(), 0);
    M_ASSERT_TRUE(empty_size_test.to<Json::Object>().empty());

    // --- Key Access and Modification ---
    Json subscript_test{Json::Object{{"first", 10}, {"second", 20}, {"third", 30}}};
    M_ASSERT_EQ(subscript_test["first"].to<Json::Number>(), 10);

    Json mixed_subscript{Json::Object{
        {"number", 42},
        {"text", "test"},
        {"flag", true},
        {"empty", nullptr}
    }};
    M_ASSERT_EQ(mixed_subscript["text"].to<Json::String>(), "test");

    // Modification via subscript
    Json modify_test{Json::Object{{"a", 1}, {"b", 2}, {"c", 3}}};
    modify_test["a"] = 100;
    modify_test["b"] = "modified";
    modify_test["c"] = false;
    M_ASSERT_EQ(modify_test["b"].to<Json::String>(), "modified");

    // Add new key
    Json new_key_test{Json::Object{{"existing", 1}}};
    new_key_test["new_key"] = "new_value";
    M_ASSERT_EQ(new_key_test.obj().size(), 2);

    // --- Const Key Access ---
    const Json const_obj{Json::Object{{"x", 1}, {"y", 2}, {"z", 3}}};
    M_ASSERT_EQ(const_obj["y"].to<Json::Number>(), 2);

    // --- Safe Access with at() ---
    Json at_test{Json::Object{{"alpha", 100}, {"beta", 200}, {"gamma", 300}}};
    M_ASSERT_NO_THROW(std::ignore = at_test.at("alpha"));
    M_ASSERT_EQ(at_test.at("gamma").to<Json::Number>(), 300);
    M_ASSERT_THROW(std::ignore = at_test.at("nonexistent"), std::out_of_range);

    // --- Reference Access and Modification ---
    Json ref_test{Json::Object{{"data1", 10}, {"data2", 20}}};
    auto& obj_ref = ref_test.obj();
    obj_ref["data1"] = 100;
    obj_ref["new_data"] = "added";
    M_ASSERT_EQ(ref_test.obj().size(), 3);

    // --- Assignment Operations ---
    Json assign_val{Json::Object{}};
    assign_val = Json::Object{{"key1", 1}, {"key2", 2}};
    M_ASSERT_EQ(assign_val.obj().size(), 2);

    // --- Nested Object Access ---
    Json nested_obj{Json::Object{
        {"person", Json::Object{{"name", "John"}, {"age", 30}, {"active", true}}},
        {"company", Json::Object{{"name", "TechCorp"}, {"employees", 100}, {"public", false}}}
    }};
    M_ASSERT_EQ(nested_obj["person"]["name"].to<Json::String>(), "John");
    M_ASSERT_EQ(nested_obj["company"]["employees"].to<Json::Number>(), 100);

    // --- Deeply Nested Object Access ---
    Json deep_nested{Json::Object{
        {"level1", Json::Object{
            {"level2", Json::Object{
                {"level3", Json::Object{
                    {"data1", "deep_value1"},
                    {"data2", "deep_value2"}
                }}
            }}
        }}
    }};
    M_ASSERT_EQ(deep_nested["level1"]["level2"]["level3"]["data2"].to<Json::String>(), "deep_value2");

    // --- Value-to-Value Comparison ---
    Json obj_cmp1{Json::Object{{"a", 1}, {"b", 2}}};
    Json obj_cmp2{Json::Object{{"a", 1}, {"b", 2}}};
    Json obj_cmp3{Json::Object{{"a", 1}, {"b", 3}}};
    M_ASSERT_TRUE(obj_cmp1 == obj_cmp2);
    M_ASSERT_FALSE(obj_cmp1 == obj_cmp3);

    // --- Serialization and Parsing ---
    Json serial_test{Json::Object{{"key1", 1}, {"key2", 2}}};
    M_ASSERT_NO_THROW(std::ignore = serial_test.dump());
    M_ASSERT_NO_THROW(std::ignore = serial_test.dumpf());

    auto parsed_simple = Json::parse(R"({"key1":1,"key2":2})");
    if (parsed_simple.has_value()) {
        M_ASSERT_TRUE(*parsed_simple == serial_test);
    } else {
        M_ASSERT_FAIL("Failed to parse simple object");
    }

    auto parsed_empty = Json::parse("{}");
    if (parsed_empty.has_value()) {
        Json empty_code{Json::Object{}};
        M_ASSERT_TRUE(*parsed_empty == empty_code);
    } else {
        M_ASSERT_FAIL("Failed to parse empty object");
    }

    // Test nested object parsing
    auto parsed_nested = Json::parse(R"({"outer":{"inner1":1,"inner2":2}})");
    if (parsed_nested.has_value()) {
        M_ASSERT_EQ( parsed_nested->type(), json::Type::eObject );
        M_ASSERT_EQ( parsed_nested->obj().size(), 1 );
        M_ASSERT_EQ( (*parsed_nested)["outer"].obj().size(), 2 );
        M_ASSERT_EQ( (*parsed_nested)["outer"]["inner1"].to<Json::Number>(), 1 );
        M_ASSERT_EQ( (*parsed_nested)["outer"]["inner2"].to<Json::Number>(), 2 );
        
        // Compare with code-created nested object
        Json nested_code{Json::Object{
            {"outer", Json::Object{{"inner1", 1}, {"inner2", 2}}}
        }};
        M_ASSERT_TRUE( *parsed_nested == nested_code );
    } else {
        M_ASSERT_FAIL("Failed to parse nested object");
    }
    
    // Test mixed type parsing
    auto parsed_mixed = Json::parse(R"({"number":42,"string":"test","boolean":true,"null_val":null})");
    if (parsed_mixed.has_value()) {
        M_ASSERT_EQ( parsed_mixed->type(), json::Type::eObject );
        M_ASSERT_EQ( parsed_mixed->obj().size(), 4 );
        M_ASSERT_EQ( (*parsed_mixed)["number"].to<Json::Number>(), 42 );
        M_ASSERT_EQ( (*parsed_mixed)["string"].to<Json::String>(), "test" );
        M_ASSERT_EQ( (*parsed_mixed)["boolean"].to<Json::Bol>(), true );
        M_ASSERT_EQ( (*parsed_mixed)["null_val"].type(), json::Type::eNul );
        
        // Compare with code-created mixed object
        Json mixed_code{Json::Object{
            {"number", 42},
            {"string", "test"},
            {"boolean", true},
            {"null_val", nullptr}
        }};
        M_ASSERT_TRUE( *parsed_mixed == mixed_code );
    } else {
        M_ASSERT_FAIL("Failed to parse mixed type object");
    }
    
    // Test round-trip serialization/parsing
    Json original_obj{Json::Object{
        {"level1", Json::Object{
            {"name", "nested"},
            {"value", 123}
        }},
        {"level2", Json::Object{
            {"items", Json::Object{{"item1", 1}, {"item2", 2}}}
        }}
    }};
    auto serialized_obj = original_obj.dump();
    auto parsed_back_obj = Json::parse(serialized_obj);
    if (parsed_back_obj.has_value()) {
        M_ASSERT_TRUE( *parsed_back_obj == original_obj );
    } else {
        M_ASSERT_FAIL("Failed to parse back serialized object");
    }
    
    // --- Contains Method ---
    Json contains_test{Json::Object{{"key1", 1}, {"key2", "hello"}}};
    M_ASSERT_TRUE(contains_test.obj().contains("key1"));
    M_ASSERT_FALSE(contains_test.obj().contains("nonexistent"));

    // --- Type Safety ---
    M_ASSERT_THROW(std::ignore = obj_val.to<Json::Number>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = obj_val.arr(), std::bad_variant_access);

    // --- Modification via Reference ---
    Json ref_modify_test{Json::Object{{"a", 1}, {"b", 2}}};
    auto& ref_a = ref_modify_test["a"];
    ref_a = 100;
    M_ASSERT_EQ(ref_modify_test["a"].to<Json::Number>(), 100);
}
