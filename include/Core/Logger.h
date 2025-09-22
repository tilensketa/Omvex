#pragma once

#include <deque>
#include <fstream>

enum class LogLevel { INFO = 0, WARN, ERR, DEBUG, SUCCESS, FATAL };
class Logger {
public:
  static Logger &get();

  static void Info(const std::string &msg) { get().log(msg, LogLevel::INFO); }
  static void Warn(const std::string &msg) { get().log(msg, LogLevel::WARN); }
  static void Error(const std::string &msg) { get().log(msg, LogLevel::ERR); }
  static void Debug(const std::string &msg) { get().log(msg, LogLevel::DEBUG); }
  static void Success(const std::string &msg) {
    get().log(msg, LogLevel::SUCCESS);
  }
  static void Fatal(const std::string &msg) { get().log(msg, LogLevel::FATAL); }

  static void ShowLogs() { get().showLogsInternal(); }

private:
  Logger();

  // Prevent copying and assignment (optional but recommended)
  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;

  void log(const std::string &message, LogLevel logLevel);
  void showLogsInternal();

  const std::string
  getTimestamp(const std::string &format = "%Y-%m-%d %H:%M:%S");

  std::pair<std::string, std::string> logLevel2ColorType(LogLevel logLevel);

private:
  static constexpr size_t MAX_LOGS = 50;
  std::deque<std::pair<LogLevel, std::string>> mLogs;
  std::ofstream mLogFile;
};
