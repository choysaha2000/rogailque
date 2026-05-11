#pragma once

#include <iostream>
#include "ILogSink.h"


namespace XYZengine
{
	class ConsoleLogSink : public ILogSink
	{
	public:
		void Write(LogSeverity severity, const std::string& message) override
		{
			std::cout << "[" << SeverityToString(severity) << "] " << message << std::endl;

		} 

	private:
		const char* SeverityToString(LogSeverity severity)
		{
			switch (severity)
			{
			case LogSeverity::Debug: return "DEBUG";
			case LogSeverity::Info:    return "INFO";
			case LogSeverity::Warning: return "WARNING";
			case LogSeverity::Error:   return "ERROR";
			case LogSeverity::Fatal:   return "FATAL";
			default:                   return "UNKNOWN";
			}
		}
	};
}