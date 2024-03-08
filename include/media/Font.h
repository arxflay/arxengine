#ifndef ARX_FONT_H
#define ARX_FONT_H
#include "ArxDefines.h"
#include <memory>
#include <string_view>
#include <vector>
#include "media/Image.h"
#include "ui/Size.h"
#include "ui/Position.h"
#include <chrono>
class FT_FaceRec_;
ARX_NAMESPACE_BEGIN

struct GlyphDimensions
{
    Position advance;
    Position bearings;
    Size size;
};

class Font final 
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
