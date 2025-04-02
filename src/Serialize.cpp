#include "Serialize.h"

namespace Serialize {
namespace ToJson {
void IVec2(nlohmann::json &j, const glm::ivec2 &vec) { j = {vec.x, vec.y}; }
void Vec2(nlohmann::json &j, const glm::vec2 &vec) { j = {vec.x, vec.y}; }
void Vec3(nlohmann::json &j, const glm::vec3 &vec) {
  j = {vec.x, vec.y, vec.z};
}
void Vec4(nlohmann::json &j, const glm::vec4 &vec) {
  j = {vec.x, vec.y, vec.z, vec.w};
}
void Mat3x3(nlohmann::json &j, const glm::mat3x3 &mat) {
  j = {{mat[0][0], mat[0][1], mat[0][2]},
       {mat[1][0], mat[1][1], mat[1][2]},
       {mat[2][0], mat[2][1], mat[2][2]}};
}
void Vec2Arr4(nlohmann::json &j, const glm::vec2 vec[4]) {
  j = json::array();
  for (int i = 0; i < 4; ++i) {
    json vecJson;
    Vec2(vecJson, vec[i]);
    j.push_back(vecJson);
  }
}
void Vec3Arr4(nlohmann::json &j, const glm::vec3 vec[4]) {
  j = json::array();
  for (int i = 0; i < 4; ++i) {
    json vecJson;
    Vec3(vecJson, vec[i]);
    j.push_back(vecJson);
  }
}
} // namespace ToJson

namespace FromJson {
void IVec2(const nlohmann::json &j, glm::ivec2 &vec) {
  vec.x = j.at(0).get<int>();
  vec.y = j.at(1).get<int>();
}
void Vec2(const nlohmann::json &j, glm::vec2 &vec) {
  vec.x = j.at(0).get<float>();
  vec.y = j.at(1).get<float>();
}
void Vec3(const nlohmann::json &j, glm::vec3 &vec) {
  vec.x = j.at(0).get<float>();
  vec.y = j.at(1).get<float>();
  vec.z = j.at(2).get<float>();
}
void Vec4(const nlohmann::json &j, glm::vec4 &vec) {
  vec.x = j.at(0).get<float>();
  vec.y = j.at(1).get<float>();
  vec.z = j.at(2).get<float>();
  vec.w = j.at(3).get<float>();
}
void Mat3x3(const nlohmann::json &j, glm::mat3x3 &mat) {
  for (int i = 0; i < 3; ++i) {
    mat[i][0] = j.at(i).at(0).get<float>();
    mat[i][1] = j.at(i).at(1).get<float>();
    mat[i][2] = j.at(i).at(2).get<float>();
  }
}
void Vec2Arr4(const nlohmann::json &j, glm::vec2 vec[4]) {
  for (int i = 0; i < 4; ++i) {
    Vec2(j.at(i), vec[i]);
  }
}
void Vec3Arr4(const nlohmann::json &j, glm::vec3 vec[4]) {
  for (int i = 0; i < 4; ++i) {
    Vec3(j.at(i), vec[i]);
  }
}
} // namespace FromJson
} // namespace Serialize
