#include "PhysicsManager.h"
#include "Random.h"
#include "Utils.h"

PhysicsManager::PhysicsManager() {
  reactphysics3d::PhysicsWorld::WorldSettings settings;
  settings.defaultVelocitySolverNbIterations = 20;
  settings.isSleepingEnabled = true;
  settings.gravity = reactphysics3d::Vector3(0, 0, -9.81);

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

void PhysicsManager::AddCube(const glm::vec3 &halfSize) {
  reactphysics3d::Vector3 position(0, 0, 0);
  reactphysics3d::Quaternion orientation =
      reactphysics3d::Quaternion::identity();
  reactphysics3d::Transform transform(position, orientation);
  reactphysics3d::RigidBody *body = mPhysicsWorld->createRigidBody(transform);
  body->setType(reactphysics3d::BodyType::DYNAMIC);
  mBodies.push_back(body);
  const reactphysics3d::Vector3 halfExtents(halfSize.x, halfSize.y, halfSize.z);
  reactphysics3d::BoxShape *boxShape =
      mPhysicsCommon->createBoxShape(halfExtents);
  reactphysics3d::Collider *collider;
  collider = body->addCollider(boxShape, reactphysics3d::Transform::identity());
}

void PhysicsManager::AddSphere(float radius) {
  reactphysics3d::Vector3 position(0, 0, 0);
  reactphysics3d::Quaternion orientation =
      reactphysics3d::Quaternion::identity();
  reactphysics3d::Transform transform(position, orientation);
  reactphysics3d::RigidBody *body = mPhysicsWorld->createRigidBody(transform);
  body->setType(reactphysics3d::BodyType::DYNAMIC);
  mBodies.push_back(body);
  reactphysics3d::SphereShape *sphereShape =
      mPhysicsCommon->createSphereShape(radius);
  reactphysics3d::Collider *collider;
  collider =
      body->addCollider(sphereShape, reactphysics3d::Transform::identity());
}

void PhysicsManager::RemoveBody(int id) {
  mBodies.erase(mBodies.begin() + (id + 1));
}

void PhysicsManager::addGroundPlane() {
  reactphysics3d::Vector3 position(0, 0, 0);
  reactphysics3d::Quaternion orientation =
      reactphysics3d::Quaternion::identity();
  reactphysics3d::Transform transform(position, orientation);
  reactphysics3d::RigidBody *body = mPhysicsWorld->createRigidBody(transform);
  body->setType(reactphysics3d::BodyType::STATIC);
  mBodies.push_back(body);
  // Collider
  const reactphysics3d::Vector3 halfExtents(124, 124, 1);
  reactphysics3d::BoxShape *boxShape =
      mPhysicsCommon->createBoxShape(halfExtents);
  reactphysics3d::Collider *collider;
  collider = body->addCollider(boxShape, reactphysics3d::Transform::identity());
}
void PhysicsManager::randomizeTransforms() {
  // Skip 0 -> ground plane
  for (int i = 1; i < mBodies.size(); i++) {
    reactphysics3d::Body *body = mBodies[i];
    glm::vec3 rndPos = Random::Vec3(-30.0f, 30.0f);
    float z = Random::Float(30, 50);
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

void PhysicsManager::Update(std::vector<std::shared_ptr<Mesh>> &meshes) {
  if (mSimulating) {
    if (mSimulationFrame == 0) {
      randomizeTransforms();
      Logger::getInstance().Log("Resimulate", LogLevel::INFO);
    }
    // Do physics
    float fixedTimeStep = 1.0f / 60.0f;
    mPhysicsWorld->update(fixedTimeStep);
    bool sleep = true;
    for (int i = 1; i < mBodies.size(); i++) {
      const reactphysics3d::Transform &transform = mBodies[i]->getTransform();
      const reactphysics3d::Vector3 &position = transform.getPosition();
      const reactphysics3d::Matrix3x3 &rot =
          transform.getOrientation().getMatrix();
      glm::mat4 mat = Utils::ReactMat3Vec3ToGlmMat4(rot, position);
      meshes[i - 1]->SetTransform(mat);
      if (!mBodies[i]->isSleeping()) {
        sleep = false;
      }
    }
    mSimulationFrame++;
    if (sleep || (mSimulationFrame >= mSimulatingFrames)) {
      mSimulating = false;
    }
  }
}
