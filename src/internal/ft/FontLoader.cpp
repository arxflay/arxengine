#include "internal/ft/FontLoader.h"
#include <ft2build.h>
#include "logging/Logger.h"
#include "misc/Utils.h"
#include FT_FREETYPE_H

ARX_NAMESPACE_BEGIN

FontLoader::FontLoader()
    : m_library(nullptr, FT_Done_FreeType)
{
    FT_Library lib = nullptr;
    FT_Init_FreeType(&lib);
    m_library.reset(lib);
}

FontLoader::FontData::FontData()
    : face(nullptr, FT_Done_Face)
{

}

FontLoader::FontData FontLoader::NewFace(std::string_view filename, long faceIndex)
{
    FUNC_LOG_ENTER;
    std::vector<uint8_t> data;
    auto status = Utils::LoadBinaryFile(filename, data);
    FontData fontData;
    if (status == Utils::LoadFileErrorCode::FailedToOpenFile)
        GLOG->Error("Failed to load file %s", filename.data());
    else
    {
        auto fontBinaryData = std::make_shared<std::vector<uint8_t>>(std::move(data));
        fontData = NewFaceFromBinaryInternal(fontBinaryData->data(), fontBinaryData->size(), faceIndex);
        if (fontData.face != nullptr)
            fontData.fontBinaryData = fontBinaryData;
    }
    return fontData;
}

FontLoader::FontData FontLoader::NewFaceFromBinary(const uint8_t *binaryData, size_t len, long faceIndex)
{
    FUNC_LOG_ENTER;
    auto fontBinaryData = std::make_shared<std::vector<uint8_t>>(binaryData, binaryData + len);
    FontData fontData = NewFaceFromBinaryInternal(fontBinaryData->data(), fontBinaryData->size(), faceIndex);
    if (fontData.face != nullptr)
        fontData.fontBinaryData = fontBinaryData;
    return fontData;
}

FontLoader::FontData FontLoader::NewFaceFromBinaryInternal(const uint8_t *data, size_t len, long faceIndex)
{
    FontData fontData;
    FT_Face newFace = nullptr;
    int status = FT_New_Memory_Face(m_library.get(), data, static_cast<FT_Long>(len), faceIndex, &newFace); 
    if (status != FT_Err_Ok)
        GLOG->Error("Failed to create face");

    fontData.face.reset(newFace);
    return fontData;
}

std::unique_ptr<FT_FaceRec_, int(*)(FT_FaceRec_*)> FontLoader::NewFaceFromExistingDataBinary(const uint8_t *data, size_t len, long faceIndex)
{
    FUNC_LOG_ENTER;
    return NewFaceFromBinaryInternal(data, len, faceIndex).face;
}

bool FontLoader::IsInvalid() const
{
    return !m_library.get();
}


ARX_NAMESPACE_END
