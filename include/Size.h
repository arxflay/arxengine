#pragma once
#include "arxdefines.h"
#include <functional>
#include <limits>
ARX_NAMESPACE_BEGIN

struct ARX_EXPORTS Size
{
    int width;
    int height;
    Size GetClampedSize() const noexcept 
    {
        return Size
        {
            std::clamp(width, defaults::IGNORE, std::numeric_limits<int>::max()),
            std::clamp(height, defaults::IGNORE, std::numeric_limits<int>::max())
        };
    }
};

namespace defaults
{
    static inline constexpr Size IGNORE_SIZE{defaults::IGNORE, defaults::IGNORE};
    static inline constexpr Size DEFAULT_SIZE{ 0, 0 };
}

ARX_NAMESPACE_END
