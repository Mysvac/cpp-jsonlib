/**
 * @file json.ixx
 * @author Mysvac
 * @date 2025-08-10
 *
 * This module provides a JSON parser and serializer.
 */
module;

#ifndef M_MYSVAC_STD_MODULE

#include <cmath>
#include <cstdint>
#include <charconv>
#include <format>
#include <iostream>
#include <iomanip>
#include <type_traits>
#include <array>
#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <unordered_map>
#include <variant>
#include <optional>
#include <iterator>
#include <ranges>

#endif

export module mysvac.json;

#ifdef M_MYSVAC_STD_MODULE

import std;

#endif

/**
 * @namespace mysvac::json
 * @brief Namespace for JSON related tools and types.
 * @note Non-export content.
 */
namespace mysvac::json {
    /**
     * @brief Concept to check if a type is a character iterator.
     * @note Non-export.
     */
    template<typename T>
    concept char_iterator =  std::disjunction_v<
        std::is_same<T, std::string_view::const_iterator>,
        std::is_same<T, std::istreambuf_iterator<char>>
    >;

    /**
     * @brief A lookup table for hexadecimal characters.
     * @note Non-export.
     */
    constexpr std::array<std::uint8_t, 256> hex_table = [] {
        std::array<std::uint8_t, 256> table{};
        for (int i = 0; i < 256; ++i) table[i] = 255; // Invalid marker
        for (int i = 0; i <= 9; ++i) table['0' + i] = i;
        for (int i = 0; i <= 5; ++i) table['A' + i] = 10 + i;
        for (int i = 0; i <= 5; ++i) table['a' + i] = 10 + i;
        return table;
    }();
}

/**
 * @namespace mysvac::json
 * @brief Namespace for JSON related tools and types.
 * @note Export content.
 */
export namespace mysvac::json {

    /**
     * @brief Concept to check if a type is a JSON type.
     * @tparam J The JSON Container Class.
     * @tparam T The type to check.
     */
    template<typename J, typename T>
    concept json_type = std::disjunction_v<
        std::is_same<T, typename J::Nul>,
        std::is_same<T, typename J::Bol>,
        std::is_same<T, typename J::Num>,
        std::is_same<T, typename J::Str>,
        std::is_same<T, typename J::Arr>,
        std::is_same<T, typename J::Obj>
    >;

    /**
     * @brief Concept to check if JSON Container class is directly constructible from a type.
     * @tparam J The JSON Container Class.
     * @tparam T The type to check.
     */
    template<typename J, typename T>
    concept constructible = std::disjunction_v<
        std::is_arithmetic<T>,
        std::is_enum<T>,
        std::is_convertible<T, typename J::Arr>,
        std::is_convertible<T, typename J::Obj>,
        std::is_convertible<T, typename J::Str>,
        std::is_convertible<T, typename J::Num>,
        std::is_convertible<T, typename J::Bol>,
        std::is_convertible<T, typename J::Nul>
    >;

    /**
     * @brief Concept to check if JSON Container class is constructible from an array-like type.
     * @tparam J The JSON Container Class.
     * @tparam T The type to check.
     */
    template<typename J, typename T>
    concept constructible_array = std::ranges::range<T> &&
        std::is_constructible_v<J, typename T::value_type>;

    /**
     * @brief Concept to check if JSON Container class is constructible from a map-like type.
     * @tparam J The JSON Container Class.
     * @tparam T The type to check.
     */
    template<typename J, typename T>
    concept constructible_map = std::ranges::range<T> &&
        std::is_convertible_v<typename T::key_type, typename J::Str> &&
        std::is_constructible_v<J, typename T::mapped_type>;

    /**
     * @brief Concept to check if JSON Container class is directly convertible to a type.
     * @tparam J The JSON Container Class.
     * @tparam T The type to check.
     */
    template<typename J, typename T>
    concept convertible = std::disjunction_v<
        std::is_arithmetic<T>,
        std::is_enum<T>,
        std::is_convertible<typename J::Arr, T>,
        std::is_convertible<typename J::Obj, T>,
        std::is_convertible<typename J::Str, T>,
        std::is_convertible<typename J::Num, T>,
        std::is_convertible<typename J::Bol, T>,
        std::is_convertible<typename J::Nul, T>,
        std::is_constructible<T, J>
    >;

    /**
     * @brief Concept to check if JSON Container class is convertible to a map-like type.
     * @tparam J The JSON Container Class.
     * @tparam T The type to check.
     * @tparam D The default mapped_type in map-like type.
     */
    template<typename J, typename T, typename D>
    concept convertible_map = std::ranges::range<T> && requires {
        requires std::is_constructible_v<typename T::mapped_type, D>;
        requires std::is_convertible_v<typename J::Str, typename T::key_type>;
        requires convertible<J, typename T::mapped_type>;
        requires std::is_default_constructible_v<T>;
        requires std::is_default_constructible_v<typename T::mapped_type>;
        requires std::is_copy_constructible_v<typename T::mapped_type>;
    } && requires (T t, typename J::Str s, typename T::mapped_type m) {
        t.emplace(static_cast<typename T::key_type>(s), std::move(m));
    };

    /**
     * @brief Concept to check if JSON Container class is convertible to an array-like type.
     * @tparam J The JSON Container Class.
     * @tparam T The type to check.
     * @tparam D The default value_type in array-like type.
     */
    template<typename J, typename T, typename D>
    concept convertible_array =  std::ranges::range<T> && requires {
        requires std::is_constructible_v<typename T::value_type,D>;
        requires convertible<J, typename T::value_type>;
        requires std::is_default_constructible_v<T>;
        requires std::is_default_constructible_v<typename T::value_type>;
        requires std::is_copy_constructible_v<typename T::value_type>;
    } && requires (T t, typename T::value_type v) {
        t.emplace_back(std::move(v));
    };

    /**
     * @brief Enum class representing the type of JSON data.
     */
    enum class Type{
        eNul = 0,  ///< Nul type
        eBol,      ///< boolean type
        eNum,    ///< Num type
        eStr,    ///< Str type
        eArr,     ///< Arr type
        eObj     ///< Obj type
    };


    /**
     * @brief Get the name of the JSON data type.
     * @return The name of the JSON data type as a string.
     */
    [[nodiscard]]
    constexpr const char* type_name(const Type type) noexcept {
        switch ( type ) {
            case Type::eObj: return "Obj";
            case Type::eArr:  return "Arr";
            case Type::eStr: return "Str";
            case Type::eNum: return "Num";
            case Type::eBol:   return "Bol";
            case Type::eNul:   return "Nul";
            default: return "Unknown Enum Value"; // should never happen
        }
    }

    /**
     * @brief A JSON container class that can represent various JSON data types.
     * @tparam UseOrderedMap  Use `std::map` for JSON objects if true, otherwise use `std::unordered_map`.
     * @tparam VecAllocator A allocator template for the vector containers, default is `std::allocator`.
     * @tparam MapAllocator A allocator template for the map/unordered_map containers, default is `std::allocator`.
     * @tparam StrAllocator A allocator template for the string containers, default is `std::allocator`.
     * @note Str is always `std::basic_string< ... >`。
     */
    template<
        bool UseOrderedMap = true,
        template<typename U> class VecAllocator = std::allocator,
        template<typename U> class MapAllocator = std::allocator,
        template<typename U> class StrAllocator = std::allocator
    >
    requires requires{
        typename std::basic_string<char, std::char_traits<char>, StrAllocator<char>>;
        typename std::vector<int, VecAllocator<int>>;
        typename std::map<std::string, int, std::less<std::string>, MapAllocator<std::pair<const std::string, int>>>;
        typename std::unordered_map<std::string, int, std::hash<std::string>, std::equal_to<std::string>, MapAllocator<std::pair<const std::string, int>>>;
    }
    class Json {
    public:
        /**
         * @brief Json's Nul Type, must be `std::nullptr_t`.
         */
        using Nul = std::nullptr_t;
        /**
         * @brief Json's Bol Type, must be `bool`.
         */
        using Bol = bool;
        /**
         * @brief Json's Num Type, must be `double`.
         * @note When converted to integers, it will be rounded to the nearest integer.
         */
        using Num = double;
        /**
         * @brief Json's Str Type, `std::basic_string<char, std::char_traits<char>, StrAllocator<char>>`.
         * @note default is `std::string`.
         */
        using Str = std::basic_string<char, std::char_traits<char>, StrAllocator<char>>;
        /**
         * @brief Json's Arr Type, `vector<Json, VecAllocator<Json>>`.
         * @note default is `std::vector<Json>`.
         */
        using Arr = std::vector<Json, VecAllocator<Json>>;
        /**
         * @brief Json's Obj Type, `std::map` or `std::unordered_map`.
         * @note default is `std::map<std::string, Json>`.
         */
        using Obj = std::conditional_t<UseOrderedMap,
            std::map<Str, Json, std::less<Str>, MapAllocator<std::pair<const Str, Json>>>,
            std::unordered_map<Str, Json, std::hash<Str>, std::equal_to<Str>, MapAllocator<std::pair<const Str, Json>>>
        >;

