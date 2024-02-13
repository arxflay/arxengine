#include "UIObject.h"
#include "ArxWindow.h"
#include <memory>

ARX_NAMESPACE_BEGIN


UIObject::UIObject(UIObject *parent, Size size, Position pos)
    : m_parent(parent)
    , m_size(size)
    , m_position(pos)
    , m_backgroundColor(defaults::COLOR_WHITE)
{
    if (m_parent == this)
        throw ArxException(ArxException::ErrorCode::FailedToConstructUIObject, "m_parent points to self");

    if (m_parent)
    {
        if (m_parent->GetOwnerWindow())
            m_ownerWindow = m_parent->GetOwnerWindow();
        else
        {
            ArxWindow *win = dynamic_cast<ArxWindow*>(parent);
            if (win == nullptr)
                throw ArxException(ArxException::ErrorCode::FailedToConstructUIObject, "m_parent has null owner window but m_parent is not a window");
        m_ownerWindow = win;
        }
    }
}

void UIObject::SetBackgroundColor(Color c) { m_backgroundColor = c; }
Color UIObject::GetColor() const { return m_backgroundColor; }

void UIObject::SetSize(Size s) { m_size = s; }
Size UIObject::GetSize() const { return m_size; }

void UIObject::SetPosition(Position pos) { m_position = pos; }
Position UIObject::GetPosition() const { return m_position; }

ArxWindow *UIObject::GetOwnerWindow() { return m_ownerWindow; }
UIObject *UIObject::GetParent() { return m_parent; }

void UIObject::Hide()
{
    Show(false);
}

ChildrenList &UIObject::GetChildren()
{
    return m_children;
}

EventManager UIObject::GetEventManager()
{
    return m_eventManager;
}

void UIObject::Destroy()
{
    for (UIObject *child : m_children)
        child->Destroy();

    m_eventManager.QueueEvent<DestroyEvent>(std::make_unique<DestroyEvent>(this));
}

void UIObject::Draw()
{
    m_eventManager.QueueEvent<DrawEvent>(std::make_unique<DrawEvent>(this));
    for (UIObject *child : m_children)
        child->Draw();
}

DestroyEvent::DestroyEvent(UIObject *sender)
    : Event(sender)
{
}

void DestroyEvent::HandleEvent()
{
    delete GetSender();
}

DrawEvent::DrawEvent(UIObject *sender)
    : Event(sender)
{
}


ARX_NAMESPACE_END