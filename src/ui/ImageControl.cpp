#include "ui/ImageControl.h"
#include "ui/Painter.h"
#include <iostream>
ARX_NAMESPACE_BEGIN

ImageControl::ImageControl(UIControl *parent, const Image &image, Size size, Position pos)
    : UIControl(parent, size, pos)
    , m_texture(new Texture2D(this))
    , m_tileData({1, 1})
{
    m_texture->SetTextureWrapping(Texture::TextureWrapping::ClampToEdge);
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
        painter.DrawTexture2D(Position(0, 0), GetClientSize(), m_texture, m_tileData.tileWidthCount, m_tileData.tileHeightCount);
    }
}

void ImageControl::SetFilteringMode(Texture::TextureFilteringMode filteringMode)
{
    m_texture->SetTextureFilteringMode(filteringMode);
}

void ImageControl::EnableTilingMode(std::optional<TileData> tileData)
{
    if (tileData.has_value())
    {
        m_texture->SetTextureWrapping(Texture::TextureWrapping::Repeat);
        m_tileData = tileData.value();
    }
    else
    {
        m_texture->SetTextureWrapping(Texture::TextureWrapping::ClampToEdge);
        m_tileData = {1, 1};
    }
}

bool ImageControl::IsTilingModeEnabled() const
{
    return m_texture->GetTextureWrapping() == Texture::TextureWrapping::Repeat;
}

ARX_NAMESPACE_END
