#include <vct/test_unit_macros.hpp>

#define M_MYSVAC_JSON_SIMPLIFY_MACROS // 定义宏，以启用简化的宏函数名
#include <mysvac/json_macros.hpp>
import std; // 建议启用 C++23 的 std 模块支持
import mysvac.json; // 导入 vct-tools-json 库
import vct.test.unit;


using namespace mysvac; // 使用命名空间，简化代码书写


struct MyData{
    int id{};
    std::string m_name{};
    bool active{false};
    double m_value{};

    MyData() = default; // 必须存在默认构造，内容可以自定义

    M_JSON_CV_FUN( MyData,  // 转换函数，必须在 public 作用域
        M_JSON_CV_MEM( id );    // 注意，MyData 后面必须有 `,`
        M_JSON_CV_MAP( name, m_name )   // 但是剩余的字段后面不能有逗号 `,` ，分号 `;` 则是可选的
        M_JSON_CV_MEM( active )
        M_JSON_CV_MAP( value, m_value )
    )
    M_JSON_CS_FUN( MyData,  // 构造函数，必须在 public 作用域
        M_JSON_CS_MEM( id )
        M_JSON_CS_MAP( name, m_name )
        M_JSON_CS_MEM_OR( active, true, nullptr ) // 默认值是 `true`
        M_JSON_CS_MAP_OR( value, m_value, 64.0, nullptr ) // nullptr 表示此类型不需要子元素默认值
    )
};


struct MyData2 {
    std::string name;   // std::string 等于 json::String，因此可以直接使用
    MyData my_data;     // MyData 已经有转换函数和构造函数，因此可以直接使用
    std::vector<MyData> data_list;  // 能够直接使用的类型构成的列表也能直接使用（但再套一层列表就不行了）
    MyData2() = default;
    M_JSON_CV_FUN( MyData2,
        M_JSON_CV_MEM( name )
        M_JSON_CV_MAP( data, my_data )
        M_JSON_CV_MEM( data_list )
    )
    M_JSON_CS_FUN( MyData2,
        M_JSON_CS_MEM( name )
        M_JSON_CS_MAP( data, my_data )
        M_JSON_CS_MEM_OR( data_list, std::vector<MyData>{}, MyData{} ) // 变量名，默认值，内部子元素的默认值
    )
};

M_TEST(README, readme) {
    // {
    //     Json null_val;                  // 默认构造，类型为 Null
    //     Json bool_val(3.3);             // 浮点初始化，类型为 Number
    //     Json obj_val = Json::Object{};  // 直接使用 Object 初始化
    //
    //     Json enum_val{ json::Type::eObject }; // 危险
    //
    //     Json smp_val = Json::Object{
    //     { "key1", 42 },
    //     {"key2", "value2"},
    //     {"key3", true },
    //     {"arr", Json::Array{ { 2, 3.14, nullptr } } },
    //     {"obj", Json::Object{ {"nested_k", "nested_v"} } }
    //     };
    //
    //     std::ignore = smp_val.type();        // 返回 Type::eObject
    //     std::ignore = json::type_name(smp_val.type());  // 返回 "Object"
    //     std::ignore = smp_val.is_arr();     // 返回 false
    //
    //     Json& vi_42 = smp_val.obj()["key1"];
    //
    //     // 虽然返回引用，但也可以用于赋值
    //     double i_42 = vi_42.num();
    //
    //     std::ignore = smp_val["arr"][1].num(); // 返回 3.14
    //     smp_val.at("obj").at("nested_k") = nullptr; // 修改对象，变为 Null 类型
    //     std::ignore = smp_val["obj"].at("nested_k").is_nul(); // 返回true, [] 和 at 可以随意混合使用
    //
    //     auto str_view = smp_val["key2"].to<std::string_view>(); // 返回内部字符串的视图
    //     auto str = smp_val["key2"].move<std::string>(); // 将内部字符串移动了出来，现在内部变为空字符串，之前的视图不再可用
    //     int int_42 = smp_val["key1"].to<int>(); // 返回内部整数的拷贝
    //
    //     auto opt_str = smp_val["key1"].to_if<std::string>(); // opt_str 是 std::optional<std::string> ，转换失败为空，但不抛出异常
    //     if(opt_str) std::cout << *opt_str << std::endl; // 如果转换成功，输出字符串
    //     std::string or_str = smp_val["key1"].to_or<std::string>("default"); // 如果转换失败，返回 "default"
    //
    //     std::string json_str1 = R"( [ 1, false, null, { "Hello": "World" } ] )";
    //     std::string json_str2 = R"( false )"; // 允许顶层类型是任一 JSON 类型
    //     Json val1 = Json::parse(json_str1).value_or( nullptr ); // 解析 JSON 字符串
    //     std::cout << val1[1].to<bool>() << std::endl; // 输出 0 （没有指定 boolaplha）
    //
    //     std::string str_ser = val1.dump(); // 不含无效空白字符的高效序列化，返回 std::string
    //     std::string str_back;
    //     val1.write( str_back ); // 将序列化结果写入 std::string 的末尾
    //     val1.write( std::cout ); // 将序列化结果直接输出到 `ostream` 中
    //
    //     std::string pretty_str = val1.dumpf();
    //     val1.writef( std::cout ); // 输出到 `ostream`
    //     // 还有 writef 写入字符串末尾，此处省略
    //
    //     Json val_arr_1 = Json::Array{{ 1, 2, 3 }};
    //     Json val_arr_2 = Json::Array{{ 1, 2, 3 }};
    //     Json val_arr_3 = Json::Array{{ 1, true, 3 }};
    //     val_arr_1 == val_arr_2; // true
    //     val_arr_1 == val_arr_3; // false
    //
    //
    //
    // }
    //
    // {
    //     Json v_null;
    //     MyData d_null{ v_null }; // 什么都没有，因此全部字段都是 CS 中的默认值
    //     d_null.active; // true，因为 CS 函数指定了默认值为 true
    //
    //     Json v_object{ Json::Object{} };
    //     v_object["id"] = 42;
    //     v_object["name"] = "Test User";
    //     v_object["active"] = false;
    //     v_object["value"] = 128.0;
    //     MyData d_object{ v_object };    // 必须显式转换，不能用 `=` 构造
    //     d_object.m_name == "Test User"; // true
    //
    //     Json v_data{ d_object }; // 将 MyData 转换为 JSON 对象
    //     v_data["id"] == 42; // true
    // }
    //
    // {
    //     Json v_data2{ MyData2{} };
    //     std::println("");
    //     v_data2.writef( std::cout );
    //     std::println("");
    //     v_data2["data"]["id"] = 8848;
    //     v_data2["data"]["name"] = "Mount Everest";
    //     v_data2["data"]["active"] = true;
    //     v_data2["data_list"].arr().push_back( v_data2["data"] );
    //     v_data2["name"] = "name_name";
    //     MyData2 d_data2{ v_data2 };
    //     M_EXPECT_TRUE( d_data2.my_data.id == 8848 ); // true
    //     M_EXPECT_TRUE( d_data2.my_data.m_name == "Mount Everest" ); // true
    //     M_EXPECT_TRUE( d_data2.data_list.size() == 1 ); // true
    //     M_EXPECT_TRUE( d_data2.data_list[0].id == 8848 ); // true
    //     M_EXPECT_TRUE( d_data2.data_list[0].m_name == "Mount Everest" ); // true
    //     M_EXPECT_TRUE( d_data2.name == "name_name" ); // true
    // }

}


