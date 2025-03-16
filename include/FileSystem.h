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

// Select folder with start path and return it
inline std::string SelectFolder(const std::string &path) {
  auto f = pfd::select_folder("Select folder", path);
  std::string file = f.result();
  return file;
}

// Helper function to split the string into patterns
// *.jpg *.jpeg *.png -> {*.jpg, *.jpeg, *.png}
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
// - pattern *.txt
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

inline std::vector<std::string>
OpenFiles(std::string &path, const std::string &name, const std::string &exts) {
  std::vector<std::string> filesToOpen;
  if (path == "") {
    path = pfd::path::home();
  }
  auto f = pfd::open_file("Choose files to read", path, {name, exts},
                          pfd::opt::multiselect);
  std::vector<std::string> parsedPatterns = splitPatterns(exts);
  for (const std::string &file : f.result()) {
    for (const auto &pattern : parsedPatterns) {
      if (matchesPattern(file, pattern)) {
        Logger::getInstance().Debug("Open file: " + file);
        filesToOpen.push_back(file);
      }
    }
  }
  return filesToOpen;
}

inline std::string SaveFile(std::string &path, std::string extension) {
  if (path == "") {
    path = pfd::path::home();
  }
  auto f = pfd::save_file("Choose file to save", path);
  std::string file = f.result();
  if (file != "") {
    file += extension;
    Logger::getInstance().Debug("Save file: " + file);
    return file;
  }
  Logger::getInstance().Debug("Save file: NOT SELECTED");
  return "";
}

inline bool
CheckFileValidExtension(const std::string &filePath,
                        const std::vector<std::string> &validExtensions) {
  std::filesystem::path path(filePath);
  std::string extension = path.extension().string();
  Logger::getInstance().Debug("File: " + filePath + ", Ext: " + extension);
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
    Logger::getInstance().Error(msg);
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

inline std::string GetFileNameFromPath(const std::string &path) {
  std::filesystem::path filePath(path);
  return filePath.filename().string();
}
inline std::string GetDirectoryFromPath(const std::string &path) {
  std::filesystem::path filePath(path);
  return filePath.parent_path().string();
}

inline std::string FindExistingFolder(const std::vector<std::string> &paths) {
  for (const auto &path : paths) {
    if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
      return path; // Return the first existing folder
    }
  }
  return "";
}

} // namespace FileSystem
  //
