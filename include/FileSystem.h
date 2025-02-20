#pragma once

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <portable-file-dialogs.h>
#include <string>
#include <vector>
namespace fs = std::filesystem;
#include <imgui.h>

#include "Logger.h"

namespace FileSystem {

inline std::string GetDirectory(const std::string &filePath) {
  return filePath;
  // return std::filesystem::path(filePath).parent_path().string();
}
inline std::string SelectFolder(const std::string &path) {
  auto f = pfd::select_folder("Select folder", path);
  std::string file = f.result();
  return file;
}

// Helper function to split the string into patterns
inline std::vector<std::string> splitPatterns(const std::string &patterns) {
  std::vector<std::string> result;
  size_t start = 0, end;
  while ((end = patterns.find(' ', start)) != std::string::npos) {
    result.push_back(patterns.substr(start, end - start));
    start = end + 1;
  }
  // Add the last pattern
  if (start < patterns.size()) {
    result.push_back(patterns.substr(start));
  }
  return result;
}

// Helper function to check if a file matches a pattern
inline bool matchesPattern(const std::string &filename,
                           const std::string &pattern) {
  // Extract the extension from the pattern (e.g., "*.txt" -> "txt")
  size_t asteriskPos = pattern.find('*');
  if (asteriskPos == std::string::npos) {
    return false; // Invalid pattern
  }
  std::string patternExtension = pattern.substr(asteriskPos + 1);
  size_t dotPos = filename.rfind('.');
  if (dotPos == std::string::npos) {
    return false; // File has no extension
  }
  std::string fileExtension = filename.substr(dotPos);
  return fileExtension == patternExtension;
}

inline std::string OpenFile(std::string &path, const std::string &name,
                            const std::string &exts) {
  if (path == "") {
    path = pfd::path::home();
  }
  auto f = pfd::open_file("Choose files to read", path, {name, exts});
  if (f.result().size() == 1) {
    std::string file = f.result()[0];
    std::vector<std::string> parsedPatterns = splitPatterns(exts);
    for (const auto &pattern : parsedPatterns) {
      if (matchesPattern(file, pattern)) {
        std::string msg = "Open file: " + file;
        Logger::getInstance().Log(msg, LogLevel::DEBUG);
        path = GetDirectory(file);
        return file;
      }
    }
  }
  Logger::getInstance().Log("Open file: NOT SELECTED", LogLevel::DEBUG);
  return "";
}
inline std::string SaveFile(std::string &path, std::string extension) {
  if (path == "") {
    path = pfd::path::home();
  }
  auto f = pfd::save_file("Choose file to save", path);
  std::string file = f.result();
  if (file != "") {
    file += extension;
    std::string msg = "Save file: " + file;
    Logger::getInstance().Log(msg, LogLevel::DEBUG);
    path = GetDirectory(file);
    return file;
  }
  Logger::getInstance().Log("Save file: NOT SELECTED", LogLevel::DEBUG);
  return "";
}

inline bool
CheckFileValidExtension(const std::string &filePath,
                        const std::vector<std::string> &validExtensions) {
  std::filesystem::path path(filePath);
  std::string extension = path.extension().string();
  std::string msg = "file: " + filePath + ", ext: " + extension;
  Logger::getInstance().Log(msg, LogLevel::DEBUG);
  if (std::find(validExtensions.begin(), validExtensions.end(), extension) !=
      validExtensions.end()) {
    return true;
  }
  return false;
}

inline std::vector<std::string>
GetFilesWithExtensions(const std::string &folderPath,
                       const std::vector<std::string> &validExtensions) {
  std::vector<std::string> files;
  try {
    for (const auto &entry : fs::directory_iterator(folderPath)) {
      if (entry.is_regular_file()) { // Ensure it's a file
        std::string fileName = entry.path().filename().string();
        std::string extension =
            entry.path().extension().string(); // Get the file extension

        // Convert extension to lowercase for case-insensitive comparison
        std::transform(extension.begin(), extension.end(), extension.begin(),
                       ::tolower);

        // Include files with extensions in the valid list
        if (std::find(validExtensions.begin(), validExtensions.end(),
                      extension) != validExtensions.end()) {
          files.push_back(fileName);
        }
      }
    }
  } catch (const std::exception &e) {
    std::string msg =
        "Error accessing folder: " + folderPath + ". Exception: " + e.what();
    Logger::getInstance().Log(msg, LogLevel::FATAL);
  }
  return files;
}

// Returns filePath of selected file
inline std::string
ShowFileDropdown(const std::string &folderPath,
                 const std::vector<std::string> &validExtensions) {
  static int currentFileIndex = 0; // Tracks the selected file
  static std::vector<std::string> files =
      GetFilesWithExtensions(folderPath, validExtensions);

  // Display dropdown
  if (ImGui::BeginCombo("Select a File",
                        files.empty() ? "No Files"
                                      : files[currentFileIndex].c_str())) {
    for (size_t i = 0; i < files.size(); ++i) {
      const bool isSelected = (currentFileIndex == i);
      if (ImGui::Selectable(files[i].c_str(), isSelected)) {
        currentFileIndex = i; // Update the selected file
      }
      if (isSelected) {
        ImGui::SetItemDefaultFocus(); // Highlight selected
      }
    }
    ImGui::EndCombo();
  }

  std::string filePath = "";
  // Show the selected file below the dropdown
  if (!files.empty()) {
    ImGui::Text("Selected File: %s", files[currentFileIndex].c_str());
    filePath = folderPath + "/" + files[currentFileIndex];
  }
  return filePath;
}
inline std::string RemoveFileExtension(const std::string &filePath) {
  // Use std::filesystem to manipulate the file path
  std::filesystem::path path(filePath);
  // Remove the extension and return the string
  return path.stem().string();
}

} // namespace FileSystem
