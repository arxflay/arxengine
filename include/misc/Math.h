#ifndef ARX_MATH_H
#define ARX_MATH_H
#include "ArxDefines.h"
#include <cmath>
#include <type_traits>

ARX_NAMESPACE_BEGIN

static inline constexpr bool AreEqualFloat(float a, float b)
{
    return std::fabs(a - b) <= std::numeric_limits<float>::epsilon();
}

ARX_NAMESPACE_END

#endif
