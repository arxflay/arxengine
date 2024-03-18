#ifndef ARX_BRUSH_H
#define ARX_BRUSH_H
#include "arxengine/ArxDefines.h"
#include "arxengine/ui/Color.h"

ARX_NAMESPACE_BEGIN

class ARX_EXPORTS Brush
{
public:
    Brush(Color color);
    Color GetColor();
    void SetColor(Color col);
private:
    Color m_color;
};

ARX_NAMESPACE_END

#endif
