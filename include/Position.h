#pragma once
#include "arxdefines.h"

ARX_NAMESPACE_BEGIN

//Probably should consider using float for smooth transition

struct ARX_EXPORTS Position 
{
    int x;
    int y;
};

namespace defaults
{
    static inline constexpr Position DEFAULT_POSITION{ 0, 0 };
}

ARX_NAMESPACE_END
