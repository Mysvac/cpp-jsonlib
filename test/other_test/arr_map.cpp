#include <vct/test_unit_macros.hpp>

import std;
import vct.test.unit;
import mysvac.json;

using namespace mysvac;

M_TEST(ARR_MAP, Simple) {
    const std::vector arr_int = { 1, 2, 3, 4, 5 };
    const Json v_arr_int{ arr_int };
    const Json v_arr_int_res = Json::Array{ {1, 2, 3, 4, 5} };
    M_EXPECT_EQ( v_arr_int, v_arr_int_res );

}

M_TEST(ARR_MAP, ArrayTypes) {
    // std::array<int, N>
    const std::array<int, 4> arr = {10, 20, 30, 40};
    Json v_arr{ arr };
    M_EXPECT_EQ( v_arr, (Json::Array{{10, 20, 30, 40}}) );

    // std::list<double>
    const std::list<double> lst = {1.1, 2.2, 3.3};
    Json v_lst{lst};
    M_EXPECT_EQ(v_lst, (Json::Array{{1.1, 2.2, 3.3}}) );

    // std::deque<bool>
    const std::deque<bool> deq = {true, false, true};
    Json v_deq{deq};
    M_EXPECT_EQ(v_deq, (Json::Array{{true, false, true}}));

    // std::vector<std::string>
    const std::vector<std::string> vec_str = {"a", "b", "c"};
    Json v_vec_str{vec_str};
    M_EXPECT_EQ(v_vec_str, (Json::Array{{"a", "b", "c"}}) );

    // std::vector<Json>
    const std::vector<Json> vec_val = {Json::Num(1), Json::Bol(false), Json::Str("x")};
    Json v_vec_val{vec_val};
    M_EXPECT_EQ(v_vec_val, (Json::Array{{1, false, "x"}}) );
}

M_TEST(ARR_MAP, MapTypes) {
    // std::map<std::string, int>
    const std::map<std::string, int> map_int = {{"a", 1}, {"b", 2}};
    Json v_map_int{map_int};
    M_EXPECT_EQ(v_map_int["a"], 1);
    M_EXPECT_EQ(v_map_int["b"], 2);

    // std::map<std::string, bool>
    const std::map<std::string, bool> map_bool = {{"x", true}, {"y", false}};
    Json v_map_bool{map_bool};
    M_EXPECT_EQ(v_map_bool["x"], true);
    M_EXPECT_EQ(v_map_bool["y"], false);

    // std::map<std::string, std::string>
    const std::map<std::string, std::string> map_str = {{"foo", "bar"}, {"baz", "qux"}};
    Json v_map_str{map_str};
    M_EXPECT_EQ(v_map_str["foo"], "bar");
    M_EXPECT_EQ(v_map_str["baz"], "qux");

    // std::map<std::string, Json>
    const std::map<std::string, Json> map_val = {{"n", Json::Num(42)}, {"b", Json::Bol(true)}, {"s", Json::Str("str")}};
    Json v_map_val{map_val};
    M_EXPECT_EQ(v_map_val["n"], 42);
    M_EXPECT_EQ(v_map_val["b"], true);
    M_EXPECT_EQ(v_map_val["s"], "str");
}

M_TEST(ARR_MAP, UnorderedMapTypes) {
    // std::unordered_map<std::string, double>
    const std::unordered_map<std::string, double> umap_num = {{"pi", 3.14}, {"e", 2.718}};
    Json v_umap_num{umap_num};
    M_EXPECT_EQ(v_umap_num["pi"], 3.14);
    M_EXPECT_EQ(v_umap_num["e"], 2.718);

    // std::unordered_map<std::string, bool>
    const std::unordered_map<std::string, bool> umap_bool = {{"t", true}, {"f", false}};
    Json v_umap_bool{umap_bool};
    M_EXPECT_EQ(v_umap_bool["t"], true);
    M_EXPECT_EQ(v_umap_bool["f"], false);

    // std::unordered_map<std::string, std::string>
    const std::unordered_map<std::string, std::string> umap_str = {{"hello", "world"}, {"key", "value"}};
    Json v_umap_str{umap_str};
    M_EXPECT_EQ(v_umap_str["hello"], "world");
    M_EXPECT_EQ(v_umap_str["key"], "value");

    // std::unordered_map<std::string, Json>
    const std::unordered_map<std::string, Json> umap_val = {{"num", Json::Num(7)}, {"flag", Json::Bol(false)}, {"txt", Json::Str("abc")}};
    Json v_umap_val{umap_val};
    M_EXPECT_EQ(v_umap_val["num"], 7);
    M_EXPECT_EQ(v_umap_val["flag"], false);
    M_EXPECT_EQ(v_umap_val["txt"], "abc");
}

M_TEST(ARR_MAP, NestedContainers) {
    // Nested array
    const std::vector<std::vector<int>> nested_vec = {{1,2}, {3,4}};
    Json v_nested_vec{nested_vec};
    M_EXPECT_EQ(v_nested_vec[0], (Json::Array{{1,2}}));
    M_EXPECT_EQ(v_nested_vec[1], (Json::Array{{3,4}}));

    // Nested map
    const std::map<std::string, std::map<std::string, int>> nested_map = {{"outer", {{"inner", 99}}}};
    Json v_nested_map{nested_map};
    M_EXPECT_EQ(v_nested_map["outer"]["inner"], 99);

    // Array of maps
    const std::vector<std::map<std::string, bool>> arr_of_map = { {{"a", true}}, {{"b", false}} };
    Json v_arr_of_map{arr_of_map};
    M_EXPECT_EQ(v_arr_of_map[0]["a"], true);
    M_EXPECT_EQ(v_arr_of_map[1]["b"], false);

    // Map of arrays
    const std::map<std::string, std::vector<std::string>> map_of_arr = {{"letters", {"a", "b"}}, {"digits", {"1", "2"}}};
    Json v_map_of_arr{map_of_arr};
    M_EXPECT_EQ(v_map_of_arr["letters"], (Json::Array{{"a", "b"}}));
    M_EXPECT_EQ(v_map_of_arr["digits"], (Json::Array{{"1", "2"}}));
}
