/*
 * @author Alexej Fedorenko
 */

#ifndef ARX_BITMAPBUTTON_H
#define ARX_BITMAPBUTTON_H
#include "arxengine/ui/UIControl.h"

ARX_NAMESPACE_BEGIN

class MouseDownEvent;
class MouseEnterEvent;
class MouseExitEvent;
class MouseUpEvent;

class ARX_EXPORTS BitmapButton : public UIControl
{
public:
    BitmapButton(UIControl *parent, const Image &img = {}, const SizeF &size = SizeF::DEFAULT_SIZE, const Position &pos = constants::DEFAULT_POSITION);
    void SetMouseEnterImage(const Image &img);
    void SetMouseHoldImage(const Image &img);
    void SetNormalImage(const Image &img);
    void SetText(std::string_view text);
    const std::string &GetText() const;
    void SetTextColor(const Color&);
    const Color &GetTextColor() const;

    void SetVerticalTextAlignment(VerticalTextAlignment verticalAlignment);
    void SetHorizontalTextAlignment(HorizontalTextAlignment horizontalAlignment);
    void SetTextAlignmentPaddingX(int x);
    void SetTextAlignmentPaddingY(int y);

    VerticalTextAlignment GetVerticalTextAlignment() const;
    HorizontalTextAlignment GetHorizontalTextAlignment() const;
    int GetTextAlignmentPaddingX() const;
    int GetTextAlignmentPaddingY() const;
    void OnDraw(DrawEvent &e) override;

private:
    void OnMouseEnter(MouseEnterEvent &e);
    void OnMouseExit(MouseExitEvent &e);
    void OnMouseDown(MouseDownEvent &e);
    void OnMouseUp(MouseUpEvent &e);

    Texture2D *m_normalTexture;
    Texture2D *m_mouseEnterTexture;
    Texture2D *m_mouseHoldTexture;
    Texture2D *m_currentTexture;
    std::string m_text;
    Color m_textColor;
    bool m_isLeftMouseBtnDown;
    bool m_isMouseEntered;
    TextExtent m_textExtent;
    VerticalTextAlignment m_verticalTextAlignment;
    HorizontalTextAlignment m_horizontalTextAlignment;
    int m_textPaddingX;
    int m_textPaddingY;
};


ARX_NAMESPACE_END

#endif
