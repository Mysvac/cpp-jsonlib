#pragma once
#ifndef MYSVAC_CPP_JSONLIB_SERIALIZABLE_HPP
#define MYSVAC_CPP_JSONLIB_SERIALIZABLE_HPP

/**
* This header file is not in the library, and needs to be copied by yourself.
* 此头文件并不在库中，如果需要，请自行复制。
*/

#define Field(name) result[#name] = name;
#define Serializable(...) \
    operator Jsonlib::JsonValue(){ \
        Jsonlib::JsonValue result(Jsonlib::JsonType::OBJECT);    \
        __VA_ARGS__    \
        return result;    \
    }

#endif
