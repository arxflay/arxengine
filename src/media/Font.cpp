#include "arxengine/media/Font.h"
#include <memory>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "arxengine/internal/ft/FontLoader.h"
#include "arxengine/GameApp.h"
#include "arxengine/misc/Convertors.h"
#include <iostream>
#include "arxengine/logging/Logger.h"
#include "arxengine/misc/Math.h"

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
    swap(m_fontData, font.m_fontData);
}

Font &Font::operator=(Font &&font)
{
    using std::swap;
    swap(m_sizeInPixels, font.m_sizeInPixels);
    swap(m_face, font.m_face);
    swap(m_fontData, font.m_fontData);
    font.UpdateLastChangeTime();
    UpdateLastChangeTime();

    return *this;
}

/*static*/ Font Font::LoadFromFile(std::string_view filename)
{
    Font font;
    auto fontData = GlobalGameApp->GetFontLoader().NewFace(filename, 0);
    font.m_face = std::move(fontData.face);
    font.m_fontData = std::move(fontData.fontBinaryData);
    FT_Select_Charmap(font.m_face.get(), ft_encoding_unicode);
    font.SetSizeInPt(10);
    return font;
}

/*static*/ Font Font::LoadFromBinary(const uint8_t *binaryData, size_t len)
{
    Font font;
    auto fontData = GlobalGameApp->GetFontLoader().NewFaceFromBinary(binaryData, len, 0);
    font.m_face = std::move(fontData.face);
    font.m_fontData = std::move(fontData.fontBinaryData);
    FT_Select_Charmap(font.m_face.get(), ft_encoding_unicode);
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
    dimensions.size = SizeUL(m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows);
    return dimensions;
}

std::vector<uint8_t> GetMonochromeImage(unsigned char *buffer, const SizeUL &bitmapSizeInBits, int pitch)
{
    //pitch = number of bytes to next row in bitmap
    std::vector<uint8_t> out;
    out.resize(bitmapSizeInBits.height * bitmapSizeInBits.width);

    for (unsigned long row = 0; row < bitmapSizeInBits.height; ++row) 
        for(unsigned long column = 0; column < bitmapSizeInBits.width; ++column)
            out[row * bitmapSizeInBits.width + column] = 0xFF * !!GET_BIT(buffer[(row * pitch + (column >> 3))], PowerOfTwoModulo(column, 3));

    return out;
}

Image Font::RenderGlyph(char ch, bool renderWithAntialising)
{
    if (!m_face.get())
        return Image{};

    LoadGlyph(ch);

    FT_Error status = FT_Render_Glyph(m_face->glyph, renderWithAntialising ? FT_RENDER_MODE_NORMAL : FT_RENDER_MODE_MONO);
    
    if (status != FT_Err_Ok)
    {
        GLOG->Debug("Font - failed to render glyph");
        return Image{};
    }

    FT_Bitmap &bitmap = m_face->glyph->bitmap;
    SizeUL size(bitmap.width, bitmap.rows);

    if (!renderWithAntialising)
    {
        std::vector<uint8_t> monochromeBuffer = GetMonochromeImage(bitmap.buffer, size, bitmap.pitch);
        return Image::LoadFromData(size, 1, monochromeBuffer.data());
    }
    return Image::LoadFromData(size, 1, bitmap.buffer);
}

void Font::UpdateLastChangeTime()
{
    m_lastChangeTime = std::chrono::steady_clock::now();
}

const Font::ChangeTime_t &Font::GetLastChangeTime()
{
    return m_lastChangeTime; 
}

Font::Font(const Font &f)
    : Font()
{
    m_fontData = f.m_fontData;
    m_face = GameApp::GetGlobalApp()->GetFontLoader().NewFaceFromExistingDataBinary(m_fontData->data(), m_fontData->size(), 0);
    SetSizeInPixels(f.m_sizeInPixels);
}

Font &Font::operator=(const Font &f)
{
    m_fontData = f.m_fontData;
    m_face = GameApp::GetGlobalApp()->GetFontLoader().NewFaceFromExistingDataBinary(m_fontData->data(), m_fontData->size(), 0);
    SetSizeInPixels(f.m_sizeInPixels);
    UpdateLastChangeTime();
    
    return *this;
}

TextExtent Font::GetTextExtent(std::string_view text) 
{
    TextExtent textExtent{};
    for (size_t i = 0; i < text.size(); i++)
    {
        GlyphDimensions dimensions = GetGlyphDimensions(text[i]);
        textExtent.maxHeight = (std::max)(static_cast<float>(dimensions.size.height), textExtent.maxHeight);
        float yMin = static_cast<float>(dimensions.size.height) - dimensions.bearings.y;
        textExtent.yMin = (std::max)(textExtent.yMin, yMin);
        textExtent.yMax = (std::max)(static_cast<float>(dimensions.size.height) - yMin, textExtent.yMax); 
        textExtent.widthSum += dimensions.advance.x + static_cast<float>(dimensions.bearings.x);
    }
    return textExtent;
}


ARX_NAMESPACE_END
