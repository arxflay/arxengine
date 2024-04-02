#include "arxengine/ui/Label.h"
#include "arxengine/ui/Painter.h"
ARX_NAMESPACE_BEGIN

Label::Label(UIControl *parent, std::string_view text, const SizeF &size, const Position &pos)
    : UIControl(parent, size, pos)
    , m_text(text)
    , m_textColor(constants::COLOR_BLACK)
    , m_verticalTextAlignment(VerticalTextAlignment::Top)
    , m_horizontalTextAlignment(HorizontalTextAlignment::Left)
    , m_textPaddingX(0)
    , m_textPaddingY(0)
    , m_autoresizeEnabled(false)
{
}

void Label::SetText(std::string_view text) { 
    m_text = text;
    if (m_autoresizeEnabled)
        UpdateTextExtent();
}
const std::string &Label::GetText() const { return m_text; }

void Label::SetTextColor(const Color &color) { m_textColor = color; }
const Color &Label::GetTextColor() const { return m_textColor; }

void Label::EnableFontSmoothing(bool enable) { GetFontCache()->EnableFontSmoothing(enable); }


void Label::SetVerticalTextAlignment(VerticalTextAlignment verticalAlignment) { m_verticalTextAlignment = verticalAlignment; }
void Label::SetHorizontalTextAlignment(HorizontalTextAlignment horizontalAlignment) { m_horizontalTextAlignment = horizontalAlignment; }
void Label::SetTextAlignmentPaddingX(int x) { m_textPaddingX = x; }
void Label::SetTextAlignmentPaddingY(int y) { m_textPaddingY = y; }

VerticalTextAlignment Label::GetVerticalTextAlignment() const { return m_verticalTextAlignment; }
HorizontalTextAlignment Label::GetHorizontalTextAlignment() const { return m_horizontalTextAlignment; }
int Label::GetTextAlignmentPaddingX() const { return m_textPaddingX; }
int Label::GetTextAlignmentPaddingY() const { return m_textPaddingY; }

void Label::EnableAutoresize(bool enableResize)
{
    m_autoresizeEnabled = enableResize;
    UpdateTextExtent();
}

void Label::UpdateTextExtent()
{
    m_textExtent = GetFont().GetTextExtent(GetText());
    if (m_autoresizeEnabled)
        SetSize(SizeF(m_textExtent.widthSum, m_textExtent.maxHeight + m_textExtent.newLineHeightSum));
}

void Label::OnDraw(DrawEvent &e)
{
    if (!GetFont().IsInvalid())
    {
        Painter p(e);
        p.SetPen(Pen(GetTextColor()));
        if (GetFont().GetLastChangeTime() != GetFontCache()->GetLastFontChangeTime())
            UpdateTextExtent(); 
        p.RenderText(GetText(), CalculateTextPosition(m_textExtent, m_verticalTextAlignment, m_horizontalTextAlignment, m_textPaddingX, m_textPaddingY));
    }
}

ARX_NAMESPACE_END
