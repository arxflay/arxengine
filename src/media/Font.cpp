#include "media/Font.h"
#include <memory>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <ArxException.h>
#include "internal/ft/FontLibrary.h"
#include "GameApp.h"
#include "misc/Convertors.h"
#include <iostream>
ARX_NAMESPACE_BEGIN

Font::Font()
    : m_sizeInPixels(10)
    , m_lastChangeTime(std::chrono::steady_clock::now())
    , m_face(nullptr, FT_Done_Face)
{

}

Font::Font(Font &&font)
    : Font()
{
    using std::swap;
    swap(m_sizeInPixels, font.m_sizeInPixels);
    swap(m_face, font.m_face);
}

Font &Font::operator=(Font &&font)
{
    using std::swap;
    swap(m_sizeInPixels, font.m_sizeInPixels);
    swap(m_face, font.m_face);
    font.UpdateLastChangeTime();
    UpdateLastChangeTime();

    return *this;
}

/*static*/ Font Font::LoadFromFile(std::string_view filename)
{
    Font font;
    font.m_face = GlobalGameApp->GetFontLibrary().NewFace(filename, 0);
    font.SetSizeInPt(10);
    return font;
}

/*static*/ Font Font::LoadFromBinary(const uint8_t *binaryData, size_t len)
{
    Font font;
    font.m_face = GlobalGameApp->GetFontLibrary().NewFaceFromBinary(binaryData, len, 0);
    font.SetSizeInPt(10);
    return font;
}

bool Font::SetSizeInPt(unsigned int heightInPt)
{
    return SetSizeInPixels(static_cast<unsigned int>(Convertors::PtToPx(heightInPt)));
}

bool Font::SetSizeInPixels(unsigned int heightInPixels)
{
    if (!m_face.get())
        return false;

    int status = FT_Set_Pixel_Sizes(m_face.get(), 0, heightInPixels);
    if(status != FT_Err_Ok)
        return false;

    UpdateLastChangeTime();

    m_sizeInPixels = heightInPixels;
    return true;
}

bool Font::Scale(float factor)
{
    unsigned int newSize = static_cast<unsigned int>(static_cast<float>(m_sizeInPixels) * factor);
    return SetSizeInPixels(newSize);
}

double Font::GetSizeInPt()
{
    return Convertors::PxToPt(m_sizeInPixels);
}

unsigned int Font::GetSizeInPixels()
{
    return m_sizeInPixels;
}

bool Font::IsInvalid()
{
    return !m_face.get();
}

void Font::LoadGlyph(char ch)
{
    unsigned int glyphIndex = FT_Get_Char_Index(m_face.get(), static_cast<FT_ULong>(ch));
    if (m_face->glyph->glyph_index == glyphIndex)
        return;

    FT_Load_Glyph(m_face.get(), glyphIndex, FT_LOAD_DEFAULT);
}

GlyphDimensions Font::GetGlyphDimensions(char ch)
{
    if (!m_face.get())
        return GlyphDimensions{};

    LoadGlyph(ch);
    GlyphDimensions dimensions;
    dimensions.advance.x = static_cast<float>(m_face->glyph->metrics.horiAdvance) / 64;
    dimensions.advance.y = static_cast<float>(m_face->glyph->metrics.vertAdvance) / 64;
    dimensions.bearings.x = static_cast<float>(m_face->glyph->metrics.horiBearingX) / 64;
    dimensions.bearings.y = static_cast<float>(m_face->glyph->metrics.horiBearingY) / 64;
    dimensions.size = Size(m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows);
    return dimensions;
}

Image Font::RenderGlyph(char ch)
{
    if (!m_face.get())
        return Image{};

    LoadGlyph(ch);

    if (m_face->glyph->bitmap.buffer == nullptr)
        FT_Render_Glyph(m_face->glyph, FT_RENDER_MODE_NORMAL);

    Size size(m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows);
    return Image::LoadFromData(size, 1, m_face->glyph->bitmap.buffer);
}

void Font::UpdateLastChangeTime()
{
    m_lastChangeTime = std::chrono::steady_clock::now();
}

const Font::ChangeTime_t &Font::GetLastChangeTime()
{
    return m_lastChangeTime; 
}


ARX_NAMESPACE_END
