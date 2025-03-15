#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <vector>

// ANSI Color Codes
#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BLUE "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN "\033[36m"
#define COLOR_WHITE "\033[37m"

enum class LogLevel { INFO, WARN, ERROR, DEBUG, SUCCESS, FATAL };
class Logger {
public:
  // Public method to get the single instance of Logger
  static Logger &getInstance() {
    static Logger instance; // Static instance created once and reused
    return instance;
  }

  inline void Info(const std::string& message){
    log(message, LogLevel::INFO);
  }
  inline void Warn(const std::string& message){
    log(message, LogLevel::WARN);
  }
  inline void Error(const std::string& message){
    log(message, LogLevel::ERROR);
  }
  inline void Debug(const std::string& message){
    log(message, LogLevel::DEBUG);
  }
  inline void Success(const std::string& message){
    log(message, LogLevel::SUCCESS);
  }
  inline void Fatal(const std::string& message){
    log(message, LogLevel::FATAL);
  }


  const std::vector<std::pair<LogLevel, std::string>> &GetLogs() const {
    return mLogs;
  }

private:
  Logger() {}                    // Private constructor to prevent instantiation
  std::vector<std::string> logs; // Store log messages

  // Prevent copying and assignment (optional but recommended)
  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;

  inline void log(const std::string &message, LogLevel logLevel) {
    std::string timeStamp = "[" + getTimestamp() + "] ";
    auto info = logLevel2ColorType(logLevel);
    std::string logMessageColor = timeStamp + info.first + info.second + message + COLOR_RESET;
    std::cout << logMessageColor << std::endl;
    std::string logMessageNoColor = timeStamp + info.second + message;
    std::pair<LogLevel, std::string> log = std::make_pair(logLevel, logMessageNoColor);
    mLogs.push_back(log);
  }

  // Get the current timestamp as a string
  inline std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm *tm_now = std::localtime(&now_time);
    std::ostringstream timestamp;
    timestamp << std::put_time(tm_now, "%Y-%m-%d %H:%M:%S");
    return timestamp.str();
  }
  inline std::pair<std::string, std::string> logLevel2ColorType(LogLevel logLevel) {
    std::string color = "";
    std::string type = "";
    switch (logLevel) {
    case LogLevel::INFO:
      color = COLOR_WHITE;
      type = "[INFO]";
      break;
    case LogLevel::SUCCESS:
      color = COLOR_GREEN;
      type = "[SUCCESS]";
      break;
    case LogLevel::WARN:
      color = COLOR_YELLOW;
      type = "[WARN]";
      break;
    case LogLevel::ERROR:
      color = COLOR_RED;
      type = "[ERROR]";
      break;
    case LogLevel::FATAL:
      color = COLOR_MAGENTA;
      type = "[FATAL]";
      break;
    case LogLevel::DEBUG:
      color = COLOR_CYAN;
      type = "[DEBUG]";
      break;
    }
    type += " ";
    return std::make_pair(color, type);
  }

private:
  std::vector<std::pair<LogLevel, std::string>> mLogs;
};
