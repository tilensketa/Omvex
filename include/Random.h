#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>
#include <random>
#include <reactphysics3d/reactphysics3d.h>

namespace Random {
static std::random_device rd;
static std::mt19937 gen(rd());

// Generate a random integer between min and max (inclusive)
inline int Int(int min, int max) {
  std::uniform_int_distribution<int> dist(min, max);
  return dist(gen);
}

// Generate a random float between min and max
inline float Float(float min, float max) {
  std::uniform_real_distribution<float> dist(min, max);
  return dist(gen);
}

// Generate a random glm::vec3 with each component between min and max
inline glm::vec3 Vec3(float min, float max) {
  return glm::vec3(Float(min, max), Float(min, max), Float(min, max));
}

// Function to generate a random quaternion
inline reactphysics3d::Quaternion RandomQuaternion() {
  // Generate a random unit axis
  glm::vec3 randomAxis = glm::sphericalRand(1.0f); // Random unit vector

  // Generate a random rotation angle (0 to 2π radians)
  float randomAngle = glm::linearRand(0.0f, glm::two_pi<float>());

  // Convert axis-angle to quaternion using the quaternion formula
  float halfAngle = randomAngle * 0.5f;
  float sinHalfAngle = sin(halfAngle);

  return reactphysics3d::Quaternion(sinHalfAngle * randomAxis.x, // x component
                                    sinHalfAngle * randomAxis.y, // y component
                                    sinHalfAngle * randomAxis.z, // z component
                                    cos(halfAngle)               // w component
  );
}
} // namespace Random
