#pragma once
#include <glm/glm.hpp>
#include <string>

namespace GlmToString {
std::string Vec2(const glm::vec2 &vec);
std::string Vec3(const glm::vec3 &vec);
std::string Vec4(const glm::vec4 &vec);
std::string Mat3(const glm::mat3 &mat);
std::string Mat3x4(const glm::mat3x4 &mat);
std::string Mat4(const glm::mat4 &mat);
} // namespace GlmToString