    protected:
        std::variant<
            Nul,
            Bol,
            Num,
            Str,
            Arr,
            Obj
        > m_data { Nul{} };

    private:
        /**
         * @brief Escape a string to a string back.
         * @param out The output string to append the escaped string to.
         * @param str The string to escape.
         */
        static void escape_to(Str& out, const std::string_view str) noexcept {
            out.push_back('\"');
            for (const char c : str) {
                switch (c) {
                    case '\\': out.append(R"(\\)"); break;
                    case '\"': out.append(R"(\")"); break;
                    case '\r': out.append(R"(\r)"); break;
                    case '\n': out.append(R"(\n)"); break;
                    case '\f': out.append(R"(\f)"); break;
                    case '\t': out.append(R"(\t)"); break;
                    case '\b': out.append(R"(\b)"); break;
                    default: {
                        if (static_cast<unsigned char>(c) < 0x20) {
                            out.append(std::format("\\u{:04x}", static_cast<unsigned char>(c)));
                        } else out.push_back(c);
                    } break;
                }
            }
            out.push_back('"');
        }

        /**
         * @brief Escape a string to an out-stream.
         * @param out The output stream to append the escaped string to.
         * @param str The string to escape.
         */
        static void escape_to(std::ostream& out, const std::string_view str) {
            out.put('\"');
            for (const char c : str) {
                switch (c) {
                    case '\\': out << R"(\\)"; break;
                    case '\"': out << R"(\")"; break;
                    case '\r': out << R"(\r)"; break;
                    case '\n': out << R"(\n)"; break;
                    case '\f': out << R"(\f)"; break;
                    case '\t': out << R"(\t)"; break;
                    case '\b': out << R"(\b)"; break;
                    default: {
                        if (static_cast<unsigned char>(c) < 0x20) {
                            out << std::format("\\u{:04x}", static_cast<unsigned char>(c));
                        } else out.put(c);
                    } break;
                }
            }
            out.put('\"');
        }

        /**
         * @brief Unescape a Unicode escape sequence in a string, and move ptr.
         * @param out The output string to append the unescaped Unicode character to.
         * @param it The iterator pointing to the current position in the string.
         * @param end_ptr The end iterator of the string.
         * @return `true` if the unescape was successful, `false` if it failed.
         */
        static bool unescape_unicode_next(
            Str& out,
            char_iterator auto& it,
            const char_iterator auto end_ptr
        ) noexcept {
            // it was in `\uABCD`'s `u` position
            ++it;
            if (it == end_ptr) return false;
            // `it` was in `\uXXXX`'s A position

            // move to \uABCD's D position and get hex4 value
            std::uint32_t code_point{ 0 };
            {
                const std::uint8_t d1 = hex_table[static_cast<unsigned char>(*it)];
                if (d1 == 255) return false; // Invalid if not a hex digit
                code_point = code_point << 4 | d1;

                ++it; // External `++it` may reduce some instructions
                if(it == end_ptr) return false; // Invalid if not enough characters
                const std::uint8_t d2 = hex_table[static_cast<unsigned char>(*it)];
                if (d2 == 255) return false; // Invalid if not a hex digit
                code_point = code_point << 4 | d2;

                ++it;
                if(it == end_ptr) return false; // Invalid if not enough characters
                const std::uint8_t d3 = hex_table[static_cast<unsigned char>(*it)];
                if (d3 == 255) return false; // Invalid if not a hex digit
                code_point = code_point << 4 | d3;

                ++it;
                if(it == end_ptr) return false; // Invalid if not enough characters
                const std::uint8_t d4 = hex_table[static_cast<unsigned char>(*it)];
                if (d4 == 255) return false; // Invalid if not a hex digit
                code_point = code_point << 4 | d4;
            }
            // `it` was in `\uABCD`'s D position and not be `end_ptr`, if hex4_next successful

            // [0xD800 , 0xE000) is agent pair, which is two consecutive \u encoding
            if (code_point >= 0xD800 && code_point <= 0xDFFF) {
                // agent pair, must be high agent + low agent
                // high agent [\uD800, \uDBFF]
                // low agent [\uDC00, \uDFFF]

                // first char must be high agent
                if (code_point >= 0xDC00) return false;

                // second char must be low agent
                ++it;
                if(it == end_ptr || *it != '\\') return false;
                ++it;
                if(it == end_ptr || *it != 'u') return false;
                ++it;
                if(it == end_ptr) return false;
                // `it` was in `\uXXXX`'s A position, and be not end_ptr

                // move to \uABCD's D position and get hex4 value
                std::uint32_t low_code_point{ 0 };
                {
                    const std::uint8_t d1 = hex_table[static_cast<unsigned char>(*it)];
                    if (d1 == 255) return false; // Invalid if not a hex digit
                    low_code_point = low_code_point << 4 | d1;

                    ++it; // External `++it` may reduce some instructions
                    if(it == end_ptr) return false; // Invalid if not enough characters
                    const std::uint8_t d2 = hex_table[static_cast<unsigned char>(*it)];
                    if (d2 == 255) return false; // Invalid if not a hex digit
                    low_code_point = low_code_point << 4 | d2;

                    ++it;
                    if(it == end_ptr) return false; // Invalid if not enough characters
                    const std::uint8_t d3 = hex_table[static_cast<unsigned char>(*it)];
                    if (d3 == 255) return false; // Invalid if not a hex digit
                    low_code_point = low_code_point << 4 | d3;

                    ++it;
                    if(it == end_ptr) return false; // Invalid if not enough characters
                    const std::uint8_t d4 = hex_table[static_cast<unsigned char>(*it)];
                    if (d4 == 255) return false; // Invalid if not a hex digit
                    low_code_point = low_code_point << 4 | d4;
                }
                if( 0xDFFF < low_code_point ||  low_code_point < 0xDC00 ) return false;
                // `it` was in `\uABCD`'s D position and not be `end_ptr`, if hex4_next successful

                // combine the agent pair into a single code point
                code_point = 0x10000 + ((code_point - 0xD800) << 10) + (low_code_point - 0xDC00);
            }

            // encode the code point to UTF-8
            if (code_point <= 0x7F) {
                out.push_back(static_cast<char>(code_point));
            } else if (code_point <= 0x7FF) {
                out.push_back(static_cast<char>(code_point >> 6 | 0xC0));
                out.push_back(static_cast<char>(code_point & 0x3F | 0x80));
            } else if (code_point <= 0xFFFF) {
                out.push_back(static_cast<char>(code_point >> 12 | 0xE0));
                out.push_back(static_cast<char>(code_point >> 6 & 0x3F | 0x80));
                out.push_back(static_cast<char>(code_point & 0x3F | 0x80));
            } else if (code_point <= 0x10FFFF) {
                out.push_back(static_cast<char>(code_point >> 18 | 0xF0));
                out.push_back(static_cast<char>(code_point >> 12 & 0x3F | 0x80));
                out.push_back(static_cast<char>(code_point >> 6 & 0x3F | 0x80));
                out.push_back(static_cast<char>(code_point & 0x3F | 0x80));
            } else return false;
            return true;
        }

        /**
         * @brief Unescape the string in a JSON string, and move ptr.
         * @param it The iterator pointing to the current position in the string.
         * @param end_ptr The end iterator of the string.
         * @return An expected Str containing the unescaped string, or a ParseError if an error occurred.
         */
        static std::optional<Str> unescape_next(
            char_iterator auto& it,
            const char_iterator auto end_ptr
        ) noexcept {
            Str res;
            ++it;
            if (it != end_ptr && *it != '\"')  res.reserve( 128 );

            while (it != end_ptr && *it != '\"') {
                if (*it == '\\') {
                    ++it;
                    if (it == end_ptr) return std::nullopt;
                    switch (*it) {
                        case '\"': res.push_back('\"'); break;
                        case '\\': res.push_back('\\'); break;
                        case 'n':  res.push_back('\n'); break;
                        case 'r':  res.push_back('\r'); break;
                        case 't':  res.push_back('\t'); break;
                        case 'f':  res.push_back('\f'); break;
                        case 'b':  res.push_back('\b'); break;
                        case 'u': case 'U': if (!unescape_unicode_next(res, it, end_ptr)) return std::nullopt; break;
                        default: return std::nullopt;
                    }
                } else if ( *it == '\b' || *it == '\n' || *it == '\f' || *it == '\r' /* || *it == '\t' */) {
                    return std::nullopt;
                } else {
                    res.push_back( *it );
                }
                ++it;
            }
            if(it == end_ptr) return std::nullopt;
            ++it;
            res.shrink_to_fit();
            return res;
        }

