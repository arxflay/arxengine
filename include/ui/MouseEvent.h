#ifndef ARX_MOUSEEVENT_H
#define ARX_MOUSEEVENT_H
#include "ArxDefines.h"
#include <evt/Event.h>
#include <ui/Position.h>

ARX_NAMESPACE_BEGIN

class MouseButtonEvent : public Event
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

class MouseDownEvent : public MouseButtonEvent
{
private:
    void HandleEvent() override {};
};

class MouseUpEvent : public MouseButtonEvent
{
private:
    void HandleEvent() override {};
};

class MouseScrollEvent : public Event
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
class MouseMoveEvent : public Event
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

class MouseEnterEvent : public Event 
{
private:
    void HandleEvent() override {};
};

class MouseExitEvent : public Event
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
