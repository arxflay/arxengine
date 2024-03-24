/*
 * @author Alexej Fedorenko
 */

#ifndef ARX_POSITION_H
#define ARX_POSITION_H
#include "arxengine/ArxDefines.h"
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

    Position &operator+=(Position pos)
    {
        x += pos.x;
        y += pos.y;
        return *this;
    }

    friend Position operator+(Position p1, Position p2)
    {
        return Position(p1.x + p2.x, p1.y + p2.y);
    }

    friend Position operator-(Position p1, Position p2)
    {
        return Position(p1.x - p2.x, p1.y - p2.y);
    }

    float x;
    float y;
};


namespace constants
{
    static inline const Position DEFAULT_POSITION = Position{ 1, 1 };
}

ARX_NAMESPACE_END

#endif