        /**
         * @brief Read a JSON value from the input iterator and create Json Obj.
         * @param it The iterator pointing to the current position in the input.
         * @param end_ptr The end iterator of the input.
         * @param max_depth The maximum depth of nested JSON objects/arrays allowed.
         * @return An expected Json object containing the parsed JSON value, or a ParseError if an error occurred.
         */
        static std::optional<Json> reader(
            char_iterator auto& it,
            const char_iterator auto end_ptr,
            const std::int32_t max_depth
        ) noexcept {
            // Check for maximum depth
            if(max_depth < 0) return std::nullopt;
            // return value
            Json json;
            // Check the first character to determine the type
            switch (*it) {
                case '{': {
                    // Obj type
                    ++it;
                    json = Obj{};
                    auto& object = json.obj();
                    // Parse the object
                    while(it != end_ptr){
                        // Skip spaces
                        while (it != end_ptr && std::isspace(*it)) ++it;
                        if(it == end_ptr || *it == '}') break;
                        // find key
                        if (*it != '\"') return std::nullopt;
                        auto key = unescape_next(it, end_ptr);
                        if(!key) return std::nullopt;
                        // find ':'
                        while (it != end_ptr && std::isspace(*it)) ++it;
                        if(it == end_ptr || *it != ':') return std::nullopt;
                        ++it;
                        // find value
                        while (it != end_ptr && std::isspace(*it)) ++it;
                        if (it == end_ptr) break;
                        auto value = reader(it, end_ptr, max_depth - 1);
                        if(!value) return value;
                        // add to object
                        object.emplace(std::move(*key), std::move(*value));

                        while(it != end_ptr && std::isspace(*it)) ++it;
                        if(it == end_ptr) break;
                        if(*it == ',') ++it;
                        else if(*it != '}') return std::nullopt;
                    }
                    if(it == end_ptr) return std::nullopt;
                    ++it;
                } break;
                case '[': {
                    // Arr type
                    ++it;
                    json = Arr{};
                    auto& array = json.arr();
                    if (it != end_ptr && *it != ']') array.reserve(8);
                    while(it != end_ptr){
                        // Skip spaces
                        while (it != end_ptr && std::isspace(*it)) ++it;
                        if(it == end_ptr || *it == ']') break;
                        // find value
                        auto value = reader(it, end_ptr, max_depth - 1);
                        if(!value) return value;
                        // add to array
                        array.emplace_back(std::move(*value));

                        while(it != end_ptr && std::isspace(*it)) ++it;
                        if(it == end_ptr) break;
                        if(*it == ',') ++it;
                        else if(*it != ']') return std::nullopt;
                    }
                    if(it == end_ptr) return std::nullopt;
                    ++it;
                    array.shrink_to_fit();
                } break;
                case '\"': {
                    // Str type
                    auto str = unescape_next(it, end_ptr);
                    if(!str) return std::nullopt;
                    json = std::move(*str);
                } break;
                case 't': {
                    // true
                    if (++it == end_ptr || *it != 'r' ||
                        ++it == end_ptr || *it != 'u' ||
                        ++it == end_ptr || *it != 'e'
                    ) return std::nullopt;
                    json = Bol{true};
                    ++it;
                } break;
                case 'f': {
                    // false
                    if (++it == end_ptr || *it != 'a' ||
                        ++it == end_ptr || *it != 'l' ||
                        ++it == end_ptr || *it != 's' ||
                        ++it == end_ptr || *it != 'e'
                    ) return std::nullopt;
                    json = Bol{false};
                    ++it;
                } break;
                case 'n': {
                    // null
                    if (++it == end_ptr || *it != 'u' ||
                        ++it == end_ptr || *it != 'l' ||
                        ++it == end_ptr || *it != 'l'
                    ) return std::nullopt;
                    json = Nul{};
                    ++it;
                } break;
                default: {
                    // number
                    std::uint8_t buffer_len{};
                    char buffer[26];    // Reserve enough space for typical numbers
                    // std::array<char, 26> buffer;

                    while(buffer_len < 26 && it != end_ptr &&
                        (std::isdigit(*it)  || *it=='-' || *it=='.' || *it=='e' || *it=='E' || *it=='+')
                    ) buffer[buffer_len++] = *it++;
                    if( buffer_len == 0 || buffer_len == 26 ) return std::nullopt;

                    Num value;
                    if(const auto [ptr, ec] = std::from_chars(buffer, buffer + buffer_len, value);
                        ec != std::errc{} || ptr != buffer + buffer_len
                    ) return std::nullopt;

                    json = value;
                } break;
            }
            return json;
        }

    public:
        /**
         * @brief Get the type of the JSON data.
         * @return The type of the JSON data as a Type enum value.
         */
        [[nodiscard]]
        constexpr Type type() const noexcept { return static_cast<Type>(m_data.index()); }

        /**
         * @brief Check if the JSON data is of type Nul.
         */
        [[nodiscard]]
        constexpr bool is_nul() const noexcept { return type() == Type::eNul; }

        /**
         * @brief Check if the JSON data is of type Bol.
         */
        [[nodiscard]]
        constexpr bool is_bol() const noexcept { return type() == Type::eBol; }

        /**
         * @brief Check if the JSON data is of type Num.
         */
        [[nodiscard]]
        constexpr bool is_num() const noexcept { return type() == Type::eNum; }

        /**
         * @brief Check if the JSON data is of type Str.
         */
        [[nodiscard]]
        constexpr bool is_str() const noexcept { return type() == Type::eStr; }

        /**
         * @brief Check if the JSON data is of type Arr.
         */
        [[nodiscard]]
        constexpr bool is_arr() const noexcept { return type() == Type::eArr; }

        /**
         * @brief Check if the JSON data is of type Obj.
         */
        [[nodiscard]]
        constexpr bool is_obj() const noexcept { return type() == Type::eObj; }


        /**
         * @brief Get a reference to the Nul type.
         * @throw std::bad_variant_access if the JSON data is not of type Nul.
         */
        [[nodiscard]]
        constexpr Nul& nul() & { return std::get<Nul>(m_data); }
        [[nodiscard]]
        constexpr Nul&& nul() && { return std::get<Nul>(std::move(m_data)); }
        [[nodiscard]]
        constexpr const Nul& nul() const & { return std::get<Nul>(m_data); }
        [[nodiscard]]
        constexpr const Nul&& nul() const && { return std::get<Nul>(std::move(m_data)); }

        /**
         * @brief Get a reference to the Bol type.
         * @throw std::bad_variant_access if the JSON data is not of type Bol.
         */
        [[nodiscard]]
        constexpr Bol& bol() & { return std::get<Bol>(m_data); }
        [[nodiscard]]
        constexpr Bol&& bol() && { return std::get<Bol>(std::move(m_data)); }
        [[nodiscard]]
        constexpr const Bol& bol() const & { return std::get<Bol>(m_data); }
        [[nodiscard]]
        constexpr const Bol&& bol() const && { return std::get<Bol>(std::move(m_data)); }

        /**
         * @brief Get a reference to the Num type.
         * @throw std::bad_variant_access if the JSON data is not of type Num.
         */
        [[nodiscard]]
        constexpr Num& num() & { return std::get<Num>(m_data); }
        [[nodiscard]]
        constexpr Num&& num() && { return std::get<Num>(std::move(m_data)); }
        [[nodiscard]]
        constexpr const Num& num() const & { return std::get<Num>(m_data); }
        [[nodiscard]]
        constexpr const Num&& num() const && { return std::get<Num>(std::move(m_data)); }

        /**
         * @brief Get a reference to the Str type.
         * @throw std::bad_variant_access if the JSON data is not of type Str.
         */
        [[nodiscard]]
        constexpr Str& str() & { return std::get<Str>(m_data); }
        [[nodiscard]]
        constexpr Str&& str() && { return std::get<Str>(std::move(m_data)); }
        [[nodiscard]]
        constexpr const Str& str() const & { return std::get<Str>(m_data); }
        [[nodiscard]]
        constexpr const Str&& str() const && { return std::get<Str>(std::move(m_data)); }

