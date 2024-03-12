#ifndef ARX_TEXTURE_H
#define ARX_TEXTURE_H
#include "ArxDefines.h"
#include <type_traits>
#include <iterator>
#include <vector>
#include "ui/UIObject.h"

ARX_NAMESPACE_BEGIN
class Image;
class ArxWindow;

class ARX_EXPORTS Texture : public UIObject 
{
public:
    //https://www.khronos.org/opengl/wiki/Texture
    enum class TextureType
    {
        Texture2D = 0x0DE1,
        Default = Texture2D
    };
    
    //https://www.khronos.org/opengl/wiki/Shader#Resource_limitations
    enum class TextureUnit
    {
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
        Tex31 = 0x84DF,
        Default = Tex0
    };

    enum class TextureFilteringMode
    {
        Nearest = 0x2600,
        Linear = 0x2601,
        Default = Nearest 
    };

    enum class TextureWrapping
    {
        ClampToBorder = 0x812D,
        ClampToEdge = 0x812F,
        Repeat = 0x2901,
        MirrorRepeat = 0x8370,
        Default = Repeat
    };
    
    virtual void SetTextureFilteringMode(TextureFilteringMode textureFiltering) = 0;
    virtual void SetTextureWrapping(TextureWrapping textureWrapping) = 0;
    virtual TextureFilteringMode GetTextureFilteringMode() const = 0;
    virtual TextureWrapping GetTextureWrapping() const = 0;

    void Bind() const;
    void Unbind() const;
    void SetTextureUnit(TextureUnit textureUnit);

    TextureType GetTextureType() const;
    TextureUnit GetTextureUnit() const;
    
    Texture(UIControl *obj);

    //cleanup
    virtual ~Texture();

    Texture *Clone() override;

    bool IsInvalid() const;
    bool IsBound() const;
protected:
    class OldTextureGuard final
    {
    public:
        OldTextureGuard(Texture::TextureType textureType);
        ~OldTextureGuard();
    private:
        Texture::TextureType m_textureType;
        unsigned int m_oldTextureUnit;
        unsigned int m_texture;
    };

    static unsigned int ImageChannelsToGL(const Image &img);
    static int DetermineBestPackingAlignmentSize(const Image &img);

    unsigned int GetTextureHandle();
    void SetTextureType(TextureType textureType);
private:
    Texture *AllocClone() override = 0;

    unsigned int m_texture;
    TextureType m_textureType;
    TextureUnit m_textureUnit;
};
ARX_NAMESPACE_END
#endif
