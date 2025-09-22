#include "Core/Logger.h"

#include "Utilities/Colors.h"
#include "Utilities/FileSystem.h"

#include <chrono>
#include <imgui.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <unordered_map>

#ifdef _WIN32
#include <windows.h>

static void
enableANSIColors() { // `static` ensures it's local to each translation unit
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut == INVALID_HANDLE_VALUE) return;

  DWORD dwMode = 0;
  if (!GetConsoleMode(hOut, &dwMode)) return;

  SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}
#endif

// Public method to get the single instance of Logger
Logger &Logger::get() {
  static Logger instance;
  return instance;
}

void Logger::showLogsInternal() {
  ImGui::Begin("Logger");
  ImGui::PushTextWrapPos(ImGui::GetWindowWidth());
  for (const auto &log : mLogs) {
    ImU32 color = Colors::GetLogColorsImU32[static_cast<int>(log.first)];
    ImGui::PushStyleColor(ImGuiCol_Text, color);
    ImGui::Text("%s", log.second.c_str());
    ImGui::PopStyleColor();
  }
  ImGui::Dummy(ImVec2(0.0f, 2 * ImGui::GetTextLineHeightWithSpacing()));
  if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
    ImGui::SetScrollHereY(1.0f);
  }
  ImGui::PopTextWrapPos();
  ImGui::End();
}

Logger::Logger() {
#ifdef _WIN32
  enableANSIColors();
#endif

  FileSystem::CreateDir("logs");

  // Create or open a log file in the 'logs' directory
  std::string logFilePath =
      "logs/logfile_" + getTimestamp("%Y-%m-%d_%H-%M-%S") + ".txt";
  mLogFile.open(logFilePath, std::ios::app);
  if (!mLogFile.is_open()) {
    Error("Failed to open log file");
  }
}

void Logger::log(const std::string &message, LogLevel logLevel) {
  std::string timeStamp = "[" + getTimestamp() + "] ";
  auto info = logLevel2ColorType(logLevel);
  std::string logMessageColor =
      timeStamp + info.first + info.second + message + Colors::ANSI::RESET;
  std::cout << logMessageColor << std::endl;
  std::string logMessageNoColor = timeStamp + info.second + message;
  if (mLogFile.is_open()) {
    mLogFile << logMessageNoColor << std::endl;
    mLogFile.flush();
  }
  // Only store non debug logs
  if (logLevel != LogLevel::DEBUG) {
    std::pair<LogLevel, std::string> log =
        std::make_pair(logLevel, logMessageNoColor);
    if (mLogs.size() >= MAX_LOGS) {
      mLogs.pop_front(); // Remove oldest log
    }
    mLogs.push_back(log);
  }
}

const std::string Logger::getTimestamp(const std::string &format) {
  auto now = std::chrono::system_clock::now();
  std::time_t now_time = std::chrono::system_clock::to_time_t(now);
  std::tm tm_now;

#ifdef _WIN32
  localtime_s(&tm_now, &now_time); // Windows
#else
  localtime_r(&now_time, &tm_now); // Linux/Mac
#endif

  std::ostringstream timestamp;
  timestamp << std::put_time(&tm_now, format.c_str());
  return timestamp.str();
}

std::pair<std::string, std::string>
Logger::logLevel2ColorType(LogLevel logLevel) {
  static const std::unordered_map<LogLevel, std::pair<std::string, std::string>>
      logMap = {{LogLevel::INFO, {Colors::ANSI::WHITE, "[INFO]"}},
                {LogLevel::SUCCESS, {Colors::ANSI::GREEN, "[SUCCESS]"}},
                {LogLevel::WARN, {Colors::ANSI::YELLOW, "[WARN]"}},
                {LogLevel::ERR, {Colors::ANSI::RED, "[ERROR]"}},
                {LogLevel::FATAL, {Colors::ANSI::MAGENTA, "[FATAL]"}},
                {LogLevel::DEBUG, {Colors::ANSI::CYAN, "[DEBUG]"}}};

  auto it = logMap.find(logLevel);
  if (it != logMap.end()) {
    return {it->second.first, it->second.second + " "};
  }
  return {Colors::ANSI::WHITE, "[UNKNOWN] "};
}