        /**
         * @brief Get a reference to the Arr type.
         * @throw std::bad_variant_access if the JSON data is not of type Arr.
         */
        [[nodiscard]]
        constexpr Arr& arr() & { return std::get<Arr>(m_data); }
        [[nodiscard]]
        constexpr Arr&& arr() && { return std::get<Arr>(std::move(m_data)); }
        [[nodiscard]]
        constexpr const Arr& arr() const & { return std::get<Arr>(m_data); }
        [[nodiscard]]
        constexpr const Arr&& arr() const && { return std::get<Arr>(std::move(m_data)); }

        /**
         * @brief Get a reference to the Obj type.
         * @throw std::bad_variant_access if the JSON data is not of type Obj.
         */
        [[nodiscard]]
        constexpr Obj& obj() & { return std::get<Obj>(m_data); }
        [[nodiscard]]
        constexpr Obj&& obj() && { return std::get<Obj>(std::move(m_data)); }
        [[nodiscard]]
        constexpr const Obj& obj() const & { return std::get<Obj>(m_data); }
        [[nodiscard]]
        constexpr const Obj&& obj() const && { return std::get<Obj>(std::move(m_data)); }

        /**
         * @brief Default constructor for Json, data is Nul type.
         */
        constexpr Json() noexcept = default;
        /**
         * @brief Destructor for Json, no special cleanup needed.
         */
        ~Json() noexcept = default;
        /**
         * @brief Copy constructor for Json, uses default copy semantics.
         */
        Json(const Json&) noexcept = default;
        /**
         * @brief Copy assignment operator for Json, uses default copy semantics.
         */
        Json& operator=(const Json&) noexcept = default;

        /**
         * @brief Move constructor for Json, transfers ownership of data.
         * @param other The Json object to move from, will be set to Nul.
         */
        Json(Json&& other) noexcept {
            m_data = std::move(other.m_data);
            other.m_data = Nul{};
        }
        /**
         * @brief Move assignment operator for Json, transfers ownership of data.
         * @param other The Json object to move from, will be set to Nul.
         */
        Json& operator=(Json&& other) noexcept {
            if (this == &other) return *this;
            m_data = std::move(other.m_data);
            other.m_data = Nul{};
            return *this;
        }

        /**
         * @brief Constructor for Json that accepts various types and converts them to Json.
         * @tparam T The type of the other object to convert to Json.
         * @param other The object to convert to Json.
         */
        template<typename T>
        requires constructible<Json, std::remove_cvref_t<T>>
        Json(T&& other) noexcept {
            if constexpr(std::is_same_v<T, Nul>) {
                m_data = Nul{};
            } else if constexpr(std::is_same_v<T, Bol>) {
                m_data = other;
            } else if constexpr(std::is_same_v<T, Num>) {
                m_data = other;
            } else if constexpr(std::is_same_v<T, Str>) {
                m_data = std::forward<T>(other);
            } else if constexpr(std::is_same_v<T, Arr>) {
                m_data = std::forward<T>(other);
            } else if constexpr(std::is_same_v<T, Obj>) {
                m_data = std::forward<T>(other);
            } else if constexpr(std::is_arithmetic_v<T> || std::is_enum_v<T>) {
                m_data = static_cast<Num>(other);
            } else if constexpr(std::is_convertible_v<T, Str>) {
                m_data = static_cast<Str>(std::forward<T>(other));
            } else if constexpr(std::is_convertible_v<T, Obj>) {
                m_data = static_cast<Obj>(std::forward<T>(other));
            } else if constexpr(std::is_convertible_v<T,Arr>) {
                m_data = static_cast<Arr>(std::forward<T>(other));
            } else if constexpr(std::is_convertible_v<T,Num>) {
                m_data = static_cast<Num>(std::forward<T>(other));
            } else if constexpr(std::is_convertible_v<T,Bol>) {
                m_data = static_cast<Bol>(std::forward<T>(other));
            } else {
                m_data = Nul{};
            }
        }

        /**
         * @brief Assignment operator for Json that accepts various types and converts them to Json.
         * @tparam T The type of the other object to convert to Json.
         * @param other The object to convert to Json.
         */
        template<typename T>
        requires constructible<Json, std::remove_cvref_t<T>>
        Json& operator=(T&& other) noexcept {
            if constexpr(std::is_same_v<T, Nul>) {
                m_data = Nul{};
            } else if constexpr(std::is_same_v<T, Bol>) {
                m_data = other;
            } else if constexpr(std::is_same_v<T, Num>) {
                m_data = other;
            } else if constexpr(std::is_same_v<T, Str>) {
                m_data = std::forward<T>(other);
            } else if constexpr(std::is_same_v<T, Arr>) {
                m_data = std::forward<T>(other);
            } else if constexpr(std::is_same_v<T, Obj>) {
                m_data = std::forward<T>(other);
            } else if constexpr(std::is_arithmetic_v<T> || std::is_enum_v<T>) {
                m_data = static_cast<Num>(other);
            } else if constexpr(std::is_convertible_v<T, Str>) {
                m_data = static_cast<Str>(std::forward<T>(other));
            } else if constexpr(std::is_convertible_v<T, Obj>) {
                m_data = static_cast<Obj>(std::forward<T>(other));
            } else if constexpr(std::is_convertible_v<T,Arr>) {
                m_data = static_cast<Arr>(std::forward<T>(other));
            } else if constexpr(std::is_convertible_v<T,Num>) {
                m_data = static_cast<Num>(std::forward<T>(other));
            } else if constexpr(std::is_convertible_v<T,Bol>) {
                m_data = static_cast<Bol>(std::forward<T>(other));
            } else {
                m_data = Nul{};
            }
            return *this;
        }

        /**
         * @brief Constructor for Json that accepts array-like types and converts them to Json.
         * @tparam T The type of the other object to convert to Json.
         * @param other The object to convert to Json.
         */
        template<typename T>
        requires !constructible<Json, std::remove_cvref_t<T>> && !constructible_map<Json, std::remove_cvref_t<T>> && constructible_array<Json, std::remove_cvref_t<T>>
        explicit Json(T&& other) noexcept : m_data( Arr{} ) {
            auto& arr = std::get<Arr>(m_data);
            for (auto&& item : std::forward<T>(other)) {
                arr.emplace_back( static_cast<Json>(static_cast<typename std::remove_cvref_t<T>::value_type>( std::forward<decltype(item)>(item))));
            }
        }

        /**
         * @brief Constructor for Json that accepts map-like types and converts them to Json.
         * @tparam T The type of the other object to convert to Json.
         * @param other The object to convert to Json.
         */
        template<typename T>
        requires !constructible<Json, std::remove_cvref_t<T>> && constructible_map<Json, std::remove_cvref_t<T>>
        explicit Json(T&& other) noexcept : m_data( Obj{} ) {
            auto& obj = std::get<Obj>(m_data);
            for (auto&& [key, val] : std::forward<T>(other)) {
                obj.emplace( static_cast<Str>(key), static_cast<Json>(static_cast<typename std::remove_cvref_t<T>::mapped_type>(std::forward<decltype(val)>(val))) );
            }
        }

        /**
         * @brief Reset the JSON data to a specific type.
         * @tparam T The type to reset the JSON data to, defaults to Nul.
         */
        template<typename T = Nul>
        requires json_type<Json, T>
        void reset() noexcept {
            if constexpr(std::is_same_v<T, Nul>) {
                m_data = Nul{};
            } else if constexpr(std::is_same_v<T, Bol>) {
                m_data = Bol{};
            } else if constexpr(std::is_same_v<T, Num>) {
                m_data = Num{};
            } else if constexpr(std::is_same_v<T, Str>) {
                m_data = Str{};
            } else if constexpr(std::is_same_v<T, Arr>) {
                m_data = Arr{};
            } else if constexpr(std::is_same_v<T, Obj>) {
                m_data = Obj{};
            }
        }

        /**
         * @brief Accessor for JSON data using the subscript operator.
         */
        [[nodiscard]]
        Json& operator[](const Str& key) { return std::get<Obj>(m_data)[key]; }
        [[nodiscard]]
        const Json& operator[](const Str& key) const { return std::get<Obj>(m_data).at(key); }
        [[nodiscard]]
        Json& operator[](const std::size_t index) { return std::get<Arr>(m_data)[index]; }
        [[nodiscard]]
        const Json& operator[](const std::size_t index) const { return std::get<Arr>(m_data).at(index); }

