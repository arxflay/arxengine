#include "arxengine/ui/Color.h"
#include "arxengine/misc/Utils.h"
#include <numeric>

ARX_NAMESPACE_BEGIN

Color::Color()
    : Color(0, 0, 0, 255)
{
}

Color::Color(uint8_t rIn, uint8_t gIn, uint8_t bIn, uint8_t aIn)
    : r(rIn)
    , g(gIn)
    , b(bIn)
    , a(aIn)
{
}

Color::Color(std::string_view hexString)
    : a(255)
{
    Utils::HexStrToBin(hexString, reinterpret_cast<uint8_t*>(this), sizeof(Color) / sizeof(uint8_t));
}

glm::vec4 Color::GetNormalizedColorRGBA()
{
    return glm::vec4 {
        static_cast<float>(r) / std::numeric_limits<uint8_t>::max(),
        static_cast<float>(g) / std::numeric_limits<uint8_t>::max(),
        static_cast<float>(b) / std::numeric_limits<uint8_t>::max(),
        static_cast<float>(a) / std::numeric_limits<uint8_t>::max()
    };
}

ARX_NAMESPACE_END
