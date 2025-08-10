#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import mysvac.json;

using namespace mysvac;

// --- Nul type tests ---
M_TEST(Equal, Nul) {
    Json null_val{};
    Json another_null{nullptr};
    M_ASSERT_TRUE(null_val == nullptr);
    M_ASSERT_TRUE(another_null == nullptr);
    M_ASSERT_TRUE(null_val == another_null);
    M_ASSERT_FALSE(null_val == 0);
    M_ASSERT_FALSE(null_val == "");
    M_ASSERT_FALSE(null_val == false);
    M_ASSERT_FALSE(null_val == std::string("null"));
}

// --- boolean equality tests ---
// Covers strict type comparison. No implicit conversion between bool and number.
M_TEST(Equal, Bol) {
    Json true_val{Json::Bol(true)};
    Json false_val{Json::Bol(false)};
    M_ASSERT_TRUE(true_val == true);      // True matches true
    M_ASSERT_FALSE(true_val == false);    // True does not match false
    M_ASSERT_TRUE(false_val == false);    // False matches false
    M_ASSERT_FALSE(false_val == true);    // False does not match true
    M_ASSERT_FALSE(true_val == false_val);// Different bool values
    M_ASSERT_TRUE(true_val == true_val);  // Reflexivity
    M_ASSERT_TRUE(false_val == false_val);// Reflexivity

    // --- No implicit conversion between bool and number ---
    // All comparisons between bool and number must be false.
    M_ASSERT_FALSE(true_val == 1);         // true != 1
    M_ASSERT_FALSE(true_val == 1.0);       // true != 1.0
    M_ASSERT_FALSE(true_val == 0);         // true != 0
    M_ASSERT_FALSE(true_val == 2);         // true != 2
    M_ASSERT_FALSE(false_val == 0);        // false != 0
    M_ASSERT_FALSE(false_val == 0.0);      // false != 0.0
    M_ASSERT_FALSE(false_val == 1);        // false != 1
    M_ASSERT_FALSE(false_val == -1);       // false != -1

    // Reverse comparison: number == bool
    M_ASSERT_FALSE(1 == true_val);
    M_ASSERT_FALSE(1.0 == true_val);
    M_ASSERT_FALSE(0 == true_val);
    M_ASSERT_FALSE(2 == true_val);
    M_ASSERT_FALSE(0 == false_val);
    M_ASSERT_FALSE(0.0 == false_val);
    M_ASSERT_FALSE(1 == false_val);
    M_ASSERT_FALSE(-1 == false_val);
}

// --- Num type tests ---
M_TEST(Equal, Num) {
    Json num_val{Json::Num(42)};
    M_ASSERT_TRUE(num_val == 42);
    M_ASSERT_TRUE(num_val == 42.0);
    M_ASSERT_FALSE(num_val == 43);
    M_ASSERT_FALSE(num_val == 41);

    Json float_val{Json::Num(3.14)};
    M_ASSERT_TRUE(float_val == 3.14);
    M_ASSERT_TRUE(float_val == 3);  // 3.14 rounds to 3
    M_ASSERT_FALSE(float_val == 4);
    M_ASSERT_FALSE(float_val == 2);

    Json negative_val{Json::Num(-100)};
    M_ASSERT_TRUE(negative_val == -100);
    M_ASSERT_FALSE(negative_val == 100);
    M_ASSERT_FALSE(negative_val == -99);

    // Boundary numbers
    Json max_val{std::numeric_limits<double>::max()};
    Json min_val{std::numeric_limits<double>::lowest()};
    M_ASSERT_TRUE(max_val == std::numeric_limits<double>::max());
    M_ASSERT_TRUE(min_val == std::numeric_limits<double>::lowest());
    M_ASSERT_FALSE(max_val == min_val);

    // No implicit conversion with bool
    M_ASSERT_FALSE(num_val == true);
    M_ASSERT_FALSE(num_val == false);

    // No implicit conversion with bool for number values
    Json num_true{Json::Num(1)};
    Json num_false{Json::Num(0)};
    M_ASSERT_FALSE(num_true == true);
    M_ASSERT_FALSE(num_false == false);
    M_ASSERT_FALSE(num_true == false);
    M_ASSERT_FALSE(num_false == true);
}

