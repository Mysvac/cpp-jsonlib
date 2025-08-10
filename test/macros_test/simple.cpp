#include <vct/test_unit_macros.hpp>
#define M_MYSVAC_JSON_SIMPLIFY_MACROS
#include <mysvac/json_macros.hpp>
import std;
import vct.test.unit;
import mysvac.json;


using namespace mysvac;

struct MyData{
    int id{};
    std::string m_name{};
    bool active{false};
    double m_value{};

    MyData() = default; // default constructor is needed

    M_JSON_CV_FUN( MyData,  // Conversion constructor, must in public section
        M_JSON_CV_MEM( id );    // `;` is optional, but can not be `,`
        M_JSON_CV_MAP( name, m_name )
        M_JSON_CV_MEM( active )
        M_JSON_CV_MAP( value, m_value )
    )
    M_JSON_CS_FUN( MyData,  // Conversion constructor, must in public section
        M_JSON_CS_MEM( id )
        M_JSON_CS_MAP( name, m_name )
        M_JSON_CS_MEM_OR( active, true, nullptr ) // default value is `true`, nullptr means no range value
        M_JSON_CS_MAP_OR( value, m_value, 64.0, nullptr ) // default value is `true`, nullptr means no range value
    )
};

M_TEST(Macros, Simple) {

    {
        Json v_null;
        MyData d_null{ v_null };
        M_ASSERT_EQ(d_null.id, 0);
        M_ASSERT_EQ(d_null.m_name, "");
        M_ASSERT_EQ(d_null.active, true); // 默认值为true
        M_ASSERT_EQ(d_null.m_value, 64.0); // 默认值为64.0
    }

    {
        Json v_object{ Json::Object{} };
        v_object["id"] = 42;
        v_object["name"] = "Test User";
        v_object["active"] = false;
        v_object["value"] = 128.0;
        MyData d_object{ v_object };
        M_ASSERT_EQ(d_object.id, 42);
        M_ASSERT_EQ(d_object.m_name, "Test User");
        M_ASSERT_EQ(d_object.active, false);
        M_ASSERT_EQ(d_object.m_value, 128.0);
    }

    {
        MyData d_out{};
        d_out.id = 23;
        d_out.m_name = "Output User";
        d_out.active = true;
        d_out.m_value = 256.0;
        const auto v_out = Json{ d_out };
        M_ASSERT_EQ(v_out["id"].to<int>(), 23);
        M_ASSERT_EQ(v_out["name"].to<std::string>(), "Output User");
        M_ASSERT_EQ(v_out["active"].to<bool>(), true);
        M_ASSERT_EQ(v_out["value"].to<double>(), 256.0);
    }

    {
        Json v_mover{ Json::Object{} };
        v_mover["name"] = "Test move xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
        MyData d_recipient{ std::move(v_mover) };
        M_ASSERT_EQ(d_recipient.m_name, "Test move xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
        M_ASSERT_EQ(v_mover.type(), json::Type::eNull);
    }

}



