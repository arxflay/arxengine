/*
 * @author Alexej Fedorenko
 */

#ifndef ARX_IMAGECONTROL_H
#define ARX_IMAGECONTROL_H
#include "arxengine/ArxDefines.h"
#include "arxengine/ui/UIControl.h"

ARX_NAMESPACE_BEGIN

struct TileData
{
    int tileWidthCount;
    int tileHeightCount;
};

class ARX_EXPORTS ImageControl : public UIControl
{
public:
    ImageControl(UIControl *parent, const Image &image = Image{}, Size size = constants::DEFAULT_SIZE, Position pos = constants::DEFAULT_POSITION);
    bool Load(const Image &image);
    void SetFilteringMode(Texture::TextureFilteringMode filteringMode);
    void EnableTilingMode(std::optional<TileData> tileData);
    bool IsTilingModeEnabled() const;
    bool IsInvalid() const;
private:
    void OnDraw(DrawEvent &e) override;
    Texture2D *m_texture;
    TileData m_tileData;
    bool m_isValid;

};


ARX_NAMESPACE_END

#endif
