#include "ui/BitmapButton.h"
#include "ui/Painter.h"
#include "ui/MouseEvent.h"

ARX_NAMESPACE_BEGIN

void BitmapButton::SetMouseEnterImage(const Image &img){ m_mouseEnterTexture->SetData(img); }
void BitmapButton::SetMouseHoldImage(const Image &img) { m_mouseHoldTexture->SetData(img); }
void BitmapButton::SetNormalImage(const Image &img) { m_normalTexture->SetData(img); }
void BitmapButton::SetText(std::string_view text) { m_text = text; }
const std::string &BitmapButton::GetText() const{ return m_text; }
void BitmapButton::SetTextColor(Color color) { m_textColor = color; }
Color BitmapButton::GetTextColor() const { return m_textColor; }

BitmapButton::BitmapButton(UIControl *parent, const Image &img, Size size, Position pos)
    : UIControl(parent, size, pos)
    , m_normalTexture(new Texture2D(this))
    , m_mouseEnterTexture(new Texture2D(this))
    , m_mouseHoldTexture(new Texture2D(this))
    , m_currentTexture(m_normalTexture)
    , m_textColor(constants::COLOR_BLACK)
    , m_isLeftMouseBtnDown(false)
    , m_isMouseEntered(false)
{
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
    p.DrawTexture2D(GetPosition(), GetClientSize(), m_currentTexture);
    if (!GetFont().IsInvalid())
    {
        p.SetPen(m_textColor);
        p.DrawText(GetText(), Position(GetPosition().x, GetSize().height / 2));
    }
}

ARX_NAMESPACE_END
