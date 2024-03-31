#include "arxengine/ui/UIControl.h"
#include "arxengine/ui/ArxWindow.h"
#include <memory>
#include "arxengine/ui/Painter.h"
ARX_NAMESPACE_BEGIN

UIControl::UIControl(UIControl *parent, const SizeF &size, const Position &pos)
    : ArxObject(nullptr)
    , m_size(size)
    , m_position(pos)
    , m_backgroundColor(constants::COLOR_WHITE)
    , m_clippingEnabled(true)
    , m_isShown(true)
    , m_canRecieveMouseEvents(false)
    , m_canBeAffectedByCamera(false)
{
    if (parent == nullptr)
        throw ArxException(ArxException::ErrorCode::GenericError, "UIControl parent is null");

    m_fontCache = new FontCache(this);
    ArxObject::Reparent(parent);
    m_ownerWindow = parent->GetWindow();
    m_font = parent->GetFont();
}

void UIControl::SetBackgroundColor(const Color &c) { m_backgroundColor = c; }
const Color &UIControl::GetColor() const { return m_backgroundColor; }

void UIControl::SetSize(const SizeF &s) 
{
    m_size = s;
    if (GetParent() != GetOwnerWindow())
    {
        SizeF parentSize = static_cast<UIControl*>(GetParent())->GetClientSize();
        m_size = SizeF(std::clamp(m_size.width, 0.0f, parentSize.width), std::clamp(m_size.height, 0.0f, parentSize.height));
    }
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

const SizeF &UIControl::GetSize() const { return m_size; }
const SizeF &UIControl::GetClientSize() const { return m_size; }

void UIControl::SetPosition(const Position &pos) { m_position = pos; }
const Position &UIControl::GetPosition() const { return m_position; }

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
    : ArxObject(nullptr)
    , m_size(SizeF::DEFAULT_SIZE)
    , m_position(constants::DEFAULT_POSITION)
    , m_ownerWindow(nullptr)
    , m_backgroundColor(constants::COLOR_WHITE)
    , m_clippingEnabled(false)
    , m_isShown(false)
    , m_canRecieveMouseEvents(false)
    , m_canBeAffectedByCamera(false)
{
    m_fontCache = new FontCache(this);
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
    Position ctrlPos;
    if (GetParent())
    {
        ctrlPos = GetPosition() + GetParentsPosition();
        if (CanBeAffectedByCamera())
        {
            ctrlPos.x -= GetOwnerWindow()->GetCameraPos().x;
            ctrlPos.y += GetOwnerWindow()->GetCameraPos().y; //inversed y axis
        }
    }    

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
    if (GetOwnerWindow() && !GetOwnerWindow()->IsDestroyCalled())
    {
        GetOwnerWindow()->ForgetMouseEnteredControl(this, false);
        GetOwnerWindow()->ForgetPressedMouseButtons(this, false);
    }
}

void UIControl::OnDraw(DrawEvent &e)
{
   Painter painter(e);
   painter.SetBrush(GetColor());
   painter.DrawRectangle(Position(0,0), GetClientSize());
}

const UIControlList &UIControl::GetMouseEventRecievers() const
{
    return m_mouseEventRecievers;
}

Position UIControl::CalculateTextPosition(const TextExtent &textExtent, VerticalTextAlignment verticalAlignment, HorizontalTextAlignment horizontalAlignment, int paddingX, int paddingY)
{
    Position pos(paddingX, paddingY);
    switch(horizontalAlignment)
    {
        case HorizontalTextAlignment::Center:
            pos.x += (GetClientSize().width - textExtent.widthSum) / 2;
            break;
        case HorizontalTextAlignment::Right:
            pos.x += GetClientSize().width - textExtent.widthSum;
            break;
        case HorizontalTextAlignment::Left:
            break;
    }

    switch(verticalAlignment)
    {
        case VerticalTextAlignment::Center:
            pos.y += std::max((GetClientSize().height + (textExtent.yMax - textExtent.yMin - textExtent.newLineHeightSum)) / 2, 0.0f);
            break;
        case VerticalTextAlignment::Bottom:
            pos.y += GetClientSize().height - textExtent.yMin - textExtent.newLineHeightSum;
            break;
        case VerticalTextAlignment::Top:
            pos.y += textExtent.yMax;
            break;
    }

    return pos;
}

bool UIControl::CanBeAffectedByCamera() const
{
    return m_canBeAffectedByCamera;
}

void UIControl::SetCanBeAffectedByCamera(bool canBeAffectedByCamera)
{
    m_canBeAffectedByCamera = canBeAffectedByCamera;
}

void UIControl::Center()
{
    SetPosition(CalculateCenterPosition());
}

Position UIControl::CalculateCenterPosition()
{
    UIControl *parent = static_cast<UIControl*>(GetParent());
    Position pos;
    pos.x = (parent->m_size.width - m_size.width) / 2;
    pos.y = (parent->m_size.height - m_size.height) / 2;
    return pos;
}

void UIControl::EnableTextSmoothing(bool enable)
{
    GetFontCache()->EnableFontSmoothing(enable);
}

void UIControl::EnableTextAntialising(bool enable)
{
    GetFontCache()->EnableAntialising(enable);
}

bool UIControl::IsTextSmoothingEnabled() const
{
    return m_fontCache->IsFontSmoothingEnabled();
}

bool UIControl::IsTextAntialisingEnabled() const 
{
    return m_fontCache->IsAntialisingEnabled();
}

ARX_NAMESPACE_END
