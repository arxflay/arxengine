#include "UIObject.h"
#include "ArxWindow.h"
#include <memory>

ARX_NAMESPACE_BEGIN

UIObject::UIObject(UIObject *parent, Size size, Position pos)
    : m_size(size)
    , m_position(pos)
    , m_backgroundColor(defaults::COLOR_WHITE)
{
    Reparent(parent);
}

void UIObject::SetBackgroundColor(Color c) { m_backgroundColor = c; }
Color UIObject::GetColor() const { return m_backgroundColor; }

void UIObject::SetSize(Size s) { m_size = s; }
Size UIObject::GetSize() const { return m_size; }
Size UIObject::GetClientSize() const { return m_size; }

void UIObject::SetPosition(Position pos) { m_position = pos; }
Position UIObject::GetPosition() const { return m_position; }

ArxWindow *UIObject::GetOwnerWindow() { return m_ownerWindow; }

void UIObject::Reparent(ArxObject *parent)
{
    if (!parent)
        throw ArxException(ArxException::ErrorCode::GenericError, "parent must be not null");
    
    UIObject *uiobjParent = dynamic_cast<UIObject*>(parent);
    if (uiobjParent)
        throw ArxException(ArxException::ErrorCode::GenericError, "parent is not uiobject");

    ArxObject::Reparent(parent);
     
    if (!uiobjParent->GetOwnerWindow())
    {
        ArxWindow *win = dynamic_cast<ArxWindow*>(parent);
        if (win)
            throw ArxException(ArxException::ErrorCode::GenericError, "Parent is not a window but doesn't have owner window");

        m_ownerWindow = win;
    }
    else
        m_ownerWindow = uiobjParent->GetOwnerWindow();
}

void UIObject::Hide()
{
    Show(false);
}

UIObject::UIObject()
    : m_size(defaults::DEFAULT_SIZE)
    , m_position(defaults::DEFAULT_POSITION)
    , m_ownerWindow(nullptr)
    , m_backgroundColor(defaults::COLOR_WHITE)
{
}

void UIObject::Draw()
{
    std::unique_ptr<DrawEvent> evt(std::make_unique<DrawEvent>());
    GetEventManager().QueueEvent<DrawEvent>(std::move(evt));

    for (ArxObject *obj : const_cast<ArxObjectList&>(GetChildren()))
    {
        UIObject *uiobject = dynamic_cast<UIObject*>(obj);
        if (uiobject)
            uiobject->Draw();
    }
}

void DrawEvent::HandleEvent()
{

}

ARX_NAMESPACE_END
