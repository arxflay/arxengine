#ifndef ARX_RECT_H
#define ARX_RECT_H

#include "arxengine/ui/Size.h"
#include "arxengine/ui/Position.h"

ARX_NAMESPACE_BEGIN

class Rect final
{
public:
    Rect() = default;
    Rect(const SizeF &size, const Position &pos);
    bool IntersectWith(const Rect &rect) const;
    bool IntersectWith(Position point) const;
    void SetSize(const SizeF &size);
    void SetPosition(const Position &pos);
    const SizeF &GetSize() const;
    const Position &GetPosition() const;
    
    float GetRight() const;
    float GetLeft() const;
    float GetBottom() const;
    float GetTop() const;
private:
    SizeF m_size;
    Position m_pos;
};

ARX_NAMESPACE_END

#endif
