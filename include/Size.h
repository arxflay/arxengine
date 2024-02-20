#pragma once
#include "ArxDefines.h"
#include <cmath>
#include <type_traits>

ARX_NAMESPACE_BEGIN

struct Size
{
    Size()
        : Size(0.0f, 0.0f)
    {
    }
    
    template<typename T>
    Size(std::enable_if_t<std::is_arithmetic_v<T>, T> inWidth, T inHeight)
        : width(static_cast<float>(inWidth)), height(static_cast<float>(inHeight))
    {
    }

    static Size Truncate(Size s)
    {
        s.height = std::truncf(s.height);
        s.width = std::truncf(s.width);
        return s;
    }

    friend Size operator+(Size s1, Size s2)
    {
        return Size(s1.width + s2.width, s1.height + s2.height);
    }

    float width;
    float height;
};

namespace defaults
{
    static inline const Size DEFAULT_SIZE = Size(1, 1);
    static inline const Size IGNORE_SIZE = Size(defaults::IGNORE, defaults::IGNORE);
}

ARX_NAMESPACE_END
