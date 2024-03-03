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
        FontCacheEntry(FontCacheEntry&&);
    private:
        FontCacheEntry(UIObject *obj);
        void UpdateTexture(const Image &img);
        void UpdateDimensions(const GlyphDimensions &dimensions);
        FontCacheEntry(const FontCacheEntry&) = delete;
        FontCacheEntry &operator=(FontCacheEntry&&);
        FontCacheEntry &operator=(const FontCacheEntry&) = delete;
        GlyphDimensions m_dimensions;
        Texture2D *m_texture;
    };

    FontCache(UIObject *parent);
    //creates cache entry if character doesnt exists
    
    //return nullopt if font is invalid
    std::optional<std::reference_wrapper<const FontCache::FontCacheEntry>> GetCacheEntry(char ch);
private:
    void UpdateCacheEntries();
    FontCache(FontCache&&) = delete;
    FontCache(const FontCache&) = delete;
    FontCache &operator=(FontCache&&) = delete;
    FontCache &operator=(const FontCache&);
    Font::ChangeTime_t m_oldChangeTime;
    std::map<char, FontCache::FontCacheEntry> m_cache; 
};

ARX_NAMESPACE_END
#endif

