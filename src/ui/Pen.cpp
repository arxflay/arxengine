#include "ui/Pen.h"

ARX_NAMESPACE_BEGIN

Pen::Pen(Color color)
    : m_color(color)
{
}

Color Pen::GetColor()
{
    return m_color;
}

void Pen::SetColor(Color col)
{
    m_color = col;
}


ARX_NAMESPACE_END
