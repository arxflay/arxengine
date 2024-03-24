#include "arxengine/ui/UIControl.h"
#include "arxengine/ui/ArxWindow.h"
#include <memory>
#include "arxengine/ui/Painter.h"
ARX_NAMESPACE_BEGIN

UIControl::UIControl(UIControl *parent, SizeF size, Position pos)
    : ArxObject(nullptr)
    , m_size(size)
    , m_position(pos)
    , m_backgroundColor(constants::COLOR_WHITE)
    , m_clippingEnabled(true)
    , m_isShown(true)
    , m_canRecieveMouseEvents(false)
{
    if (parent == nullptr)
        throw ArxException(ArxException::ErrorCode::GenericError, "UIControl parent is null");

    m_fontCache = new FontCache(this);
    ArxObject::Reparent(parent);
    m_ownerWindow = parent->GetWindow();
}

void UIControl::SetBackgroundColor(Color c) { m_backgroundColor = c; }
Color UIControl::GetColor() const { return m_backgroundColor; }

void UIControl::SetSize(SizeF s) 
{
    if (GetParent() != GetOwnerWindow())
    {
        Size parentSize = static_cast<UIControl*>(GetParent())->GetClientSize();
        s = Size(std::clamp(s.width, 0.0f, parentSize.width), std::clamp(s.height, 0.0f, parentSize.height));
    }
        
    m_size = s;
}

bool UIControl::CanRecieveMouseEvents() const { return m_canRecieveMouseEvents; }

void UIControl::SetCanRecieveMouseEvents(bool canRecieve)
{
    if (m_canRecieveMouseEvents == canRecieve)
        return;
        
    UIControl *parent = static_cast<UIControl*>(GetParent());
    auto &recieversList = parent->m_mouseEventRecievers;
    auto &parentChildren = parent->GetChildren();
    if (canRecieve)
    {
        if (parentChildren.back() == this)
            recieversList.push_front(this);
        else if (parentChildren.front() == this || recieversList.empty())
            recieversList.push_back(this);
        else
        {
            ///VERY EXPENSIVE OPERATION
            //AVOID FOR NOW
            auto thisChildIt = std::find(parentChildren.begin(), parentChildren.end(), this);
            bool foundControlBehind = false;
            for (auto recieversListIt = recieversList.begin(); recieversListIt != recieversList.end(); recieversListIt++)
            {
                auto it = std::find(parentChildren.begin(), parentChildren.end(), *recieversListIt);
                if (std::distance(thisChildIt, it) < 0) //this reciever is before this element
                {
                    foundControlBehind = true;
                    recieversList.insert(recieversListIt, this);
                    break;
                }
            }

            if (!foundControlBehind)
                recieversList.push_back(this);
        }
    }
    else
    {
        GetOwnerWindow()->ForgetMouseEnteredControl(this, true);
        GetOwnerWindow()->ForgetPressedMouseButtons(this, true);
        recieversList.remove(this);
    }

    m_canRecieveMouseEvents = canRecieve;
}

SizeF UIControl::GetSize() const { return m_size; }
SizeF UIControl::GetClientSize() const { return m_size; }

void UIControl::SetPosition(Position pos) { m_position = pos; }
Position UIControl::GetPosition() const { return m_position; }

ArxWindow *UIControl::GetOwnerWindow() { return m_ownerWindow; }
const ArxWindow *UIControl::GetOwnerWindow() const { return m_ownerWindow; }

ArxWindow *UIControl::GetWindow() { return GetOwnerWindow(); }
const ArxWindow *UIControl::GetWindow() const { return GetOwnerWindow(); }

void UIControl::Reparent(ArxObject *parent)
{
    UIControl *uiobjParent = dynamic_cast<UIControl*>(parent);
    if (uiobjParent == nullptr)
        throw ArxException(ArxException::ErrorCode::GenericError, "UIControl parent is not UIControl or is null");
 
    if (uiobjParent->GetOwnerWindow() != GetOwnerWindow())
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
    : m_size(SizeF::DEFAULT_SIZE)
    , m_position(constants::DEFAULT_POSITION)
    , m_ownerWindow(nullptr)
    , m_backgroundColor(constants::COLOR_WHITE)
    , m_clippingEnabled(false)
    , m_fontCache(new FontCache(this))
    , m_isShown(false)
    , m_canRecieveMouseEvents(false)
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
    UIControl *sender = static_cast<UIControl*>(GetSender());
    if (sender->m_isShown && !WillBeShown() && sender->GetOwnerWindow())
    {
        sender->GetOwnerWindow()->ForgetMouseEnteredControl(sender, true);            
        sender->GetOwnerWindow()->ForgetPressedMouseButtons(sender, true);
    }
    sender->m_isShown = WillBeShown();
}

bool ShowEvent::WillBeShown() const
{
    return m_show;
}

Position UIControl::GetParentsPosition() const
{
    const UIControl *parent = static_cast<const UIControl*>(GetParent());
    Position parentsPos(0, 0);
    while (parent && parent->GetParent())
    {
        parentsPos += parent->GetPosition();
        parent = static_cast<const UIControl*>(parent->GetParent());
    }
    return parentsPos;
}

bool UIControl::HitTest(Position pos) const
{
    Position ctrlPos = (GetParent() ? GetPosition() + GetParentsPosition() : Position(0, 0));
    return pos.x >= ctrlPos.x && pos.x <= ctrlPos.x + GetClientSize().width 
        && pos.y >= ctrlPos.y && pos.y <= ctrlPos.y + GetClientSize().height; 
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
        if (m_canRecieveMouseEvents)
        {
            auto &recieversList = static_cast<UIControl*>(GetParent())->m_mouseEventRecievers;
            auto receiverIt = std::find(recieversList.begin(), recieversList.end(), this);
            recieversList.splice(recieversList.begin(), recieversList, receiverIt, std::next(receiverIt));
        }
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
        if (m_canRecieveMouseEvents)
        {
            auto &recieversList = static_cast<UIControl*>(GetParent())->m_mouseEventRecievers;
            auto receiverIt = std::find(recieversList.begin(), recieversList.end(), this);
            recieversList.splice(recieversList.end(), recieversList, receiverIt, std::next(receiverIt));
        }
    }
}

UIControl::~UIControl()
{
    if (GetOwnerWindow())
    {
        GetOwnerWindow()->ForgetMouseEnteredControl(this, false);
        GetOwnerWindow()->ForgetPressedMouseButtons(this, false);
    }
}

void UIControl::OnDraw(DrawEvent &e)
{
   Painter painter(e);
   painter.SetBrush(GetColor());
   painter.Clear();
}

const UIControlList &UIControl::GetMouseEventRecievers() const
{
    return m_mouseEventRecievers;
}

ARX_NAMESPACE_END
