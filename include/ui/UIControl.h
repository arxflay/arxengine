#ifndef ARX_UICONTROL_H
#define ARX_UICONTROL_H

#include "ArxDefines.h"
#include "Size.h"
#include "Color.h"
#include "Position.h"
#include "ArxObject.h"
#include <list>
#include "FontCache.h"

ARX_NAMESPACE_BEGIN

class ArxWindow;
class UIControl;

class ARX_EXPORTS DrawEvent : public Event
{
friend class ArxWindow;
private:
    DrawEvent() = default;
    void HandleEvent() override;
};

class ARX_EXPORTS ShowEvent : public Event
{
friend class UIControl; 
public:
    void SetWillBeShown(bool show);
    bool WillBeShown() const;
private:
    void HandleEvent() override;
    ShowEvent();
    bool m_show;
};


class ARX_EXPORTS UIControl : public ArxObject
{
friend class ArxWindow;
friend class DrawEvent;
friend class ShowEvent;
public:
    UIControl(UIControl *parent, Size size = constants::DEFAULT_SIZE, Position pos = constants::DEFAULT_POSITION);
    
    virtual void SetBackgroundColor(Color c);
    virtual Color GetColor() const;

    virtual void SetSize(Size s);
    
    //size with borders (limits)
    virtual Size GetSize() const;

    //size available for client
    virtual Size GetClientSize() const;
    
    virtual void SetPosition(Position pos);
    virtual Position GetPosition() const;

    //returns nullptr if UIControl is ArxWindow
    ArxWindow *GetOwnerWindow();
    const ArxWindow *GetOwnerWindow() const;
    
    //always returns window if has no owner window
    virtual ArxWindow *GetWindow();
    virtual const ArxWindow *GetWindow() const;
    
    void Show(bool visible = true);
    void Hide();
    bool IsShown() const;

    //only UIControl can be parent
    void Reparent(ArxObject *parent) override;
    virtual void EnableClipToBounds(bool enable = true);
    virtual bool IsEnabledClipToBounds() const;

    bool HitTest(Position pos) const;

    FontCache *GetFontCache();
    Font &GetFont();
    void SetFont(Font &&font);
    void SetFont(const Font &font);

    void MoveToFront();
    void MoveToBack();

private:
    virtual void OnDraw(DrawEvent &) { /*do nothing*/ };
    
    //this constructor is used for windows
    UIControl();

private:
    Size m_size;
    Position m_position;
    ArxWindow *m_ownerWindow;
    Color m_backgroundColor;
    bool m_clippingEnabled;
    Font m_font;
    FontCache *m_fontCache;
    bool m_isShown;
};

ARX_NAMESPACE_END

#endif
