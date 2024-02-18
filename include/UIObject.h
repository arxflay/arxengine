#pragma once
#include "ArxDefines.h"
#include "Size.h"
#include "Color.h"
#include "Position.h"
#include "ArxObject.h"
#include <list>

ARX_NAMESPACE_BEGIN

class ArxWindow;
class UIObject;



class DrawEvent : public Event
{
private:
    void HandleEvent() override;
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
    
    ArxWindow *GetOwnerWindow();

    virtual void Show(bool visible = true) = 0;
    virtual void Hide();
    void Draw();

    //only UIObject can be parent
    void Reparent(ArxObject *parent) override;

protected:
    //this constructor is used for windows
    UIObject();
private:
    Size m_size;
    Position m_position;
    ArxWindow *m_ownerWindow;
    Color m_backgroundColor;
};

ARX_NAMESPACE_END