        /**
         * @brief Accessor for JSON data using the at() method.
         */
        [[nodiscard]]
        Json& at(const Str& key) { return std::get<Obj>(m_data).at(key); }
        [[nodiscard]]
        const Json& at(const Str& key) const { return std::get<Obj>(m_data).at(key); }
        [[nodiscard]]
        Json& at(const std::size_t index) { return std::get<Arr>(m_data).at(index); }
        [[nodiscard]]
        const Json& at(const std::size_t index) const { return std::get<Arr>(m_data).at(index); }

        /**
         * @brief Write the JSON data to a string back.
         */
        void write(Str& out) const noexcept {
            switch (type()) {
                case Type::eObj: {
                    out.push_back('{');
                    for (const auto& [key, val] : std::get<Obj>(m_data)) {
                        escape_to(out, key);
                        out.push_back(':');
                        val.write(out);
                        out.push_back(',');
                    }
                    if (*out.rbegin() == ',') *out.rbegin() = '}';
                    else out.push_back('}');
                } break;
                case Type::eArr: {
                    out.push_back('[');
                    for (const auto& val : std::get<Arr>(m_data)) {
                        val.write(out);
                        out.push_back(',');
                    }
                    if (*out.rbegin() == ',') *out.rbegin() = ']';
                    else out.push_back(']');
                } break;
                case Type::eBol:
                    out.append(std::get<Bol>(m_data) ? "true" : "false");
                    break;
                case Type::eNul:
                    out.append("null");
                    break;
                case Type::eStr:
                    escape_to(out, std::get<Str>(m_data));
                    break;
                case Type::eNum: {
                    char buffer[25]; // Reserve enough space for typical numbers
                    const auto [ptr, ec]  = std::to_chars(
                        buffer,
                        buffer + 25,
                        std::get<Num>(m_data),
                        std::chars_format::general,
                        17
                    );
                    if(ec != std::errc{}) {
                        out.append(std::format("{:.17}",std::get<Num>(m_data)));
                    } else out.append(buffer, ptr);
                } break;
            }
        }

        /**
         * @brief Write the JSON data to an output stream.
         */
        void write(std::ostream& out) const {
            if(out.fail()) return;
            switch (type()) {
                case Type::eObj: {
                    out.put('{');
                    for(bool first = true;
                        const auto& [key, val] : std::get<Obj>(m_data)
                    ) {
                        if(!first) out.put(',');
                        else first = false;
                        escape_to(out, key);
                        out.put(':');
                        val.write(out);
                        if(out.fail()) return;
                    }
                    out.put('}');
                } break;
                case Type::eArr: {
                    out.put('[');
                    for(bool first = true;
                        const auto& val : std::get<Arr>(m_data)
                    ) {
                        if(!first) out.put(',');
                        else first = false;
                        val.write(out);
                        if(out.fail()) return;
                    }
                    out.put(']');
                } break;
                case Type::eBol:
                    out << (std::get<Bol>(m_data) ? "true" : "false");
                    break;
                case Type::eNul:
                    out << "null";
                    break;
                case Type::eStr:
                    escape_to(out, std::get<Str>(m_data));
                    break;
                case Type::eNum: {
                    char buffer[25]; // Reserve enough space for typical numbers
                    const auto [ptr, ec]  = std::to_chars(
                        buffer,
                        buffer + 25,
                        std::get<Num>(m_data),
                        std::chars_format::general,
                        17
                    );
                    if(ec != std::errc{}) {
                        out << std::format("{:.17}", std::get<Num>(m_data));
                    } else out.write(buffer, static_cast<std::size_t>(ptr-buffer));
                } break;
            }
        }

        /**
         * @brief Dump the JSON data to a string.
         */
        [[nodiscard]]
        Str dump() const noexcept {
            Str res;
            this->write(res);
            return res;
        }

        /**
         * @brief Write the JSON data to a string with formatting.
         * @param out The output string to write to.
         * @param space_num The number of spaces to use for indentation (default is 2).
         * @param depth The current depth of indentation (default is 0).
         */
        void writef(
            Str& out,
            const std::uint16_t space_num = 2,
            const std::uint16_t depth = 0
        ) const noexcept {
            const std::uint32_t tabs  = depth * space_num + space_num;

            switch (type()) {
                case Type::eObj: {
                    out.push_back('{');
                    for (const auto& [key, val] : std::get<Obj>(m_data)) {
                        out.push_back('\n');
                        out.append(tabs, ' ');
                        escape_to(out, key);
                        out.append(": ");
                        val.writef(out, space_num, depth + 1);
                        out.push_back(',');
                    }
                    if (*out.rbegin() == ',') *out.rbegin() = '\n';
                    if(!std::get<Obj>(m_data).empty()){
                        out.append(tabs - space_num, ' ');
                    }
                    out.push_back('}');
                } break;
                case Type::eArr: {
                    out.push_back('[');
                    for (const auto& val : std::get<Arr>(m_data)) {
                        out.push_back('\n');
                        out.append(tabs, ' ');
                        val.writef(out, space_num, depth + 1);
                        out.push_back(',');
                    }
                    if (*out.rbegin() == ',') *out.rbegin() = '\n';
                    if(!std::get<Arr>(m_data).empty()){
                        out.append(tabs - space_num, ' ');
                    }
                    out.push_back(']');
                } break;
                case Type::eBol:
                    out.append(std::get<Bol>(m_data) ? "true" : "false");
                    break;
                case Type::eNul:
                    out.append("null");
                    break;
                case Type::eStr:
                    escape_to(out, std::get<Str>(m_data));
                    break;
                case Type::eNum: {
                    char buffer[25]; // Reserve enough space for typical numbers
                    const auto [ptr, ec]  = std::to_chars(
                        buffer,
                        buffer + 25,
                        std::get<Num>(m_data),
                        std::chars_format::general,
                        17
                    );
                    if(ec != std::errc{}) {
                        out.append(std::format("{:.17}",std::get<Num>(m_data)));
                    } else out.append(buffer, ptr);
                } break;
            }
        }

        /**
         * @brief Write the JSON data to an output stream with formatting.
         * @param out The output stream to write to.
         * @param space_num The number of spaces to use for indentation (default is 2).
         * @param depth The current depth of indentation (default is 0).
         */
        void writef(
            std::ostream& out,
            const std::uint16_t space_num = 2,
            const std::uint16_t depth = 0
        ) const {
            if(out.fail()) return;
            const std::uint32_t tabs  = depth * space_num + space_num;

            switch (type()) {
                case Type::eObj: {
                    out.put('{');
                    bool first = true;
                    for(const auto& [key, val] : std::get<Obj>(m_data)) {
                        if(!first) out.put(',');
                        else first = false;
                        out.put('\n');
                        out << std::setfill(' ') << std::setw(tabs) << "";
                        escape_to(out, key);
                        out.put(':');
                        out.put(' ');
                        val.writef(out, space_num, depth + 1);
                        if(out.fail()) return;
                    }
                    if(!first) out.put('\n');
                    if(!std::get<Obj>(m_data).empty()){
                        out << std::setfill(' ') << std::setw(tabs - space_num) << "";
                    }
                    out.put('}');
                } break;
                case Type::eArr: {
                    out.put('[');
                    bool first = true;
                    for (const auto& val : std::get<Arr>(m_data)) {
                        if(!first) out.put(',');
                        else first = false;
                        out.put('\n');
                        out << std::setfill(' ') << std::setw(tabs) << "";
                        val.writef(out, space_num, depth + 1);
                        if(out.fail()) return;
                    }
                    if(!first) out.put('\n');
                    if(!std::get<Arr>(m_data).empty()){
                        out << std::setfill(' ') << std::setw(tabs - space_num) << "";
                    }
                    out.put(']');
                } break;
                case Type::eBol:
                    out << (std::get<Bol>(m_data) ? "true" : "false");
                    break;
                case Type::eNul:
                    out << "null";
                    break;
                case Type::eStr:
                    escape_to(out, std::get<Str>(m_data));
                    break;
                case Type::eNum: {
                    char buffer[25]; // Reserve enough space for typical numbers
                    const auto [ptr, ec]  = std::to_chars(
                        buffer,
                        buffer + 25,
                        std::get<Num>(m_data),
                        std::chars_format::general,
                        17
                    );
                    if(ec != std::errc{}) {
                        out << std::format("{:.17}", std::get<Num>(m_data));
                    } else out.write(buffer, static_cast<std::size_t>(ptr-buffer));
                } break;
            }
            if(out.fail()) return;
        }

