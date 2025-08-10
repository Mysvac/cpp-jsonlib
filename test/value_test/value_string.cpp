#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import mysvac.json;

using namespace mysvac;

// --- Construction tests ---
M_TEST(Value, String) {
    // Default construction
    M_ASSERT_NO_THROW(Json str_default{ json::Type::eString });
    M_ASSERT_EQ(Json{json::Type::eString}.is_num(), true);
    M_ASSERT_EQ(Json{json::Type::eString}.is_nul(), false);
    M_ASSERT_EQ(Json{json::Type::eString}.is_arr(), false);
    M_ASSERT_EQ(Json{json::Type::eString}.is_obj(), false);
    M_ASSERT_EQ(Json{json::Type::eString}.is_bol(), false);
    M_ASSERT_EQ(Json{json::Type::eString}.is_str(), false);
    M_ASSERT_EQ(Json{json::Type::eString}.type(), json::Type::eNum);

    M_ASSERT_NO_THROW(Json str_default{Json::String{}});
    M_ASSERT_EQ(Json{Json::String{}}.to<Json::String>(), "");
    M_ASSERT_EQ(Json{Json::String{}}.type(), json::Type::eString);

    M_ASSERT_EQ(Json{Json::String{}}.is_num(), false);
    M_ASSERT_EQ(Json{Json::String{}}.is_nul(), false);
    M_ASSERT_EQ(Json{Json::String{}}.is_arr(), false);
    M_ASSERT_EQ(Json{Json::String{}}.is_obj(), false);
    M_ASSERT_EQ(Json{Json::String{}}.is_bol(), false);
    M_ASSERT_EQ(Json{Json::String{}}.is_str(), true);

    // Construction with jsonlicit String wrapper
    M_ASSERT_NO_THROW(Json str_hello{Json::String("hello")});
    M_ASSERT_NO_THROW(Json str_empty{Json::String("")});
    M_ASSERT_NO_THROW(Json str_special{Json::String("Hello, 世界!")});

    // Direct string construction
    M_ASSERT_NO_THROW(Json direct_str{"direct string"});
    M_ASSERT_NO_THROW(Json direct_empty{""});

    // Implicit construction
    Json implicit_str = "implicit string";
    Json implicit_empty = "";
    M_ASSERT_EQ(implicit_str.type(), json::Type::eString);
    M_ASSERT_EQ(implicit_empty.type(), json::Type::eString);
    M_ASSERT_EQ(implicit_str.to<Json::String>(), "implicit string");
    M_ASSERT_EQ(implicit_empty.to<Json::String>(), "");

    // Construction from std::string
    std::string std_str = "std::string test";
    Json copy_init_std = std_str;
    M_ASSERT_EQ(copy_init_std.type(), json::Type::eString);
    M_ASSERT_EQ(copy_init_std.to<Json::String>(), "std::string test");

    // --- Type checking ---
    Json str_val{"test string"};
    Json empty_val{""};
    M_ASSERT_TRUE(str_val.is_str());
    M_ASSERT_FALSE(str_val.is_num());
    M_EXPECT_STREQ(json::type_name(str_val.type()), "String");

    // --- Reference access ---
    M_ASSERT_EQ(str_val.str(), "test string");
    M_ASSERT_EQ(empty_val.str(), "");
    const Json const_str{"const string"};
    M_ASSERT_EQ(const_str.str(), "const string");
    M_ASSERT_THROW(std::ignore = Json{42}.str(), std::bad_variant_access);

    // --- Assignment tests ---
    Json assign_val{Json::String{}};
    assign_val = Json::String("assigned");
    M_ASSERT_EQ(assign_val.to<Json::String>(), "assigned");
    assign_val = "reassigned";
    M_ASSERT_EQ(assign_val.to<Json::String>(), "reassigned");

    // Reference modification
    Json mod_val{"original"};
    auto& str_ref = mod_val.str();
    str_ref = "modified";
    M_ASSERT_EQ(mod_val.to<Json::String>(), "modified");
    str_ref += " appended";
    M_ASSERT_EQ(mod_val.to<Json::String>(), "modified appended");

    // --- Comparison tests ---
    Json val_str1{"same string"};
    Json val_str2{"same string"};
    Json val_str3{"different string"};
    M_ASSERT_TRUE(val_str1 == val_str2);
    M_ASSERT_FALSE(val_str1 == val_str3);
    M_ASSERT_TRUE(val_str1 == "same string");
    M_ASSERT_FALSE(val_str1 == "");

    // Mixed type comparison
    Json num_val{42.0};
    Json bool_val{true};
    Json null_val{nullptr};
    M_ASSERT_FALSE(val_str1 == num_val);
    M_ASSERT_FALSE(val_str1 == bool_val);
    M_ASSERT_FALSE(val_str1 == null_val);

    // --- Serialization tests ---
    M_ASSERT_EQ(Json{"hello"}.dump(), "\"hello\"");
    M_ASSERT_EQ(Json{""}.dump(), "\"\"");
    M_ASSERT_EQ(Json{"simple"}.dump(), "\"simple\"");

    // Pretty serialization
    M_ASSERT_EQ(Json{"pretty"}.dumpf(), "\"pretty\"");
    M_ASSERT_EQ(Json{""}.dumpf(), "\"\"");

    // Escape sequences serialization
    M_ASSERT_EQ(Json{"line1\nline2"}.dump(), "\"line1\\nline2\"");
    M_ASSERT_EQ(Json{"col1\tcol2"}.dump(), "\"col1\\tcol2\"");
    M_ASSERT_EQ(Json{"say \"hello\""}.dump(), "\"say \\\"hello\\\"\"");
    M_ASSERT_EQ(Json{"path\\to\\file"}.dump(), "\"path\\\\to\\\\file\"");

    // --- Parsing tests ---
    auto parsed_hello = Json::parse("\"hello world\"");
    if (parsed_hello.has_value()) {
        M_ASSERT_EQ(parsed_hello->type(), json::Type::eString);
        M_ASSERT_EQ(parsed_hello->str(), "hello world");
    }

    auto parsed_empty = Json::parse("\"\"");
    if (parsed_empty.has_value()) {
        M_ASSERT_EQ(parsed_empty->type(), json::Type::eString);
        M_ASSERT_EQ(parsed_empty->str(), "");
    }

    // Escape sequence parsing
    auto parsed_newline = Json::parse(R"("line1\nline2")");
    if (parsed_newline.has_value()) {
        M_ASSERT_EQ(parsed_newline->str(), "line1\nline2");
    }

    auto parsed_tab = Json::parse(R"("col1\tcol2")");
    if (parsed_tab.has_value()) {
        M_ASSERT_EQ(parsed_tab->str(), "col1\tcol2");
    }

    auto parsed_quote = Json::parse(R"("say \"hello\"")");
    if (parsed_quote.has_value()) {
        M_ASSERT_EQ(parsed_quote->str(), "say \"hello\"");
    }

    auto parsed_backslash = Json::parse(R"("path\\to\\file")");
    if (parsed_backslash.has_value()) {
        M_ASSERT_EQ(parsed_backslash->str(), "path\\to\\file");
    }

    // --- Unicode escape sequence parsing ---
    auto parsed_unicode_a = Json::parse(R"("\u0041")");
    if (parsed_unicode_a.has_value()) {
        M_ASSERT_EQ(parsed_unicode_a->str(), "A");
    }

    auto parsed_unicode_chinese = Json::parse(R"("\u4e2d\u6587")");
    if (parsed_unicode_chinese.has_value()) {
        M_ASSERT_EQ(parsed_unicode_chinese->str(), "中文");
    }

    auto parsed_unicode_mixed = Json::parse(R"("Hello \u4e16\u754c!")");
    if (parsed_unicode_mixed.has_value()) {
        M_ASSERT_EQ(parsed_unicode_mixed->str(), "Hello 世界!");
    }

    // Unicode round-trip: parse then serialize
    auto unicode_roundtrip = Json::parse(R"("\u4e16\u754c")");
    if (unicode_roundtrip.has_value()) {
        auto roundtrip_serialized = unicode_roundtrip->dump();
        M_ASSERT_EQ(roundtrip_serialized, "\"世界\"");
    }

    // Control characters serialization
    Json control_chars{"\x01\x02\x03\x1f"};
    auto control_serialized = control_chars.dump();
    M_ASSERT_TRUE(control_serialized.find("\\u") != std::string::npos);

    // --- Type safety ---
    M_ASSERT_THROW(std::ignore = str_val.to<Json::Num>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = str_val.to<Json::Bol>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = str_val.to<Json::Array>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = str_val.to<Json::Object>(), std::runtime_error);
    M_ASSERT_THROW(std::ignore = str_val.to<std::nullptr_t>(), std::runtime_error);

    // --- String conversion ---
    Json str_conv_test{"conversion test"};
    M_ASSERT_EQ(str_conv_test.to<std::string>(), "conversion test");
    Json str_view_test{"string_view test"};
    auto view = str_view_test.to<std::string_view>();
    M_ASSERT_EQ(view, "string_view test");

    // --- String properties ---
    Json length_test{"12345"};
    M_ASSERT_EQ(length_test.to<Json::String>().length(), 5);
    M_ASSERT_FALSE(length_test.to<Json::String>().empty());

    Json empty_length_test{""};
    M_ASSERT_EQ(empty_length_test.to<Json::String>().length(), 0);
    M_ASSERT_TRUE(empty_length_test.to<Json::String>().empty());

    // --- Consistency checks ---
    Json consistency_str1{"same content"};
    Json consistency_str2{"same content"};
    M_ASSERT_TRUE(consistency_str1 == consistency_str2);
    M_ASSERT_EQ(consistency_str1.dump(), "\"same content\"");
}
