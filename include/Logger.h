#pragma once

#include "Colors.h"
#include <chrono>
#include <ctime>
#include <imgui.h>
#include <iomanip>
#include <iostream>
#include <vector>
#include <sstream>

#ifdef _WIN32
#include <windows.h>

static void
enableANSIColors() { // `static` ensures it's local to each translation unit
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut == INVALID_HANDLE_VALUE)
    return;

  DWORD dwMode = 0;
  if (!GetConsoleMode(hOut, &dwMode))
    return;

  SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}
#endif

enum class LogLevel { INFO = 0, WARN, ERR, DEBUG, SUCCESS, FATAL };
class Logger {
public:
  // Public method to get the single instance of Logger
  static Logger &getInstance() {
    static Logger instance; // Static instance created once and reused
    return instance;
  }

  inline void Info(const std::string &message) { log(message, LogLevel::INFO); }
  inline void Warn(const std::string &message) { log(message, LogLevel::WARN); }
  inline void Error(const std::string &message) { log(message, LogLevel::ERR); }
  inline void Debug(const std::string &message) {
    log(message, LogLevel::DEBUG);
  }
  inline void Success(const std::string &message) {
    log(message, LogLevel::SUCCESS);
  }
  inline void Fatal(const std::string &message) {
    log(message, LogLevel::FATAL);
  }

  const std::vector<std::pair<LogLevel, std::string>> &GetLogs() const {
    return mLogs;
  }
  void ShowLogs() {
    ImGui::Begin("Logger");
    std::vector<std::pair<LogLevel, std::string>> logs =
        Logger::getInstance().GetLogs();
    ImGui::PushTextWrapPos(ImGui::GetWindowWidth());
    for (const auto &log : logs) {
      ImU32 color = Colors::GetLogColorsImU32()[static_cast<int>(log.first)];
      ImGui::PushStyleColor(ImGuiCol_Text, color);
      ImGui::Text("%s", log.second.c_str());
      ImGui::PopStyleColor();
    }
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
      ImGui::SetScrollHereY(1.0f);
    }
    ImGui::PopTextWrapPos();
    ImGui::End();
  }

private:
  Logger() {
#ifdef _WIN32
    enableANSIColors();
#endif

  } // Private constructor to prevent instantiation
  std::vector<std::string> logs; // Store log messages

  // Prevent copying and assignment (optional but recommended)
  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;

  inline void log(const std::string &message, LogLevel logLevel) {
    std::string timeStamp = "[" + getTimestamp() + "] ";
    auto info = logLevel2ColorType(logLevel);
    std::string logMessageColor =
        timeStamp + info.first + info.second + message + Colors::ANSI::RESET;
    std::cout << logMessageColor << std::endl;
    std::string logMessageNoColor = timeStamp + info.second + message;
    std::pair<LogLevel, std::string> log =
        std::make_pair(logLevel, logMessageNoColor);
    mLogs.push_back(log);
  }

  inline std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_now;

#ifdef _WIN32
    localtime_s(&tm_now, &now_time); // Windows
#else
    localtime_r(&now_time, &tm_now); // Linux/Mac
#endif

    std::ostringstream timestamp;
    timestamp << std::put_time(&tm_now, "%Y-%m-%d %H:%M:%S");
    return timestamp.str();
  }

  inline std::pair<std::string, std::string>
  logLevel2ColorType(LogLevel logLevel) {
    std::string color = "";
    std::string type = "";
    switch (logLevel) {
    case LogLevel::INFO:
      color = Colors::ANSI::WHITE;
      type = "[INFO]";
      break;
    case LogLevel::SUCCESS:
      color = Colors::ANSI::GREEN;
      type = "[SUCCESS]";
      break;
    case LogLevel::WARN:
      color = Colors::ANSI::YELLOW;
      type = "[WARN]";
      break;
    case LogLevel::ERR:
      color = Colors::ANSI::RED;
      type = "[ERROR]";
      break;
    case LogLevel::FATAL:
      color = Colors::ANSI::MAGENTA;
      type = "[FATAL]";
      break;
    case LogLevel::DEBUG:
      color = Colors::ANSI::CYAN;
      type = "[DEBUG]";
      break;
    }
    type += " ";
    return std::make_pair(color, type);
  }

private:
  std::vector<std::pair<LogLevel, std::string>> mLogs;
};
