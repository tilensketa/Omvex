#pragma once

#include "Model.h"
#include <glm/glm.hpp>
#include <reactphysics3d/reactphysics3d.h>

class PhysicsManager {
public:
  PhysicsManager();

  void AddModel(std::shared_ptr<Model> &model);
  void AddCuboid(const glm::vec3 &halfSize);
  void RemoveBody(int id);

  void Update(std::vector<std::shared_ptr<Model>> &models);

  void SetSimulating(bool toggle) { mSimulating = toggle; }
  void SetSimulationFrame(int frame) { mSimulationFrame = frame; }

  void SetSpawningSpace(float space) { mSpawningSpace = space; }
  const float GetSpawningSpace() const { return mSpawningSpace; }
  float &ModSpawningSpace() { return mSpawningSpace; }

  bool IsSimulating() { return mSimulating; }

  const int GetBodyCount() const { return static_cast<int>(mBodies.size()); }

private:
  void addGroundPlane();
  void randomizeTransforms();
  reactphysics3d::Transform defaultTransform();

private:
  std::unique_ptr<reactphysics3d::PhysicsCommon> mPhysicsCommon;
  std::unique_ptr<reactphysics3d::PhysicsWorld,
                  std::function<void(reactphysics3d::PhysicsWorld *)>>
      mPhysicsWorld;
  std::vector<reactphysics3d::RigidBody *> mBodies;
  bool mSimulating = false;
  int mSimulatingFrames = 1000;
  int mSimulationFrame = 0;
  // Square +-
  float mSpawningSpace = 8;
  glm::vec2 mSpawningHeightMinMax = glm::vec2(30, 50);
};
