#pragma once

#include <fstream>
#include <string>
#include "ILogSink.h"

                 
namespace XYZengine
{
    class FileLogSink : public ILogSink
    {
    public: 
        explicit FileLogSink(const std::string& filePath)
            : file(filePath, std::ios::out) // app
        {

        }

        void Write(LogSeverity severity, const std::string& message) override
        {
            if (!file.is_open())
                return;

            file << "[" << SeverityToString(severity) << "] " << message << std::endl;


        }


    private:
        const char* SeverityToString(LogSeverity severity)
        {
            switch (severity)
            {
            case LogSeverity::Debug:   return "DEBUG";
            case LogSeverity::Info:    return "INFO";
            case LogSeverity::Warning: return "WARNING";
            case LogSeverity::Error:   return "ERROR";
            case LogSeverity::Fatal:   return "FATAL";
            default:                   return "UNKNOWN";
            }
        }

    private:
        std::ofstream file;


    };
}