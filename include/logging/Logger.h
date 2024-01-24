#pragma once
#include "ArxDefines.h"
#include <string_view>
#include <Utils.h>
#include <memory>
#include "EnumHelpers.h"
#include <iostream>
ARX_NAMESPACE_BEGIN
#define GLOG Logger::GetGlobalLogger()

class Logger
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


#define EMPTY_LOGGER Logger{}

ARX_NAMESPACE_END
