#include <vct/test_unit_macros.hpp>
#define M_MYSVAC_JSON_SIMPLIFY_MACROS
#include <mysvac/json_macros.hpp>
import std;
import vct.test.unit;
import mysvac.json;


using namespace mysvac;

// 定义内部类型，支持宏转换
struct InnerType {
    int id{};
    std::string name{};
    bool flag{true};

    InnerType() = default;

    InnerType(int i, const char * str, bool cond) {
        this->id = i;
        this->name = str;
        this->flag = cond;
    }

    M_JSON_CV_FUN(InnerType,
        M_JSON_CV_MEM(id)
        M_JSON_CV_MEM(name)
        M_JSON_CV_MEM(flag)
    )
    M_JSON_CS_FUN(InnerType,
        M_JSON_CS_MEM(id)
        M_JSON_CS_MEM_OR(name, "default", nullptr)
        M_JSON_CS_MEM_OR(flag, true, nullptr)
    )
};

// 外部类型，包含InnerType成员
struct OuterType {
    double value{};
    InnerType inner{};
    std::string desc{};

    OuterType() = default;

    OuterType(double x, const InnerType & inner, const char * str) {
        this->value = x;
        this->inner = inner;
        this->desc = str;
    }

    M_JSON_CV_FUN(OuterType,
        M_JSON_CV_MEM(value)
        M_JSON_CV_MEM(inner)
        M_JSON_CV_MEM(desc)
    )
    M_JSON_CS_FUN(OuterType,
        M_JSON_CS_MEM(value)
        M_JSON_CS_MEM(inner)    // equal to OR(inner, InnerType{}, nullptr), is not Value{}.to_or<InnerType>( InnerType{} );
        M_JSON_CS_MEM_OR(desc, "empty", nullptr)
    )
};

// 新类型，测试 M_JSON_CS_MEM_OR 的行为
struct OuterTypeWithOr {
    double value{};
    InnerType inner{};
    std::string desc{};

    OuterTypeWithOr() = default;

    M_JSON_CV_FUN(OuterTypeWithOr,
        M_JSON_CV_MEM(value)
        M_JSON_CV_MEM(inner)
        M_JSON_CV_MEM(desc)
    )
    // 注意这里 inner 的默认值来自 JSON解析
    M_JSON_CS_FUN(OuterTypeWithOr,
        M_JSON_CS_MEM(value)
        M_JSON_CS_MEM_OR(inner, InnerType(99, "json_default", false), nullptr)
        M_JSON_CS_MEM_OR(desc, "empty", nullptr)
    )
};

M_TEST(Macros, Complex) {
    // 1. 构造并序列化
    OuterType o1;
    o1.value = 3.14;
    o1.inner.id = 7;
    o1.inner.name = "hello";
    o1.inner.flag = false;
    o1.desc = "outer";
    Json v1{o1};
    M_ASSERT_EQ(v1["value"].to<double>(), 3.14);
    M_ASSERT_EQ(v1["inner"]["id"].to<int>(), 7);
    M_ASSERT_EQ(v1["inner"]["name"].to<std::string>(), "hello");
    M_ASSERT_EQ(v1["inner"]["flag"].to<bool>(), false);
    M_ASSERT_EQ(v1["desc"].to<std::string>(), "outer");


    // 2. 反序列化
    Json v2{ Json::Obj{} };
    v2["value"] = 2.718;
    v2["inner"] = Json::Obj{};
    v2["inner"]["id"] = 42;
    v2["inner"]["name"] = "world";
    v2["inner"]["flag"] = true;
    v2["desc"] = "macro";
    OuterType o2{v2};
    M_ASSERT_EQ(o2.value, 2.718);
    M_ASSERT_EQ(o2.inner.id, 42);
    M_ASSERT_EQ(o2.inner.name, "world");
    M_ASSERT_EQ(o2.inner.flag, true);
    M_ASSERT_EQ(o2.desc, "macro");

    // 3. 默认值测试
    Json v3{ Json::Obj{} };
    v3["value"] = 1.0;
    // inner未设置，desc未设置
    OuterType o3{ v3 };
    M_ASSERT_EQ(o3.value, 1.0);
    // inner为默认构造，内容为空（id=0, name="", flag=true）
    M_ASSERT_EQ(o3.inner.id, 0);
    M_ASSERT_EQ(o3.inner.name, "");
    M_ASSERT_EQ(o3.inner.flag, true);
    M_ASSERT_EQ(o3.desc, "empty");


    // 4. move语义测试
    OuterType o4;
    o4.value = 9.9;
    o4.inner.id = 99;
    o4.inner.name = "move";
    o4.inner.flag = false;
    o4.desc = "move_desc";
    Json v4{std::move(o4)};
    M_ASSERT_EQ(v4["value"].to<double>(), 9.9);
    M_ASSERT_EQ(v4["inner"]["id"].to<int>(), 99);
    M_ASSERT_EQ(v4["inner"]["name"].to<std::string>(), "move");
    M_ASSERT_EQ(v4["inner"]["flag"].to<bool>(), false);
    M_ASSERT_EQ(v4["desc"].to<std::string>(), "move_desc");


    // 5. 嵌套类型的序列化/反序列化一致性
    OuterType o5{v1};
    M_ASSERT_EQ(o5.value, 3.14);
    M_ASSERT_EQ(o5.inner.id, 7);           // 应与 v1["inner"]["id"] 一致
    M_ASSERT_EQ(o5.inner.name, "hello");   // 应与 v1["inner"]["name"] 一致
    M_ASSERT_EQ(o5.inner.flag, false);     // 应与 v1["inner"]["flag"] 一致
    M_ASSERT_EQ(o5.desc, "outer");
}

