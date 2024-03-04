#include "gl/Texture2D.h"
#include "ui/UIControl.h"
#include <glad/glad.h>

ARX_NAMESPACE_BEGIN
class Image;


Texture2D::Texture2D(UIControl *obj)
    : Texture(obj)
    , m_filteringMode(Texture::TextureFilteringMode::Default)
    , m_wrappingMode(Texture::TextureWrapping::Default)
{
    SetTextureType(Texture::TextureType::Texture2D);
    SetTextureUnit(Texture::TextureUnit::Tex0);
    SetTextureFilteringMode(Texture::TextureFilteringMode::Default);
    SetTextureWrapping(Texture::TextureWrapping::Repeat);
}

bool Texture2D::SetData(const Image &image)
{
    if (IsInvalid())
    {
        GLOG->Error("Texture2D is in invalid state");
        return false;
    }
    else if (image.IsInvalid())
    {
        GLOG->Error("Provided image is invalid");
        return false;
    }

    OldTextureGuard guard(GetTextureType());
    Bind();

    //By default opengl reads by four bytes
    glPixelStorei(GL_UNPACK_ALIGNMENT, DetermineBestPackingAlignmentSize(image));
    
    //format = how interpret data
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(ImageChannelsToGL(image)), static_cast<GLsizei>(image.GetSize().width), static_cast<GLsizei>(image.GetSize().height), 0, ImageChannelsToGL(image), GL_UNSIGNED_BYTE, image.GetData().data());
    glGenerateMipmap(GL_TEXTURE_2D);
    return true;
}

void Texture2D::SetTextureFilteringMode(TextureFilteringMode textureFiltering)
{
    OldTextureGuard guard(GetTextureType());
    Bind();
    glTexParameteri(static_cast<GLenum>(GetTextureType()), GL_TEXTURE_MAG_FILTER, static_cast<GLint>(textureFiltering));
    glTexParameteri(static_cast<GLenum>(GetTextureType()), GL_TEXTURE_MIN_FILTER, (textureFiltering == Texture::TextureFilteringMode::Nearest) ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR);
    m_filteringMode = textureFiltering;
}

void Texture2D::SetTextureWrapping(TextureWrapping textureWrapping)
{
    OldTextureGuard guard(GetTextureType());
    Bind();
    glTexParameteri(static_cast<GLenum>(GetTextureType()), GL_TEXTURE_WRAP_S, static_cast<GLint>(textureWrapping));
    glTexParameteri(static_cast<GLenum>(GetTextureType()), GL_TEXTURE_WRAP_T, static_cast<GLint>(textureWrapping));
    m_wrappingMode = textureWrapping;
}

Texture2D::TextureFilteringMode Texture2D::GetTextureFilteringMode() const { return m_filteringMode; }
Texture2D::TextureWrapping Texture2D::GetTextureWrapping() const { return m_wrappingMode; }

Texture2D *Texture2D::Clone()
{
    //FIXME
    throw ArxException(ArxException::ErrorCode::GenericError, "Texture2D::Clone not implemented");
    std::unique_ptr<Texture2D> clone(static_cast<Texture2D*>(Texture::Clone()));
    clone->SetTextureFilteringMode(m_filteringMode);
    clone->SetTextureWrapping(m_wrappingMode);
    //implement copying image
    return clone.release();
}

Texture2D *Texture2D::AllocClone()
{
    return new Texture2D(GetOwnerUIControl());
}

ARX_NAMESPACE_END
