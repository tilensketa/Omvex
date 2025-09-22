#pragma once

#include <glm/glm.hpp>
#include <reactphysics3d/reactphysics3d.h>

inline glm::mat4
ReactMat3Vec3ToGlmMat4(const reactphysics3d::Matrix3x3 &orientation,
                       const reactphysics3d::Vector3 &position) {
  glm::mat4 mat = glm::mat4(1.0f);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      mat[i][j] = orientation[j][i];
    }
  }
  mat[3][0] = position.x;
  mat[3][1] = position.y;
  mat[3][2] = position.z;
  mat[3][3] = 1.0;
  return mat;
}
