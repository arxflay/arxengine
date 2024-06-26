#include "arxengine/ui/FontCache.h"
#include "arxengine/ArxException.h"
#include "arxengine/ui/UIControl.h"
#include <iostream>
#include <ctype.h>
ARX_NAMESPACE_BEGIN



const Texture2D *FontCache::FontCacheEntry::GetTexture() const
{
    return m_texture;
}

const GlyphDimensions &FontCache::FontCacheEntry::GetGlyphDimensions() const
{
    return m_dimensions;
}

FontCache::FontCacheEntry::FontCacheEntry(UIControl *obj)
    : m_texture(new Texture2D(obj))
{
    m_texture->SetTextureWrapping(Texture::TextureWrapping::ClampToEdge);
}

void FontCache::FontCacheEntry::UpdateTexture(const Image &img)
{
    m_texture->SetData(img);
}

void FontCache::FontCacheEntry::UpdateDimensions(const GlyphDimensions &dimensions)
{
    m_dimensions = dimensions;
}

void FontCache::FontCacheEntry::UpdateTextureFiltering(Texture::TextureFilteringMode filtering)
{
    m_texture->SetTextureFilteringMode(filtering);
}

FontCache::FontCacheEntry::FontCacheEntry(Texture2D *texture, GlyphDimensions dimensions)
    : m_dimensions(dimensions)
    , m_texture(texture)
{
    m_texture->SetTextureWrapping(Texture::TextureWrapping::ClampToEdge);
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

FontCache::FontCache(UIControl *parent)
    : UIObject(parent)
    , m_oldChangeTime(parent->GetFont().GetLastChangeTime())
    , m_textureFiltering(Texture::TextureFilteringMode::Default)
    , m_antialisingEnabled(true)
{

}

//creates cache entry if character doesnt exists
const FontCache::FontCacheEntry &FontCache::GetCacheEntry(char ch)
{
    UIControl *parent = GetOwnerUIControl();
    Font &font = parent->GetFont();
    if (font.IsInvalid())
        throw ArxException(ArxException::ErrorCode::GenericError, "FontCacheEntry invalid font set");

    if (m_oldChangeTime != font.GetLastChangeTime())
    {
        UpdateCacheEntries();
        m_oldChangeTime = font.GetLastChangeTime();
    }
     
    auto it = m_cache.find(ch);
    if (it == m_cache.end())
    {
        FontCache::FontCacheEntry entry(parent);
        UpdateCacheEntry(ch, entry);
        it = m_cache.emplace(ch, std::move(entry)).first;
    }

    return it->second;
}

void FontCache::EnableFontSmoothing(bool enable)
{
    Texture::TextureFilteringMode filtering = enable ? Texture::TextureFilteringMode::Linear : Texture::TextureFilteringMode::Nearest;
    if (m_textureFiltering == filtering)
        return;
    m_textureFiltering = filtering;
    m_oldChangeTime = decltype(m_oldChangeTime)(std::chrono::seconds(-1));
}

bool FontCache::IsFontSmoothingEnabled() const
{
    return m_textureFiltering == Texture::TextureFilteringMode::Linear;
}

void FontCache::EnableAntialising(bool enable)
{
    if (m_antialisingEnabled == enable)
        return;

    m_antialisingEnabled = enable;
    m_oldChangeTime = decltype(m_oldChangeTime)(std::chrono::seconds(-1));
}

void FontCache::UpdateCacheEntry(char ch, FontCache::FontCacheEntry &e)
{
    Font &font = GetOwnerUIControl()->GetFont();
    if (ch >= '!' && ch <= '~')
    {
        e.UpdateTexture(font.RenderGlyph(ch, m_antialisingEnabled));
        e.UpdateTextureFiltering(m_textureFiltering);
    }

    e.UpdateDimensions(font.GetGlyphDimensions(ch));
}

void FontCache::UpdateCacheEntries()
{
    for(auto &[ch, entry] : m_cache)
        UpdateCacheEntry(ch, entry);
}

bool FontCache::IsAntialisingEnabled() const
{
    return m_antialisingEnabled;
}

const Font::ChangeTime_t &FontCache::GetLastFontChangeTime() const
{
    return m_oldChangeTime;
}

ARX_NAMESPACE_END

