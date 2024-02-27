#ifndef ARX_UIOBJECT_H
#define ARX_UIOBJECT_H

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
    
    //size with borders (limits)
    virtual Size GetSize() const;

    //size available for client
    virtual Size GetClientSize() const;
    
    virtual void SetPosition(Position pos);
    virtual Position GetPosition() const;
    
    ArxWindow *GetOwnerWindow();

    virtual void Show(bool visible = true) = 0;
    virtual void Hide();

    //only UIObject can be parent
    void Reparent(ArxObject *parent) override;
    virtual void EnableClipToBounds(bool enable = true);
    virtual bool IsEnabledClipToBounds() const;

protected:
    //this constructor is used for windows
    UIObject();
private:
    Size m_size;
    Position m_position;
    ArxWindow *m_ownerWindow;
    Color m_backgroundColor;
    bool m_clippingEnabled;
};

ARX_NAMESPACE_END

#endif
