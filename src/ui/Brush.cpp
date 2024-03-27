#include "arxengine/ui/Brush.h"

ARX_NAMESPACE_BEGIN


Brush::Brush(const Color &color)
    : m_color(color)
{
}

const Color &Brush::GetColor() const
{
    return m_color;
}

void Brush::SetColor(const Color &col)
{
    m_color = col;
}


ARX_NAMESPACE_END
