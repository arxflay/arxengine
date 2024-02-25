#ifndef ARX_FONT_H
#define ARX_FONT_H
#include "ArxDefines.h"

ARX_NAMESPACE_BEGIN

class FontLibrary;

/*user is reponsible for cleanup*/
FontLibrary *CreateFontLibrary();
void DestroyFontLibrary(FontLibrary *library);

ARX_NAMESPACE_END

#endif
