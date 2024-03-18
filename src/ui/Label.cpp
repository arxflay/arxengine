#include "arxengine/ui/Label.h"
#include "arxengine/ui/Painter.h"
ARX_NAMESPACE_BEGIN

Label::Label(UIControl *parent, std::string_view text, Size size, Position pos)
    : UIControl(parent, size, pos)
    , m_text(text)
    , m_textColor(constants::COLOR_BLACK)
{
}

void Label::SetText(std::string_view text) { m_text = text; }
const std::string &Label::GetText() const { return m_text; }

void Label::SetTextColor(Color color) { m_textColor = color; }
Color Label::GetTextColor() const { return m_textColor; }

void Label::EnableFontSmoothing(bool enable) { GetFontCache()->EnableFontSmoothing(enable); }

void Label::OnDraw(DrawEvent &e)
{
    if (!GetFont().IsInvalid())
    {
        Painter p(e);
        //for now only centered position is supported
        p.SetPen(Pen(GetTextColor()));
        p.DrawText(GetText(), Position(GetPosition().x, GetPosition().y + (GetSize().height / 2)));
    }
}

ARX_NAMESPACE_END
