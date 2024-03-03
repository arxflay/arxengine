#include "ui/FontCache.h"
#include "ArxException.h"
#include "ui/UIObject.h"
#include <iostream>
ARX_NAMESPACE_BEGIN



const Texture2D *FontCache::FontCacheEntry::GetTexture() const
{
    return m_texture;
}

const GlyphDimensions &FontCache::FontCacheEntry::GetGlyphDimensions() const
{
    return m_dimensions;
}

FontCache::FontCacheEntry::FontCacheEntry(UIObject *obj)
    : m_texture(new Texture2D(obj))
{
}

void FontCache::FontCacheEntry::UpdateTexture(const Image &img)
{
    m_texture->SetData(img);
}

void FontCache::FontCacheEntry::UpdateDimensions(const GlyphDimensions &dimensions)
{
    m_dimensions = dimensions;
}

FontCache::FontCacheEntry::FontCacheEntry(FontCacheEntry &&cacheEntry)
    : m_texture(nullptr)
{
    using std::swap;
    swap(m_texture, cacheEntry.m_texture);
    swap(m_dimensions, cacheEntry.m_dimensions);
}

FontCache::FontCacheEntry &FontCache::FontCacheEntry::operator=(FontCacheEntry &&cacheEntry)
{
    using std::swap;
    swap(m_texture, cacheEntry.m_texture);
    swap(m_dimensions, cacheEntry.m_dimensions);
    return *this;
}

FontCache::FontCache(UIObject *parent)
    : ArxObject(nullptr)
    , m_oldChangeTime(parent->GetFont().GetLastChangeTime())
{
    if (parent == nullptr)
        throw ArxException(ArxException::ErrorCode::GenericError, "FontCache parent is nullptr when it's expected to be not null");

    Reparent(parent);
}
//creates cache entry if character doesnt exists
std::optional<std::reference_wrapper<const FontCache::FontCacheEntry>> FontCache::GetCacheEntry(char ch)
{
    UIObject *parent = dynamic_cast<UIObject*>(GetParent());
    Font &font = parent->GetFont();
    if (font.IsInvalid())
        return std::nullopt;

    if (m_oldChangeTime != font.GetLastChangeTime())
        UpdateCacheEntries();

    auto it = m_cache.find(ch);
    if (it == m_cache.end())
    {
        FontCache::FontCacheEntry entry(parent);
        entry.UpdateDimensions(font.GetGlyphDimensions(ch));
        entry.UpdateTexture(font.RenderGlyph(ch));
        it = m_cache.emplace(ch, std::move(entry)).first;
    }

    return std::cref(it->second);
}

void FontCache::UpdateCacheEntries()
{
    Font &font = static_cast<UIObject*>(GetParent())->GetFont();
    m_oldChangeTime = font.GetLastChangeTime();
    for(auto &[ch, entry] : m_cache)
    {
        entry.UpdateDimensions(font.GetGlyphDimensions(ch));
        entry.UpdateTexture(font.RenderGlyph(ch));
    }
}

ARX_NAMESPACE_END

