#pragma once
#include "ArxDefines.h"
#include "Position.h"
#include "Size.h"
#include <glm/mat4x4.hpp>

ARX_NAMESPACE_BEGIN
class DrawEvent;
class UIObject;
class ArxWindow;
class UICache;

class Painter
{
public:
    Painter(DrawEvent &evt);
    void DrawRectangle(Position pos, Size size);

    Position GetSenderPosition();
    const glm::mat4 &GetViewport();
private:
    Position CalculateDrawPosition(Position uiobjectPos, Size uiobjectSize);
    UIObject *GetSender();
    ArxWindow *GetWindow();
    UICache *GetUICache();
    UIObject *m_sender;
};

ARX_NAMESPACE_END
