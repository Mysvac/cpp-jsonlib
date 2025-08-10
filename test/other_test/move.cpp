#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import mysvac.json;

using namespace mysvac;

enum class MyEnum { A = 5, B = 6, C = -1 };

M_TEST(Value, Move) {
    // Nul
    Json v_null{nullptr};
    auto moved_null = v_null.move<Json::Nul>();
    M_ASSERT_EQ(moved_null, nullptr);
    M_ASSERT_EQ(v_null.type(), json::Type::eNul);

    // Bol
    Json v_true{true};
    auto moved_bool = v_true.move<Json::Bol>();
    M_ASSERT_EQ(moved_bool, true);
    M_ASSERT_EQ(v_true.type(), json::Type::eBol);
    M_ASSERT_EQ(v_true.to<Json::Bol>(), true); // 简单类型移动后仍为原值

    // Num
    Json v_num{42.5};
    auto moved_num = v_num.move<Json::Num>();
    M_ASSERT_EQ(moved_num, 42.5);
    M_ASSERT_EQ(v_num.type(), json::Type::eNum);
    M_ASSERT_EQ(v_num.to<Json::Num>(), 42.5);

    // Str
    Json v_str{"hello"};
    auto moved_str = v_str.move<Json::Str>();
    M_ASSERT_EQ(moved_str, "hello");
    M_ASSERT_EQ(v_str.type(), json::Type::eStr);
    M_ASSERT_EQ(v_str.to<Json::Str>(), ""); // 移动后原字符串为空

    // Arr
    Json v_arr{Json::Arr{{1, 2, 3}}};
    auto moved_arr = v_arr.move<Json::Arr>();
    M_ASSERT_EQ(moved_arr.size(), 3);
    M_ASSERT_EQ(moved_arr[0].to<int>(), 1);
    M_ASSERT_EQ(v_arr.type(), json::Type::eArr);
    M_ASSERT_EQ(v_arr.to<Json::Arr>().size(), 0); // 移动后原数组为空

    // Obj
    Json v_obj{Json::Obj{{"k", 1}, {"b", 2}}};
    auto moved_obj = v_obj.move<Json::Obj>();
    M_ASSERT_EQ(moved_obj.size(), 2);
    M_ASSERT_EQ(moved_obj.at("k").to<int>(), 1);
    M_ASSERT_EQ(v_obj.type(), json::Type::eObj);
    M_ASSERT_EQ(v_obj.to<Json::Obj>().size(), 0); // 移动后原对象为空

    // Enum
    Json v_enum{5};
    auto moved_enum = v_enum.move<MyEnum>();
    M_ASSERT_EQ(moved_enum, MyEnum::A);

    // Integral
    Json v_int{123};
    auto moved_int = v_int.move<int>();
    M_ASSERT_EQ(moved_int, 123);

    // Floating point
    Json v_fp{3.14};
    auto moved_fp = v_fp.move<double>();
    M_ASSERT_EQ(moved_fp, 3.14);

    // std::string_view
    Json v_sv{"abc"};
    auto moved_sv = v_sv.move<std::string_view>();
    M_ASSERT_EQ(moved_sv, std::string_view("abc"));
    M_ASSERT_EQ(v_sv.to<Json::Str>(), "abc"); // string_view 不修改自身

    // std::string_view
    Json v_sv2{"abc"};
    auto moved_sv2 = v_sv2.move<std::string>();
    M_ASSERT_EQ(moved_sv2, std::string_view("abc"));
    M_ASSERT_EQ(v_sv2.to<Json::Str>(), ""); // string 移动

    // move_if: 成功
    Json v_arr2{Json::Arr{{4, 5}}};
    auto opt_arr = v_arr2.move_if<Json::Arr>();
    M_ASSERT_TRUE(opt_arr.has_value());
    M_ASSERT_EQ(opt_arr->size(), 2);
    M_ASSERT_EQ(v_arr2.to<Json::Arr>().size(), 0);

    // move_if: 失败
    Json v_fail{true};
    auto opt_fail = v_fail.move_if<Json::Arr>();
    M_ASSERT_FALSE(opt_fail.has_value());

    // move_or: 成功
    Json v_obj2{Json::Obj{{"x", 7}}};
    auto moved_obj2 = v_obj2.move_or<Json::Obj>(Json::Obj{{"default", 0}});
    M_ASSERT_EQ(moved_obj2.size(), 1);
    M_ASSERT_EQ(moved_obj2.at("x").to<int>(), 7);
    M_ASSERT_EQ(v_obj2.to<Json::Obj>().size(), 0);

    // move_or: 失败返回默认
    Json v_fail2{false};
    auto moved_default = v_fail2.move_or<Json::Arr>(Json::Arr{{9}});
    M_ASSERT_EQ(moved_default.size(), 1);
    M_ASSERT_EQ(moved_default[0].to<int>(), 9);

    // 边界值
    Json v_empty_arr{Json::Arr{}};
    auto moved_empty_arr = v_empty_arr.move<Json::Arr>();
    M_ASSERT_EQ(moved_empty_arr.size(), 0);

    Json v_empty_obj{Json::Obj{}};
    auto moved_empty_obj = v_empty_obj.move<Json::Obj>();
    M_ASSERT_EQ(moved_empty_obj.size(), 0);

    Json v_empty_str{""};
    auto moved_empty_str = v_empty_str.move<Json::Str>();
    M_ASSERT_EQ(moved_empty_str, "");
    M_ASSERT_EQ(v_empty_str.to<Json::Str>(), "");

    // 复杂嵌套结构
    Json v_nested{Json::Arr{
        Json::Obj{{"a", 1}},
        Json::Arr{{2, 3}},
        Json::Str("deep")
    }};
    auto moved_nested = v_nested.move<Json::Arr>();
    M_ASSERT_EQ(moved_nested.size(), 3);
    M_ASSERT_EQ(moved_nested[0]["a"].to<int>(), 1);
    M_ASSERT_EQ(moved_nested[1][0].to<int>(), 2);
    M_ASSERT_EQ(moved_nested[2].to<Json::Str>(), "deep");
    M_ASSERT_EQ(v_nested.to<Json::Arr>().size(), 0);

    // move: 不兼容类型抛异常
    Json v_wrong{true};
    M_ASSERT_THROW(std::ignore = v_wrong.move<Json::Arr>(), std::runtime_error);

    // move_or: 不兼容类型返回默认
    auto moved_or_default = v_wrong.move_or<Json::Arr>(Json::Arr{{99}});
    M_ASSERT_EQ(moved_or_default.size(), 1);
    M_ASSERT_EQ(moved_or_default[0].to<int>(), 99);

    // move_if: 不兼容类型返回nullopt
    auto opt_wrong = v_wrong.move_if<Json::Arr>();
    M_ASSERT_FALSE(opt_wrong.has_value());
}
