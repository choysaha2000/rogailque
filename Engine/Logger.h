#pragma once

#include <vector>
#include <memory>
#include <string>
#include "ILogSink.h"


namespace XYZengine
{
	class Logger
	{
	public:
		static Logger& Instance()
		{
			static Logger instance;
			return instance;
		}


		Logger(const Logger&) = delete;
		Logger& operator = (const Logger&) = delete;

		void AddSink(std::unique_ptr<ILogSink> sink)
		{
			sinks.push_back(std::move(sink));
		}

		void Log(LogSeverity severity, const std::string& message)
		{
			for (auto& sink : sinks)
			{
				sink->Write(severity, message);
			}
		}


		void Debug(const std::string& message) { Log(LogSeverity::Debug, message); }
		void Info(const std::string& message) { Log(LogSeverity::Info, message); }
		void Warning(const std::string& message) { Log(LogSeverity::Warning, message); }
		void Error(const std::string& message) { Log(LogSeverity::Error, message); }
		void Fatal(const std::string& message) { Log(LogSeverity::Fatal, message); }


	private:
		Logger() = default;
	private:
		std::vector<std::unique_ptr<ILogSink>> sinks;
	};
}