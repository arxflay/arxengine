#include "color.h"
#include <cctype>
#include <iostream>
#include "utils/stringutils.h"

ARX_NAMESPACE_BEGIN

Color::Color() noexcept
    : r(0), g(0), b(0), a(0)
{
}

 Color::Color(uint8_t rIn, uint8_t gIn, uint8_t bIn, uint8_t aIn) noexcept
    : r(rIn), g(gIn), b(bIn), a(aIn)
{
}


#define RGB_HEX_LEN 7
#define RGBA_HEX_LEN 9

/*static*/  Color::Color(std::string_view hexString)
    : Color()
{
    if(hexString.size() < RGB_HEX_LEN || hexString[0] != '#')
        return;

    a = UINT8_MAX;

    r = StringUtils::GetByteFromHexStr(hexString.data() + 1);
    g = StringUtils::GetByteFromHexStr(hexString.data() + 3);
    b = StringUtils::GetByteFromHexStr(hexString.data() + 5);
    
    if (hexString.size() < RGBA_HEX_LEN)
        return;
    
    a = StringUtils::GetByteFromHexStr(hexString.data() + 7);
}

 glm::vec3 Color::GetNormalizedRGB() const noexcept
{
    return glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f);
}

glm::vec4 Color::GetNormalizedRGBA() const noexcept
{
    return glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

ARX_NAMESPACE_END
