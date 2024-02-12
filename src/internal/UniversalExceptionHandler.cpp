#include "internal/UniversalExceptionHandler.h"
#include "logging/Logger.h"

ARX_NAMESPACE_BEGIN

/*static*/ ArxException::ErrorCode UniversalExceptionHandler::HandleException()
{
    ArxException::ErrorCode code = ArxException::ErrorCode::NoError;
    try
    {
        throw;
    }
    catch(const ArxException &e)
    {
        GLOG->Error("ArxException exception, %s", e.ToString().c_str());
        code = e.GetErrorCode();
    }
    catch(const std::exception &e)
    {
        GLOG->Error("std exception, %s", e.what());
        code = ArxException::ErrorCode::CaughtStdException;
    }
    catch(...)
    {
        GLOG->Error("unknown error");
        code = ArxException::ErrorCode::Unknown;
    }

    return code;
}

ARX_NAMESPACE_END
