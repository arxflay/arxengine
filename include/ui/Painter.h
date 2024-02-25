#ifndef ARX_PAINTER_H
#define ARX_PAINTER_H
#include "ArxDefines.h"
#include "Position.h"
#include "Size.h"
#include <glm/mat4x4.hpp>

ARX_NAMESPACE_BEGIN
class DrawEvent;
class UIObject;

class Painter
{
public:
    Painter(DrawEvent &evt);
    void DrawRectangle(Position pos, Size size);
    const glm::mat4 &GetViewport();
private:
    Position CalculateDrawPosition(Position uiobjectPos, Size uiobjectSize);
    UIObject *GetSender();
    UIObject *m_sender;
};

ARX_NAMESPACE_END

#endif
