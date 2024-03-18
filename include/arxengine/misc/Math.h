/*
 * @author Alexej Fedorenko
 */

#ifndef ARX_MATH_H
#define ARX_MATH_H
#include "arxengine/ArxDefines.h"
#include <cmath>
#include <limits>

ARX_NAMESPACE_BEGIN

static inline bool AreEqualFloat(float a, float b)
{
    return std::fabs(a - b) <= std::numeric_limits<float>::epsilon();
}

ARX_NAMESPACE_END

#endif
