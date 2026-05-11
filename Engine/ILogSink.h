#pragma once

#include <string>
#include "LogSeverity.h"


namespace XYZengine
{
	class ILogSink
	{
	public:
		virtual ~ILogSink() = default;
		virtual void Write(LogSeverity severity, const std::string& message) = 0;
	
	
	};
}