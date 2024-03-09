#ifndef ARX_IMAGECONTROL_H
#define ARX_IMAGECONTROL_H
#include "ArxDefines.h"
#include "ui/UIControl.h"

ARX_NAMESPACE_BEGIN

class ImageControl : public UIControl
{
public:
    ImageControl(UIControl *parent, const Image &image = Image{}, Size size = defaults::DEFAULT_SIZE, Position pos = defaults::DEFAULT_POSITION);
    bool Load(const Image &image);
    void SetFilteringMode(Texture::TextureFilteringMode filteringMode);
    void SetWrappingMode(Texture::TextureWrapping wrappingMode); 
    bool IsInvalid() const;
private:
    ImageControl *AllocClone() override { return nullptr; } /*unsupported*/
    void OnDraw(DrawEvent &e) override;
    Texture2D *m_texture;
    bool m_isValid;

};


ARX_NAMESPACE_END

#endif
