#include "logging/FileLogger.h"
#include <ctime>
#include <cstdio>
#include <memory>

ARX_NAMESPACE_BEGIN
constexpr size_t MAX_LOGGING_ATTEMPTS = 10;

FileLogger::FileLogger(LoggingLevel maxLogLevel, std::string_view filename)
    : m_filename(filename)
{
    LoggingCallbacks callbacks{};
    LoggingCallback *callbackArray = reinterpret_cast<Logger::LoggingCallback*>(&callbacks);
    for (size_t i = 0; i <= static_cast<size_t>(maxLogLevel); i++)
        callbackArray[i] = &FileLogger::WriteToFile;
    SetCallbacks(callbacks);
    Info("++BEGIN LOGGING");
}

std::string_view FileLogger::GetFilename() const { return m_filename; }

FileLogger::~FileLogger()
{
    Info("--END LOGGING");
}

/*static*/ void FileLogger::WriteToFile(Logger::LoggingLevel level, const char *data, const Logger *instance)
{
    const FileLogger *fileLogger = static_cast<const FileLogger*>(instance);
    time_t timeNow = time(NULL);
    char logLineBeggining[256]{};
    std::string timeString(asctime(gmtime(&timeNow)));
    if (timeString.empty())
        return;
    timeString.pop_back(); 
    int len = snprintf(logLineBeggining, 255, "[UTC: %s][%s]: ", timeString.data(), Logger::StringFromLoggingLevel(level));
    if (len <= 0) //ignore data
        return;
    for(size_t i = 0; i < MAX_LOGGING_ATTEMPTS; i++)
    {
        FILE *file = fopen(fileLogger->GetFilename().data(), "ab+"); //append and binary
        if(file)
        {
            fwrite(logLineBeggining, sizeof(char), static_cast<size_t>(len), file);
            fputs(data, file);
            fputc('\n', file);
            fclose(file);
            break;
        }
    }

}

ARX_NAMESPACE_END
