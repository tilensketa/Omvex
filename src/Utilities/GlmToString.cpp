#include "Utilities/GlmToString.h"

#include <sstream>

namespace GlmToString {
std::string Vec2(const glm::vec2 &vec) {
  std::ostringstream oss;
  oss << "[" << vec.x << ", " << vec.y << "]";
  return oss.str();
}
std::string Vec3(const glm::vec3 &vec) {
  std::ostringstream oss;
  oss << "[" << vec.x << ", " << vec.y << ", " << vec.z << "]";
  return oss.str();
}
std::string Vec4(const glm::vec4 &vec) {
  std::ostringstream oss;
  oss << "[" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.z << "]";
  return oss.str();
}
std::string Mat3(const glm::mat3 &mat) {
  std::ostringstream oss;
  for (int i = 0; i < 3; ++i) {
    oss << "[" << mat[0][i] << ", " << mat[1][i] << ", " << mat[2][i] << "]\n";
  }
  return oss.str();
}
std::string Mat4(const glm::mat4 &mat) {
  std::ostringstream oss;
  for (int i = 0; i < 4; ++i) {
    oss << "[" << mat[0][i] << ", " << mat[1][i] << ", " << mat[2][i] << ", "
        << mat[3][i] << "]\n";
  }
  return oss.str();
}
} // namespace GlmToString
