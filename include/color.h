#pragma once

#include "arxdefines.h"
#include <string_view>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

ARX_NAMESPACE_BEGIN

struct ARX_EXPORTS Color final
{
    Color() noexcept;
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) noexcept;
    Color(std::string_view hexString);
    glm::vec3 GetNormalizedRGB() const noexcept;
    glm::vec4 GetNormalizedRGBA() const noexcept;
    
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

ARX_NAMESPACE_END
