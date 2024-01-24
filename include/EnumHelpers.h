#pragma once
#include "ArxException.h"
#include <string_view>
#include <algorithm>
#include <iterator>

/*use only for enums that starts from 0 and each enum one is increased by one(0,1,2...)*/
#define ENUM_FROM_STRING_DECLARE(EnumName) EnumName StringTo##EnumName(std::string_view str)
#define ENUM_FROM_STRING_DEFINE(EnumName, ...) EnumName StringTo##EnumName(std::string_view str)\
{\
    static const char *conversions[]{ __VA_ARGS__ };\
    auto it = std::find(std::begin(conversions), std::end(conversions), str);\
    if (it == std::end(conversions))\
        throw ARX_NAMESPACE::ArxException((ARX_NAMESPACE::ArxException::ErrorCode::EnumFromStringError, "Failed to convert string to enum " #EnumName);\
    return static_cast<EnumName>(it - std::begin(conversions));\
}

#define ENUM_TO_STRING_DECLARE(EnumName) const char *StringFrom##EnumName(EnumName ourEnum)
#define ENUM_TO_STRING_DEFINE(EnumName, ...) const char *StringFrom##EnumName(EnumName ourEnum)\
{\
    static const char *conversions[]{ __VA_ARGS__ };\
    if (static_cast<int>(ourEnum) > (sizeof(conversions) / sizeof(char*)))\
        throw ARX_NAMESPACE::ArxException(ARX_NAMESPACE::ArxException::ErrorCode::EnumToStringError, "Failed to convert enum " #EnumName " to string"); \
    return conversions[static_cast<int>(ourEnum)];\
}

#define ENUM_FROM_TO_STRING_DECLARE(EnumName)\
    ENUM_FROM_STRING_DECLARE(EnumName);\
    ENUM_TO_STRING_DECLARE(EnumName)

#define ENUM_FROM_TO_STRING_DEFINE(EnumName, ...)\
    ENUM_FROM_STRING_DEFINE(EnumName, __VA_ARGS__)\
    ENUM_TO_STRING_DEFINE(EnumName, __VA_ARGS__)


/*if enum is nested in namespace or class*/


#define ENUM_FROM_STRING_DECLARE_NESTED(Scope, EnumName) Scope::EnumName StringTo##EnumName(std::string_view str)
#define ENUM_FROM_STRING_DEFINE_NESTED(Scope, EnumName, ...) Scope::EnumName StringTo##EnumName(std::string_view str)\
{\
    static const char *conversions[]{ __VA_ARGS__ };\
    auto it = std::find(std::begin(conversions), std::end(conversions), str);\
    if (it == std::end(conversions))\
        throw ARX_NAMESPACE::ArxException(ARX_NAMESPACE::ArxException::ErrorCode::EnumFromStringError, "Failed to convert string to enum " #Scope "::" #EnumName);\
    return static_cast<Scope::EnumName>(it - std::begin(conversions));\
}

#define ENUM_TO_STRING_DECLARE_NESTED(Scope, EnumName) const char *StringFrom##EnumName(Scope, EnumName ourEnum)
#define ENUM_TO_STRING_DEFINE_NESTED(Scope, EnumName, ...) const char *StringFrom##EnumName(Scope::EnumName ourEnum)\
{\
    static const char *conversions[]{ __VA_ARGS__ };\
    if (static_cast<size_t>(ourEnum) > (sizeof(conversions) / sizeof(char*)))\
        throw ARX_NAMESPACE::ArxException(ARX_NAMESPACE::ArxException::ErrorCode::EnumToStringError, "Failed to convert enum " #Scope "::" #EnumName " to string"); \
    return conversions[static_cast<size_t>(ourEnum)];\
}

#define ENUM_FROM_TO_STRING_DECLARE_NESTED(Scope, EnumName)\
    ENUM_FROM_STRING_DECLARE_NESTED(Scope, EnumName);\
    ENUM_TO_STRING_DECLARE_NESTED(Scope, EnumName)

#define ENUM_FROM_TO_STRING_DEFINE_NESTED(Scope, EnumName, ...)\
    ENUM_FROM_STRING_DEFINE_NESTED(Scope, EnumName, __VA_ARGS__)\
    ENUM_TO_STRING_DEFINE_NESTED(Scope, EnumName, __VA_ARGS__)


