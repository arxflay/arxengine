/*
 * @author Alexej Fedorenko
 */

#ifndef ARX_CONVERTORS
#define ARX_CONVERTORS
#include "arxengine/ArxDefines.h"
#include <type_traits>
ARX_NAMESPACE_BEGIN

class ARX_EXPORTS Convertors
{
public:
    static constexpr double PxToPt(unsigned int px) { return px * 0.75; }
    static constexpr double PtToPx(unsigned int pt) { return pt * (1.0 / 0.75); }
};

ARX_NAMESPACE_END

#endif

