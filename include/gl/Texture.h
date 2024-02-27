#ifndef ARX_TEXTURE_H
#define ARX_TEXTURE_H
#include "ArxDefines.h"
#include <type_traits>
#include <iterator>
#include <vector>
#include "ArxObject.h"

ARX_NAMESPACE_BEGIN
class Image;
class UIObject;
class ArxWindow;


//FIXME error when texture is deleted after window is closed
class Texture : public ArxObject
{
public:
    //https://www.khronos.org/opengl/wiki/Texture
    enum class TextureType
    {
        InvalidTexture = 0,
        Texture2D = 0x0DE1 
    };
    
    //https://www.khronos.org/opengl/wiki/Shader#Resource_limitations
    enum class TextureUnit
    {
        InvalidTextureUnit = 0,
        Tex0 = 0x84C0,
        Tex1 = 0x84C1,
        Tex2 = 0x84C2,
        Tex3 = 0x84C3,
        Tex4 = 0x84C4,
        Tex5 = 0x84C5,
        Tex6 = 0x84C6,
        Tex7 = 0x84C7,
        Tex8 = 0x84C8,
        Tex9 = 0x84C9,
        Tex10 = 0x84CA,
        Tex11 = 0x84CB,
        Tex12 = 0x84CC,
        Tex13 = 0x84CD,
        Tex14 = 0x84CE,
        Tex15 = 0x84CF,
        Tex16 = 0x84D0,
        Tex17 = 0x84D1,
        Tex18 = 0x84D2,
        Tex19 = 0x84D3,
        Tex20 = 0x84D4,
        Tex21 = 0x84D5,
        Tex22 = 0x84D6,
        Tex23 = 0x84D7,
        Tex24 = 0x84D8,
        Tex25 = 0x84D9,
        Tex26 = 0x84DA,
        Tex27 = 0x84DB,
        Tex28 = 0x84DC,
        Tex29 = 0x84DD,
        Tex30 = 0x84DE,
        Tex31 = 0x84DF
    };

    virtual void Bind() const;
    virtual void Unbind() const;
    void SetTextureType(TextureType textureType);
    virtual void SetTextureUnit(TextureUnit textureUnit);
    bool Set2DData(const Image &image);

    TextureType GetTextureType() const;
    TextureUnit GetTextureUnit() const;

    Texture(UIObject *obj);

    //cleanup
    virtual ~Texture();

    //context unsafe
    Texture(Texture&&);
    Texture &operator=(Texture&&);

    virtual bool IsInvalid() const;
    bool IsBound() const;
private:
    ArxWindow *m_window;
    TextureType m_textureType;
    TextureUnit m_textureUnit;
    unsigned int m_texture;
};
ARX_NAMESPACE_END
#endif
