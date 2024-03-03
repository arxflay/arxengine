#include "internal/ft/FontLibrary.h"
#include <ft2build.h>
#include "logging/Logger.h"
#include FT_FREETYPE_H

ARX_NAMESPACE_BEGIN

FontLibrary::FontLibrary()
    : m_library(nullptr, FT_Done_FreeType)
{
    FT_Library lib = nullptr;
    FT_Init_FreeType(&lib);
    m_library.reset(lib);
}

std::unique_ptr<FT_FaceRec_, int(*)(FT_FaceRec_*)> FontLibrary::NewFace(std::string_view filename, long faceIndex)
{
    FT_Face newFace = nullptr;
    int status = FT_New_Face(m_library.get(), filename.data(), faceIndex, &newFace);
    if (status != FT_Err_Ok)
        GLOG->Error("Failed to load font from path %s", filename.data());
    return std::unique_ptr<FT_FaceRec_, int(*)(FT_FaceRec_*)>(newFace, FT_Done_Face);
}

std::unique_ptr<FT_FaceRec_, int(*)(FT_FaceRec_*)> FontLibrary::NewFaceFromBinary(const uint8_t *binaryData, size_t len, long faceIndex)
{
    FT_Face newFace = nullptr;
    int status = FT_New_Memory_Face(m_library.get(), binaryData, static_cast<FT_Long>(len), faceIndex, &newFace); 
    if (status != FT_Err_Ok)
        GLOG->Error("Failed to load font from binary data");
    return std::unique_ptr<FT_FaceRec_, int(*)(FT_FaceRec_*)>(newFace, FT_Done_Face);
}

bool FontLibrary::IsInvalid() const
{
    return !m_library.get();
}


ARX_NAMESPACE_END
