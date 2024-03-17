#include "gl/Texture.h"
#include "media/Image.h"
#include "ui/ArxWindow.h"
#include <utility>
#include <ArxException.h>
#include <glad/glad.h>

ARX_NAMESPACE_BEGIN

namespace
{
    GLenum TextureTypeToTextureBinding(Texture::TextureType textureType)
    {
        switch (textureType)
        {
            case Texture::TextureType::Texture2D:
                return GL_TEXTURE_BINDING_2D;
            default:
                return GL_INVALID_ENUM;
        }
    }

    GLuint GetCurrentlyBoundTexture(Texture::TextureType textureType)
    {
       GLint texture = 0;
       glGetIntegerv(TextureTypeToTextureBinding(textureType), &texture);
       return static_cast<GLuint>(texture);
    }

    GLuint GetCurrentlyBoundTextureUnit()
    {
        GLint textureUnit = 0;
        glGetIntegerv(GL_ACTIVE_TEXTURE, &textureUnit);
        return static_cast<GLuint>(textureUnit);
    }
}

/*static*/ unsigned int Texture::ImageChannelsToGL(const Image &img)
{
    switch (img.GetColorChannels())
    {
        case 1:
            return GL_RED;
        case 2:
            return GL_RG;
        case 3:
            return GL_RGB;
        case 4:
            return GL_RGBA;
        default:
            return GL_INVALID_ENUM;
    } 
}

Texture::OldTextureGuard::OldTextureGuard(Texture::TextureType textureType)
    : m_textureType(textureType)
{
    m_texture = GetCurrentlyBoundTexture(m_textureType);
    m_oldTextureUnit = GetCurrentlyBoundTextureUnit();
}

Texture::OldTextureGuard::~OldTextureGuard()
{
    glActiveTexture(m_oldTextureUnit);
    glBindTexture(static_cast<GLenum>(m_textureType), m_texture);
}

void Texture::Bind() const
{
    GetWindow()->SetAsCurrentContext();
    glActiveTexture(static_cast<GLenum>(m_textureUnit));
    glBindTexture(static_cast<GLenum>(m_textureType), m_texture);
}

void Texture::Unbind() const
{
    if (IsBound())
        glBindTexture(static_cast<GLenum>(GetTextureType()), 0);
}

void Texture::SetTextureType(TextureType textureType)
{
    m_textureType = textureType;
}

bool Texture::IsBound() const
{
    return (GetCurrentlyBoundTexture(GetTextureType()) == m_texture && GetCurrentlyBoundTextureUnit() == static_cast<GLuint>(GetTextureUnit()));
}

void Texture::SetTextureUnit(TextureUnit textureUnit)
{
    if (IsBound())
    {
        glBindTexture(static_cast<GLenum>(GetTextureType()), m_texture);
        glActiveTexture(static_cast<GLenum>(textureUnit));
        glBindTexture(static_cast<GLenum>(GetTextureType()), m_texture);
    }

    m_textureUnit = textureUnit;
}

/*static*/ int Texture::DetermineBestPackingAlignmentSize(const Image &img)
{
    int packAlignment = 1;
    size_t width = static_cast<size_t>(img.GetSize().width);
    if (width % 4 == 0)
        packAlignment = 4;
    else if (width % 2 == 0)
        packAlignment = 2;

    return packAlignment;
}

Texture::Texture(UIControl *parent)
    : UIObject(parent)
    , m_texture(0)
    , m_textureType(Texture::TextureType::Default)
    , m_textureUnit(Texture::TextureUnit::Default)
{
    GetWindow()->SetAsCurrentContext();
    glGenTextures(1, &m_texture);
    if (m_texture == 0)
        GLOG->Error("failed to create texture, gl_error %d",  glGetError());
}

Texture::TextureType Texture::GetTextureType() const
{
    return m_textureType;
}

Texture::TextureUnit Texture::GetTextureUnit() const
{
    return m_textureUnit;
}

//cleanup
Texture::~Texture()
{
    if (m_texture != 0)
    {
        GetWindow()->SetAsCurrentContext();
        glDeleteTextures(1, &m_texture);
    }
}

bool Texture::IsInvalid() const
{
    return m_texture == 0;
}

ARX_NAMESPACE_END