        /**
         * @brief Dump the JSON data to a string with formatting.
         * @param space_num The number of spaces to use for indentation (default is 2).
         * @param depth The current depth of indentation (default is 0).
         * @return A string containing the formatted JSON data.
         */
        [[nodiscard]]
        Str dumpf(
            const std::uint16_t space_num = 2,
            const std::uint16_t depth = 0
        ) const noexcept {
            Str res;
            this->writef(res, space_num, depth);
            return res;
        }

        /**
         * @brief Parse a JSON string or stream into a Json object.
         * @param text The JSON string to parse.
         * @param max_depth The maximum depth of nested structures allowed (default is 256).
         * @return A Json object if parsing is successful, or an error if it fails.
         */
        [[nodiscard]]
        static std::optional<Json> parse(const std::string_view text, const std::int32_t max_depth = 256) noexcept  {
            auto it = text.begin();
            const auto end_ptr = text.end();
            // Skip spaces
            while(it != end_ptr && std::isspace(*it)) ++it;
            if(it == end_ptr) return std::nullopt;
            // Parse the JSON
            const auto result = reader(it, end_ptr, max_depth-1);
            if(!result) return result;
            // check for trailing spaces
            while(it != end_ptr && std::isspace(*it)) ++it;
            if(it != end_ptr) return std::nullopt;
            return result;
        }
        [[nodiscard]]
        static std::optional<Json> parse(std::istream& is_text, const std::int32_t max_depth = 256) noexcept {
            auto it = std::istreambuf_iterator<char>(is_text);
            constexpr auto end_ptr = std::istreambuf_iterator<char>();
            // Skip spaces
            while(it != end_ptr && std::isspace(*it)) ++it;
            if(it == end_ptr) return std::nullopt;
            // Parse the JSON
            const auto result = reader(it, end_ptr, max_depth-1);
            if(!result) return result;
            // check for trailing spaces
            while(it != end_ptr && std::isspace(*it)) ++it;
            if(it != end_ptr) return std::nullopt;
            return result;
        }

        /**
         * @brief type conversion, copy inner value to specified type
         * @tparam T The target type to convert to
         * @tparam D The mapped_type or value_type of the target type, used for range conversion, default is Nul for other types(useless).
         * @param default_range_elem if T is a range type and is not json::Arr or json::Obj, must be specified for safe conversion. Else, please use default value.
         * @return The converted value
         * @note Num is double, so conversions to integral (and enum) types will round to nearest.
         * @details
         * Attempt sequence of conversions:
         * inner value type -> target type
         * 1. Nul -> Nul
         * 2. Obj -> Obj
         * 3. Arr -> Arr
         * 4. Str -> Str
         * 5. Bol -> Bol
         * 6. Num -> enum types (us llround, round to nearest)
         * 7. Num -> integral types (us llround, round to nearest)
         * 8. Num -> floating_point types
         * 9. Any -> T is constructible from json::Value
         * 10. Obj -> implicit convertible types
         * 11. Arr -> implicit convertible types
         * 12. Str -> implicit convertible types
         * 13. Num -> implicit convertible types
         * 14. Bol -> implicit convertible types
         * 15. Nul -> implicit convertible types (Nul is not convertible to bool !!!!!)
         * 16. Obj -> Try copy to `range && Str->key_type && Value->mapped_type types && have default_range_value`
         * 17. Arr -> Try copy to `range && Value->value_type types && have default_range_value`
         * 18. return std::nullopt;
         */
        template<typename T, typename D = Nul>
        requires convertible<Json, T> || convertible_map<Json, T, D> || convertible_array<Json, T, D>
        [[nodiscard]]
        std::optional<T>  to_if( D default_range_elem = D{} ) const noexcept {
            if constexpr (std::is_same_v<T, Nul>) {
                if (type() == Type::eNul) return Nul{};
            } else if constexpr (std::is_same_v<T, Obj>) {
                if (type() == Type::eObj) return std::get<Obj>(m_data);
            } else if constexpr (std::is_same_v<T, Arr>) {
                if (type() == Type::eArr) return std::get<Arr>(m_data);
            } else if constexpr (std::is_same_v<T, Str>) {
                if (type() == Type::eStr) return std::get<Str>(m_data);
            } else if constexpr (std::is_same_v<T, Bol>) {
                if (type() == Type::eBol) return std::get<Bol>(m_data);
            } else if constexpr (std::is_enum_v<T>) {
                if (type() == Type::eNum) return static_cast<T>(std::llround(std::get<Num>(m_data)));
            } else if constexpr (std::is_integral_v<T>) {
                if (type() == Type::eNum) return static_cast<T>(std::llround(std::get<Num>(m_data)));
            } else if constexpr (std::is_floating_point_v<T>) {
                if (type() == Type::eNum) return static_cast<T>(std::get<Num>(m_data));
            }
            if constexpr (std::is_constructible_v<T, Json>) {
                return static_cast<T>(*this);
            }
            if constexpr (std::is_convertible_v<Obj, T>) {
                if (type() == Type::eObj) return static_cast<T>(std::get<Obj>(m_data));
            }
            if constexpr (std::is_convertible_v<Arr, T>) {
                if (type() == Type::eArr) return static_cast<T>(std::get<Arr>(m_data));
            }
            if constexpr (std::is_convertible_v<Str, T>) {
                if (type() == Type::eStr) return static_cast<T>(std::get<Str>(m_data));
            }
            if constexpr (std::is_convertible_v<Num, T>) {
                if (type() == Type::eNum) return static_cast<T>(std::get<Num>(m_data));
            }
            if constexpr (std::is_convertible_v<Bol, T>) {
                if (type() == Type::eBol) return static_cast<T>(std::get<Bol>(m_data));
            }
            if constexpr (std::is_convertible_v<Nul, T>) {
                if (type() == Type::eNul) return static_cast<T>(Nul{});
            }
            if constexpr ( convertible_map<Json, T, D> ) {
                if (type() == Type::eObj) {
                    T result{};
                    for (auto& [key, value] : std::get<Obj>(m_data)) {
                        auto val = value.template to_if<typename T::mapped_type>();
                        if (!val) result.emplace(static_cast<typename T::key_type>(key), static_cast<typename T::mapped_type>(default_range_elem));
                        else result.emplace(static_cast<typename T::key_type>(key), std::move(*val));
                    }
                    return result;
                }
            }
            if constexpr ( convertible_array<Json, T, D> ) {
                if (type() == Type::eArr) {
                    T result{};
                    for (auto& value : std::get<Arr>(m_data)) {
                        auto val = value.template to_if<typename T::value_type>();
                        if (!val) result.emplace_back(default_range_elem);
                        else result.emplace_back(std::move(*val));
                    }
                    return result;
                }
            }
            return std::nullopt;
        }

        /**
         * @brief type conversion, copy inner value to specified type
         * @tparam T The target type to convert to
         * @tparam D The mapped_type or value_type of the target type, used for range conversion, default is Nul for other types(useless).
         * @param default_range_elem if T is a range type and is not json::Arr or json::Obj, must be specified for safe conversion. Else, please use default value.
         * @return The converted value
         * @throws std::runtime_error if conversion fails
         * @note Num is double, so conversions to integral (and enum) types will round to nearest.
         * @details
         * Attempt sequence of conversions:
         * inner value type -> target type
         * 1-17: equal to `to_if<T, D>()`
         * 18. throw std::runtime_error
         */
        template<typename T, typename D = Nul>
        requires convertible<Json, T> || convertible_map<Json, T, D> || convertible_array<Json, T, D>
        [[nodiscard]]
        T  to( D default_range_elem = D{} ) const {
            auto opt = to_if<T, D>( std::move(default_range_elem) );
            if (!opt) throw std::runtime_error("Cast fail.");
            return *opt;
        }

        /**
         * @brief type conversion, copy inner value to specified type
         * @tparam T The target type to convert to
         * @tparam D The mapped_type or value_type of the target type, used for range conversion, default is Nul for other types(useless).
         * @param default_result The default value to return if conversion fails
         * @param default_range_elem if T is a range type and is not json::Arr or json::Obj, must be specified for safe conversion. Else, please use default value.
         * @return The converted value or default_value if conversion fails
         * @note Num is double, so conversions to integral (and enum) types will round to nearest.
         * @details
         * Attempt sequence of conversions:
         * inner value type -> target type
         * 1-17: equal to `to_if<T, D>()`
         * 18: return default_result
         */
        template<typename T, typename D = Nul>
        requires convertible<Json, T> || convertible_map<Json, T, D> || convertible_array<Json, T, D>
        [[nodiscard]]
        T  to_or( T default_result, D default_range_elem = D{} ) const noexcept {
            auto opt = to_if<T, D>( std::move(default_range_elem) );
            if (!opt) return std::move(default_result);
            return *opt;
        }

