/*
 * @author Alexej Fedorenko
 */

#ifndef ARX_LABEL_H
#define ARX_LABEL_H
#include "arxengine/ui/UIControl.h"

ARX_NAMESPACE_BEGIN

class ARX_EXPORTS Label : public UIControl
{
public:
    Label(UIControl *parent, std::string_view text = "", const SizeF &size = SizeF::DEFAULT_SIZE, const Position &pos = constants::DEFAULT_POSITION);
    void SetText(std::string_view text); 
    const std::string &GetText() const;

    void SetTextColor(const Color &color);
    const Color &GetTextColor() const;

    void EnableFontSmoothing(bool enable);
    void SetVerticalTextAlignment(VerticalTextAlignment verticalAlignment);
    void SetHorizontalTextAlignment(HorizontalTextAlignment horizontalAlignment);
    void SetTextAlignmentPaddingX(int x);
    void SetTextAlignmentPaddingY(int y);

    VerticalTextAlignment GetVerticalTextAlignment() const;
    HorizontalTextAlignment GetHorizontalTextAlignment() const;
    int GetTextAlignmentPaddingX() const;
    int GetTextAlignmentPaddingY() const;

    void EnableAutoresize(bool enableResize);
private:
    void OnDraw(DrawEvent &e) override;
    void UpdateTextExtent();
    std::string m_text;
    Color m_textColor;
    TextExtent m_textExtent;
    VerticalTextAlignment m_verticalTextAlignment;
    HorizontalTextAlignment m_horizontalTextAlignment;
    int m_textPaddingX;
    int m_textPaddingY;
    bool m_autoresizeEnabled;
};

ARX_NAMESPACE_END

#endif
