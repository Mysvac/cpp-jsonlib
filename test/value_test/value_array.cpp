#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import mysvac.json;

using namespace mysvac;

// Array1: 基础构造与访问
M_TEST(Value, Array) {
    M_ASSERT_NO_THROW(Json arr_default{ Json::Array{} });
    M_ASSERT_EQ(Json{json::Type::eArray}.type(), json::Type::eNumber);
    M_ASSERT_FALSE(Json{json::Type::eArray}.is_arr());
    M_ASSERT_TRUE(Json{json::Type::eArray}.is_num());
    M_ASSERT_EQ(Json{Json::Array{}}.arr().size(), 0);
    M_ASSERT_TRUE(Json{Json::Array{}}.to<Json::Array>().empty());
    M_ASSERT_NO_THROW(Json arr_jsonlicit{Json::Array{{}}});
    M_ASSERT_NO_THROW(Json arr_init{Json::Array{{1, 2, 3}}});
    Json arr_val{Json::Array{{1, 2, 3}}};
    M_ASSERT_TRUE(arr_val.is_arr());
    M_ASSERT_FALSE(arr_val.is_str());
    M_EXPECT_STREQ(json::type_name(arr_val.type()), "Array");


    // Array2: 简单与混合类型访问

    Json simple_arr{Json::Array{{10, 20, 30}}};
    M_ASSERT_EQ(simple_arr.type(), json::Type::eArray);
    M_ASSERT_EQ(simple_arr.arr().size(), 3);
    M_ASSERT_EQ(simple_arr[0].to<Json::Number>(), 10);
    M_ASSERT_EQ(simple_arr[1].to<Json::Number>(), 20);
    M_ASSERT_EQ(simple_arr[2].to<Json::Number>(), 30);

    Json mixed_arr{Json::Array{{42, "hello", true, nullptr}}};
    M_ASSERT_EQ(mixed_arr.arr().size(), 4);
    M_ASSERT_EQ(mixed_arr[0].to<Json::Number>(), 42);
    M_ASSERT_EQ(mixed_arr[1].to<Json::String>(), "hello");
    M_ASSERT_EQ(mixed_arr[2].to<Json::Bool>(), true);
    M_ASSERT_EQ(mixed_arr[3].type(), json::Type::eNull);

    const Json const_arr{Json::Array{{1, 2, 3}}};
    M_ASSERT_EQ(const_arr[2].to<Json::Number>(), 3);


    // Array3: 元素修改与赋值
    Json modify_test{Json::Array{{1, 2, 3}}};
    modify_test[0] = 100;
    modify_test[1] = "modified";
    modify_test[2] = false;
    M_ASSERT_EQ(modify_test[0].to<Json::Number>(), 100);
    M_ASSERT_EQ(modify_test[1].to<Json::String>(), "modified");
    M_ASSERT_EQ(modify_test[2].to<Json::Bool>(), false);

    Json assign_val{Json::Array{}};
    M_ASSERT_NO_THROW(assign_val = Json::Array{{1, 2, 3}});
    M_ASSERT_EQ(assign_val.arr().size(), 3);
    M_ASSERT_EQ(assign_val[0].to<Json::Number>(), 1);
    M_ASSERT_NO_THROW(assign_val = Json::Array{{10, 20}});
    M_ASSERT_EQ(assign_val.arr().size(), 2);
    M_ASSERT_EQ(assign_val[0].to<Json::Number>(), 10);
    M_ASSERT_EQ(assign_val[1].to<Json::Number>(), 20);


    // Array4: 安全访问与异常
    Json at_test{Json::Array{{100, 200, 300}}};
    M_ASSERT_NO_THROW(std::ignore = at_test.at(0));
    M_ASSERT_NO_THROW(std::ignore = at_test.at(1));
    M_ASSERT_NO_THROW(std::ignore = at_test.at(2));
    M_ASSERT_EQ(at_test.at(1).to<Json::Number>(), 200);
    M_ASSERT_THROW(std::ignore = at_test.at(3), std::out_of_range);

    const Json const_at_test{Json::Array{{1, 2, 3}}};
    M_ASSERT_EQ(const_at_test.at(1).to<Json::Number>(), 2);

    Json wrong_type_val{42};
    M_ASSERT_THROW(std::ignore = wrong_type_val.arr(), std::bad_variant_access);

    Json string_val{"not array"};
    M_ASSERT_THROW(std::ignore = string_val.arr(), std::bad_variant_access);


    // Array5: get<T>引用与容器操作
    Json get_test{Json::Array{{1, 2, 3}}};
    M_ASSERT_NO_THROW(std::ignore = get_test.to<Json::Array>());
    auto& arr_ref = get_test.arr();
    arr_ref[0] = 100;
    arr_ref.push_back(40);
    M_ASSERT_EQ(get_test.arr().size(), 4);
    M_ASSERT_EQ(get_test[0].to<Json::Number>(), 100);
    M_ASSERT_EQ(get_test[3].to<Json::Number>(), 40);

    const Json const_ref_test{Json::Array{{1, 2, 3}}};
    M_ASSERT_NO_THROW(std::ignore = const_ref_test.arr());
    const auto& const_arr_ref = const_ref_test.arr();
    M_ASSERT_EQ(const_arr_ref.size(), 3);
    M_ASSERT_EQ(const_arr_ref[0].to<Json::Number>(), 1);


    // Array6: 多维嵌套结构
    Json nested_2d{Json::Array{{
        Json::Array{{1, 2, 3}},
        Json::Array{{4, 5, 6}},
        Json::Array{{7, 8, 9}}
    }}};
    M_ASSERT_EQ(nested_2d.type(), json::Type::eArray);
    M_ASSERT_EQ(nested_2d.arr().size(), 3);
    M_ASSERT_EQ(nested_2d[1][0].to<Json::Number>(), 4);

    Json nested_3d{Json::Array{{
        Json::Array{{
            Json::Array{{1, 2}},
            Json::Array{{3, 4}}
        }},
        Json::Array{{
            Json::Array{{5, 6}},
            Json::Array{{7, 8}}
        }}
    }}};
    M_ASSERT_EQ(nested_3d.arr().size(), 2);
    M_ASSERT_EQ(nested_3d[0].arr().size(), 2);
    M_ASSERT_EQ(nested_3d[0][0].arr().size(), 2);
    M_ASSERT_EQ(nested_3d[0][1][1].to<Json::Number>(), 4);


// Array7: 混合嵌套与不规则结构
    Json mixed_nested{Json::Array{{
        Json::Array{{1, 2, 3}},
        Json::String("nested string"),
        Json::Array{{
            Json::Bool(true),
            Json::Array{{4, 5}},
            nullptr
        }}
    }}};
    M_ASSERT_EQ(mixed_nested.arr().size(), 3);
    M_ASSERT_EQ(mixed_nested[0].arr().size(), 3);
    M_ASSERT_EQ(mixed_nested[1].type(), json::Type::eString);
    M_ASSERT_EQ(mixed_nested[2].arr().size(), 3);
    M_ASSERT_EQ(mixed_nested[2][1][1].to<Json::Number>(), 5);

    Json irregular{Json::Array{{
        Json::Array{{1}},
        Json::Array{{2, 3}},
        Json::Array{{4, 5, 6}},
        Json::Array{{7, 8, 9, 10}}
    }}};
    M_ASSERT_EQ(irregular.arr().size(), 4);
    M_ASSERT_EQ(irregular[0].arr().size(), 1);
    M_ASSERT_EQ(irregular[1].arr().size(), 2);
    M_ASSERT_EQ(irregular[2].arr().size(), 3);
    M_ASSERT_EQ(irregular[3].arr().size(), 4);
    M_ASSERT_EQ(irregular[1][1].to<Json::Number>(), 3);


// Array8: 比较与序列化
    Json arr1{Json::Array{{1, 2, 3}}};
    Json arr2{Json::Array{{1, 2, 3}}};
    Json arr3{Json::Array{{1, 2, 4}}};
    M_ASSERT_TRUE(arr1 == arr2);
    M_ASSERT_FALSE(arr1 == arr3);
    M_ASSERT_TRUE(arr1 != arr3);

    Json empty1{Json::Array{{}}};
    Json empty2{Json::Array{{}}};
    M_ASSERT_TRUE(empty1 == empty2);

    Json short_arr{ Json::Array{{1, 2}}};
    Json long_arr{ Json::Array{{1, 2, 3}}};
    M_ASSERT_FALSE(short_arr == long_arr);

    Json nested1{Json::Array{{
        Json::Array{{1, 2}},
        Json::Array{{3, 4}}
    }}};
    Json nested2{Json::Array{{
        Json::Array{{1, 2}},
        Json::Array{{3, 4}}
    }}};
    Json nested3{Json::Array{{
        Json::Array{{1, 2}},
        Json::Array{{3, 5}}
    }}};
    M_ASSERT_TRUE(nested1 == nested2);
    M_ASSERT_FALSE(nested1 == nested3);

    Json simple3_arr{Json::Array{{1, 2, 3}}};
    M_ASSERT_EQ(simple3_arr.dump(), "[1,2,3]");

    // empty must be use Json::Array{} to avoid ambiguity with Json
    Json empty_serial{Json::Array{}};
    M_ASSERT_EQ(empty_serial.dump(), "[]");

    Json empty_serial_2{Json::Array{{} }};
    M_ASSERT_EQ(empty_serial_2.dump(), "[null]");

    Json mixed{Json::Array{{42, "test", true, nullptr}}};
    M_ASSERT_EQ(mixed.dump(), "[42,\"test\",true,null]");
    Json nested{Json::Array{{
        Json::Array{{1, 2}},
        Json::Array{{3, 4}}
    }}};
    M_ASSERT_EQ(nested.dump(), "[[1,2],[3,4]]");


// Array9: 解析与序列化回环
    auto parsed_simple = Json::parse("[1,2,3]");
    if (parsed_simple.has_value()) {
        M_ASSERT_EQ(parsed_simple->type(), json::Type::eArray);
        M_ASSERT_EQ(parsed_simple->arr().size(), 3);
        M_ASSERT_EQ((*parsed_simple)[0].to<Json::Number>(), 1);
        M_ASSERT_EQ((*parsed_simple)[2].to<Json::Number>(), 3);
    } else {
        M_ASSERT_FAIL("Failed to parse simple array");
    }

    Json simple2_arr{Json::Array{{1, 2, 3}}};
    auto serialized_arr = simple2_arr.dump();
    auto parsed_back_arr = Json::parse(serialized_arr);
    if (parsed_back_arr.has_value()) {
        M_ASSERT_TRUE(*parsed_back_arr == simple2_arr);
    } else {
        M_ASSERT_FAIL("Failed to parse back serialized array");
    }


// Array10: 多维混合结构与边界
    Json simplified_multi_dim{Json::Array{{
        Json::Array{{
            Json::Array{{1, 2, 3}},
            Json::Array{{4, 5, 6}},
            Json::Array{{7, 8, 9}}
        }},
        Json::Array{{
            Json::Array{{"a", "b", "c"}},
            Json::Array{{"d", "e", "f"}},
            Json::Array{{"g", "h", "i"}}
        }},
        Json::Array{{
            Json::Array{{true, false, nullptr}},
            Json::Array{{100, "mixed", 3.14}}
        }}
    }}};
    M_ASSERT_EQ(simplified_multi_dim.arr().size(), 3);
    M_ASSERT_EQ(simplified_multi_dim[0].arr().size(), 3);
    M_ASSERT_EQ(simplified_multi_dim[1].arr().size(), 3);
    M_ASSERT_EQ(simplified_multi_dim[2].arr().size(), 2);
    M_ASSERT_EQ(simplified_multi_dim[0][0][0].to<Json::Number>(), 1);
    M_ASSERT_EQ(simplified_multi_dim[1][1][1].to<Json::String>(), "e");
    M_ASSERT_EQ(simplified_multi_dim[2][0][2].type(), json::Type::eNull);
    M_ASSERT_EQ(simplified_multi_dim[2][0][2].is_nul(), true);
    M_ASSERT_EQ(simplified_multi_dim[2][1][1].to<Json::String>(), "mixed");
    M_ASSERT_EQ(simplified_multi_dim[2][1][2].to<Json::Number>(), 3.14);
    M_ASSERT_THROW(std::ignore = simplified_multi_dim.at(3), std::out_of_range);
    M_ASSERT_THROW(std::ignore = simplified_multi_dim.at(0).at(3), std::out_of_range);
    M_ASSERT_THROW(std::ignore = simplified_multi_dim.at(1).at(0).at(3), std::out_of_range);
    M_ASSERT_THROW(std::ignore = simplified_multi_dim.at(2).at(2), std::out_of_range);
}