M_TEST(Macros, ComplexArrMap) {
    // InnerType数组
    std::vector<InnerType> inner_vec{
        InnerType{1, "A", true},
        InnerType{2, "B", false}
    };
    Json v_inner_vec{inner_vec};
    M_ASSERT_EQ(v_inner_vec.type(), json::Type::eArr);
    auto back_vec = v_inner_vec.to<std::vector<InnerType>>( InnerType{ });
    M_ASSERT_EQ(back_vec.size(), 2);
    M_ASSERT_EQ(back_vec[0].id, 1);
    M_ASSERT_EQ(back_vec[0].name, "A");
    M_ASSERT_EQ(back_vec[0].flag, true);
    M_ASSERT_EQ(back_vec[1].id, 2);
    M_ASSERT_EQ(back_vec[1].name, "B");
    M_ASSERT_EQ(back_vec[1].flag, false);

    // InnerType map
    std::map<std::string, InnerType> inner_map{
        {"x", InnerType{3, "X", true}},
        {"y", InnerType{4, "Y", false}}
    };
    Json v_inner_map{inner_map};
    M_ASSERT_EQ(v_inner_map.type(), json::Type::eObj);
    auto back_map = v_inner_map.to<std::map<std::string, InnerType>>( InnerType{} );
    M_ASSERT_EQ(back_map.size(), 2);
    M_ASSERT_EQ(back_map["x"].id, 3);
    M_ASSERT_EQ(back_map["x"].name, "X");
    M_ASSERT_EQ(back_map["x"].flag, true);
    M_ASSERT_EQ(back_map["y"].id, 4);
    M_ASSERT_EQ(back_map["y"].name, "Y");
    M_ASSERT_EQ(back_map["y"].flag, false);

    // OuterType数组
    std::vector<OuterType> outer_vec{
        OuterType{1.1, InnerType{5, "V", true}, "desc1"},
        OuterType{2.2, InnerType{6, "W", false}, "desc2"}
    };
    Json v_outer_vec{outer_vec};
    M_ASSERT_EQ(v_outer_vec.type(), json::Type::eArr);
    auto back_outer_vec = v_outer_vec.to<std::vector<OuterType>>( OuterType{} );
    M_ASSERT_EQ(back_outer_vec.size(), 2);
    M_ASSERT_EQ(back_outer_vec[0].value, 1.1);
    M_ASSERT_EQ(back_outer_vec[0].inner.id, 5);
    M_ASSERT_EQ(back_outer_vec[0].desc, "desc1");
    M_ASSERT_EQ(back_outer_vec[1].value, 2.2);
    M_ASSERT_EQ(back_outer_vec[1].inner.id, 6);
    M_ASSERT_EQ(back_outer_vec[1].desc, "desc2");

    // OuterType map
    std::map<std::string, OuterType> outer_map{
        {"a", OuterType{3.3, InnerType{7, "A", true}, "descA"}},
        {"b", OuterType{4.4, InnerType{8, "B", false}, "descB"}}
    };
    Json v_outer_map{outer_map};
    M_ASSERT_EQ(v_outer_map.type(), json::Type::eObj);
    auto back_outer_map = v_outer_map.to<std::map<std::string, OuterType>>( OuterType{} );
    M_ASSERT_EQ(back_outer_map.size(), 2);
    M_ASSERT_EQ(back_outer_map["a"].value, 3.3);
    M_ASSERT_EQ(back_outer_map["a"].inner.id, 7);
    M_ASSERT_EQ(back_outer_map["a"].desc, "descA");
    M_ASSERT_EQ(back_outer_map["b"].value, 4.4);
    M_ASSERT_EQ(back_outer_map["b"].inner.id, 8);
    M_ASSERT_EQ(back_outer_map["b"].desc, "descB");
}

M_TEST(Macros, ComplexDefaultBehavior) {
    // 默认构造，未设置 inner
    Json v_obj{ Json::Obj{} };
    v_obj["value"] = 1.23;
    // 不设置 inner
    OuterType o1{v_obj};
    OuterTypeWithOr o2{v_obj};

    // OuterType: inner 用默认构造
    M_ASSERT_EQ(o1.inner.id, 0);
    M_ASSERT_EQ(o1.inner.name, "");
    M_ASSERT_EQ(o1.inner.flag, true);

    // OuterTypeWithOr: inner 用 JSON解析的默认值
    M_ASSERT_EQ(o2.inner.id, 99);
    M_ASSERT_EQ(o2.inner.name, "json_default");
    M_ASSERT_EQ(o2.inner.flag, false);
}
