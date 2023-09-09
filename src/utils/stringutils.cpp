#include "utils/stringutils.h"
#include <cctype>
#include <stdexcept>
#include "ArxException.h"

ARX_NAMESPACE_BEGIN

/*static*/ uint8_t StringUtils::GetByteFromHexStr(const char *hexStr)
{
    uint8_t byte = 0;
    for (size_t i = 0; i < 2; i++)
    {
        char letter = static_cast<char>(std::tolower(hexStr[i]));
        if (std::isdigit(hexStr[i]))
            byte |= static_cast<uint8_t>((letter - 0x30) << (((i + 1) % 2) * 4));
        else if (letter >= 'a' && letter <= 'f')
            byte |= static_cast<uint8_t>((letter - 'a' + 10) << (((i + 1) % 2) * 4));
        else 
            throw ArxException(ArxException::ErrorCode::HexConversionError, std::string("Got non hex letter '") + hexStr[i] + "' during conversion"); 
    }
    
    return byte;
}

/*static*/ std::vector<uint8_t> StringUtils::HexStrToBin(std::string_view hexStr)
{
    if ((hexStr.size() + !hexStr.size()) % 2 == 1)
        throw ArxException(ArxException::ErrorCode::HexConversionError, "Length of hex string is odd");

    std::vector<uint8_t> bytes(hexStr.size() >> 1);
    for (size_t i = 0; i < hexStr.size(); i += 2)
        bytes[i >> 1] = GetByteFromHexStr(hexStr.data() + (i << 1));

    return bytes;
}

ARX_NAMESPACE_END
