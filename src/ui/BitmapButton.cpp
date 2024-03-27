#include "arxengine/ui/BitmapButton.h"
#include "arxengine/ui/Painter.h"
#include "arxengine/ui/MouseEvent.h"

ARX_NAMESPACE_BEGIN

void BitmapButton::SetMouseEnterImage(const Image &img){ m_mouseEnterTexture->SetData(img); }
void BitmapButton::SetMouseHoldImage(const Image &img) { m_mouseHoldTexture->SetData(img); }
void BitmapButton::SetNormalImage(const Image &img) { m_normalTexture->SetData(img); }

void BitmapButton::SetText(std::string_view text) 
{ 
    m_text = text;
    m_textExtent = GetFont().GetTextExtent(text);
}

const std::string &BitmapButton::GetText() const{ return m_text; }
void BitmapButton::SetTextColor(const Color &color) { m_textColor = color; }
const Color &BitmapButton::GetTextColor() const { return m_textColor; }

void BitmapButton::SetVerticalTextAlignment(VerticalTextAlignment verticalAlignment) { m_verticalTextAlignment = verticalAlignment; }
void BitmapButton::SetHorizontalTextAlignment(HorizontalTextAlignment horizontalAlignment) { m_horizontalTextAlignment = horizontalAlignment; }
void BitmapButton::SetTextAlignmentPaddingX(int x) { m_textPaddingX = x; }
void BitmapButton::SetTextAlignmentPaddingY(int y) { m_textPaddingY = y; }

VerticalTextAlignment BitmapButton::GetVerticalTextAlignment() const { return m_verticalTextAlignment; }
HorizontalTextAlignment BitmapButton::GetHorizontalTextAlignment() const { return m_horizontalTextAlignment; }
int BitmapButton::GetTextAlignmentPaddingX() const { return m_textPaddingX; }
int BitmapButton::GetTextAlignmentPaddingY() const { return m_textPaddingY; }

BitmapButton::BitmapButton(UIControl *parent, const Image &img, const SizeF &size, const Position &pos)
    : UIControl(parent, size, pos)
    , m_normalTexture(new Texture2D(this))
    , m_mouseEnterTexture(new Texture2D(this))
    , m_mouseHoldTexture(new Texture2D(this))
    , m_currentTexture(m_normalTexture)
    , m_textColor(constants::COLOR_BLACK)
    , m_isLeftMouseBtnDown(false)
    , m_isMouseEntered(false)
    , m_verticalTextAlignment(VerticalTextAlignment::Center)
    , m_horizontalTextAlignment(HorizontalTextAlignment::Center)
    , m_lastFontChange(GetFont().GetLastChangeTime())
    , m_textPaddingX(0)
    , m_textPaddingY(0)
{
    SetCanRecieveMouseEvents(true);
    m_normalTexture->SetData(img);
    m_mouseEnterTexture->SetData(img);
    m_mouseHoldTexture->SetData(img);
    m_normalTexture->SetTextureWrapping(Texture::TextureWrapping::ClampToEdge);
    m_mouseEnterTexture->SetTextureWrapping(Texture::TextureWrapping::ClampToEdge);
    m_mouseHoldTexture->SetTextureWrapping(Texture::TextureWrapping::ClampToEdge);

    GetEventManager().Bind<MouseEnterEvent>(&BitmapButton::OnMouseEnter, this);
    GetEventManager().Bind<MouseUpEvent>(&BitmapButton::OnMouseUp, this);
    GetEventManager().Bind<MouseDownEvent>(&BitmapButton::OnMouseDown, this);
    GetEventManager().Bind<MouseExitEvent>(&BitmapButton::OnMouseExit, this);
}

void BitmapButton::OnMouseEnter(MouseEnterEvent &)
{
    if (!m_isLeftMouseBtnDown)
        m_currentTexture = m_mouseEnterTexture;

    m_isMouseEntered = true;
}

void BitmapButton::OnMouseExit(MouseExitEvent &)
{
    if (!m_isLeftMouseBtnDown)
        m_currentTexture = m_normalTexture;

    m_isMouseEntered = false;
}

void BitmapButton::OnMouseDown(MouseDownEvent &e)
{
    if (e.GetMouseButtonType() == MouseButtonEvent::ButtonType::Left)
    {
        m_currentTexture = m_mouseHoldTexture;
        m_isLeftMouseBtnDown = true;
    }
}

void BitmapButton::OnMouseUp(MouseUpEvent &e)
{
    if (e.GetMouseButtonType() == MouseButtonEvent::ButtonType::Left)
    {
        if (m_isMouseEntered)
            m_currentTexture = m_mouseEnterTexture;
        else
            m_currentTexture = m_normalTexture;
        m_isLeftMouseBtnDown = false;
    }
}

void BitmapButton::OnDraw(DrawEvent &e)
{
    Painter p(e);
    p.DrawTexture2D(Position(0, 0), GetClientSize(), m_currentTexture);
    if (!GetFont().IsInvalid())
    {
        p.SetPen(m_textColor);
        if (GetFont().GetLastChangeTime() != GetFontCache()->GetLastFontChangeTime())
            m_textExtent = GetFont().GetTextExtent(GetText());

        p.RenderText(GetText(), CalculateTextPosition(m_textExtent, m_verticalTextAlignment, m_horizontalTextAlignment, m_textPaddingX, m_textPaddingY));
    }
}

ARX_NAMESPACE_END
