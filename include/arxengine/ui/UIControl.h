/*
 * @author Alexej Fedorenko
 */

#ifndef ARX_UICONTROL_H
#define ARX_UICONTROL_H

#include "arxengine/ArxDefines.h"
#include "arxengine/ui/Size.h"
#include "arxengine/ui/Color.h"
#include "arxengine/ui/Position.h"
#include "arxengine/ArxObject.h"
#include <list>
#include "arxengine/ui/FontCache.h"

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

/*
 * @descriptions controls that bind their own handler should call e.Skip()!
 */
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

using UIControlList = std::list<UIControl*>;

enum class VerticalTextAlignment
{
    Top, Center, Bottom
};

enum class HorizontalTextAlignment
{
    Left, Center, Right
};

class ARX_EXPORTS UIControl : public ArxObject
{
friend class ArxWindow;
friend class DrawEvent;
friend class ShowEvent;
public:
    UIControl(UIControl *parent, SizeF size = SizeF::DEFAULT_SIZE, Position pos = constants::DEFAULT_POSITION);
    
    virtual void SetBackgroundColor(Color c);
    virtual Color GetColor() const;

    virtual void SetSize(SizeF s);
    
    //size with borders (limits)
    virtual SizeF GetSize() const;

    //size available for client
    virtual SizeF GetClientSize() const;
    
    virtual void SetPosition(Position pos);
    virtual Position GetPosition() const;

    Position GetParentsPosition() const;

    //returns nullptr if UIControl is ArxWindow
    ArxWindow *GetOwnerWindow();
    const ArxWindow *GetOwnerWindow() const;
    
    //always returns window if has no owner window
    virtual ArxWindow *GetWindow();
    virtual const ArxWindow *GetWindow() const;
    
    void Show(bool visible = true);
    void Hide();
    bool IsShown() const;

    bool CanRecieveMouseEvents() const;
    virtual void SetCanRecieveMouseEvents(bool canRecieve);

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

    virtual ~UIControl();

    const UIControlList &GetMouseEventRecievers() const;

protected:
    Position CalculateTextPosition(const TextExtent &textExtent, VerticalTextAlignment verticalAlignment, HorizontalTextAlignment horizontalAlignment);

private:
    virtual void OnDraw(DrawEvent &);
    
    //this constructor is used for windows
    UIControl();
private:
    SizeF m_size;
    Position m_position;
    ArxWindow *m_ownerWindow;
    Color m_backgroundColor;
    bool m_clippingEnabled;
    Font m_font;
    FontCache *m_fontCache;
    bool m_isShown;
    UIControlList m_mouseEventRecievers;
    bool m_canRecieveMouseEvents;
};

ARX_NAMESPACE_END

#endif
