#pragma once

#include <glm/glm.hpp>

#include "Core/Camera/CameraParameters.h"

namespace CameraMath {
glm::vec2 ProjectPoint3DTo2D(const glm::vec3 &point3D,
                             CameraParameters &params);
void CalculateRotationTranslation(CameraParameters &params,
                                  const glm::vec2 &imageSize);
void RecalculateParamPoints(CameraParameters &params);
void Recalculate(CameraParameters &params, const glm::vec2 &imageSize);
} // namespace CameraMath
