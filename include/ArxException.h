#pragma once
#include "ArxDefines.h"
#include <string_view>
#include <string>
#include <memory>

ARX_NAMESPACE_BEGIN

class ARX_EXPORTS ArxException
{
public:
    enum class ErrorCode : int
    {
        NoError                     = 0,
        GenericError                = 1,
        EnumToStringError           = 2,
        EnumFromStringError         = 3,
        FailedToInitializeGLFW      = 4,
        FailedToInitializeGlad      = 5,
        FailedToInitializeOpenAL    = 6,
        FailedToInitializeFreetype  = 7,
        GameAppAlreadyInitialized   = 8,
        GameAppNotSetAsGlobal       = 9,
        GameAppNotInitialized       = 10,
        CaughtStdException          = 11,
        FailedToConstructUIObject   = 12,
        FailedToConstructArxWindow  = 13,
        GameAppIsRunning            = 14,
        GameAppAlreadyScheduledExit = 15
    };

    ArxException();
    ArxException(ErrorCode errCode, std::string_view msg = "Unspecified");
    virtual ~ArxException() = default;

    virtual std::string ToString() const;
    virtual std::unique_ptr<ArxException> Clone() const;
    
    std::string_view GetMsg() const noexcept;
    ErrorCode GetErrorCode() const noexcept;
    int GetErrorCodeInt() const noexcept;

protected:
    ErrorCode m_errCode;
    std::string m_msg;
};

ARX_NAMESPACE_END
