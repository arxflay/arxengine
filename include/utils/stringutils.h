#pragma once
#include "../arxdefines.h"
#include <vector>
#include <string_view>

ARX_NAMESPACE_BEGIN

class ARX_EXPORTS StringUtils
{
public:
    static uint8_t GetByteFromHexStr(const char *hexStr);
    static std::vector<uint8_t> HexStrToBin(std::string_view hexStr);
};

ARX_NAMESPACE_END
