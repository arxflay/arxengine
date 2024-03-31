#include "arxengine/ui/MouseEvent.h"
ARX_NAMESPACE_BEGIN

void MouseButtonEvent::SetMouseButtonType(ButtonType buttonType) { m_buttonType = buttonType; }

MouseButtonEvent::ButtonType MouseButtonEvent::GetMouseButtonType() const { return m_buttonType; }

bool MouseButtonEvent::WasForced() const { return m_wasForced; }
void MouseButtonEvent::SetWasForced(bool forced) { m_wasForced = forced; }

void MouseScrollEvent::SetScrollDirection(Direction direction) { m_direction = direction; }
MouseScrollEvent::Direction MouseScrollEvent::GetScrollDirection() const { return m_direction; }

void MouseMoveEvent::SetPosition(Position pos) { m_pos = pos;}
Position MouseMoveEvent::GetPosition() const { return m_pos; }

MouseButtonEvent::MouseButtonEvent()
    : m_buttonType(MouseButtonEvent::ButtonType::Left)
{
}

MouseMoveEvent::MouseMoveEvent()
    : m_pos(0, 0)
    , m_viewportAffectedPosition(0, 0)
{
}

void MouseMoveEvent::SetViewportAffectedPosition(Position pos) { m_viewportAffectedPosition = pos; }

Position MouseMoveEvent::GetViewportAffectedPosition() const { return m_viewportAffectedPosition; }

MouseExitEvent::MouseExitEvent()
    : m_wasForced(false)
{
}

bool MouseExitEvent::WasForced() const { return m_wasForced; }

void MouseExitEvent::SetWasForced(bool force) {m_wasForced = force; }

ARX_NAMESPACE_END