        /**
         * @brief type conversion, Move or Copy inner value to specified type
         * @tparam T The target type to convert to
         * @tparam D The mapped_type or value_type of the target type, used for range conversion, default is Nul for other types(useless).
         * @param default_range_elem if T is a range type and is not json::Arr or json::Obj, must be specified for safe conversion. Else, please use default value.
         * @return The converted value
         * @note
         * Num is double, so conversions to integral (and enum) types will round to nearest.
         * Complex types like Obj, Arr, Str will be moved if possible.
         * Simple types like Bol, Num, Nul will be copied.
         * @details
         * Attempt sequence of conversions:
         * inner value type -> target type
         * 1. Nul -> Nul
         * 2. Obj -> Obj (Move)
         * 3. Arr -> Arr (Move)
         * 4. Str -> Str (Move)
         * 5. Bol -> Bol
         * 6. Num -> enum types (us llround, round to nearest)
         * 7. Num -> integral types (us llround, round to nearest)
         * 8. Num -> floating_point types
         * 9. Any -> T is constructible from json::Value (try Move)
         * 10. Obj -> implicit convertible types (try Move)
         * 11. Arr -> implicit convertible types (try Move)
         * 12. Str -> implicit convertible types (try Move)
         * 13. Num -> implicit convertible types
         * 14. Bol -> implicit convertible types
         * 15. Nul -> implicit convertible types (Nul is not convertible to bool !!!!!)
         * 16. Obj -> Try copy to `range && Str->key_type && Value->mapped_type types && have default_range_value`  (try Move)
         * 17. Arr -> Try copy to `range && Value->value_type types && have default_range_value`  (try Move)
         * 18. return std::nullopt;
         */
        template<typename T, typename D = Nul>
        requires convertible<Json, T> || convertible_map<Json, T, D> || convertible_array<Json, T, D>
        [[nodiscard]]
        std::optional<T>  move_if( D default_range_elem = D{} ) noexcept  {
            if constexpr (std::is_same_v<T, Nul>) {
                if (type() == Type::eNul) return Nul{};
            } else if constexpr (std::is_same_v<T, Obj>) {
                if (type() == Type::eObj) return std::move(std::get<Obj>(m_data));
            } else if constexpr (std::is_same_v<T, Arr>) {
                if (type() == Type::eArr) return std::move(std::get<Arr>(m_data));
            } else if constexpr (std::is_same_v<T, Str>) {
                if (type() == Type::eStr) return std::move(std::get<Str>(m_data));
            } else if constexpr (std::is_same_v<T, Bol>) {
                if (type() == Type::eBol) return std::get<Bol>(m_data);
            } else if constexpr (std::is_enum_v<T>) {
                if (type() == Type::eNum) return static_cast<T>(std::llround(std::get<Num>(m_data)));
            } else if constexpr (std::is_integral_v<T>) {
                if (type() == Type::eNum) return static_cast<T>(std::llround(std::get<Num>(m_data)));
            } else if constexpr (std::is_floating_point_v<T>) {
                if (type() == Type::eNum) return static_cast<T>(std::get<Num>(m_data));
            }
            if constexpr (std::is_constructible_v<T, Json>) {
                return static_cast<T>(std::move(*this));
            }
            if constexpr (std::is_convertible_v<Obj, T>) {
                if (type() == Type::eObj) return static_cast<T>(std::move(std::get<Obj>(m_data)));
            }
            if constexpr (std::is_convertible_v<Arr, T>) {
                if (type() == Type::eArr) return static_cast<T>(std::move(std::get<Arr>(m_data)));
            }
            if constexpr (std::is_convertible_v<Str, T>) {
                if (type() == Type::eStr) return static_cast<T>(std::move(std::get<Str>(m_data)));
            }
            if constexpr (std::is_convertible_v<Num, T>) {
                if (type() == Type::eNum) return static_cast<T>(std::get<Num>(m_data));
            }
            if constexpr (std::is_convertible_v<Bol, T>) {
                if (type() == Type::eBol) return static_cast<T>(std::get<Bol>(m_data));
            }
            if constexpr (std::is_convertible_v<Nul, T>) {
                if (type() == Type::eNul) return static_cast<T>(Nul{});
            }
            if constexpr ( convertible_map<Json, T, D> ) {
                if (type() == Type::eObj) {
                    T result{};
                    for (auto& [key, value] : std::get<Obj>(m_data)) {
                        auto val = value.template move_if<typename T::mapped_type>();
                        if (!val) result.emplace(static_cast<typename T::key_type>(key), static_cast<typename T::mapped_type>(default_range_elem));
                        else result.emplace(static_cast<typename T::key_type>(key), std::move(*val));
                    }
                    return result;
                }
            }
            if constexpr ( convertible_array<Json, T, D> ) {
                if (type() == Type::eArr) {
                    T result{};
                    for (auto& value : std::get<Arr>(m_data)) {
                        auto val = value.template move_if<typename T::value_type>();
                        if (!val) result.emplace_back( static_cast<typename T::value_type>(default_range_elem) );
                        else result.emplace_back(std::move(*val));
                    }
                    return result;
                }
            }
            return std::nullopt; // return nullopt if conversion fails
        }

        /**
         * @brief type conversion, Move or Copy inner value to specified type
         * @tparam T The target type to convert to
         * @tparam D The mapped_type or value_type of the target type, used for range conversion, default is Nul for other types(useless).
         * @param default_range_elem if T is a range type and is not json::Arr or json::Obj, must be specified for safe conversion. Else, please use default value.
         * @return The converted value
         * @throws std::runtime_error if conversion fails
         * @note
         * Num is double, so conversions to integral (and enum) types will round to nearest.
         * Complex types like Obj, Arr, Str will be moved if possible.
         * Simple types like Bol, Num, Nul will be copied.
         * @details
         * Attempt sequence of conversions:
         * inner value type -> target type
         * 1-17: equal to `move_if<T, D>()`
         * 18: throw std::runtime_error
         */
        template<typename T, typename D = Nul>
        requires convertible<Json, T> || convertible_map<Json, T, D> || convertible_array<Json, T, D>
        [[nodiscard]]
        T  move( D default_range_elem = D{} ) {
            auto opt = move_if<T, D>( std::move(default_range_elem) );
            if (!opt) throw std::runtime_error("Cast fail.");
            return *opt;
        }

        /**
         * @brief type conversion, Move or Copy inner value to specified type
         * @tparam T The target type to convert to
         * @tparam D The mapped_type or value_type of the target type, used for range conversion, default is Nul for other types(useless).
         * @param default_result The default value to return if conversion fails
         * @param default_range_elem if T is a range type and is not json::Arr or json::Obj, must be specified for safe conversion. Else, please use default value.
         * @return The converted value
         * @note
         * Num is double, so conversions to integral (and enum) types will round to nearest.
         * Complex types like Obj, Arr, Str will be moved if possible.
         * Simple types like Bol, Num, Nul will be copied.
         * @details
         * Attempt sequence of conversions:
         * inner value type -> target type
         * 1-17: equal to `move_if<T, D>()`
         * 18: return default_result;
         */
        template<typename T, typename D = Nul>
        requires convertible<Json, T> || convertible_map<Json, T, D> || convertible_array<Json, T, D>
        [[nodiscard]]
        T  move_or( T default_result, D default_range_elem = D{} ) noexcept {
            auto opt = move_if<T, D>( std::move(default_range_elem) );
            if (!opt) return default_result;
            return *opt;
        }

        /**
         * @brief For Value-to-Value accurate comparison
         * @tparam T Type that satisfies std::equality_comparable concept
         * @warning Num is double, so the comparison of numbers will be very strict
         * @param other The value to compare with
         * @return True if values are equal, false otherwise
         */
        [[nodiscard]]
        bool operator==(const Json& other) const noexcept {
            if (type() != other.type()) return false; // Different types cannot be equal
            switch (type()) {
                case Type::eNul: return true; // Both are null
                case Type::eBol: return std::get<Bol>(m_data) == std::get<Bol>(other.m_data);
                case Type::eNum: return std::get<Num>(m_data) == std::get<Num>(other.m_data);
                case Type::eStr: return std::get<Str>(m_data) == std::get<Str>(other.m_data);
                case Type::eObj: return std::get<Obj>(m_data) == std::get<Obj>(other.m_data);
                case Type::eArr: return std::get<Arr>(m_data) == std::get<Arr>(other.m_data);
            }
            return false; // Should never reach here, but added for safety
        }

