#pragma once
#include "ArxDefines.h"

ARX_NAMESPACE_BEGIN

struct Size
{
    float width;
    float height;
};

namespace defaults
{
    static inline const Size IGNORE_SIZE = Size{ defaults::IGNORE, defaults::IGNORE };
}

ARX_NAMESPACE_END
