/**
 * @file json_macros.hpp
 * @brief Comprehensive JSON serialization and deserialization macro system for C++23
 * @author Mysvac
 * @version 0.2.0
 */


#ifndef _M_MYSVAC_JSON_MACROS_HPP
#define _M_MYSVAC_JSON_MACROS_HPP

/**
 * @defgroup JSON_CONVERSION_MACROS JSON Conversion Macros
 * @brief Macros for converting C++ objects to JSON Values
 * @details These macros provide automatic serialization of C++ class members to JSON format.
 *          They generate operator::mysvac::Json() with perfect forwarding support.
 */


#define M_MYSVAC_JSON_CONVERSION_FIELD( member_name ) \
    do {    \
        static_assert( std::is_constructible_v< ::mysvac::Json, decltype(this->member_name) >, "MYSVAC_JSON: " #member_name " use macros CONVERSION_FILED, Json must be constructible from it. " );  \
        json_value[ #member_name ] = ::mysvac::Json{ _move_if_rvalue(this->member_name) };  \
    }while(false);
    
/**
 * @def M_MYSVAC_JSON_CONVERSION_MAP_FIELD(field_name, member_name)
 * @brief Convert a class member to JSON field with custom field name
 * @param field_name The name to use for the JSON field (string literal)
 * @param member_name The name of the class member to serialize
 */
#define M_MYSVAC_JSON_CONVERSION_MAP_FIELD( field_name, member_name ) \
    do {    \
        static_assert( std::is_constructible_v< ::mysvac::Json, decltype(this->member_name) >, "MYSVAC_JSON: " #member_name " use macros CONVERSION_FILED, Json must be constructible from it. " );  \
        json_value[ #field_name ] = ::mysvac::Json{ _move_if_rvalue(this->member_name) };  \
    }while(false);

/**
 * @def M_MYSVAC_JSON_CONVERSION_FUNCTION(class_name, ...)
 * @brief Generate JSON conversion operator for a class
 * @param class_name The name of the class being converted
 * @param ... Variable arguments containing conversion field macros
 */
#define M_MYSVAC_JSON_CONVERSION_FUNCTION( class_name, ... )   \
    explicit operator ::mysvac::Json() const & noexcept { \
        ::mysvac::Json json_value{ ::mysvac::Json::Obj{} }; \
        auto _move_if_rvalue = [](const auto& val) -> const auto& { return val; }; \
        __VA_ARGS__ \
        return json_value; \
    } \
    explicit operator ::mysvac::Json() && noexcept { \
    ::mysvac::Json json_value{ ::mysvac::Json::Obj{} }; \
    auto _move_if_rvalue = [](auto& val) -> auto&& { return std::move(val); }; \
    __VA_ARGS__ \
    return json_value; \
    } \
    explicit operator ::mysvac::Json() & noexcept { \
        ::mysvac::Json json_value{ ::mysvac::Json::Obj{} }; \
        auto _move_if_rvalue = [](auto& val) -> auto& { return val; }; \
        __VA_ARGS__ \
        return json_value; \
    } \



/**
 * @def M_MYSVAC_JSON_CONSTRUCTOR_FIELD_OR(member_name, default_result, default_range_value)
 * @brief Deserialize JSON field to class member with fallback default value
 * @param member_name The class member to initialize
 * @param default_result The default value to use if JSON field is missing or conversion fails
 * @param default_range_value Default value for range-type conversions (arrays/objects)
 */
#define M_MYSVAC_JSON_CONSTRUCTOR_FIELD_OR(member_name, default_result, default_range_value) \
    do{ \
        static_assert( std::is_convertible_v<decltype(default_result), std::remove_cvref_t<decltype(member_name)>>, "MYSVAC_JSON: " #member_name " use macros FIELD_OR, default_result must be convertible to member_type. "  );  \
        this->member_name = ( \
            (json_value.is_obj() && json_value.obj().contains( #member_name )) ? \
            json_value[ #member_name ].template move_or<std::remove_cvref_t<decltype(member_name)>,std::remove_cvref_t<decltype(default_range_value)>>( default_result, default_range_value ) : \
            default_result \
        );  \
    }while(false);

/**
 * @def M_MYSVAC_JSON_CONSTRUCTOR_FIELD_DEFAULT(member_name)
 * @brief Deserialize JSON field to class member with default-constructed fallback
 * @param member_name The class member to initialize
 */
#define M_MYSVAC_JSON_CONSTRUCTOR_FIELD_DEFAULT(member_name) \
    do {    \
        static_assert( std::is_default_constructible_v<std::remove_cvref_t<decltype(member_name)>>, "MYSVAC_JSON: " #member_name " use macros FIELD_DEFAULT, must is_default_constructible. "    );  \
        this->member_name = ( \
            (json_value.is_obj() && json_value.obj().contains( #member_name )) ? \
            json_value[ #member_name ].template move_or<std::remove_cvref_t<decltype(member_name)>>( std::remove_cvref_t<decltype(member_name)>{} ) : \
            std::remove_cvref_t<decltype(member_name)>{} \
        );  \
    }while(false);

/**
 * @def M_MYSVAC_JSON_CONSTRUCTOR_MAP_FIELD_OR(field_name, member_name, default_result, default_range_value)
 * @brief Deserialize custom-named JSON field to class member with fallback default value
 * @param field_name The JSON field name to look for (string literal)
 * @param member_name The class member to initialize
 * @param default_result The default value to use if JSON field is missing or conversion fails
 * @param default_range_value Default value for range-type conversions (arrays/objects)
 */
#define M_MYSVAC_JSON_CONSTRUCTOR_MAP_FIELD_OR(field_name, member_name, default_result, default_range_value) \
    do{ \
        static_assert( std::is_convertible_v<decltype(default_result), std::remove_cvref_t<decltype(member_name)>>, "MYSVAC_JSON: " #member_name " use macros FIELD_OR, default_result must be convertible to member_type. " );  \
        this->member_name = ( \
            (json_value.is_obj() && json_value.obj().contains( #field_name )) ? \
            json_value[ #field_name ].template move_or<std::remove_cvref_t<decltype(member_name)>,std::remove_cvref_t<decltype(default_range_value)>>( default_result, default_range_value ) : \
            default_result \
        );  \
    }while(false);

/**
 * @def M_MYSVAC_JSON_CONSTRUCTOR_MAP_FIELD_DEFAULT(field_name, member_name)
 * @brief Deserialize custom-named JSON field to class member with default-constructed fallback
 * @param field_name The JSON field name to look for (string literal)
 * @param member_name The class member to initialize
 */
#define M_MYSVAC_JSON_CONSTRUCTOR_MAP_FIELD_DEFAULT(field_name, member_name) \
    do {    \
        static_assert( std::is_default_constructible_v<std::remove_cvref_t<decltype(member_name)>>, "MYSVAC_JSON: " #member_name " use macros FIELD_DEFAULT, must is_default_constructible. "    );  \
        this->member_name = ( \
            (json_value.is_obj() && json_value.obj().contains( #field_name )) ? \
            json_value[ #field_name ].template move_or<std::remove_cvref_t<decltype(member_name)>>( std::remove_cvref_t<decltype(member_name)>{} ) : \
            std::remove_cvref_t<decltype(member_name)>{} \
        );  \
    }while(false);

/**
 * @def M_MYSVAC_JSON_CONSTRUCTOR_FUNCTION(class_name, ...)
 * @brief Generate JSON constructor for a class
 * @param class_name The name of the class being constructed
 * @param ... Variable arguments containing constructor field macros
 */
#define M_MYSVAC_JSON_CONSTRUCTOR_FUNCTION( class_name, ... ) \
    explicit class_name ( ::mysvac::Json json_value ) noexcept {  \
        __VA_ARGS__     \
    }



#ifdef M_MYSVAC_JSON_SIMPLIFY_MACROS
    /**
     * @def M_JSON_CV_MEM
     * @brief Simplified alias for M_MYSVAC_JSON_CONVERSION_FIELD
     * @see M_MYSVAC_JSON_CONVERSION_FIELD
     */
    #define M_JSON_CV_MEM       M_MYSVAC_JSON_CONVERSION_FIELD

    /**
     * @def M_JSON_CV_MAP
     * @brief Simplified alias for M_MYSVAC_JSON_CONVERSION_MAP_FIELD
     * @see M_MYSVAC_JSON_CONVERSION_MAP_FIELD
     */
    #define M_JSON_CV_MAP       M_MYSVAC_JSON_CONVERSION_MAP_FIELD

    /**
     * @def M_JSON_CV_FUN
     * @brief Simplified alias for M_MYSVAC_JSON_CONVERSION_FUNCTION
     * @see M_MYSVAC_JSON_CONVERSION_FUNCTION
     */
    #define M_JSON_CV_FUN       M_MYSVAC_JSON_CONVERSION_FUNCTION

    /**
     * @def M_JSON_CS_MEM
     * @brief Simplified alias for M_MYSVAC_JSON_CONSTRUCTOR_FIELD_DEFAULT
     * @see M_MYSVAC_JSON_CONSTRUCTOR_FIELD_DEFAULT
     */
    #define M_JSON_CS_MEM       M_MYSVAC_JSON_CONSTRUCTOR_FIELD_DEFAULT

    /**
     * @def M_JSON_CS_MAP
     * @brief Simplified alias for M_MYSVAC_JSON_CONSTRUCTOR_MAP_FIELD_DEFAULT
     * @see M_MYSVAC_JSON_CONSTRUCTOR_MAP_FIELD_DEFAULT
     */
    #define M_JSON_CS_MAP       M_MYSVAC_JSON_CONSTRUCTOR_MAP_FIELD_DEFAULT

    /**
     * @def M_JSON_CS_MEM_OR
     * @brief Simplified alias for M_MYSVAC_JSON_CONSTRUCTOR_FIELD_OR
     * @see M_MYSVAC_JSON_CONSTRUCTOR_FIELD_OR
     */
    #define M_JSON_CS_MEM_OR    M_MYSVAC_JSON_CONSTRUCTOR_FIELD_OR

    /**
     * @def M_JSON_CS_MAP_OR
     * @brief Simplified alias for M_MYSVAC_JSON_CONSTRUCTOR_MAP_FIELD_OR
     * @see M_MYSVAC_JSON_CONSTRUCTOR_MAP_FIELD_OR
     */
    #define M_JSON_CS_MAP_OR    M_MYSVAC_JSON_CONSTRUCTOR_MAP_FIELD_OR

    /**
     * @def M_JSON_CS_FUN
     * @brief Simplified alias for M_MYSVAC_JSON_CONSTRUCTOR_FUNCTION
     * @see M_MYSVAC_JSON_CONSTRUCTOR_FUNCTION
     */
    #define M_JSON_CS_FUN       M_MYSVAC_JSON_CONSTRUCTOR_FUNCTION
#endif // M_MYSVAC_JSON_SIMPLIFY_MACROS


#endif // _M_MYSVAC_JSON_MACROS_HPP
