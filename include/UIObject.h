#pragma once
#include "color.h"
#include "Size.h"
#include "Position.h"
#include <vector>
#include <type_traits>
#include "EventHandler.h"

ARX_NAMESPACE_BEGIN

class SizeEvent : public Event
{

};

class DrawEvent : public Event
{

};

class CloseEvent : public Event
{

};

class ARX_EXPORTS UIObject : public IEventHandlable 
{
public:
    UIObject(UIObject *parent, Size size = defaults::DEFAULT_SIZE, Position position = defaults::DEFAULT_POSITION);
    virtual ~UIObject();
private:
    int GetNewMinSizeParameter(int newParam, int maxSizeParam, int rectParam);
    int GetNewMaxSizeParameter(int newParam, int minSizeParam, int rectParam);
    int GetNewSizeParameter(int newParam, int maxSizeParam, int minSizeParam, int rectParam);
public:
    virtual Size GetRectSize() = 0;
    virtual void SetMinSize(const Size &s);
    Size GetMinSize() const noexcept; 
    virtual void SetMaxSize(const Size &s);
    Size GetMaxSize() const noexcept;
    virtual void SetSize(const Size &s);
    Size GetSize() const noexcept;
    
    virtual void SetBackgroundColor(Color c);
    Color GetBackgroundColor() const noexcept;

    virtual void SetPosition(const Position &pos);
    Position GetPosition() const noexcept;
    
    virtual void Show(bool visible = true) = 0;
    virtual void Hide();
    virtual void Draw();
    virtual void SetFocus(bool focus = true) = 0;

    UIObject *GetParent() noexcept;
    std::vector<UIObject*> &GetChildren() noexcept;
    void SetParent(UIObject *parent);
    void RemoveChild(UIObject *child);

    EventHandler &GetEventHandler() noexcept override;
    const EventHandler &GetEventHandler() const noexcept override;
    
    /**
     * @brief Destroys function in preferable way
     * @details Use this function instead of delete keyword to avoid dangling events.
     * Raises close event to this object and children
     */
    void Destroy();

private:


    UIObject *m_parent;
    Size m_minSize;
    Size m_maxSize;
    Size m_size;
    Position m_position;
    Color m_color;
    std::vector<UIObject*> m_children;
    EventHandler m_eventHandler;
    bool m_destroyCalled;

    /*event part*/
};

ARX_NAMESPACE_END
