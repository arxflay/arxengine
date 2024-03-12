#ifndef ARX_COLOR_H
#define ARX_COLOR_H
#include "ArxDefines.h"
#include <glm/vec4.hpp>
#include <string_view>

ARX_NAMESPACE_BEGIN

struct ARX_EXPORTS Color
{
    Color();
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    Color(std::string_view hexString);
    glm::vec4 GetNormalizedColorRGBA();
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};


namespace constants
{
    static inline Color COLOR_BLACK = Color(0, 0, 0);
    static inline Color COLOR_WHITE = Color(255, 255, 255);
    static inline Color COLOR_TRANSPARENT = Color(0, 0, 0, 0);
}

ARX_NAMESPACE_END

#endif
