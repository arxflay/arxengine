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

    GLenum ImageChannelsToGL(const Image &img)
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

    class OldTextureGuard final
    {
    public:
        OldTextureGuard(Texture::TextureType textureType)
            : m_textureType(textureType)
        {
            m_texture = GetCurrentlyBoundTexture(m_textureType);
            m_oldTextureUnit = GetCurrentlyBoundTextureUnit();
        }

        ~OldTextureGuard()
        {
            glActiveTexture(m_oldTextureUnit);
            glBindTexture(static_cast<GLenum>(m_textureType), m_texture);
        }
    private:
        Texture::TextureType m_textureType;
        unsigned int m_oldTextureUnit;
        unsigned int m_texture;
    };
}

void Texture::Bind() const
{
    m_window->SetAsCurrentContext();
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
namespace
{
    int DetermineBestPackingAlignmentSize(const Image &img)
    {
        int unpackAlignment = 1;
        size_t width = static_cast<size_t>(img.GetSize().width);
        if (width % 4 == 0)
            unpackAlignment = 4;
        else if (width % 2 == 0)
            unpackAlignment = 2;

        return unpackAlignment;
    }
}

Texture::Texture(UIObject *obj)
    : m_texture(0)
{
    if (!obj)
        throw ArxException(ArxException::ErrorCode::GenericError, "Error during creating texture, uiobject is nullptr");

    Reparent(obj);
    m_window = (obj->GetOwnerWindow() == nullptr) ? static_cast<ArxWindow*>(obj) : obj->GetOwnerWindow();
    m_window->SetAsCurrentContext();
    glGenTextures(1, &m_texture);
    if (m_texture == 0)
        GLOG->Error("failed to create texture, gl_error %d",  glGetError());
}

bool Texture::Set2DData(const Image &img)
{
    if (IsInvalid())
    {
        GLOG->Error("Texture is in invalid state, textureType=%d, textureUnit=%d, textureHandle=%d", m_textureType, m_textureUnit, m_texture);
        return false;
    }
    else if (m_textureType != TextureType::Texture2D)
    {
        GLOG->Error("Invalid texture type, got %d but expected", m_textureType, TextureType::Texture2D);
        return false;
    }
    else if (img.IsInvalid())
    {
        GLOG->Error("Provided image is invalid");
        return false;
    }

    OldTextureGuard guard(GetTextureType());
    Bind();

    //By default opengl reads by four bytes
    glPixelStorei(GL_UNPACK_ALIGNMENT, DetermineBestPackingAlignmentSize(img));
    
    //format = how interpret data
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(ImageChannelsToGL(img)), static_cast<GLsizei>(img.GetSize().width), static_cast<GLsizei>(img.GetSize().height), 0, ImageChannelsToGL(img), GL_UNSIGNED_BYTE, img.GetData().data());
    glGenerateMipmap(GL_TEXTURE_2D);
    return true;
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
        m_window->SetAsCurrentContext();
        glDeleteTextures(1, &m_texture);
    }
}

//context unsafe
Texture::Texture(Texture &&tex)
{
    m_texture = std::exchange(tex.m_texture, 0);
}

Texture &Texture::operator=(Texture &&tex)
{
    std::swap(m_texture, tex.m_texture);
    return *this;
}

bool Texture::IsInvalid() const
{
    return m_texture == 0 || m_textureType == TextureType::InvalidTexture || m_textureUnit == TextureUnit::InvalidTextureUnit;
}

ARX_NAMESPACE_END
