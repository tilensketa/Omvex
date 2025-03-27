#pragma once

#include <string>
#include <vector>

namespace FileSystem {

// Extension
std::string RemoveFileExtension(const std::string &filePath);
std::string GetFileExtension(const std::string &filePath);

// From path
std::string GetFileNameFromPath(const std::string &path);
std::string GetDirectoryFromPath(const std::string &path);

std::string FindExistingFolder(const std::vector<std::string> paths);
void CreateDir(const std::string &directoryPath);
std::vector<std::string> OpenFiles(std::string &path, const std::string &name,
                                   const std::string &exts);
std::string SelectFolder(const std::string &path);

std::vector<std::string> splitPatterns(const std::string &patterns);
bool matchesPattern(const std::string &filename, const std::string &pattern);

} // namespace FileSystem
