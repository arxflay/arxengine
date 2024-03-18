#ifndef ARX_UNIVERSALEXCEPTIONHANDLER_H
#define ARX_UNIVERSALEXCEPTIONHANDLER_H
#include "arxengine/ArxDefines.h"
#include <string>
#include "arxengine/ArxException.h"

ARX_NAMESPACE_BEGIN

class UniversalExceptionHandler
{
public:
    static ArxException::ErrorCode HandleException();
};

ARX_NAMESPACE_END

#endif
