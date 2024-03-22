/*
 * @author Alexej Fedorenko
 */

#ifndef ARX_ARXEXCEPTION_H
#define ARX_ARXEXCEPTION_H
#include "arxengine/ArxDefines.h"
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
        FailedToInitializeOpenAL    = 5,
        FailedToInitializeFreetype  = 6,
        GameAppAlreadyInitialized   = 7,
        GameAppNotSetAsGlobal       = 8,
        GameAppNotInitialized       = 9,
        CaughtStdException          = 10,
        FailedToConstructUIControl  = 11,
        FailedToConstructArxWindow  = 12,
        GameAppIsRunning            = 13,
        GameAppAlreadyScheduledExit = 14,
        JsonValueTypeMismatch       = 15,
        DefaultJsonLexerFailure     = 16,
        DefaultJsonConvertorFailure = 17
        
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

#endif
