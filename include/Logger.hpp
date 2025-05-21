#pragma once

#include <string>

namespace Logger
{
   enum class LogLevel { Trace, Debug, Info, Warn, Error, Fatal };

   extern LogLevel defaultLogLevel;
   extern LogLevel logLevel;

   std::string currentTime();

   void SetLogLevel(LogLevel level);

   void Trace(const std::string& message);
   void Debug(const std::string& message);
   void Info(const std::string& message);
   void Warn(const std::string& message);
   void Error(const std::string& message);
   void Fatal(const std::string& message);

   void logToConsole(const std::string& message);
   void logToFile(const std::string& message, const std::string& filename = "log.txt");

   void log(LogLevel level, const std::string& message);

} // namespace Logger
