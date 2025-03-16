#pragma once

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace Serialize {

// Serialize glm::ivec2 as an array
inline void ToJson(nlohmann::json &j, const glm::ivec2 &vec) {
  j = {vec.x, vec.y};
}

inline void FromJson(const nlohmann::json &j, glm::ivec2 &vec) {
  vec.x = j.at(0).get<int>();
  vec.y = j.at(1).get<int>();
}

// Serialize glm::vec2 as an array
inline void ToJson(nlohmann::json &j, const glm::vec2 &vec) {
  j = {vec.x, vec.y};
}

inline void FromJson(const nlohmann::json &j, glm::vec2 &vec) {
  vec.x = j.at(0).get<float>();
  vec.y = j.at(1).get<float>();
}

// Serialize glm::vec3 as an array
inline void ToJson(nlohmann::json &j, const glm::vec3 &vec) {
  j = {vec.x, vec.y, vec.z};
}

inline void FromJson(const nlohmann::json &j, glm::vec3 &vec) {
  vec.x = j.at(0).get<float>();
  vec.y = j.at(1).get<float>();
  vec.z = j.at(2).get<float>();
}

// Serialize glm::vec4 as an array
inline void ToJson(nlohmann::json &j, const glm::vec4 &vec) {
  j = {vec.x, vec.y, vec.z, vec.w};
}

inline void FromJson(const nlohmann::json &j, glm::vec4 &vec) {
  vec.x = j.at(0).get<float>();
  vec.y = j.at(1).get<float>();
  vec.z = j.at(2).get<float>();
  vec.w = j.at(3).get<float>();
}

// Serialize glm::mat3x3 as an array of arrays
inline void ToJson(nlohmann::json &j, const glm::mat3x3 &mat) {
  j = {{mat[0][0], mat[0][1], mat[0][2]},
       {mat[1][0], mat[1][1], mat[1][2]},
       {mat[2][0], mat[2][1], mat[2][2]}};
}

inline void FromJson(const nlohmann::json &j, glm::mat3x3 &mat) {
  for (size_t i = 0; i < 3; ++i) {
    mat[i][0] = j.at(i).at(0).get<float>();
    mat[i][1] = j.at(i).at(1).get<float>();
    mat[i][2] = j.at(i).at(2).get<float>();
  }
}

// Serialize an array of glm::vec2 (C-style array with size 4)
inline void ToJson(nlohmann::json &j, const glm::vec2 vec[4]) {
  j = json::array();
  for (size_t i = 0; i < 4; ++i) {
    json vecJson;
    ToJson(vecJson, vec[i]);
    j.push_back(vecJson);
  }
}

// Deserialize an array of glm::vec3 (C-style array with size 4)
inline void FromJson(const nlohmann::json &j, glm::vec3 vec[4]) {
  for (size_t i = 0; i < 4; ++i) {
    FromJson(j.at(i), vec[i]);
  }
}

// Serialize an array of glm::vec3 (C-style array with size 4)
inline void ToJson(nlohmann::json &j, const glm::vec3 vec[4]) {
  j = json::array();
  for (size_t i = 0; i < 4; ++i) {
    json vecJson;
    ToJson(vecJson, vec[i]);
    j.push_back(vecJson);
  }
}

// Deserialize an array of glm::vec2 (C-style array with size 4)
inline void FromJson(const nlohmann::json &j, glm::vec2 vec[4]) {
  for (size_t i = 0; i < 4; ++i) {
    FromJson(j.at(i), vec[i]);
  }
}
}
