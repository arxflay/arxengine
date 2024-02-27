#ifndef ARX_PAINTER_H
#define ARX_PAINTER_H
#include "ArxDefines.h"
#include "Position.h"
#include "Size.h"
#include "Brush.h"
#include <glm/mat4x4.hpp>

ARX_NAMESPACE_BEGIN
class DrawEvent;
class UIObject;
class Texture2D;

class Painter
{
public:
    Painter(DrawEvent &evt);
    void DrawRectangle(Position pos, Size size);
    void DrawTexture2D(Position pos, Size size, const Texture2D *tex);
    void SetBrush(const Brush &brush);
    const Brush &GetBrush() const;
    const glm::mat4 &GetViewport();
    void Clear();
private:
    Position CalculateDrawPosition(Position uiobjectPos, Size uiobjectSize);
    UIObject *GetSender();
    UIObject *m_sender;
    Brush m_brush;
};

ARX_NAMESPACE_END

#endif
