#include "ui/UIControl.h"
#include "ui/ArxWindow.h"
#include <memory>

ARX_NAMESPACE_BEGIN

UIControl::UIControl(UIControl *parent, Size size, Position pos)
    : m_size(size)
    , m_position(pos)
    , m_backgroundColor(constants::COLOR_WHITE)
    , m_clippingEnabled(true)
    , m_fontCache(new FontCache(this))
    , m_isShown(true)
{
    if (!parent)
        throw ArxException(ArxException::ErrorCode::GenericError, "UIControl parent is null");

    ArxObject::Reparent(parent);
    m_ownerWindow = parent->GetWindow();

}

void UIControl::SetBackgroundColor(Color c) { m_backgroundColor = c; }
Color UIControl::GetColor() const { return m_backgroundColor; }

void UIControl::SetSize(Size s) { m_size = s; }
Size UIControl::GetSize() const { return m_size; }
Size UIControl::GetClientSize() const { return m_size; }

void UIControl::SetPosition(Position pos) { m_position = pos; }
Position UIControl::GetPosition() const { return m_position; }

ArxWindow *UIControl::GetOwnerWindow() { return m_ownerWindow; }
const ArxWindow *UIControl::GetOwnerWindow() const { return m_ownerWindow; }

ArxWindow *UIControl::GetWindow() { return GetOwnerWindow(); }
const ArxWindow *UIControl::GetWindow() const { return GetOwnerWindow(); }

void UIControl::Reparent(ArxObject *parent)
{
    UIControl *uiobjParent = dynamic_cast<UIControl*>(parent);
    if (!uiobjParent)
        throw ArxException(ArxException::ErrorCode::GenericError, "UIControl parent is not UIControl or is null");
 
    if (uiobjParent->GetWindow() != GetWindow())
        throw ArxException(ArxException::ErrorCode::GenericError, "UIControl window change is not allowed");

    ArxObject::Reparent(parent);
}

void UIControl::Show(bool visible)
{
    std::unique_ptr<ShowEvent> showEvent(new ShowEvent);
    showEvent->SetWillBeShown(visible);
    GetEventManager().QueueEvent<ShowEvent>(std::move(showEvent));
}

void UIControl::Hide()
{
    Show(false);
}

UIControl::UIControl()
    : m_size(constants::DEFAULT_SIZE)
    , m_position(constants::DEFAULT_POSITION)
    , m_ownerWindow(nullptr)
    , m_backgroundColor(constants::COLOR_WHITE)
    , m_clippingEnabled(false)
    , m_fontCache(new FontCache(this))
    , m_isShown(false)
{

}

void DrawEvent::HandleEvent()
{
    static_cast<UIControl*>(GetSender())->OnDraw(*this);
}

void UIControl::EnableClipToBounds(bool enable)
{
    m_clippingEnabled = enable;
}

bool UIControl::IsEnabledClipToBounds() const
{
    return m_clippingEnabled;
}

FontCache *UIControl::GetFontCache()
{
    return m_fontCache;
}

Font &UIControl::GetFont() 
{
    return m_font;
}

void UIControl::SetFont(Font &&font)
{
    m_font = std::move(font);
}

void UIControl::SetFont(const Font &font)
{
    m_font = font;
}

bool UIControl::IsShown() const
{
    return m_isShown;
}

ShowEvent::ShowEvent()
    : m_show(false)
{
}

void ShowEvent::HandleEvent() 
{
    static_cast<UIControl*>(GetSender())->m_isShown = WillBeShown();    
}

bool ShowEvent::WillBeShown() const
{
    return m_show;
}

bool UIControl::HitTest(Position pos) const
{
    return pos.x >= m_position.x && pos.x <= m_position.x + GetClientSize().width 
        && pos.y >= m_position.y && pos.y <= m_position.y + GetClientSize().height; 
}

void ShowEvent::SetWillBeShown(bool show)
{
    m_show = show;
}

void UIControl::MoveToFront()
{
    ArxObject *parent = GetParent();
    if (parent)
    {
        auto &parentChildren = const_cast<ArxObjectList&>(parent->GetChildren());
        auto childIt = std::find(parentChildren.begin(), parentChildren.end(), this);
        parentChildren.splice(parentChildren.end(), parentChildren, childIt, std::next(childIt));
    }
}

void UIControl::MoveToBack()
{
    ArxObject *parent = GetParent();
    if (parent)
    {
        auto &parentChildren = const_cast<ArxObjectList&>(parent->GetChildren());
        auto childIt = std::find(parentChildren.begin(), parentChildren.end(), this);
        parentChildren.splice(parentChildren.begin(), parentChildren, childIt, std::next(childIt));
    }
}

ARX_NAMESPACE_END
