#ifndef ARX_FILELOGGER_H
#define ARX_FILELOGGER_H
#include "logging/Logger.h"
#include <string>

ARX_NAMESPACE_BEGIN

class FileLogger final : public Logger
{
public:
    //throws if file doesnt exist
    FileLogger(Logger::LoggingLevel maxLogLevel, std::string_view filename);
    std::string_view GetFilename() const;
    ~FileLogger();
private:
    static void WriteToFile(Logger::LoggingLevel level, const char *data, const Logger *loggerInstance);
    std::string m_filename;
};

ARX_NAMESPACE_END

#endif
