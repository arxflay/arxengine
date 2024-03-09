#include "ui/ImageControl.h"
#include "ui/Painter.h"
#include <iostream>
ARX_NAMESPACE_BEGIN

ImageControl::ImageControl(UIControl *parent, const Image &image, Size size, Position pos)
    : UIControl(parent, size, pos)
    , m_texture(new Texture2D(this))
{
    Load(image);
}

bool ImageControl::Load(const Image &image)
{
    m_isValid = m_texture->SetData(image);
    return m_isValid;
}

bool ImageControl::IsInvalid() const
{
    return !m_isValid || m_texture->IsInvalid();
}

void ImageControl::OnDraw(DrawEvent &e)
{
    if (!IsInvalid())
    {
        Painter painter(e);
        painter.DrawTexture2D(Position(0, 0), GetClientSize(), m_texture);
    }
}

void ImageControl::SetFilteringMode(Texture::TextureFilteringMode filteringMode)
{
    m_texture->SetTextureFilteringMode(filteringMode);
}

void ImageControl::SetWrappingMode(Texture::TextureWrapping wrappingMode)
{
    m_texture->SetTextureWrapping(wrappingMode);
}

ARX_NAMESPACE_END
