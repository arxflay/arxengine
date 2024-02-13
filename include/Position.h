#pragma once
#include "ArxDefines.h"
#include <cmath>

ARX_NAMESPACE_BEGIN

struct Position
{
    Position()
        : Position(0.0f, 0.0f)
    {
    }
    
    template<typename T>
    Position(std::enable_if_t<std::is_arithmetic_v<T>, T> xIn, T yIn)
        : x(static_cast<float>(xIn)), y(static_cast<float>(yIn))
    {
    }

    static Position Truncate(Position p)
    {
        p.x = std::truncf(p.x);
        p.y = std::truncf(p.y);
        return p;
    }

    float x;
    float y;
};


namespace defaults
{
    static inline const Position DEFAULT_POSITION = Position{ 1, 1 };
}

ARX_NAMESPACE_END
