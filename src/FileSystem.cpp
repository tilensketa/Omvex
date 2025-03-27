#include "FileSystem.h"

#include <cctype>
#include <filesystem>
#include <portable-file-dialogs.h>

#include "Logger.h"

namespace FileSystem {

std::string RemoveFileExtension(const std::string &filePath) {
  std::filesystem::path path(filePath);
  return path.stem().string();
}
std::string GetFileExtension(const std::string &filePath) {
  std::filesystem::path path(filePath);
  return path.extension().string();
}

std::string GetFileNameFromPath(const std::string &path) {
  std::filesystem::path filePath(path);
  return filePath.filename().string();
}
std::string GetDirectoryFromPath(const std::string &path) {
  std::filesystem::path filePath(path);
  return filePath.parent_path().string();
}

std::string FindExistingFolder(const std::vector<std::string> paths) {
  for (std::string path : paths) {
    if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
      return path; // Return the first existing folder
    }
  }
  return "";
}
void CreateDir(const std::string &directoryPath) {
  std::filesystem::create_directory(directoryPath);
}

std::string SelectFolder(const std::string &path) {
  auto f = pfd::select_folder("Select folder", path);
  std::string folder = f.result();
  Logger::getInstance().Debug("Selected folder: " + folder);
  return folder;
}

// Helper function to split the string into patterns
// *.jpg *.jpeg *.png -> {*.jpg, *.jpeg, *.png}
std::vector<std::string> splitPatterns(const std::string &patterns) {
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
bool matchesPattern(const std::string &filename, const std::string &pattern) {
  // Extract the extension from the pattern (e.g., "*.txt" -> ".txt")
  size_t asteriskPos = pattern.find('*');
  if (asteriskPos == std::string::npos) {
    return false; // Invalid pattern
  }
  std::string patternExtension = pattern.substr(asteriskPos + 1);
  std::string fileExtension = GetFileExtension(filename);
  return fileExtension == patternExtension;
}

std::vector<std::string> OpenFiles(std::string &path, const std::string &name,
                                   const std::string &exts) {
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
        path = GetDirectoryFromPath(file);
        break;
      }
    }
  }
  return filesToOpen;
}
} // namespace FileSystem
