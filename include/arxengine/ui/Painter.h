/*
 * @author Alexej Fedorenko
 */

#ifndef ARX_PAINTER_H
#define ARX_PAINTER_H
#include "arxengine/ArxDefines.h"
#include "arxengine/ui/Position.h"
#include "arxengine/ui/Viewport.h"
#include "arxengine/ui/Brush.h"
#include "arxengine/ui/Pen.h"
#include <memory>

ARX_NAMESPACE_BEGIN
class DrawEvent;
class UIControl;
class Texture2D;
class ClippingArea;

class ARX_EXPORTS Painter final
{
public:
    Painter(DrawEvent &evt);
    void DrawRectangle(Position pos, Size size);
    void DrawTexture2D(Position pos, Size size, const Texture2D *tex, int tileWidthCount = 1, int tileHeightCount = 1);
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
    UIControl *GetSender();
    UIControl *m_sender;
    std::unique_ptr<ClippingArea> m_clippingArea;
    Brush m_brush;
    Pen m_pen;
};

ARX_NAMESPACE_END

#endif
