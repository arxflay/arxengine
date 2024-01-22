#pragma once
#include "ArxDefines.h"
#include <vector>
#include <string_view>
#include <string>

ARX_NAMESPACE_BEGIN

class Utils
{
public:
    enum class Base64DecodeErrCode
    {
        NoError,
        EmptyString,
        InvalidSize,
        InvalidBase64String
    };
    static Base64DecodeErrCode Base64Decode(std::string_view encData, std::string &out);
    static Base64DecodeErrCode Base64Decode(std::string_view encData, std::vector<uint8_t> &out);
    static std::string Base64Encode(std::string_view data);
    static std::string Base64Encode(const std::vector<uint8_t> &data);
    static std::string Base64Encode(const uint8_t *data, size_t size);
};

ARX_NAMESPACE_END
