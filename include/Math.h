#pragma once
#include "ArxDefines.h"
#include <cmath>
#include <type_traits>

ARX_NAMESPACE_BEGIN

template<typename T>
static inline constexpr std::enable_if_t<std::is_arithmetic_v<T>, T> AreNumbersEqual(float a, float b)
{
    return std::fabs(a - b) <= std::numeric_limits<T>::epsilon();
}

ARX_NAMESPACE_END
