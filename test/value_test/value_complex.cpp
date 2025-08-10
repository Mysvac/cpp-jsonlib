#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import mysvac.json;
using namespace mysvac;

M_TEST(Value, Complex) {
    // Test complex nested structures with all 6 JSON types
    // Num, Str, Bol, Nul, Array, Object

    Json complex_data = Json::Object{
        {"application", Json::Object{
            {"name", "MyApp"},
            {"version", "2.1.0"},
            {"active", true},
            {"config", Json::Object{
                {"debug", false},
                {"port", 8080},
                {"host", "localhost"},
                {"features", Json::Array{{
                    Json::Object{{"name", "auth"}, {"enabled", true}, {"priority", 1}},
                    Json::Object{{"name", "logging"}, {"enabled", false}, {"priority", 2}},
                    Json::Object{{"name", "cache"}, {"enabled", true}, {"priority", 3}}
                }}},
                {"database", Json::Object{
                    {"type", "postgresql"},
                    {"connection", Json::Object{
                        {"host", "db.example.com"},
                        {"port", 5432},
                        {"database", "myapp_db"},
                        {"ssl", true},
                        {"pool", Json::Object{
                            {"min", 5},
                            {"max", 20},
                            {"idle_timeout", 30000},
                            {"connection_timeout", nullptr}
                        }}
                    }},
                    {"migrations", Json::Array{{
                        Json::Object{{"version", "001"}, {"applied", true}, {"timestamp", 1640995200}},
                        Json::Object{{"version", "002"}, {"applied", true}, {"timestamp", 1641081600}},
                        Json::Object{{"version", "003"}, {"applied", false}, {"timestamp", nullptr}}
                    }}}
                }}
            }}
        }},
        { "users" , Json::Array{{
                Json::Object{
                    {"id", 1},
                    {"name", "John Doe"},
                    {"email", "john@example.com"},
                    {"active", true},
                    {"permissions", Json::Array{{"read", "write", "admin"}}},
                    {"profile", Json::Object{
                        {"age", 30},
                        {"country", "USA"},
                        {"preferences", Json::Object{
                            {"theme", "dark"},
                            {"notifications", true},
                            {"language", "en"},
                            {"timezone", "UTC-5"}
                        }}
                    }},
                    {"last_login", 1640995200},
                    {"metadata", nullptr}
            }},
            Json::Object{
                {"id", 2},
                {"name", "Jane Smith"},
                {"email", "jane@example.com"},
                {"active", false},
                {"permissions", Json::Array{{"read"}}},
                {"profile", Json::Object{
                    {"age", 25},
                    {"country", "Canada"},
                    {"preferences", Json::Object{
                        {"theme", "light"},
                        {"notifications", false},
                        {"language", "fr"},
                        {"timezone", "UTC-4"}
                    }}
                }},
                {"last_login", nullptr},
                {"metadata", Json::Object{{"notes", "Inactive user"}, {"created", 1640908800}}}
            }
        }},
        {"statistics", Json::Object{
            {"total_users", 2},
            {"active_users", 1},
            {"avg_login_time", 1640995200},
            {"performance", Json::Object{
                {"response_times", Json::Array{{120, 85, 95, 110, 88}}},
                {"error_rates", Json::Array{{0.01, 0.02, 0.015, 0.008, 0.012}}},
                {"uptime", 99.95},
                {"last_restart", nullptr}
            }},
            {"features_usage", Json::Array{{
                Json::Object{{"feature", "auth"}, {"usage_count", 1500}, {"success_rate", 0.98}},
                Json::Object{{"feature", "logging"}, {"usage_count", 0}, {"success_rate", nullptr}},
                Json::Object{{"feature", "cache"}, {"usage_count", 800}, {"success_rate", 0.95}}
            }}}
        }}
        // Additional nested structures can be added here
    };
    
    
    // Test basic structure validation
    M_ASSERT_EQ( complex_data.type(), json::Type::eObject );
    M_ASSERT_TRUE( complex_data.is_obj() );
    M_ASSERT_FALSE( complex_data.is_nul() );
    M_ASSERT_FALSE( complex_data.is_num() );
    M_ASSERT_FALSE( complex_data.is_arr() );
    M_ASSERT_FALSE( complex_data.is_str() );
    M_ASSERT_FALSE( complex_data.is_bol() );
    M_ASSERT_EQ( complex_data.obj().size(), 3 );
    M_ASSERT_TRUE( complex_data.obj().contains("application") );
    M_ASSERT_TRUE( complex_data.obj().contains("users") );
    M_ASSERT_TRUE( complex_data.obj().contains("statistics") );
    
    // Test deep access through [] operator
    M_ASSERT_EQ( complex_data["application"]["name"].to<Json::Str>(), "MyApp" );
    M_ASSERT_EQ( complex_data["application"]["version"].to<Json::Str>(), "2.1.0" );
    M_ASSERT_EQ( complex_data["application"]["active"].to<Json::Bol>(), true );
    M_ASSERT_EQ( complex_data["application"]["config"]["port"].to<Json::Num>(), 8080 );
    
    // Test array access within nested structure
    M_ASSERT_EQ( complex_data["application"]["config"]["features"].type(), json::Type::eArray );
    M_ASSERT_EQ( complex_data["application"]["config"]["features"].arr().size(), 3 );
    M_ASSERT_EQ( complex_data["application"]["config"]["features"][0]["name"].to<Json::Str>(), "auth" );
    M_ASSERT_EQ( complex_data["application"]["config"]["features"][0]["enabled"].to<Json::Bol>(), true );
    M_ASSERT_EQ( complex_data["application"]["config"]["features"][0]["priority"].to<Json::Num>(), 1 );
    
    // Test very deep nesting access
    M_ASSERT_EQ( complex_data["application"]["config"]["database"]["connection"]["pool"]["min"].to<Json::Num>(), 5 );
    M_ASSERT_EQ( complex_data["application"]["config"]["database"]["connection"]["pool"]["max"].to<Json::Num>(), 20 );
    M_ASSERT_EQ( complex_data["application"]["config"]["database"]["connection"]["pool"]["connection_timeout"].type(), json::Type::eNul );
    
    // Test migrations array access
    M_ASSERT_EQ( complex_data["application"]["config"]["database"]["migrations"].arr().size(), 3 );
    M_ASSERT_EQ( complex_data["application"]["config"]["database"]["migrations"][0]["version"].to<Json::Str>(), "001" );
    M_ASSERT_EQ( complex_data["application"]["config"]["database"]["migrations"][0]["applied"].to<Json::Bol>(), true );
    M_ASSERT_EQ( complex_data["application"]["config"]["database"]["migrations"][2]["timestamp"].type(), json::Type::eNul );
    
    // Test users array access
    M_ASSERT_EQ( complex_data["users"].type(), json::Type::eArray );
    M_ASSERT_EQ( complex_data["users"].arr().size(), 2 );
    M_ASSERT_EQ( complex_data["users"][0]["name"].to<Json::Str>(), "John Doe" );
    M_ASSERT_EQ( complex_data["users"][0]["permissions"].arr().size(), 3 );
    M_ASSERT_EQ( complex_data["users"][0]["permissions"][0].to<Json::Str>(), "read" );
    M_ASSERT_EQ( complex_data["users"][0]["permissions"][1].to<Json::Str>(), "write" );
    M_ASSERT_EQ( complex_data["users"][0]["permissions"][2].to<Json::Str>(), "admin" );
    
    // Test deep user profile access
    M_ASSERT_EQ( complex_data["users"][0]["profile"]["preferences"]["theme"].to<Json::Str>(), "dark" );
    M_ASSERT_EQ( complex_data["users"][0]["profile"]["preferences"]["notifications"].to<Json::Bol>(), true );
    M_ASSERT_EQ( complex_data["users"][1]["profile"]["preferences"]["theme"].to<Json::Str>(), "light" );
    M_ASSERT_EQ( complex_data["users"][1]["last_login"].type(), json::Type::eNul );
    
    // Test statistics access
    M_ASSERT_EQ( complex_data["statistics"]["total_users"].to<Json::Num>(), 2 );
    M_ASSERT_EQ( complex_data["statistics"]["performance"]["response_times"].arr().size(), 5 );
    M_ASSERT_EQ( complex_data["statistics"]["performance"]["response_times"][0].to<Json::Num>(), 120 );
    M_ASSERT_EQ( complex_data["statistics"]["performance"]["uptime"].to<Json::Num>(), 99.95 );
    
    // Test deep access through at() method
    M_ASSERT_NO_THROW( std::ignore = complex_data.at("application").at("config").at("database").at("connection").at("pool").at("min") );
    M_ASSERT_EQ( complex_data.at("application").at("config").at("database").at("connection").at("pool").at("min").to<Json::Num>(), 5 );
    
    // Test array at() access
    M_ASSERT_NO_THROW( std::ignore = complex_data.at("users").at(0).at("permissions").at(1) );
    M_ASSERT_EQ( complex_data.at("users").at(0).at("permissions").at(1).to<Json::Str>(), "write" );
    
    // Test bounds checking with at()
    M_ASSERT_THROW( std::ignore = complex_data.at("nonexistent"), std::out_of_range );
    M_ASSERT_THROW( std::ignore = complex_data.at("users").at(0).at("permissions").at(10), std::out_of_range );
    M_ASSERT_THROW( std::ignore = complex_data.at("users").at(5), std::out_of_range );
    
    // Test deep modification through [] operator
    complex_data["application"]["config"]["port"] = 9090;
    M_ASSERT_EQ( complex_data["application"]["config"]["port"].to<Json::Num>(), 9090 );
    
    complex_data["application"]["config"]["features"][0]["enabled"] = false;
    M_ASSERT_EQ( complex_data["application"]["config"]["features"][0]["enabled"].to<Json::Bol>(), false );
    
    complex_data["users"][0]["profile"]["preferences"]["theme"] = "auto";
    M_ASSERT_EQ( complex_data["users"][0]["profile"]["preferences"]["theme"].to<Json::Str>(), "auto" );
    
    // Test deep modification through at() method
    complex_data.at("statistics").at("total_users") = 3;
    M_ASSERT_EQ( complex_data.at("statistics").at("total_users").to<Json::Num>(), 3 );
    
    complex_data.at("users").at(1).at("active") = true;
    M_ASSERT_EQ( complex_data.at("users").at(1).at("active").to<Json::Bol>(), true );
    
    // Test adding new nested elements
    complex_data["application"]["config"]["new_feature"] = Json::Object{
        {"name", "monitoring"},
        {"enabled", true},
        {"settings", Json::Array{{
            Json::Object{{"metric", "cpu"}, {"threshold", 0.8}},
            Json::Object{{"metric", "memory"}, {"threshold", 0.9}}
        }}}
    };
    
    M_ASSERT_TRUE( complex_data["application"]["config"].obj().contains("new_feature") );
    M_ASSERT_EQ( complex_data["application"]["config"]["new_feature"]["name"].to<Json::Str>(), "monitoring" );
    M_ASSERT_EQ( complex_data["application"]["config"]["new_feature"]["settings"].arr().size(), 2 );
    M_ASSERT_EQ( complex_data["application"]["config"]["new_feature"]["settings"][0]["metric"].to<Json::Str>(), "cpu" );
    
    // Test copy semantics
    Json complex_copy = complex_data;
    M_ASSERT_TRUE( complex_copy == complex_data );
    M_ASSERT_EQ( complex_copy.obj().size(), complex_data.obj().size() );
    
    // Modify copy and ensure original is unchanged
    complex_copy["application"]["name"] = "CopiedApp";
    M_ASSERT_EQ( complex_copy["application"]["name"].to<Json::Str>(), "CopiedApp" );
    M_ASSERT_EQ( complex_data["application"]["name"].to<Json::Str>(), "MyApp" );
    M_ASSERT_FALSE( complex_copy == complex_data );
    
    // Test move semantics for std::map, std::vector, std::string
    std::map<std::string, Json> test_map{
        {"key1", Json::Num(100)},
        {"key2", Json::Str("value2")},
        {"key3", Json::Bol(true)}
    };
    
    std::vector<Json> test_vector{
        Json::Num(1),
        Json::Str("two"),
        Json::Bol(false),
        Json::Object{{"nested", "value"}}
    };
    
    std::string test_string = "movable_string";
    
    // Test move construction
    Json moved_object{std::move(test_map)};
    Json moved_array{std::move(test_vector)};
    Json moved_string{std::move(test_string)};
    
    M_ASSERT_EQ( moved_object.type(), json::Type::eObject );
    M_ASSERT_EQ( moved_object.obj().size(), 3 );
    M_ASSERT_EQ( moved_object["key1"].to<Json::Num>(), 100 );
    M_ASSERT_EQ( moved_object["key2"].to<Json::Str>(), "value2" );
    M_ASSERT_EQ( moved_object["key3"].to<Json::Bol>(), true );
    
    M_ASSERT_EQ( moved_array.type(), json::Type::eArray );
    M_ASSERT_EQ( moved_array.arr().size(), 4 );
    M_ASSERT_EQ( moved_array[0].to<Json::Num>(), 1 );
    M_ASSERT_EQ( moved_array[1].to<Json::Str>(), "two" );
    M_ASSERT_EQ( moved_array[2].to<Json::Bol>(), false );
    M_ASSERT_EQ( moved_array[3]["nested"].to<Json::Str>(), "value" );
    
    M_ASSERT_EQ( moved_string.type(), json::Type::eStr );
    M_ASSERT_EQ( moved_string.to<Json::Str>(), "movable_string" );
    
    // Test move assignment
    Json move_target{};
    
    std::map<std::string, Json> another_map{{"move_key", Json::Num(999)}};
    move_target = std::move(another_map);
    
    M_ASSERT_EQ( move_target.type(), json::Type::eObject );
    M_ASSERT_EQ( move_target.obj().size(), 1 );
    M_ASSERT_EQ( move_target["move_key"].to<Json::Num>(), 999 );
    
    // Test complex structure with move semantics
    std::vector<Json> complex_array{
        Json::Object{
            {"item1", Json::Array{{1, 2, 3}}},
            {"item2", Json::Object{{"nested", true}}}
        },
        Json::Array{{
            Json::Str("first"),
            42,
            Json::Object{{"deep", Json::Array{{"very", "deep", "nesting"}}}}
        }}
    };
    
    Json moved_complex{std::move(complex_array)};
    M_ASSERT_EQ( moved_complex.type(), json::Type::eArray );
    M_ASSERT_EQ( moved_complex.arr().size(), 2 );
    M_ASSERT_EQ( moved_complex[0]["item1"].arr().size(), 3 );
    M_ASSERT_EQ( moved_complex[0]["item1"][0].to<Json::Num>(), 1 );
    M_ASSERT_EQ( moved_complex[0]["item2"]["nested"].to<Json::Bol>(), true );
    M_ASSERT_EQ( moved_complex[1][0].to<Json::Str>(), "first" );
    M_ASSERT_EQ( moved_complex[1][2]["deep"][0].to<Json::Str>(), "very" );
    
    // Test get<T>() with complex structures (returns copies)
    auto complex_obj_copy = complex_data.to<Json::Object>();
    M_ASSERT_EQ( complex_obj_copy.size(), 3 );
    M_ASSERT_TRUE( complex_obj_copy.contains("application") );
    
    auto users_array_copy = complex_data["users"].to<Json::Array>();
    M_ASSERT_EQ( users_array_copy.size(), 2 );
    M_ASSERT_EQ( users_array_copy[0]["name"].to<Json::Str>(), "John Doe" );
    
    auto features_array_copy = complex_data["application"]["config"]["features"].to<Json::Array>();
    M_ASSERT_EQ( features_array_copy.size(), 3 );
    M_ASSERT_EQ( features_array_copy[0]["name"].to<Json::Str>(), "auth" );
    
    // Test get_ref<T>() with complex structures (returns references)
    auto& complex_obj_ref = complex_data.obj();
    M_ASSERT_EQ( complex_obj_ref.size(), 3 );
    M_ASSERT_TRUE( complex_obj_ref.contains("application") );
    
    auto& users_array_ref = complex_data["users"].arr();
    M_ASSERT_EQ( users_array_ref.size(), 2 );
    M_ASSERT_EQ( users_array_ref[0]["name"].to<Json::Str>(), "John Doe" );
    
    auto& features_array_ref = complex_data["application"]["config"]["features"].arr();
    M_ASSERT_EQ( features_array_ref.size(), 3 );
    M_ASSERT_EQ( features_array_ref[0]["name"].to<Json::Str>(), "auth" );
    
    // Test get_ref<T>() with modifications
    auto& stats_ref = complex_data["statistics"].obj();
    stats_ref["new_metric"] = Json::Num(123.45);
    M_ASSERT_TRUE( complex_data["statistics"].obj().contains("new_metric") );
    M_ASSERT_EQ( complex_data["statistics"]["new_metric"].to<Json::Num>(), 123.45 );
    
    auto& response_times_ref = complex_data["statistics"]["performance"]["response_times"].arr();
    response_times_ref.push_back(Json::Num(75));
    M_ASSERT_EQ( complex_data["statistics"]["performance"]["response_times"].arr().size(), 6 );
    M_ASSERT_EQ( complex_data["statistics"]["performance"]["response_times"][5].to<Json::Num>(), 75 );
    
    // Test size() method on nested structures
    M_ASSERT_EQ( complex_data.obj().size(), 3 );
    M_ASSERT_EQ( complex_data["application"].obj().size(), 4 );
    M_ASSERT_EQ( complex_data["application"]["config"].obj().size(), 6 );  // debug, port, host, features, database, new_feature
    M_ASSERT_EQ( complex_data["users"].arr().size(), 2 );
    M_ASSERT_EQ( complex_data["users"][0].obj().size(), 8 );
    M_ASSERT_EQ( complex_data["users"][0]["profile"].obj().size(), 3 );
    M_ASSERT_EQ( complex_data["statistics"].obj().size(), 6 );  // total_users, active_users, avg_login_time, performance, features_usage, new_metric
    M_ASSERT_EQ( complex_data["statistics"]["performance"].obj().size(), 4 );
    
    // Test equality comparison with complex structures
    Json complex_clone{Json::Object{
        {"application", Json::Object{
            {"name", "MyApp"},
            {"version", "2.1.0"},
            {"active", true},
            {"config", Json::Object{
                {"debug", false},
                {"port", 9090},  // Modified value
                {"host", "localhost"},
                {"features", Json::Array{{
                    Json::Object{{"name", "auth"}, {"enabled", false}, {"priority", 1}},  // Modified
                    Json::Object{{"name", "logging"}, {"enabled", false}, {"priority", 2}},
                    Json::Object{{"name", "cache"}, {"enabled", true}, {"priority", 3}}
                }}},
                {"database", Json::Object{
                    {"type", "postgresql"},
                    {"connection", Json::Object{
                        {"host", "db.example.com"},
                        {"port", 5432},
                        {"database", "myapp_db"},
                        {"ssl", true},
                        {"pool", Json::Object{
                            {"min", 5},
                            {"max", 20},
                            {"idle_timeout", 30000},
                            {"connection_timeout", nullptr}
                        }}
                    }},
                    {"migrations", Json::Array{{
                        Json::Object{{"version", "001"}, {"applied", true}, {"timestamp", 1640995200}},
                        Json::Object{{"version", "002"}, {"applied", true}, {"timestamp", 1641081600}},
                        Json::Object{{"version", "003"}, {"applied", false}, {"timestamp", nullptr}}
                    }}}
                }},
                {"new_feature", Json::Object{  // Added feature
                    {"name", "monitoring"},
                    {"enabled", true},
                    {"settings", Json::Array{{
                        Json::Object{{"metric", "cpu"}, {"threshold", 0.8}},
                        Json::Object{{"metric", "memory"}, {"threshold", 0.9}}
                    }}}
                }}
            }}
        }},
        {"users", Json::Array{{
            Json::Object{
                {"id", 1},
                {"name", "John Doe"},
                {"email", "john@example.com"},
                {"active", true},
                {"permissions", Json::Array{{"read", "write", "admin"}}},
                {"profile", Json::Object{
                    {"age", 30},
                    {"country", "USA"},
                    {"preferences", Json::Object{
                        {"theme", "auto"},  // Modified
                        {"notifications", true},
                        {"language", "en"},
                        {"timezone", "UTC-5"}
                    }}
                }},
                {"last_login", 1640995200},
                {"metadata", nullptr}
            }},
            Json::Object{
                {"id", 2},
                {"name", "Jane Smith"},
                {"email", "jane@example.com"},
                {"active", true},  // Modified
                {"permissions", Json::Array{{"read"}}},
                {"profile", Json::Object{
                    {"age", 25},
                    {"country", "Canada"},
                    {"preferences", Json::Object{
                        {"theme", "light"},
                        {"notifications", false},
                        {"language", "fr"},
                        {"timezone", "UTC-4"}
                    }}
                }},
                {"last_login", nullptr},
                {"metadata", Json::Object{{"notes", "Inactive user"}, {"created", 1640908800}}}
            }
        }},
        {"statistics", Json::Object{
            {"total_users", 3},  // Modified
            {"active_users", 1},
            {"avg_login_time", 1640995200},
            {"performance", Json::Object{
                {"response_times", Json::Array{{120, 85, 95, 110, 88, 75}}},  // Added element
                {"error_rates", Json::Array{{0.01, 0.02, 0.015, 0.008, 0.012}}},
                {"uptime", 99.95},
                {"last_restart", nullptr}
            }},
            {"features_usage", Json::Array{{
                Json::Object{{"feature", "auth"}, {"usage_count", 1500}, {"success_rate", 0.98}},
                Json::Object{{"feature", "logging"}, {"usage_count", 0}, {"success_rate", nullptr}},
                Json::Object{{"feature", "cache"}, {"usage_count", 800}, {"success_rate", 0.95}}
            }}},
            {"new_metric", 123.45}  // Added metric
        }}
    }};
    
    M_ASSERT_TRUE( complex_data == complex_clone );
    
    // Test inequality by changing one deep value
    complex_clone["users"][0]["profile"]["preferences"]["theme"] = "different";
    M_ASSERT_FALSE( complex_data == complex_clone );
    
    // Test serialization of complex structure
    M_ASSERT_NO_THROW( std::ignore = complex_data.dump() );
    auto serialized = complex_data.dump();
    M_ASSERT_TRUE( serialized.size() > 1000 );  // Should be quite large
    
    // Test pretty serialization
    M_ASSERT_NO_THROW( std::ignore = complex_data.dumpf() );
    auto pretty_serialized = complex_data.dumpf();
    M_ASSERT_TRUE( pretty_serialized.size() > serialized.size() );
    
    // Test round-trip parsing
    auto parsed_complex = Json::parse(serialized);
    if (parsed_complex.has_value()) {
        M_ASSERT_TRUE( *parsed_complex == complex_data );
        
        // Test deep access on parsed data
        M_ASSERT_EQ( (*parsed_complex)["application"]["config"]["port"].to<Json::Num>(), 9090 );
        M_ASSERT_EQ( (*parsed_complex)["users"][0]["profile"]["preferences"]["theme"].to<Json::Str>(), "auto" );
        M_ASSERT_EQ( (*parsed_complex)["statistics"]["new_metric"].to<Json::Num>(), 123.45 );
    } else {
        M_ASSERT_FAIL("Failed to parse complex structure");
    }
    
    // Test type validation throughout complex structure
    M_ASSERT_EQ( complex_data.type(), json::Type::eObject );
    M_ASSERT_EQ( complex_data["application"].type(), json::Type::eObject );
    M_ASSERT_EQ( complex_data["application"]["name"].type(), json::Type::eStr );
    M_ASSERT_EQ( complex_data["application"]["active"].type(), json::Type::eBol );
    M_ASSERT_EQ( complex_data["application"]["config"]["port"].type(), json::Type::eNum );
    M_ASSERT_EQ( complex_data["application"]["config"]["features"].type(), json::Type::eArray );
    M_ASSERT_EQ( complex_data["application"]["config"]["database"]["connection"]["pool"]["connection_timeout"].type(), json::Type::eNul );
    M_ASSERT_EQ( complex_data["users"].type(), json::Type::eArray );
    M_ASSERT_EQ( complex_data["users"][0].type(), json::Type::eObject );
    M_ASSERT_EQ( complex_data["users"][0]["permissions"].type(), json::Type::eArray );
    M_ASSERT_EQ( complex_data["users"][0]["permissions"][0].type(), json::Type::eStr );
    M_ASSERT_EQ( complex_data["users"][1]["last_login"].type(), json::Type::eNul );
    M_ASSERT_EQ( complex_data["statistics"]["performance"]["uptime"].type(), json::Type::eNum );
    
    // Test contains() method on various levels
    M_ASSERT_TRUE( complex_data.obj().contains("application") );
    M_ASSERT_TRUE( complex_data.obj().contains("users") );
    M_ASSERT_TRUE( complex_data.obj().contains("statistics") );
    M_ASSERT_FALSE( complex_data.obj().contains("nonexistent") );
    
    M_ASSERT_TRUE( complex_data["application"].obj().contains("config") );
    M_ASSERT_TRUE( complex_data["application"]["config"].obj().contains("database") );
    M_ASSERT_TRUE( complex_data["application"]["config"]["database"].obj().contains("connection") );
    M_ASSERT_FALSE( complex_data["application"]["config"]["database"].obj().contains("nonexistent") );
    
    M_ASSERT_TRUE( complex_data["users"][0].obj().contains("profile") );
    M_ASSERT_TRUE( complex_data["users"][0]["profile"].obj().contains("preferences") );
    M_ASSERT_FALSE( complex_data["users"][0]["profile"].obj().contains("nonexistent") );
    
    // Test performance with large nested structures
    Json large_nested{Json::Object{}};
    for (int i = 0; i < 10; ++i) {
        std::string section_name = "section_" + std::to_string(i);
        large_nested[section_name] = Json::Object{};
        
        for (int j = 0; j < 10; ++j) {
            std::string subsection_name = "subsection_" + std::to_string(j);
            large_nested[section_name][subsection_name] = Json::Array{};

            for (int k = 0; k < 10; ++k) {
                large_nested[section_name][subsection_name].arr().push_back(
                    Json::Object{
                        {"id", k},
                        {"name", "item_" + std::to_string(k)},
                        {"active", k % 2 == 0},
                        {"value", k * 10.5},
                        {"metadata", (k % 3 == 0) ? Json{nullptr} : Json{Json::Object{{"info", "data"}}}}
                    }
                );
            }
        }
    }
    
    // Test access to large nested structure
    M_ASSERT_EQ( large_nested.obj().size(), 10 );
    M_ASSERT_EQ( large_nested["section_5"]["subsection_7"].arr().size(), 10 );
    M_ASSERT_EQ( large_nested["section_5"]["subsection_7"][8]["id"].to<Json::Num>(), 8 );
    M_ASSERT_EQ( large_nested["section_5"]["subsection_7"][8]["name"].to<Json::Str>(), "item_8" );
    M_ASSERT_EQ( large_nested["section_5"]["subsection_7"][8]["active"].to<Json::Bol>(), true );
    M_ASSERT_EQ( large_nested["section_5"]["subsection_7"][8]["value"].to<Json::Num>(), 84.0 );
    M_ASSERT_EQ( large_nested["section_5"]["subsection_7"][9]["metadata"].type(), json::Type::eNul );
    M_ASSERT_EQ( large_nested["section_5"]["subsection_7"][7]["metadata"]["info"].to<Json::Str>(), "data" );
    
    // Test serialization performance on large structure
    M_ASSERT_NO_THROW( std::ignore = large_nested.dump() );
    auto large_serialized = large_nested.dump();
    M_ASSERT_TRUE( large_serialized.size() > 10000 );
    
    // Test final validation of all modifications
    M_ASSERT_EQ( complex_data["application"]["config"]["port"].to<Json::Num>(), 9090 );
    M_ASSERT_EQ( complex_data["application"]["config"]["features"][0]["enabled"].to<Json::Bol>(), false );
    M_ASSERT_EQ( complex_data["users"][0]["profile"]["preferences"]["theme"].to<Json::Str>(), "auto" );
    M_ASSERT_EQ( complex_data["statistics"]["total_users"].to<Json::Num>(), 3 );
    M_ASSERT_EQ( complex_data["users"][1]["active"].to<Json::Bol>(), true );
    M_ASSERT_TRUE( complex_data["statistics"].obj().contains("new_metric") );
    M_ASSERT_EQ( complex_data["statistics"]["new_metric"].to<Json::Num>(), 123.45 );
}
