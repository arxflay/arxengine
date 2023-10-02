#pragma once

#include "arxdefines.h"
#include <string_view>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

ARX_NAMESPACE_BEGIN

/**
 * @class Color
 * @brief struct that represents Color
 */
struct ARX_EXPORTS Color final
{
    /**
     * @brief Default constructor
     * @description all color channels are initialized with 0
     */
    Color() noexcept;
    
    /**
    * @brief Constructor that creates color from RGB/RGBA bytes
    */
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) noexcept;
    
    /**
     * @brief Constructor that creates color from hexstring
     * @description hexString must start with letter '#'.
     * Supported color palletes are RGB (6 characters) or RBGA (8 characters)
     * If used RGB hexstring, A channel is equal to 255
     * If hexstring doesn't start with letter # or shorter than 6 characters, color channels are initialized with 0
     * If hexstring is invalid, exception is thrown.
     */
    Color(std::string_view hexString);

    /**
     * @brief Default constructor
     * @description all color channels are initialized with 0
     */
    glm::vec3 GetNormalizedRGB() const noexcept;
    
    /**
     * @brief Default constructor
     * @description all color channels are initialized with 0
     */
    glm::vec4 GetNormalizedRGBA() const noexcept;
    
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

namespace defaults
{
    static inline const Color WHITE_COLOR(0xFF, 0xFF, 0xFF);
};

ARX_NAMESPACE_END
