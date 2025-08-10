#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import mysvac.json;

using namespace mysvac;

// Arr1: 基础构造与访问
M_TEST(Value, Arr) {
    M_ASSERT_NO_THROW(Json arr_default{ Json::Arr{} });
    M_ASSERT_EQ(Json{json::Type::eArr}.type(), json::Type::eNum);
    M_ASSERT_FALSE(Json{json::Type::eArr}.is_arr());
    M_ASSERT_TRUE(Json{json::Type::eArr}.is_num());
    M_ASSERT_EQ(Json{Json::Arr{}}.arr().size(), 0);
    M_ASSERT_TRUE(Json{Json::Arr{}}.to<Json::Arr>().empty());
    M_ASSERT_NO_THROW(Json arr_jsonlicit{Json::Arr{{}}});
    M_ASSERT_NO_THROW(Json arr_init{Json::Arr{{1, 2, 3}}});
    Json arr_val{Json::Arr{{1, 2, 3}}};
    M_ASSERT_TRUE(arr_val.is_arr());
    M_ASSERT_FALSE(arr_val.is_str());
    M_EXPECT_STREQ(json::type_name(arr_val.type()), "Arr");


    // Arr2: 简单与混合类型访问

    Json simple_arr{Json::Arr{{10, 20, 30}}};
    M_ASSERT_EQ(simple_arr.type(), json::Type::eArr);
    M_ASSERT_EQ(simple_arr.arr().size(), 3);
    M_ASSERT_EQ(simple_arr[0].to<Json::Num>(), 10);
    M_ASSERT_EQ(simple_arr[1].to<Json::Num>(), 20);
    M_ASSERT_EQ(simple_arr[2].to<Json::Num>(), 30);

    Json mixed_arr{Json::Arr{{42, "hello", true, nullptr}}};
    M_ASSERT_EQ(mixed_arr.arr().size(), 4);
    M_ASSERT_EQ(mixed_arr[0].to<Json::Num>(), 42);
    M_ASSERT_EQ(mixed_arr[1].to<Json::Str>(), "hello");
    M_ASSERT_EQ(mixed_arr[2].to<Json::Bol>(), true);
    M_ASSERT_EQ(mixed_arr[3].type(), json::Type::eNul);

    const Json const_arr{Json::Arr{{1, 2, 3}}};
    M_ASSERT_EQ(const_arr[2].to<Json::Num>(), 3);


    // Arr3: 元素修改与赋值
    Json modify_test{Json::Arr{{1, 2, 3}}};
    modify_test[0] = 100;
    modify_test[1] = "modified";
    modify_test[2] = false;
    M_ASSERT_EQ(modify_test[0].to<Json::Num>(), 100);
    M_ASSERT_EQ(modify_test[1].to<Json::Str>(), "modified");
    M_ASSERT_EQ(modify_test[2].to<Json::Bol>(), false);

    Json assign_val{Json::Arr{}};
    M_ASSERT_NO_THROW(assign_val = Json::Arr{{1, 2, 3}});
    M_ASSERT_EQ(assign_val.arr().size(), 3);
    M_ASSERT_EQ(assign_val[0].to<Json::Num>(), 1);
    M_ASSERT_NO_THROW(assign_val = Json::Arr{{10, 20}});
    M_ASSERT_EQ(assign_val.arr().size(), 2);
    M_ASSERT_EQ(assign_val[0].to<Json::Num>(), 10);
    M_ASSERT_EQ(assign_val[1].to<Json::Num>(), 20);


    // Arr4: 安全访问与异常
    Json at_test{Json::Arr{{100, 200, 300}}};
    M_ASSERT_NO_THROW(std::ignore = at_test.at(0));
    M_ASSERT_NO_THROW(std::ignore = at_test.at(1));
    M_ASSERT_NO_THROW(std::ignore = at_test.at(2));
    M_ASSERT_EQ(at_test.at(1).to<Json::Num>(), 200);
    M_ASSERT_THROW(std::ignore = at_test.at(3), std::out_of_range);

    const Json const_at_test{Json::Arr{{1, 2, 3}}};
    M_ASSERT_EQ(const_at_test.at(1).to<Json::Num>(), 2);

    Json wrong_type_val{42};
    M_ASSERT_THROW(std::ignore = wrong_type_val.arr(), std::bad_variant_access);

    Json string_val{"not array"};
    M_ASSERT_THROW(std::ignore = string_val.arr(), std::bad_variant_access);


    // Arr5: get<T>引用与容器操作
    Json get_test{Json::Arr{{1, 2, 3}}};
    M_ASSERT_NO_THROW(std::ignore = get_test.to<Json::Arr>());
    auto& arr_ref = get_test.arr();
    arr_ref[0] = 100;
    arr_ref.push_back(40);
    M_ASSERT_EQ(get_test.arr().size(), 4);
    M_ASSERT_EQ(get_test[0].to<Json::Num>(), 100);
    M_ASSERT_EQ(get_test[3].to<Json::Num>(), 40);

    const Json const_ref_test{Json::Arr{{1, 2, 3}}};
    M_ASSERT_NO_THROW(std::ignore = const_ref_test.arr());
    const auto& const_arr_ref = const_ref_test.arr();
    M_ASSERT_EQ(const_arr_ref.size(), 3);
    M_ASSERT_EQ(const_arr_ref[0].to<Json::Num>(), 1);


    // Arr6: 多维嵌套结构
    Json nested_2d{Json::Arr{{
        Json::Arr{{1, 2, 3}},
        Json::Arr{{4, 5, 6}},
        Json::Arr{{7, 8, 9}}
    }}};
    M_ASSERT_EQ(nested_2d.type(), json::Type::eArr);
    M_ASSERT_EQ(nested_2d.arr().size(), 3);
    M_ASSERT_EQ(nested_2d[1][0].to<Json::Num>(), 4);

    Json nested_3d{Json::Arr{{
        Json::Arr{{
            Json::Arr{{1, 2}},
            Json::Arr{{3, 4}}
        }},
        Json::Arr{{
            Json::Arr{{5, 6}},
            Json::Arr{{7, 8}}
        }}
    }}};
    M_ASSERT_EQ(nested_3d.arr().size(), 2);
    M_ASSERT_EQ(nested_3d[0].arr().size(), 2);
    M_ASSERT_EQ(nested_3d[0][0].arr().size(), 2);
    M_ASSERT_EQ(nested_3d[0][1][1].to<Json::Num>(), 4);


// Arr7: 混合嵌套与不规则结构
    Json mixed_nested{Json::Arr{{
        Json::Arr{{1, 2, 3}},
        Json::Str("nested string"),
        Json::Arr{{
            Json::Bol(true),
            Json::Arr{{4, 5}},
            nullptr
        }}
    }}};
    M_ASSERT_EQ(mixed_nested.arr().size(), 3);
    M_ASSERT_EQ(mixed_nested[0].arr().size(), 3);
    M_ASSERT_EQ(mixed_nested[1].type(), json::Type::eStr);
    M_ASSERT_EQ(mixed_nested[2].arr().size(), 3);
    M_ASSERT_EQ(mixed_nested[2][1][1].to<Json::Num>(), 5);

    Json irregular{Json::Arr{{
        Json::Arr{{1}},
        Json::Arr{{2, 3}},
        Json::Arr{{4, 5, 6}},
        Json::Arr{{7, 8, 9, 10}}
    }}};
    M_ASSERT_EQ(irregular.arr().size(), 4);
    M_ASSERT_EQ(irregular[0].arr().size(), 1);
    M_ASSERT_EQ(irregular[1].arr().size(), 2);
    M_ASSERT_EQ(irregular[2].arr().size(), 3);
    M_ASSERT_EQ(irregular[3].arr().size(), 4);
    M_ASSERT_EQ(irregular[1][1].to<Json::Num>(), 3);


// Arr8: 比较与序列化
    Json arr1{Json::Arr{{1, 2, 3}}};
    Json arr2{Json::Arr{{1, 2, 3}}};
    Json arr3{Json::Arr{{1, 2, 4}}};
    M_ASSERT_TRUE(arr1 == arr2);
    M_ASSERT_FALSE(arr1 == arr3);
    M_ASSERT_TRUE(arr1 != arr3);

    Json empty1{Json::Arr{{}}};
    Json empty2{Json::Arr{{}}};
    M_ASSERT_TRUE(empty1 == empty2);

    Json short_arr{ Json::Arr{{1, 2}}};
    Json long_arr{ Json::Arr{{1, 2, 3}}};
    M_ASSERT_FALSE(short_arr == long_arr);

    Json nested1{Json::Arr{{
        Json::Arr{{1, 2}},
        Json::Arr{{3, 4}}
    }}};
    Json nested2{Json::Arr{{
        Json::Arr{{1, 2}},
        Json::Arr{{3, 4}}
    }}};
    Json nested3{Json::Arr{{
        Json::Arr{{1, 2}},
        Json::Arr{{3, 5}}
    }}};
    M_ASSERT_TRUE(nested1 == nested2);
    M_ASSERT_FALSE(nested1 == nested3);

    Json simple3_arr{Json::Arr{{1, 2, 3}}};
    M_ASSERT_EQ(simple3_arr.dump(), "[1,2,3]");

    // empty must be use Json::Arr{} to avoid ambiguity with Json
    Json empty_serial{Json::Arr{}};
    M_ASSERT_EQ(empty_serial.dump(), "[]");

    Json empty_serial_2{Json::Arr{{} }};
    M_ASSERT_EQ(empty_serial_2.dump(), "[null]");

    Json mixed{Json::Arr{{42, "test", true, nullptr}}};
    M_ASSERT_EQ(mixed.dump(), "[42,\"test\",true,null]");
    Json nested{Json::Arr{{
        Json::Arr{{1, 2}},
        Json::Arr{{3, 4}}
    }}};
    M_ASSERT_EQ(nested.dump(), "[[1,2],[3,4]]");


// Arr9: 解析与序列化回环
    auto parsed_simple = Json::parse("[1,2,3]");
    if (parsed_simple.has_value()) {
        M_ASSERT_EQ(parsed_simple->type(), json::Type::eArr);
        M_ASSERT_EQ(parsed_simple->arr().size(), 3);
        M_ASSERT_EQ((*parsed_simple)[0].to<Json::Num>(), 1);
        M_ASSERT_EQ((*parsed_simple)[2].to<Json::Num>(), 3);
    } else {
        M_ASSERT_FAIL("Failed to parse simple array");
    }

    Json simple2_arr{Json::Arr{{1, 2, 3}}};
    auto serialized_arr = simple2_arr.dump();
    auto parsed_back_arr = Json::parse(serialized_arr);
    if (parsed_back_arr.has_value()) {
        M_ASSERT_TRUE(*parsed_back_arr == simple2_arr);
    } else {
        M_ASSERT_FAIL("Failed to parse back serialized array");
    }


// Arr10: 多维混合结构与边界
    Json simplified_multi_dim{Json::Arr{{
        Json::Arr{{
            Json::Arr{{1, 2, 3}},
            Json::Arr{{4, 5, 6}},
            Json::Arr{{7, 8, 9}}
        }},
        Json::Arr{{
            Json::Arr{{"a", "b", "c"}},
            Json::Arr{{"d", "e", "f"}},
            Json::Arr{{"g", "h", "i"}}
        }},
        Json::Arr{{
            Json::Arr{{true, false, nullptr}},
            Json::Arr{{100, "mixed", 3.14}}
        }}
    }}};
    M_ASSERT_EQ(simplified_multi_dim.arr().size(), 3);
    M_ASSERT_EQ(simplified_multi_dim[0].arr().size(), 3);
    M_ASSERT_EQ(simplified_multi_dim[1].arr().size(), 3);
    M_ASSERT_EQ(simplified_multi_dim[2].arr().size(), 2);
    M_ASSERT_EQ(simplified_multi_dim[0][0][0].to<Json::Num>(), 1);
    M_ASSERT_EQ(simplified_multi_dim[1][1][1].to<Json::Str>(), "e");
    M_ASSERT_EQ(simplified_multi_dim[2][0][2].type(), json::Type::eNul);
    M_ASSERT_EQ(simplified_multi_dim[2][0][2].is_nul(), true);
    M_ASSERT_EQ(simplified_multi_dim[2][1][1].to<Json::Str>(), "mixed");
    M_ASSERT_EQ(simplified_multi_dim[2][1][2].to<Json::Num>(), 3.14);
    M_ASSERT_THROW(std::ignore = simplified_multi_dim.at(3), std::out_of_range);
    M_ASSERT_THROW(std::ignore = simplified_multi_dim.at(0).at(3), std::out_of_range);
    M_ASSERT_THROW(std::ignore = simplified_multi_dim.at(1).at(0).at(3), std::out_of_range);
    M_ASSERT_THROW(std::ignore = simplified_multi_dim.at(2).at(2), std::out_of_range);
}
