#pragma once
#include "ArxDefines.h"
#include <string>
#include "ArxException.h"

ARX_NAMESPACE_BEGIN

class UniversalExceptionHandler
{
public:
    static ArxException::ErrorCode HandleException();
};

ARX_NAMESPACE_END
