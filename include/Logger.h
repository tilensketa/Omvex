#pragma once

#include <deque>
#include <fstream>

enum class LogLevel { INFO = 0, WARN, ERR, DEBUG, SUCCESS, FATAL };
class Logger {
public:
  static Logger &getInstance();

  void Info(const std::string &msg) { log(msg, LogLevel::INFO); }
  void Warn(const std::string &msg) { log(msg, LogLevel::WARN); }
  void Error(const std::string &msg) { log(msg, LogLevel::ERR); }
  void Debug(const std::string &msg) { log(msg, LogLevel::DEBUG); }
  void Success(const std::string &msg) { log(msg, LogLevel::SUCCESS); }
  void Fatal(const std::string &msg) { log(msg, LogLevel::FATAL); }

  void ShowLogs();

private:
  Logger();

  // Prevent copying and assignment (optional but recommended)
  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;

  void log(const std::string &message, LogLevel logLevel);

  const std::string
  getTimestamp(const std::string &format = "%Y-%m-%d %H:%M:%S");

  std::pair<std::string, std::string> logLevel2ColorType(LogLevel logLevel);

private:
  static constexpr size_t MAX_LOGS = 50;
  std::deque<std::pair<LogLevel, std::string>> mLogs;
  std::ofstream mLogFile;
};
