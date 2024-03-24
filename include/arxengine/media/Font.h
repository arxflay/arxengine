/*
 * @author Alexej Fedorenko
 */

#ifndef ARX_FONT_H
#define ARX_FONT_H
#include "arxengine/ArxDefines.h"
#include <memory>
#include <string_view>
#include <vector>
#include "arxengine/media/Image.h"
#include "arxengine/ui/Size.h"
#include "arxengine/ui/Position.h"
#include <chrono>
struct FT_FaceRec_;
ARX_NAMESPACE_BEGIN

struct GlyphDimensions
{
    Position advance;
    Position bearings;
    SizeUL size;
};

struct TextExtent
{
    float maxHeight;
    float widthSum;
    float yMin;
    float yMax;
};

class ARX_EXPORTS Font final 
{
public:
    using ChangeTime_t = std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds>;
    Font();
    Font(Font&&);
    Font &operator=(Font&&);
    Font(const Font&);
    Font &operator=(const Font&);
    /*loading functions begin*/
    static Font LoadFromFile(std::string_view filename);
    static Font LoadFromBinary(const uint8_t *data, size_t len);
    bool SetSizeInPt(unsigned int heightInPt);
    bool SetSizeInPixels(unsigned int heightInPixels);
    bool Scale(float factor);
    double GetSizeInPt();
    unsigned int GetSizeInPixels();
    bool IsInvalid();
    GlyphDimensions GetGlyphDimensions(char ch);
    Image RenderGlyph(char ch);
    const ChangeTime_t &GetLastChangeTime();
    TextExtent GetTextExtent(std::string_view text);

private:
    void LoadGlyph(char ch);
    void UpdateLastChangeTime();
    unsigned int m_sizeInPixels;
    ChangeTime_t m_lastChangeTime;
    std::shared_ptr<std::vector<uint8_t>> m_fontData;
    std::unique_ptr<FT_FaceRec_, int(*)(FT_FaceRec_ *)> m_face;
};

ARX_NAMESPACE_END

#endif
