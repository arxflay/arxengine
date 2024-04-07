#include "arxengine/ui/Rect.h"
#include "arxengine/ui/Size.h"
#include "arxengine/ui/Position.h"

ARX_NAMESPACE_BEGIN

Rect::Rect(const SizeF &size, const Position &pos)
    : m_size(size)
    , m_pos(pos)
{
}

bool Rect::IntersectWith(const Rect &rect) const
{
    return (GetRight() > rect.GetLeft() && GetLeft() < rect.GetRight()) 
        && (GetBottom() > rect.GetTop() && GetTop() < rect.GetBottom());
}

bool Rect::IntersectWith(Position point) const
{
    return point.x >= GetLeft() && point.x <= GetRight() 
        && point.y >= GetTop() &&  point.y <= GetBottom();
}

void Rect::SetSize(const SizeF &size)
{
    m_size = size;
}

void Rect::SetPosition(const Position &pos)
{
    m_pos = pos;
}

const SizeF &Rect::GetSize() const
{
    return m_size;
}

const Position &Rect::GetPosition() const
{
    return m_pos;
}

float Rect::GetRight() const
{
    return GetLeft() + m_size.width;
}

float Rect::GetLeft() const
{
    return m_pos.x;
}

float Rect::GetBottom() const
{
    return GetTop() + m_size.height;
}

float Rect::GetTop() const
{
    return m_pos.y;
}



ARX_NAMESPACE_END

