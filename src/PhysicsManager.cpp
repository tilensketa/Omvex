#include "PhysicsManager.h"
#include "Random.h"
#include "Utils.h"

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

void PhysicsManager::AddModel(std::shared_ptr<Model> &model) {
  glm::vec3 half = (model->GetMaxVert() - model->GetMinVert()) / 2.0f;
  AddCuboid(half);
  Logger::getInstance().Info("PhysicsManager: Added body " + model->GetPath());
}

void PhysicsManager::AddCuboid(const glm::vec3 &halfSize) {
  reactphysics3d::Transform transform = defaultTransform();
  reactphysics3d::RigidBody *body = mPhysicsWorld->createRigidBody(transform);
  body->setType(reactphysics3d::BodyType::DYNAMIC);
  mBodies.push_back(body);
  const reactphysics3d::Vector3 halfExtents(halfSize.x, halfSize.y, halfSize.z);
  reactphysics3d::BoxShape *boxShape =
      mPhysicsCommon->createBoxShape(halfExtents);
  reactphysics3d::Collider *collider;
  collider = body->addCollider(boxShape, reactphysics3d::Transform::identity());
}

void PhysicsManager::RemoveBody(int id) {
  if (id != -1) {
    mBodies.erase(mBodies.begin() + (id + 1));
    Logger::getInstance().Info("PhysicsManager: Removed body ");
  }
}

void PhysicsManager::addGroundPlane() {
  reactphysics3d::Transform transform = defaultTransform();
  reactphysics3d::RigidBody *body = mPhysicsWorld->createRigidBody(transform);
  body->setType(reactphysics3d::BodyType::STATIC);
  mBodies.push_back(body);
  // Collider
  const reactphysics3d::Vector3 halfExtents(1000, 1000, 1);
  reactphysics3d::BoxShape *boxShape =
      mPhysicsCommon->createBoxShape(halfExtents);
  reactphysics3d::Collider *collider;
  collider = body->addCollider(boxShape, reactphysics3d::Transform::identity());
  Logger::getInstance().Info("PhysicsManager: Added ground plane");
}
void PhysicsManager::randomizeTransforms() {
  // Skip 0 -> ground plane
  for (size_t i = 1; i < mBodies.size(); i++) {
    reactphysics3d::Body *body = mBodies[i];
    glm::vec2 rndPos = Random::Vec2(-mSpawningSpace, mSpawningSpace);
    float z = Random::Float(mSpawningHeightMinMax.x, mSpawningHeightMinMax.y);
    reactphysics3d::Vector3 position =
        reactphysics3d::Vector3(rndPos.x, rndPos.y, z);
    reactphysics3d::Quaternion orientation = Random::RandomQuaternion();
    reactphysics3d::Transform transform(position, orientation);
    body->setTransform(transform);
    reactphysics3d::RigidBody *rigidBody =
        dynamic_cast<reactphysics3d::RigidBody *>(body);
    if (rigidBody) {
      rigidBody->setLinearVelocity(reactphysics3d::Vector3(0, 0, 0));
      rigidBody->setAngularVelocity(reactphysics3d::Vector3(0, 0, 0));
    }
    body->setIsActive(true);
  }
}

void PhysicsManager::Update(std::vector<std::shared_ptr<Model>> &models) {
  if (!mSimulating)
    return;

  // Initialize random transforms
  if (mSimulationFrame == 0) {
    randomizeTransforms();
    Logger::getInstance().Info("Resimulate");
  }
  // Do physics
  float fixedTimeStep = 1.0f / 60.0f;
  mPhysicsWorld->update(fixedTimeStep);
  bool sleep = true;
  // i = 1 -> beacuse ground plane is 0
  // Models dont include ground, bodies do
  for (size_t i = 1; i < mBodies.size(); i++) {
    const reactphysics3d::Transform &transform = mBodies[i]->getTransform();
    const reactphysics3d::Vector3 &position = transform.getPosition();
    const reactphysics3d::Matrix3x3 &rot =
        transform.getOrientation().getMatrix();
    glm::mat4 mat = Utils::ReactMat3Vec3ToGlmMat4(rot, position);
    for (Mesh &mesh : models[i - 1]->GetMeshes()) {
      mesh.SetTransform(mat);
    }
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

reactphysics3d::Transform PhysicsManager::defaultTransform() {
  reactphysics3d::Vector3 position(0, 0, 0);
  reactphysics3d::Quaternion orientation =
      reactphysics3d::Quaternion::identity();
  reactphysics3d::Transform transform(position, orientation);
  return transform;
}
