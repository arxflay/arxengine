#include "arxengine/ui/Pen.h"

ARX_NAMESPACE_BEGIN

Pen::Pen(const Color &color)
    : m_color(color)
{
}

const Color &Pen::GetColor() const
{
    return m_color;
}

void Pen::SetColor(const Color &col)
{
    m_color = col;
}


ARX_NAMESPACE_END
