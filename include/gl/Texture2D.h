#ifndef ARX_TEXTURE2D_H
#define ARX_TEXTURE2D_H
#include "ArxDefines.h"
#include <type_traits>
#include <iterator>
#include <vector>
#include "Texture.h"
#include <memory>

ARX_NAMESPACE_BEGIN
class Image;

class Texture2D final: public Texture 
{
public:
    Texture2D(UIObject *obj);
    virtual void Bind() const override;
    virtual void Unbind() const override;
    void SetData(const Image &image);
    void SetTextureUnit(Texture::TextureUnit textureUnit) override;
    virtual bool IsInvalid() const override;
private:
};


ARX_NAMESPACE_END
#endif
