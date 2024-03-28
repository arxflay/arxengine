/*
 * @author Alexej Fedorenko
 */

#ifndef ARX_FONTCACHE_H
#define ARX_FONTCACHE_H
#include "arxengine/ArxDefines.h"
#include "arxengine/ui/UIObject.h"
#include <map>
#include "arxengine/media/Font.h"
#include "arxengine/gl/Texture2D.h"
ARX_NAMESPACE_BEGIN

class ARX_EXPORTS FontCache final : public UIObject 
{
public:
    class ARX_EXPORTS FontCacheEntry final
    {
    friend class FontCache;
    public:
        const Texture2D *GetTexture() const;
        const GlyphDimensions &GetGlyphDimensions() const;
        void UpdateTextureFiltering(Texture::TextureFilteringMode filtering);
        void UpdateTexture(const Image &img);
        void UpdateDimensions(const GlyphDimensions &dimensions);
        FontCacheEntry(FontCacheEntry&&);

    private:
        FontCacheEntry(UIControl *obj);
        FontCacheEntry(Texture2D *texture, GlyphDimensions dimensions);
        FontCacheEntry(const FontCacheEntry&) = delete;
        FontCacheEntry &operator=(FontCacheEntry&&);
        FontCacheEntry &operator=(const FontCacheEntry&) = delete;
        GlyphDimensions m_dimensions;
        Texture2D *m_texture;
    };

    FontCache(UIControl *parent);
    //creates cache entry if character doesnt exists
    const FontCache::FontCacheEntry &GetCacheEntry(char ch);
    void EnableFontSmoothing(bool enable);
    void EnableAntialising(bool enable);
    bool IsAntialisingEnabled() const;
    bool IsFontSmoothingEnabled() const;
    const Font::ChangeTime_t &GetLastFontChangeTime() const;

private:
    void UpdateCacheEntries();
    void UpdateCacheEntry(char ch, FontCacheEntry &e);

    FontCache(FontCache&&) = delete;
    FontCache(const FontCache&) = delete;
    FontCache &operator=(FontCache&&) = delete;
    FontCache &operator=(const FontCache&);
    Font::ChangeTime_t m_oldChangeTime;
    Texture::TextureFilteringMode m_textureFiltering;
    bool m_antialisingEnabled;
    std::map<char, FontCache::FontCacheEntry> m_cache; 
};

ARX_NAMESPACE_END
#endif

