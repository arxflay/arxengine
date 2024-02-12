#pragma once
#include "ArxDefines.h"
#include <type_traits>
ARX_NAMESPACE_BEGIN

class ARX_EXPORTS Convertors
{
public:
    static constexpr double PxToPt(unsigned int px) { return px * 0.75; }
    static constexpr double PtToPx(unsigned int pt) { return pt * (1.0 / 0.75); }
};

ARX_NAMESPACE_END