        /**
         * @brief Generic equality comparison operator
         * @tparam T Type that satisfies std::equality_comparable concept
         * @param other The value to compare with
         * @return True if values are equal, false otherwise
         *
         * @details
         * rules:
         * A. different json types are not equal( Bol, Num, Str, Arr, Obj, Nul )
         * B. IF T is Value, use Value::operator==(const Value& other) ,
         *    it's Accurate comparison !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
         * --------------------------------------------------------------------------------------------
         * 1.  IF T is Nul, return true if this Value is null, false otherwise
         * 2.  Else if T is Bol, return true if Value is Bol and Equal to Tvalue
         * 3.  Else if T is Num, return true if Value is Num and Equal to Tvalue
         * 4.  Else if T is Str, return true if Value is Str and Equal to Tvalue
         * 5.  Else if T is Arr, return true if Value is Arr and Equal to Tvalue
         * 6.  Else if T is Obj, return true if Value is Obj and Equal to Tvalue
         * 7.  Else if T is enum, return true if Value is Num and Equal to Tvalue, Num will be rounded to nearest integer
         * 8.  Else if T is integral, return true if Value is Num or Bol and Equal to Tvalue, Num will be rounded to nearest integer
         * 9.  Else if T is floating_point, return true if Value is Num and Equal to Tvalue, double to double comparison is very strict
         * 10. Else if T is convertible to std::string_view, return true if Value is Str and Equal to std::string_view( Tvalue )
         * 11. Else if T is equality_comparable and constructible from Value, return true if Tvalue == T(*this);
         * 12. Else if Value is constructible from T, return true if *this == json::Value(Tvalue);
         * 13. Else return false
         */
        template<typename  T>
        requires (!std::is_same_v<T, Json>)
        [[nodiscard]]
        bool operator==(const T& other) const  noexcept {
            if constexpr ( std::is_same_v<T,Nul> ) {
                return type() == Type::eNul;
            } else if constexpr ( std::is_same_v<T,Bol> ) {
                if ( type() == Type::eBol ) return std::get<Bol>(m_data) == other;
            } else if constexpr ( std::is_same_v<T,Num> ) {
                if ( type() == Type::eNum ) return std::get<Num>(m_data) == other;
            } else if constexpr ( std::is_same_v<T,Str> ) {
                if ( type() == Type::eStr ) return std::get<Str>(m_data) == other;
            } else if constexpr ( std::is_same_v<T,Arr> ) {
                if ( type() == Type::eArr ) return std::get<Arr>(m_data) == other;
            } else if constexpr ( std::is_same_v<T,Obj> ) {
                if ( type() == Type::eObj ) return std::get<Obj>(m_data) == other;
            } else if constexpr (std::is_enum_v<T>) {
                if ( type() == Type::eNum) return static_cast<T>(std::llround(std::get<Num>(m_data))) == other;
            } else if constexpr (std::is_integral_v<T>) {
                if ( type() == Type::eNum) return static_cast<T>(std::llround(std::get<Num>(m_data))) == other;
            } else if constexpr (std::is_floating_point_v<T>) {
                if ( type() == Type::eNum) return static_cast<T>(std::get<Num>(m_data)) == other;
            } else if constexpr (std::is_convertible_v<T, std::string_view>) {
                if( type() == Type::eStr) return std::get<Str>(m_data) == std::string_view( other );
            } else if constexpr (std::equality_comparable<T> && std::is_constructible_v<T, Json>) {
                return other == static_cast<T>(*this);     // Use T's operator==
            } else if constexpr (std::is_constructible_v<Json, T>) {
                return static_cast<Json>(other) == *this; // Use Value's operator==
            }
            return false;
        }

        /**
         * @brief Get inner container size.
         * @return The size of the inner container(Arr or Obj), or 0 for Nul, Bol, Num, Str.
         */
        [[nodiscard]]
        std::size_t size() const noexcept {
            if (type() == Type::eObj)  return std::get<Obj>(m_data).size();
            if (type() == Type::eArr) return std::get<Arr>(m_data).size();
            return 0; // Nul, Bol, Num, Str are considered to have size 0
        }

        /**
         * @brief Check if the JSON object is empty.
         * @return False if it contains any elements (Obj or Arr), true otherwise.
         */
        [[nodiscard]]
        bool empty() const noexcept {
            if (type() == Type::eObj)  return std::get<Obj>(m_data).empty();
            if (type() == Type::eArr) return std::get<Arr>(m_data).empty();
            return true; // Nul, Bol, Num, Str are considered empty
        }

        /**
         * @brief Check if the JSON object contains a key.
         * @param key The key to check in the JSON object.
         * @return True if the key exists in the JSON object, false if the JSON is not an object or the key does not exist.
         */
        [[nodiscard]]
        bool contains(const Str& key) const noexcept {
            if (type() == Type::eObj) return std::get<Obj>(m_data).contains(key);
            return false; // Not an object
        }

        /**
         * @brief Erase a key from the JSON object or an index from the JSON array.
         * @param key The key to erase from the JSON object.
         * @return True if the key was erased, false if the JSON is not an object or the key does not exist.
         */
        bool erase(const Str& key) noexcept{
            if (type() == Type::eObj) return std::get<Obj>(m_data).erase(key);
            return false;
        }

        /**
         * @brief Erase an element at the specified index from the JSON array.
         * @param index The index of the element to erase from the JSON array.
         * @return True if the element was erased, false if the JSON is not an array or the index is out of bounds.
         */
        bool erase(const std::size_t index) noexcept {
            if (type() == Type::eArr && index < std::get<Arr>(m_data).size()) {
                std::get<Arr>(m_data).erase(std::get<Arr>(m_data).begin() + index);
                return true;
            }
            return false;
        }

        /**
         * @brief Insert a key-value pair into the JSON object.
         * @param key The key to insert into the JSON object.
         * @param value The value to insert into the JSON object.
         * @return True if the key-value pair was inserted, false if the JSON is not an object.
         */
        template<typename  K, typename V>
        requires std::convertible_to<K, Str> && std::convertible_to<V, Json>
        bool insert(K&& key, V&& value) noexcept {
            if (type() == Type::eObj) {
                std::get<Obj>(m_data).emplace(static_cast<Str>(std::forward<K>(key)), static_cast<Json>(std::forward<V>(value)));
                return true;
            }
            return false;
        }

        /**
         * @brief Insert a value at the specified index into the JSON array.
         * @param index The index at which to insert the value into the JSON array.
         * @param value The value to insert into the JSON array.
         * @return True if the value was inserted, false if the JSON is not an array or the index is out of bounds.
         */
        template<typename V>
        requires std::convertible_to<V, Json>
        bool insert(const std::size_t index, V&& value) noexcept {
            if (type() == Type::eArr && index <= std::get<Arr>(m_data).size()) {
                std::get<Arr>(m_data).emplace(std::get<Arr>(m_data).begin() + index, static_cast<Json>(std::forward<V>(value)));
                return true;
            }
            return false;
        }

        /**
         * @brief Push a value to the end of the JSON array.
         * @param value The value to push to the end of the JSON array.
         * @return True if the value was pushed, false if the JSON is not an array.
         */
        template<typename V>
        requires std::convertible_to<V, Json>
        bool push_back(V&& value) noexcept {
            if (type() == Type::eArr) {
                std::get<Arr>(m_data).emplace_back( static_cast<Json>(std::forward<V>(value)) );
                return true;
            }
            return false;
        }

        /**
         * @brief Pop the last value from the JSON array.
         * @return True if the value was popped, false if the JSON is not an array or is empty.
         */
        bool pop_back() noexcept {
            if (type() == Type::eArr && !std::get<Arr>(m_data).empty()) {
                std::get<Arr>(m_data).pop_back();
                return true;
            }
            return false;
        }

    };

}

export namespace mysvac {
    /**
     * @brief Alias for the Json class with default template parameters.
     *
     * Use `std::allocator` for memory allocation,
     * Use `std::map` for Obj, `std::vector` for Arr, `std::string` for Str.
     */
    using Json = ::mysvac::json::Json<>;
}
