#include "arxengine/ui/Brush.h"

ARX_NAMESPACE_BEGIN


Brush::Brush(Color color)
    : m_color(color)
{
}

Color Brush::GetColor()
{
    return m_color;
}

void Brush::SetColor(Color col)
{
    m_color = col;
}


ARX_NAMESPACE_END
