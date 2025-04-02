#pragma once

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Serialize {

// Serialize glm::ivec2 as an array
namespace ToJson {
void IVec2(nlohmann::json &j, const glm::ivec2 &vec);
void Vec2(nlohmann::json &j, const glm::vec2 &vec);
void Vec3(nlohmann::json &j, const glm::vec3 &vec);
void Vec4(nlohmann::json &j, const glm::vec4 &vec);
void Mat3x3(nlohmann::json &j, const glm::mat3x3 &mat);
void Vec2Arr4(nlohmann::json &j, const glm::vec2 vec[4]);
void Vec3Arr4(nlohmann::json &j, const glm::vec3 vec[4]);
} // namespace ToJson

namespace FromJson {
void IVec2(const nlohmann::json &j, glm::ivec2 &vec);
void Vec2(const nlohmann::json &j, glm::vec2 &vec);
void Vec3(const nlohmann::json &j, glm::vec3 &vec);
void Vec4(const nlohmann::json &j, glm::vec4 &vec);
void Mat3x3(const nlohmann::json &j, glm::mat3x3 &mat);
void Vec2Arr4(const nlohmann::json &j, glm::vec2 vec[4]);
void Vec3Arr4(const nlohmann::json &j, glm::vec3 vec[4]);
} // namespace FromJson
} // namespace Serialize
