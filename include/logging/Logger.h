#ifndef ARX_LOGGER_H
#define ARX_LOGGER_H
#include "ArxDefines.h"
#include <string_view>
#include "misc/Utils.h"
#include <memory>
#include "misc/EnumHelpers.h"
ARX_NAMESPACE_BEGIN
#define GLOG Logger::GetGlobalLogger()

class ARX_EXPORTS Logger
{
public:
    Logger();
    
    enum class LoggingLevel
    {
        Error = 0,
        Warn = 1,
        Info = 2,
        Debug = 3
    };

    static ENUM_TO_STRING_DECLARE(LoggingLevel);
    static ENUM_FROM_STRING_DECLARE(LoggingLevel);

    using LoggingCallback = void(*)(LoggingLevel level, const char *data, const Logger *loggerInstance);
    struct LoggingCallbacks
    {
        LoggingCallback errorFn;
        LoggingCallback warnFn;
        LoggingCallback infoFn;
        LoggingCallback debugFn;
    };

    static const Logger *GetGlobalLogger();
    //null values are not allowed
    static void SetGlobalLogger(std::unique_ptr<Logger> &&logger);
    
    constexpr Logger::LoggingCallback GetCallbackFromLevel(const Logger::LoggingCallbacks &callbacks, Logger::LoggingLevel level) const
    {
        switch(level)
        {
            case Logger::LoggingLevel::Error:
                return callbacks.errorFn;
            case Logger::LoggingLevel::Warn:
                return callbacks.warnFn;
            case Logger::LoggingLevel::Info:
                return callbacks.infoFn;
            case Logger::LoggingLevel::Debug:
                return callbacks.debugFn;
        }

        return nullptr;
    }

    template<typename ...Args>
    void Log(LoggingLevel level, const char *format, Args&&...args) const
    {
        auto callback = GetCallbackFromLevel(m_callbacks, level);
        if (!callback) //ignore
            return;
        std::string buffer = Utils::Format(format, std::forward<Args&&>(args)...);
        
        return callback(level, buffer.data(), this);
    }

    template<typename ...Args>
    void Error(const char *format, Args&&...args) const
    {
        Log(LoggingLevel::Error, format, std::forward<Args&&>(args)...);
    }

    template<typename ...Args>
    void Warn(const char *format, Args&&...args) const
    {
        Log(LoggingLevel::Warn, format, std::forward<Args&&>(args)...);
    }
    
    template<typename ...Args>
    void Info(const char *format, Args&&...args) const
    {
        Log(LoggingLevel::Info, format, std::forward<Args&&>(args)...);
    }

    template<typename ...Args>
    void Debug(const char *format, Args&&...args) const
    {
        Log(LoggingLevel::Debug, format, std::forward<Args&&>(args)...);
    }

    void SetSingleCallbackForAll(LoggingCallback callback);
    void SetCallbacks(const LoggingCallbacks &callbacks);
    const LoggingCallbacks &GetCallbacks() const;
    
    Logger(const Logger&) = delete;
    Logger &operator=(const Logger&) = delete;

    virtual ~Logger() = default;

private:
    Logger(Logger&&);
    Logger &operator=(Logger&&);
    LoggingCallbacks m_callbacks;
};

namespace
{
    class FnLogGuard final
    {
    public:
        FnLogGuard(std::string_view functionName)
            : m_functionName(functionName)
        {
            GLOG->Info("Entered function %s", functionName.data());
        }
        
        ~FnLogGuard()
        {
            GLOG->Info("Exited function %s", m_functionName.data());
        }

    private:
        std::string_view m_functionName;
    };
}

//strange name for collision avoidance
#define FUNC_LOG_ENTER FnLogGuard fn_14f1f341zvxt431af0a94(__FUNCTION__)

ARX_NAMESPACE_END

#endif
