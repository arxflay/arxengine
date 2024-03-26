/*
 * @author Alexej Fedorenko
 */

#ifndef ARX_MOUSEEVENT_H
#define ARX_MOUSEEVENT_H
#include "arxengine/ArxDefines.h"
#include "arxengine/evt/Event.h"
#include "arxengine/ui/Position.h"

ARX_NAMESPACE_BEGIN

class ARX_EXPORTS MouseButtonEvent : public Event
{
public:
    MouseButtonEvent();
    enum class ButtonType
    {
        Left,
        Right,
        Scroll,
        Btn4,
        Btn5,
        Btn6,
        Btn7
    };

    void SetMouseButtonType(ButtonType buttonType);
    ButtonType GetMouseButtonType() const; 

private:
    void HandleEvent() override = 0;
    ButtonType m_buttonType;
};

class ARX_EXPORTS MouseDownEvent : public MouseButtonEvent
{
private:
    void HandleEvent() override {};
};

class ARX_EXPORTS MouseUpEvent : public MouseButtonEvent
{
private:
    void HandleEvent() override {};
};

class ARX_EXPORTS MouseScrollEvent : public Event
{
public:
    enum class Direction
    {
        Up,
        Down
    };
    
    void SetScrollDirection(Direction direction);
    Direction GetScrollDirection() const; 

private:
    void HandleEvent() override {};
    Direction m_direction;
};

//emitted by window
class ARX_EXPORTS MouseMoveEvent : public Event
{
public:
    MouseMoveEvent();
    void SetPosition(Position pos);
    Position GetPosition() const;
    void SetViewportAffectedPosition(Position pos);
    Position GetViewportAffectedPosition() const;
private:
    void HandleEvent() override {};
    Position m_pos;
    Position m_viewportAffectedPosition;
};

class ARX_EXPORTS MouseEnterEvent : public Event 
{
private:
    void HandleEvent() override {};
};

class ARX_EXPORTS MouseExitEvent : public Event
{
public:
    MouseExitEvent();
    bool WasForced() const;
    void SetWasForced(bool force);
private:
    void HandleEvent() override {};
    bool m_wasForced;
};

ARX_NAMESPACE_END

#endif
