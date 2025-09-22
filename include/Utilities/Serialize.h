#pragma once

#include "Core/Logger.h"

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Serialize {
namespace ToJson {
// generic for glm::vecN (glm::vec2, glm::vec3, glm::vec4, glm::ivec2, etc.)
template <typename T, glm::length_t L>
void Vec(json &j, const glm::vec<L, T, glm::defaultp> &v) {
  j = json::array();
  for (glm::length_t i = 0; i < L; ++i) {
    j.push_back(v[i]);
  }
}
// generic for glm::matCxR (glm::mat3x3, glm::mat4x4, etc.)
template <typename T, glm::length_t C, glm::length_t R>
void Mat(json &j, const glm::mat<C, R, T, glm::defaultp> &m) {
  j = json::array();
  for (glm::length_t c = 0; c < C; ++c) {
    json col = json::array();
    for (glm::length_t r = 0; r < R; ++r) {
      col.push_back(m[c][r]);
    }
    j.push_back(col);
  }
}
// generic for std::vector of glm::vecN
template <typename T> void Vector(json &j, const std::vector<T> &vec) {
  j = json::array();
  for (const auto &v : vec) {
    json elem;
    Vec(elem, v); // will call the right overload
    j.push_back(elem);
  }
}
} // namespace ToJson
namespace FromJson {
// glm::vecN
template <typename T, glm::length_t L>
void vec(const json &j, glm::vec<L, T, glm::defaultp> &v) {
  for (glm::length_t i = 0; i < L; ++i) {
    v[i] = j.at(i).get<T>();
  }
}
// glm::matCxR
template <typename T, glm::length_t C, glm::length_t R>
void mat(const json &j, glm::mat<C, R, T, glm::defaultp> &m) {
  for (glm::length_t c = 0; c < C; ++c) {
    for (glm::length_t r = 0; r < R; ++r) {
      m[c][r] = j.at(c).at(r).get<T>();
    }
  }
}
// std::vector<glm::vecN>
template <typename T> void vector(const json &j, std::vector<T> &v) {
  v.resize(j.size());
  for (size_t i = 0; i < j.size(); ++i) {
    vec(j.at(i), v[i]);
  }
}

template <typename T>
inline void SafeVec(const json &j, const std::string &key, T &out) {
  try {
    if (j.contains(key)) {
      vec(j.at(key), out);
    } else {
      Logger::Error("SafeVec: Missing key '" + key + "'");
    }
  } catch (const std::exception &e) {
    Logger::Error("SafeVec: Failed to parse key '" + key + "': " + e.what());
  }
}
template <typename T>
inline void SafeMat(const json &j, const std::string &key, T &out) {
  try {
    if (j.contains(key)) {
      mat(j.at(key), out);
    } else {
      Logger::Error("SafeMat: Missing key '" + key + "'");
    }
  } catch (const std::exception &e) {
    Logger::Error("SafeMat: Failed to parse key '" + key + "': " + e.what());
  }
}
template <typename T>
inline void SafeVector(const json &j, const std::string &key, T &out) {
  try {
    if (j.contains(key)) {
      vector(j.at(key), out);
    } else {
      Logger::Error("SafeVector: Missing key '" + key + "'");
    }
  } catch (const std::exception &e) {
    Logger::Error("SafeVector: Failed to parse key '" + key + "': " + e.what());
  }
}
inline void SafeString(const json &j, const std::string &key,
                       std::string &out) {
  try {
    if (j.contains(key)) {
      out = j.at(key).get<std::string>();
    } else {
      Logger::Error("SafeString: Missing key '" + key + "'");
    }
  } catch (const std::exception &e) {
    Logger::Error("SafeString: Failed to parse key '" + key + "': " + e.what());
  }
}

} // namespace FromJson
} // namespace Serialize
