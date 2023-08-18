#include "color.h"
#include <cctype>
#include <iostream>

ARX_NAMESPACE_BEGIN

Color::Color()
    : r(0), g(0), b(0), a(0)
{
}

 Color::Color(uint8_t rIn, uint8_t gIn, uint8_t bIn, uint8_t aIn)
    : r(rIn), g(gIn), b(bIn), a(aIn)
{
}


#define RGB_HEX_LEN 7
#define RGBA_HEX_LEN 9
static constexpr uint8_t GetByteFromHex(const char *ptr)
{
    uint8_t byte = 0;
    for (size_t i = 0; i < 2; i++)
    {
        char letter = static_cast<char>(std::tolower(ptr[i]));
        if (std::isdigit(ptr[i]))
            byte |= static_cast<uint8_t>((letter - 0x30) << (((i + 1) % 2) * 4));
        else if (letter >= 'a' && letter <= 'f')
            byte |= static_cast<uint8_t>((letter - 'a' + 10) << (((i + 1) % 2) * 4));
        else 
            throw std::runtime_error("invalid hexString"); 
    }
    
    return byte;
}

/*static*/  Color::Color(std::string_view hexString)
    : Color()
{
    if(hexString.size() < RGB_HEX_LEN || hexString[0] != '#')
        return;

    a = 255;

    r = GetByteFromHex(hexString.data() + 1);
    g = GetByteFromHex(hexString.data() + 3);
    b = GetByteFromHex(hexString.data() + 5);
    
    if (hexString.size() < RGBA_HEX_LEN)
        return;
    
    a = GetByteFromHex(hexString.data() + 7);
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
