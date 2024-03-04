#ifndef ARX_FONTCACHE_H
#define ARX_FONTCACHE_H
#include "ArxDefines.h"
#include "ArxObject.h"
#include <map>
#include "media/Font.h"
#include "gl/Texture2D.h"
ARX_NAMESPACE_BEGIN

class UIObject;


class FontCache final : public ArxObject
{
public:
    class FontCacheEntry final
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
        FontCacheEntry(UIObject *obj);
    

        FontCacheEntry(const FontCacheEntry&) = delete;
        FontCacheEntry &operator=(FontCacheEntry&&);
        FontCacheEntry &operator=(const FontCacheEntry&) = delete;
        GlyphDimensions m_dimensions;
        Texture2D *m_texture;
    };

    FontCache(UIObject *parent);
    //creates cache entry if character doesnt exists
    const FontCache::FontCacheEntry &GetCacheEntry(char ch);
    void EnableFontSmoothing(bool enable);
    bool IsFontSmoothingEnabled() const;
private:
    void UpdateCacheEntries();
    FontCache(FontCache&&) = delete;
    FontCache(const FontCache&) = delete;
    FontCache &operator=(FontCache&&) = delete;
    FontCache &operator=(const FontCache&);
    Font::ChangeTime_t m_oldChangeTime;
    Texture::TextureFilteringMode m_textureFiltering;
    std::map<char, FontCache::FontCacheEntry> m_cache; 
};

ARX_NAMESPACE_END
#endif

