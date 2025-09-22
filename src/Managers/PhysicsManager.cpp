#include "Managers/PhysicsManager.h"

#include "Utilities/Random.h"
#include "Utilities/ReactPhysicsHelpers.h"

PhysicsManager::PhysicsManager() {
  reactphysics3d::PhysicsWorld::WorldSettings settings;
  settings.defaultVelocitySolverNbIterations = 20;
  settings.isSleepingEnabled = true;
  settings.gravity =
      reactphysics3d::Vector3(0, 0, reactphysics3d::decimal(-9.81));

  mPhysicsCommon = std::make_unique<reactphysics3d::PhysicsCommon>();
  reactphysics3d::PhysicsWorld *rawWorld =
      mPhysicsCommon->createPhysicsWorld(settings);
  mPhysicsWorld =
      std::unique_ptr<reactphysics3d::PhysicsWorld,
                      std::function<void(reactphysics3d::PhysicsWorld *)>>(
          rawWorld, [this](reactphysics3d::PhysicsWorld *w) {
            if (mPhysicsCommon) {
              mPhysicsCommon->destroyPhysicsWorld(w);
            }
          });
  addGroundPlane();
}

void PhysicsManager::AddModel(Model *model) {
  glm::vec3 half = (model->GetMaxVert() - model->GetMinVert()) / 2.0f;
  auto *body =
      createRigidBody(reactphysics3d::BodyType::DYNAMIC, defaultTransform());
  attachBoxCollider(body, {half.x, half.y, half.z});
  Logger::Debug("PhysicsManager: Added body " + model->GetPath());
}

void PhysicsManager::RemoveBody(int id) {
  if (id >= 0 && id < static_cast<int>(mBodies.size())) {
    auto *body = mBodies[id];
    mPhysicsWorld->destroyRigidBody(body);
    mBodies.erase(mBodies.begin() + id);
    Logger::Debug("PhysicsManager: Removed body");
  }
}

void PhysicsManager::addGroundPlane() {
  mGround = createRigidBody(reactphysics3d::BodyType::STATIC,
                            defaultTransform(), false);
  attachBoxCollider(mGround, {1000, 1000, 1});
  Logger::Debug("PhysicsManager: Added ground plane");
}
void PhysicsManager::randomizeTransforms() {
  for (size_t i = 0; i < mBodies.size(); i++) {
    reactphysics3d::RigidBody *body = mBodies[i];
    glm::vec2 pos = Random::Vec2(-mSpawningSpace, mSpawningSpace);

    const reactphysics3d::AABB aabb = body->getAABB();
    reactphysics3d::Vector3 extents = aabb.getMax() - aabb.getMin();
    float z = glm::max(extents.x, glm::max(extents.y, extents.z));

    reactphysics3d::Vector3 position = reactphysics3d::Vector3(pos.x, pos.y, z);
    reactphysics3d::Quaternion orientation = Random::RandomQuaternion();
    reactphysics3d::Transform transform(position, orientation);

    body->setTransform(transform);
    body->setLinearVelocity(reactphysics3d::Vector3(0, 0, 0));
    body->setAngularVelocity(reactphysics3d::Vector3(0, 0, 0));
    body->setIsActive(true);
  }
}

void PhysicsManager::Update(std::vector<std::unique_ptr<Model>> &models) {
  if (!mSimulating) return;

  // Initialize random transforms
  if (mSimulationFrame == 0) {
    randomizeTransforms();
    Logger::Info("Resimulate");
  }
  // Do physics
  constexpr float fixedTimeStep = 1.0f / 60.0f;
  mPhysicsWorld->update(fixedTimeStep);
  bool sleep = true;
  for (size_t i = 0; i < mBodies.size(); i++) {
    const reactphysics3d::RigidBody *body = mBodies[i];
    const auto &transform = body->getTransform();
    const auto &position = transform.getPosition();
    const auto &rot = transform.getOrientation().getMatrix();
    glm::mat4 mat = ReactMat3Vec3ToGlmMat4(rot, position);
    models[i]->SetModelMatrix(mat);
    if (!mBodies[i]->isSleeping()) {
      sleep = false;
    }
  }
  mSimulationFrame++;
  // Stop simulating if all bodies are sleeping or max simulation frames reached
  if (sleep || (mSimulationFrame >= mSimulatingFrames)) {
    mSimulating = false;
  }
}

reactphysics3d::Transform PhysicsManager::defaultTransform() const {
  reactphysics3d::Vector3 position(0, 0, 0);
  reactphysics3d::Quaternion orientation =
      reactphysics3d::Quaternion::identity();
  reactphysics3d::Transform transform(position, orientation);
  return transform;
}
reactphysics3d::RigidBody *
PhysicsManager::createRigidBody(reactphysics3d::BodyType type,
                                const reactphysics3d::Transform &transform,
                                bool trackInBodies) {
  auto *body = mPhysicsWorld->createRigidBody(transform);
  body->setType(type);
  if (trackInBodies) mBodies.push_back(body);
  return body;
}
void PhysicsManager::attachBoxCollider(
    reactphysics3d::RigidBody *body,
    const reactphysics3d::Vector3 &halfExtents) {
  auto *boxShape = mPhysicsCommon->createBoxShape(halfExtents);
  body->addCollider(boxShape, reactphysics3d::Transform::identity());
}
