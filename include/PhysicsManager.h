#pragma once

#include "Mesh.h"
#include <glm/glm.hpp>
#include <reactphysics3d/reactphysics3d.h>

class PhysicsManager {
public:
  PhysicsManager();

  void AddCube(const glm::vec3 &halfSize);
  void AddSphere(float radius);
  void RemoveBody(int id);

  void Update(std::vector<std::shared_ptr<Mesh>> &meshes);

  void SetSimulating(bool toggle) { mSimulating = toggle; }
  void SetSimulationFrame(int frame) { mSimulationFrame = frame; }

  bool IsSimulating() { return mSimulating; }

private:
  void addGroundPlane();
  void randomizeTransforms();

private:
  std::unique_ptr<reactphysics3d::PhysicsCommon> mPhysicsCommon;
  std::unique_ptr<reactphysics3d::PhysicsWorld,
                  std::function<void(reactphysics3d::PhysicsWorld *)>>
      mPhysicsWorld;
  std::vector<reactphysics3d::RigidBody *> mBodies;
  bool mSimulating = false;
  int mSimulatingFrames = 1000;
  int mSimulationFrame = 0;
};
