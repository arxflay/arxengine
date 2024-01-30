#include "Font.h"
#include <memory>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <Utils.h>
#include <ArxException.h>
ARX_NAMESPACE_BEGIN

class FontLibrary
{
public:
    FontLibrary()
        : m_library(nullptr, FT_Done_FreeType)
    {
        FT_LibraryRec_ *library = nullptr;
        FT_Error err = FT_Init_FreeType(&library);
        if (err != FT_Err_Ok)
            throw ArxException(ArxException::ErrorCode::FailedToInitializeFreetype, Utils::Format("Failed to initialize freetype library, ft_error='%d'", FT_Error_String(err)));
        
        m_library.reset(library);
    }
private:
    std::unique_ptr<FT_LibraryRec_, FT_Error(*)(FT_LibraryRec_ *)> m_library;    
};

/*user is reponsible for cleanup*/
FontLibrary *CreateFontLibrary()
{
    return std::make_unique<FontLibrary>().release();
}

void DestroyFontLibrary(FontLibrary *library)
{
    delete library;
}

ARX_NAMESPACE_END
