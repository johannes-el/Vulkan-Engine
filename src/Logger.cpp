#include <iostream>
#include <string>
#include <mutex>
#include <chrono>
#include <fstream>
#include <sstream>

#include "Logger.hpp"

namespace Logger
{
	LogLevel defaultLogLevel = LogLevel::Info;
	LogLevel logLevel = defaultLogLevel;

	static std::mutex logMutex;

	std::string currentTime()
	{
		auto now = std::chrono::system_clock::now();
		auto time = std::chrono::system_clock::to_time_t(now);
		std::stringstream ss;
		ss << std::ctime(&time);
		std::string timeStr = ss.str();
		timeStr.pop_back();
		return timeStr;
	}

	void logToConsole(const std::string &message)
	{
		std::cout << message << '\n';
	}

	void logToFile(const std::string& message, const std::string& filename)
	{
		std::ofstream file(filename, std::ios_base::app);
		if (file.is_open())
		{
			file << message << std::endl;
		}
	}

	void log(LogLevel level, const std::string& message)
	{
		if (level >= logLevel)
		{
			std::lock_guard<std::mutex> lock(logMutex);

			std::string logMessage = "[" + currentTime() + "] ";

			switch(level)
			{
			case LogLevel::Trace: logMessage += "[Trace] "; break;
			case LogLevel::Debug: logMessage += "[Debug] "; break;
			case LogLevel::Info: logMessage += "[Info] "; break;
			case LogLevel::Warn: logMessage += "[Warn] "; break;
			case LogLevel::Error: logMessage += "[Error] "; break;
			case LogLevel::Fatal: logMessage += "[Fatal] "; break;
			}

			logMessage += message;

			logToConsole(logMessage);
		}
	}

	void Trace(const std::string& message) { log(LogLevel::Trace, message); }
	void Debug(const std::string& message) { log(LogLevel::Debug, message); }
	void Info(const std::string& message) { log(LogLevel::Info, message); }
	void Warn(const std::string& message) { log(LogLevel::Warn, message); }
	void Error(const std::string& message) { log(LogLevel::Error, message); }
	void Fatal(const std::string& message) { log(LogLevel::Fatal, message); }

	void SetLogLevel(LogLevel level)
	{
		logLevel = level;
	}
} // namespace Logger
