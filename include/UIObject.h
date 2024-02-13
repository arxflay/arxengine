#pragma once
#include "ArxDefines.h"
#include "Size.h"
#include "Color.h"
#include "Position.h"
#include "ArxObject.h"
#include <list>
#include "EventManager.h"

ARX_NAMESPACE_BEGIN

class ArxWindow;
class UIObject;
using ChildrenList = std::list<UIObject*>;

class DestroyEvent : public Event
{
public:
    DestroyEvent(UIObject *sender);
private:
    void HandleEvent() override;
};

class DrawEvent : public Event
{
public:
    DrawEvent(UIObject *sender);
private:
    void HandleEvent() override {};
};

class UIObject : public ArxObject
{
public:
    UIObject(UIObject *parent, Size size = defaults::DEFAULT_SIZE, Position pos = defaults::DEFAULT_POSITION);
    
    virtual void SetBackgroundColor(Color c);
    virtual Color GetColor() const;

    virtual void SetSize(Size s);
    virtual Size GetSize() const;
    
    virtual void SetPosition(Position pos);
    virtual Position GetPosition() const;
    
    virtual ArxWindow *GetOwnerWindow();
    virtual UIObject *GetParent();

    virtual void Show(bool visible = true) = 0;
    virtual void Hide();


    ChildrenList &GetChildren();

    void Draw();
    void Destroy();
    EventManager GetEventManager();

    //void Draw(); //TODO
private:
    UIObject *m_parent;
    Size m_size;
    Position m_position;
    ArxWindow *m_ownerWindow;

    Color m_backgroundColor;
    ChildrenList m_children;
    EventManager m_eventManager;
};

ARX_NAMESPACE_END
