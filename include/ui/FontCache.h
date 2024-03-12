#ifndef ARX_FONTCACHE_H
#define ARX_FONTCACHE_H
#include "ArxDefines.h"
#include "UIObject.h"
#include <map>
#include "media/Font.h"
#include "gl/Texture2D.h"
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

        FontCacheEntry Clone();
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
    void ManuallyUpdateLastFontChangeTime(Font::ChangeTime_t time);
    bool IsFontSmoothingEnabled() const;
    FontCache *Clone() override;

private:
    FontCache *AllocClone() override;
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

