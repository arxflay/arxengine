#include "logging/Logger.h"
#include <memory>
#include <cstring>
#include <memory>
#include <iostream>
ARX_NAMESPACE_BEGIN

std::unique_ptr<Logger> g_Logger = std::make_unique<Logger>();

Logger::Logger()
    : m_callbacks({})
{

}

ENUM_FROM_TO_STRING_DEFINE_NESTED(Logger, LoggingLevel, "Error", "Warn", "Info", "Debug");

/*static*/ const Logger *Logger::GetGlobalLogger()
{
    return g_Logger.get();
}

/*static*/ void Logger::SetGlobalLogger(std::unique_ptr<Logger> &&logger)
{
    if (!logger.get()) //set empty logger if no logger is set
        g_Logger = std::make_unique<Logger>();
    else
    {
        g_Logger = std::move(logger);
        g_Logger->Info("Setting logger as a global logger");
    }
}

void Logger::SetSingleCallbackForAll(LoggingCallback callback)
{
    Logger::LoggingCallback *callbacks = reinterpret_cast<Logger::LoggingCallback*>(&m_callbacks);
    std::fill(callbacks, callbacks + (sizeof(Logger::LoggingCallbacks) / sizeof(Logger::LoggingCallback)), callback); 
}

void Logger::SetCallbacks(const Logger::LoggingCallbacks &callbacks) { m_callbacks = callbacks; }

const Logger::LoggingCallbacks &Logger::GetCallbacks() const { return m_callbacks; }

Logger::Logger(Logger&& logger)
    : m_callbacks(std::move(logger.m_callbacks))
{

}

Logger &Logger::operator=(Logger&& logger)
{
    using std::swap;
    swap(m_callbacks, logger.m_callbacks);
    return *this;
}

ARX_NAMESPACE_END
