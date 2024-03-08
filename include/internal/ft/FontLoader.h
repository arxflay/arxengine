#ifndef ARX_FONTLOADER_H
#define ARX_FONTLOADER_H
#include "ArxDefines.h"
#include <memory>
#include <string_view>
#include <vector>

struct FT_LibraryRec_;
struct FT_FaceRec_;

ARX_NAMESPACE_BEGIN

class FontLoader
{
public:
    struct FontData
    {
        FontData();
        std::unique_ptr<FT_FaceRec_, int(*)(FT_FaceRec_*)> face;
        std::shared_ptr<std::vector<uint8_t>> fontBinaryData;
    };
    FontLoader();
    FontData NewFace(std::string_view filename, long faceIndex);
    FontData NewFaceFromBinary(const uint8_t *data, size_t len, long faceIndex);
    std::unique_ptr<FT_FaceRec_, int(*)(FT_FaceRec_*)> NewFaceFromExistingDataBinary(const uint8_t *data, size_t len, long faceIndex);

    bool IsInvalid() const;
private:
    FontData NewFaceFromBinaryInternal(const uint8_t *data, size_t len, long faceIndex);
    std::unique_ptr<FT_LibraryRec_, int(*)(FT_LibraryRec_ *)> m_library; 
};

ARX_NAMESPACE_END

#endif
