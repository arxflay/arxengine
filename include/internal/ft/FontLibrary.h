#ifndef ARX_FONTLIBRARY_H
#define ARX_FONTLIBRARY_H
#include "ArxDefines.h"
#include <memory>
#include <string_view>
#include <vector>

struct FT_LibraryRec_;
struct FT_FaceRec_;

ARX_NAMESPACE_BEGIN

class FontLibrary
{
public:
    FontLibrary();
    std::unique_ptr<FT_FaceRec_, int(*)(FT_FaceRec_*)> NewFace(std::string_view filename, long faceIndex);
    std::unique_ptr<FT_FaceRec_, int(*)(FT_FaceRec_*)> NewFaceFromBinary(const uint8_t *data, size_t len, long faceIndex);

    bool IsInvalid() const;
private:
    std::unique_ptr<FT_LibraryRec_, int(*)(FT_LibraryRec_ *)> m_library; 
};

ARX_NAMESPACE_END

#endif