// --- Str type tests ---
M_TEST(Equal, Str) {
    Json str_val{Json::Str("hello")};
    M_ASSERT_TRUE(str_val == "hello");
    M_ASSERT_TRUE(str_val == std::string("hello"));
    M_ASSERT_FALSE(str_val == "Hello");
    M_ASSERT_FALSE(str_val == "world");
    M_ASSERT_FALSE(str_val == std::string("HELLO"));

    Json empty_str_val{Json::Str("")};
    M_ASSERT_TRUE(empty_str_val == "");
    M_ASSERT_TRUE(empty_str_val == std::string(""));
    M_ASSERT_FALSE(empty_str_val == " ");
    M_ASSERT_FALSE(empty_str_val == "empty");

    // 特殊字符
    Json unicode_val{Json::Str("你好世界")};
    M_ASSERT_TRUE(unicode_val == "你好世界");
    M_ASSERT_TRUE(unicode_val == std::string("你好世界"));
    M_ASSERT_FALSE(unicode_val == "Hello World");

    Json special_str{Json::Str("Hello\nWorld\t!")};
    M_ASSERT_TRUE(special_str == "Hello\nWorld\t!");
    M_ASSERT_TRUE(special_str == std::string("Hello\nWorld\t!"));
    M_ASSERT_FALSE(special_str == "Hello World !");
}

// --- Array type tests ---
M_TEST(Equal, Array) {
    Json arr1{Json::Array{1, 2, 3}};
    Json arr2{Json::Array{1, 2, 3}};
    Json arr3{Json::Array{1, 2, 4}};
    Json arr4{Json::Array{1, 2}};
    Json empty_arr1{Json::Array{}};
    Json empty_arr2{Json::Array{}};

    M_ASSERT_TRUE(arr1 == arr2);
    M_ASSERT_FALSE(arr1 == arr3);
    M_ASSERT_FALSE(arr1 == arr4);
    M_ASSERT_FALSE(arr1 == empty_arr1);
    M_ASSERT_TRUE(empty_arr1 == empty_arr2);

    // 不允许与原生类型比较
    M_ASSERT_FALSE(arr1 == 123);
    M_ASSERT_FALSE(arr1 == "array");
    M_ASSERT_FALSE(arr1 == true);
    M_ASSERT_FALSE(arr1 == nullptr);
    M_ASSERT_FALSE(empty_arr1 == 0);
    M_ASSERT_FALSE(empty_arr1 == "");
    M_ASSERT_FALSE(empty_arr1 == false);
}

// --- Object type tests ---
M_TEST(Equal, Object) {
    Json obj1{Json::Object{{"name", "John"}, {"age", 30}}};
    Json obj2{Json::Object{{"name", "John"}, {"age", 30}}};
    Json obj3{Json::Object{{"name", "Jane"}, {"age", 30}}};
    Json obj4{Json::Object{{"name", "John"}, {"age", 31}}};
    Json obj5{Json::Object{{"name", "John"}}};
    Json empty_obj1{Json::Object{}};
    Json empty_obj2{Json::Object{}};

    M_ASSERT_TRUE(obj1 == obj2);
    M_ASSERT_FALSE(obj1 == obj3);
    M_ASSERT_FALSE(obj1 == obj4);
    M_ASSERT_FALSE(obj1 == obj5);
    M_ASSERT_FALSE(obj1 == empty_obj1);
    M_ASSERT_TRUE(empty_obj1 == empty_obj2);

    // 不允许与原生类型比较
    M_ASSERT_FALSE(obj1 == 42);
    M_ASSERT_FALSE(obj1 == "object");
    M_ASSERT_FALSE(obj1 == true);
    M_ASSERT_FALSE(obj1 == nullptr);
    M_ASSERT_FALSE(empty_obj1 == 0);
    M_ASSERT_FALSE(empty_obj1 == "");
    M_ASSERT_FALSE(empty_obj1 == false);
}

// --- Mixed type and nested structure tests ---
M_TEST(Equal, MixedAndNested) {
    // 混合数组
    Json mixed1{Json::Array{
        Json::Num(42),
        Json::Str("hello"),
        Json::Bol(true),
        Json{nullptr},
        Json::Array{1, 2},
        Json::Object{{"key", "value"}}
    }};
    Json mixed2{Json::Array{
        Json::Num(42),
        Json::Str("hello"),
        Json::Bol(true),
        Json{nullptr},
        Json::Array{1, 2},
        Json::Object{{"key", "value"}}
    }};
    Json mixed3{Json::Array{
        Json::Num(42),
        Json::Str("hello"),
        Json::Bol(false),
        Json{nullptr},
        Json::Array{1, 2},
        Json::Object{{"key", "value"}}
    }};
    M_ASSERT_TRUE(mixed1 == mixed2);
    M_ASSERT_FALSE(mixed1 == mixed3);

    // 嵌套对象
    Json nested1{Json::Object{
        {"user", Json::Object{
            {"name", "Alice"},
            {"profile", Json::Object{
                {"age", 25},
                {"active", true}
            }}
        }},
        {"data", Json::Array{1, 2, 3}}
    }};
    Json nested2 = nested1;
    Json nested3{Json::Object{
        {"user", Json::Object{
            {"name", "Alice"},
            {"profile", Json::Object{
                {"age", 26},
                {"active", true}
            }}
        }},
        {"data", Json::Array{1, 2, 3}}
    }};
    M_ASSERT_TRUE(nested1 == nested2);
    M_ASSERT_FALSE(nested1 == nested3);
}

