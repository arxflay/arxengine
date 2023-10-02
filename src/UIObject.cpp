#include "UIObject.h"
#include <algorithm>
#include <iostream>
ARX_NAMESPACE_BEGIN

void TestHandler(SizeEvent &)
{
    std::cout << "Ahoj" << std::endl;
}

void CloseEventHandler(CloseEvent &e)
{
    delete e.GetCallingObject();
}

UIObject::UIObject(UIObject *parent, Size size, Position position)
    : m_parent(nullptr)
    , m_minSize(defaults::IGNORE_SIZE)
    , m_maxSize(defaults::IGNORE_SIZE)
    , m_size(size)
    , m_position(position)
    , m_color(defaults::WHITE_COLOR)
    , m_eventHandler(this)
    , m_destroyCalled(false)
{
    SetParent(parent);
    GetEventHandler().Bind<CloseEvent>(CloseEventHandler);
    GetEventHandler().Bind(TestHandler);
}

void UIObject::SetParent(UIObject *parent)
{
    assert(parent == nullptr || !parent->m_destroyCalled);
    
    if (m_parent)
        m_parent->RemoveChild(this); 
    m_parent = parent;
    if(parent)
        parent->m_children.push_back(this);
}

UIObject::~UIObject()
{
    if (!m_destroyCalled)
        for (UIObject *obj : m_children)
            obj->Destroy();
}

int UIObject::GetNewMinSizeParameter(int newParam, int maxSizeParam, int rectParam)
{
    if (newParam == defaults::IGNORE)
        return newParam;
    
    if(newParam > maxSizeParam && maxSizeParam != defaults::IGNORE)
        newParam = maxSizeParam; 
    else if (newParam < rectParam && rectParam != defaults::IGNORE)
        newParam = rectParam;

    return newParam;
}

int UIObject::GetNewMaxSizeParameter(int newParam, int minSizeParam, int rectParam)
{
    if (newParam == defaults::IGNORE)
        return newParam;
    
    if(newParam < minSizeParam && minSizeParam != defaults::IGNORE)
        newParam = minSizeParam; 
    else if (newParam < rectParam && rectParam != defaults::IGNORE)
        newParam = rectParam;

    return newParam;
}

int UIObject::GetNewSizeParameter(int newParam, int maxSizeParam, int minSizeParam, int rectParam)
{
    if(newParam > maxSizeParam && maxSizeParam != defaults::IGNORE)
        newParam = maxSizeParam;
    else if (newParam < minSizeParam && minSizeParam != defaults::IGNORE)
        newParam = minSizeParam;
    else if (newParam < rectParam && rectParam != defaults::IGNORE)
        newParam = rectParam;

    return newParam;
}

/* connect all sizes with set size callback (GLFWwindow*) */
void UIObject::SetMinSize(const Size &s)
{
    Size newSize = s.GetClampedSize();
    newSize.width = GetNewMinSizeParameter(s.width, m_maxSize.width, GetRectSize().width);
    newSize.height = GetNewMinSizeParameter(s.height, m_maxSize.height, GetRectSize().height);
    m_minSize = newSize;
}

Size UIObject::GetMinSize() const noexcept
{
    return m_minSize;
}

void UIObject::SetMaxSize(const Size &s)
{
    Size newSize = s.GetClampedSize();
    newSize.width = GetNewMaxSizeParameter(s.width, m_minSize.width, GetRectSize().width);
    newSize.height = GetNewMaxSizeParameter(s.height, m_minSize.height, GetRectSize().height);
    m_minSize = newSize;
}

Size UIObject::GetMaxSize() const noexcept
{
    return m_maxSize;
}

void UIObject::SetSize(const Size &s)
{
    Size newSize = s.GetClampedSize();
    newSize.width = GetNewSizeParameter(s.width, m_maxSize.width, m_minSize.width, GetRectSize().width);
    newSize.height = GetNewSizeParameter(s.height, m_maxSize.height, m_minSize.height, GetRectSize().height);
    m_size = newSize;

    SizeEvent evt;
    GetEventHandler().RaiseEvent(std::move(evt));
}

Size UIObject::GetSize() const noexcept
{
    return m_size;
}

void UIObject::SetBackgroundColor(Color c)
{
    m_color = c;
}

Color UIObject::GetBackgroundColor() const noexcept
{
    return m_color;
}

void UIObject::SetPosition(const Position &pos)
{
    m_position = pos;
}

Position UIObject::GetPosition() const noexcept
{
    return m_position;
}

void UIObject::Hide()
{
    Show(false);
}

/*
bool UIObject::Show(bool visible = true) = 0;
bool UIObject::Hide() = 0;
*/
void UIObject::Draw()
{
    for (UIObject *obj : GetChildren())
        obj->Draw();
}

/*
bool UIObject::SetFocus(bool focus = true) = 0;
*/

UIObject *UIObject::GetParent() noexcept
{
    return m_parent;
}

std::vector<UIObject*> &UIObject::GetChildren() noexcept
{
    return m_children;
}

void UIObject::RemoveChild(UIObject *child)
{
    assert(!m_destroyCalled);
    auto it = std::find_if(m_children.begin(), m_children.end(), [child](UIObject *o) { return child == o; });
    assert(it != m_children.end());
    m_children.erase(it);
}

EventHandler &UIObject::GetEventHandler() noexcept
{
    return m_eventHandler;
}

const EventHandler &UIObject::GetEventHandler() const noexcept
{
    return m_eventHandler;
}

void UIObject::Destroy()
{
    if (!m_destroyCalled)
    {
        m_destroyCalled = true;
        //All children should be also destroyed
        for (UIObject *obj : GetChildren())
            obj->Destroy(); //TODO exception could occur during Destruction
        
        //Raise CloseEvent
        GetEventHandler().RaiseEvent<CloseEvent>(CloseEvent{});

        //Seal event handler 
        GetEventHandler().Seal();
    }
}

ARX_NAMESPACE_END
