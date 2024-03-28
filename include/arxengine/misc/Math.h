/*
 * @author Alexej Fedorenko
 */

#ifndef ARX_MATH_H
#define ARX_MATH_H
#include "arxengine/ArxDefines.h"
#include <cmath>
#include <limits>

ARX_NAMESPACE_BEGIN

template<typename T>
static inline std::enable_if_t<std::is_arithmetic_v<T>, bool> AreNumsEqual(const T &a, const T &b)
{
    return a == b;
}

template<>
inline bool AreNumsEqual<float>(const float &a, const float &b)
{
    return std::fabs(a - b) <= std::numeric_limits<float>::epsilon();
}

template<>
inline bool AreNumsEqual<double>(const double &a, const double &b)
{
    return std::abs(a - b) <= std::numeric_limits<double>::epsilon();
}

static inline unsigned long constexpr PowerOfTwoModulo(unsigned long value, unsigned long powerOfTwo)
{
    return value - ((value >> powerOfTwo) << powerOfTwo);
}

#define GET_BIT(byteVal, pos) (byteVal & (1 << (7 - (pos))))

ARX_NAMESPACE_END

#endif