// --- Strict type and edge case tests ---
M_TEST(Equal, StrictTypeAndEdge) {
    // No implicit conversion between bool and number
    Json num_one{Json::Num(1)};
    Json bool_true{Json::Bol(true)};
    M_ASSERT_FALSE(num_one == bool_true);
    M_ASSERT_FALSE(bool_true == num_one);
    Json num_zero{Json::Num(0)};
    Json bool_false{Json::Bol(false)};
    M_ASSERT_FALSE(num_zero == bool_false);
    M_ASSERT_FALSE(bool_false == num_zero);

    // Strict type comparison between different types
    Json str_val{Json::Str("test")};
    Json arr_val{Json::Array{1, 2, 3}};
    Json obj_val{Json::Object{{"key", "value"}}};
    Json null_val{nullptr};

    M_ASSERT_FALSE(str_val == arr_val);      // Str vs Array
    M_ASSERT_FALSE(arr_val == obj_val);      // Array vs Object
    M_ASSERT_FALSE(obj_val == null_val);     // Object vs Nul

    // Reverse comparison
    M_ASSERT_TRUE(42 == Json{42});
    M_ASSERT_TRUE("hello" == Json{"hello"});
    M_ASSERT_TRUE(true == Json{true});
    M_ASSERT_FALSE(1 == Json{true});
    M_ASSERT_FALSE(0 == Json{false});
    M_ASSERT_FALSE(false == Json{0});
    M_ASSERT_FALSE(true == Json{1});
}

// --- Symmetry and reflexivity tests ---
M_TEST(Equal, SymmetryAndReflexivity) {
    Json val{Json::Num(42)};
    M_ASSERT_TRUE(val == 42);
    M_ASSERT_TRUE(42 == val);

    Json str_val{Json::Str("hello")};
    M_ASSERT_TRUE(str_val == "hello");
    M_ASSERT_TRUE("hello" == str_val);

    Json bool_val{Json::Bol(true)};
    M_ASSERT_TRUE(bool_val == true);
    M_ASSERT_TRUE(true == bool_val);
    M_ASSERT_FALSE(bool_val == 1);
    M_ASSERT_FALSE(1 == bool_val);

    Json false_val{Json::Bol(false)};
    M_ASSERT_TRUE(false_val == false);
    M_ASSERT_TRUE(false == false_val);
    M_ASSERT_FALSE(false_val == 0);
    M_ASSERT_FALSE(0 == false_val);

    // Reflexivity
    M_ASSERT_TRUE(val == val);
    M_ASSERT_TRUE(str_val == str_val);
    M_ASSERT_TRUE(bool_val == bool_val);
    M_ASSERT_TRUE(false_val == false_val);

    // No implicit conversion between bool and number
    Json bool_true{Json::Bol(true)};
    Json bool_false{Json::Bol(false)};
    M_ASSERT_FALSE(bool_true == 1);
    M_ASSERT_FALSE(1 == bool_true);
    M_ASSERT_FALSE(bool_false == 0);
    M_ASSERT_FALSE(0 == bool_false);
    M_ASSERT_FALSE(bool_true == 0);
    M_ASSERT_FALSE(0 == bool_true);
    M_ASSERT_FALSE(bool_false == 1);
    M_ASSERT_FALSE(1 == bool_false);
}

// --- Transitivity tests ---
M_TEST(Equal, Transitivity) {
    Json val1{static_cast<Json::Num>(42)};
    Json val2{Json::Num(42)};
    Json val3{Json::Num(42)};
    M_ASSERT_TRUE(val1 == val2);
    M_ASSERT_TRUE(val2 == val3);
    M_ASSERT_TRUE(val1 == val3);

    Json str1{Json::Str("test")};
    Json str2{Json::Str("test")};
    Json str3{Json::Str("test")};
    M_ASSERT_TRUE(str1 == str2);
    M_ASSERT_TRUE(str2 == str3);
    M_ASSERT_TRUE(str1 == str3);
}
