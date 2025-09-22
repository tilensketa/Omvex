#pragma once

#include "Rendering/Models/Model.h"

#include <glm/glm.hpp>
#include <reactphysics3d/reactphysics3d.h>

class PhysicsManager {
public:
  PhysicsManager();

  void AddModel(Model *model);
  void RemoveBody(int id);

  void Update(std::vector<std::unique_ptr<Model>> &models);

  void Simulate() {
    mSimulating = true;
    mSimulationFrame = 0;
  }

  const float GetSpawningSpace() const { return mSpawningSpace; }
  float &ModSpawningSpace() { return mSpawningSpace; }

  bool IsSimulating() { return mSimulating; }

  const int GetBodyCount() const { return static_cast<int>(mBodies.size()); }
  const std::vector<reactphysics3d::RigidBody *> GetBodies() const {
    return mBodies;
  }

private:
  void addGroundPlane();
  void randomizeTransforms();
  reactphysics3d::Transform defaultTransform() const;
  reactphysics3d::RigidBody *
  createRigidBody(reactphysics3d::BodyType type,
                  const reactphysics3d::Transform &transform,
                  bool trackInBodies = true);
  void attachBoxCollider(reactphysics3d::RigidBody *body,
                         const reactphysics3d::Vector3 &halfExtents);

private:
  std::unique_ptr<reactphysics3d::PhysicsCommon> mPhysicsCommon;
  std::unique_ptr<reactphysics3d::PhysicsWorld,
                  std::function<void(reactphysics3d::PhysicsWorld *)>>
      mPhysicsWorld;
  std::vector<reactphysics3d::RigidBody *> mBodies;
  reactphysics3d::RigidBody *mGround;
  bool mSimulating = false;
  int mSimulatingFrames = 1000;
  int mSimulationFrame = 0;
  // Square +-
  float mSpawningSpace = 8;
};
