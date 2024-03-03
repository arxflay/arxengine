#ifndef ARX_PAINTER_H
#define ARX_PAINTER_H
#include "ArxDefines.h"
#include "Position.h"
#include "Viewport.h"
#include "Brush.h"
#include "Pen.h"
#include <memory>

ARX_NAMESPACE_BEGIN
class DrawEvent;
class UIObject;
class Texture2D;
class ClippingArea;

class Painter final
{
public:
    Painter(DrawEvent &evt);
    void DrawRectangle(Position pos, Size size);
    void DrawTexture2D(Position pos, Size size, const Texture2D *tex);
    void DrawText(std::string_view text, Position pos);
    void SetBrush(const Brush &brush);
    void SetPen(const Pen &pen);

    const Brush &GetBrush() const;
    const Pen &GetPen() const;
    const Viewport &GetViewport();
    
    void Clear();
    ~Painter();
private:
    Position CalculateDrawPosition(Position uiobjectPos, Size uiobjectSize);
    UIObject *GetSender();
    UIObject *m_sender;
    std::unique_ptr<ClippingArea> m_clippingArea;
    Brush m_brush;
    Pen m_pen;
};

ARX_NAMESPACE_END

#endif
