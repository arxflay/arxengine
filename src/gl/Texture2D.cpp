#include "gl/Texture2D.h"
#include "ui/UIObject.h"

ARX_NAMESPACE_BEGIN
class Image;


Texture2D::Texture2D(UIObject *obj)
    : Texture(obj)
{
    SetTextureType(Texture::TextureType::Texture2D);
    SetTextureUnit(Texture::TextureUnit::Tex0);
}

void Texture2D::Bind() const
{
    Texture::Bind();
}

void Texture2D::Unbind() const
{
    Texture::Unbind();
}

void Texture2D::SetData(const Image &image)
{
    Texture::Set2DData(image);
}

void Texture2D::SetTextureUnit(Texture::TextureUnit textureUnit)
{
    Texture::SetTextureUnit(textureUnit);
}

bool Texture2D::IsInvalid() const
{
    return Texture::IsInvalid();
}

ARX_NAMESPACE_END
