#pragma once
#include "arxdefines.h"
#include <functional>
#include <limits>
ARX_NAMESPACE_BEGIN

/**
 * @class Size
 * @brief struct that represents Size
 */
struct ARX_EXPORTS Size
{
    int width;
    int height;

    /**
     * @brief Returns clamped size that varies from -1(defaults::IGNORE) to INT_MAX
     */
    constexpr Size GetClampedSize() const noexcept 
    {
        return Size {
            std::clamp(width, defaults::IGNORE, std::numeric_limits<int>::max()),
            std::clamp(height, defaults::IGNORE, std::numeric_limits<int>::max())
        };
    }
    
    constexpr bool operator >(const Size &z)  const { return width > z.width && height > z.height;   }
    constexpr bool operator <(const Size &z)  const { return width < z.width && height < z.height;   }
    constexpr bool operator ==(const Size &z) const { return width == z.width && height == z.height; } 
    constexpr bool operator >=(const Size &z) const { return operator>(z) || operator==(z); }
    constexpr bool operator <=(const Size &z) const { return operator<(z) || operator==(z); }
};

namespace defaults
{
    static inline constexpr Size IGNORE_SIZE{defaults::IGNORE, defaults::IGNORE};
    static inline constexpr Size DEFAULT_SIZE{ 0, 0 };
}

ARX_NAMESPACE_END
