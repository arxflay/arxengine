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

class ARX_EXPORTS Texture2D final: public Texture 
{
public:
    virtual void SetTextureFilteringMode(TextureFilteringMode textureFiltering) override;
    virtual void SetTextureWrapping(TextureWrapping textureWrapping) override;
    virtual TextureFilteringMode GetTextureFilteringMode() const override;
    virtual TextureWrapping GetTextureWrapping() const override;

    Texture2D(UIControl *obj);
    bool SetData(const Image &image);
    
    //for now clone copies only parameters, not texture
    Texture2D *Clone() override;
    Texture2D *AllocClone() override;

private:
    TextureFilteringMode m_filteringMode;
    TextureWrapping m_wrappingMode;
};


ARX_NAMESPACE_END
#endif
