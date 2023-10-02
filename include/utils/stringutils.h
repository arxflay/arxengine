#pragma once
#include "../arxdefines.h"
#include <vector>
#include <string_view>

ARX_NAMESPACE_BEGIN


/**
 * @class StringUtils
 * @brief Static class that is used for various string operations
 */
class ARX_EXPORTS StringUtils
{
public:

    /**
     * @brief Converts 2 hex characters to bytes
     */
    static uint8_t GetByteFromHexStr(const char *hexStr);

    /**
     * @brief Converts hex string to vector of bytes
     */
    static std::vector<uint8_t> HexStrToBin(std::string_view hexStr);
private:
    StringUtils() = default;
};

ARX_NAMESPACE_END
