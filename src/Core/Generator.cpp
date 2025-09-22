#include "Core/Generator.h"

#include "Utilities/FileSystem.h"

#define SUBFOLDER_COLOR "color/"
#define SUBFOLDER_SEGMENTATION "segmentation/"
#define SUBFOLDER_POSE_DATA "poses/"

Generator::Generator(CameraManager *camMng, ModelManager *modelMng,
                     PhysicsManager *phyMng, ViewMode *viewMode)
    : mCameraManager(camMng), mModelManager(modelMng), mPhysicsManager(phyMng),
      mViewMode(viewMode) {}

void Generator::Start(const std::string &outputFolder) {
  if (outputFolder.empty()) return;

  mStartTime = std::chrono::high_resolution_clock::now();
  mOutputFolder = outputFolder + "/";

  mRenderId = 0;
  mRenderSubId = 0;
  mRunning = true;

  FileSystem::CreateDir(mOutputFolder + SUBFOLDER_COLOR);
  FileSystem::CreateDir(mOutputFolder + SUBFOLDER_SEGMENTATION);
  FileSystem::CreateDir(mOutputFolder + SUBFOLDER_POSE_DATA);

  std::vector<glm::vec3> colors = mModelManager->GetSegmentedColors();
  Texture uniqueColorsTexture = Texture(colors);
  uniqueColorsTexture.Save(mOutputFolder + "unique_colors.png");

  std::ofstream file(mOutputFolder + "model_names.txt");
  if (file.is_open()) {
    for (const std::string &modelName : mModelManager->GetModelNames()) {
      file << modelName << std::endl;
    }
    file.close();
  } else {
    Logger::Error("Failed to open modle names file");
  }
  Logger::Info("Generator started");
}

void Generator::Stop() {
  Logger::Info("Generator stopped");
  mRunning = false;
  mRenderId = 0;
  mRenderSubId = 0;
  auto endTime = std::chrono::high_resolution_clock::now();
  auto duration = endTime - mStartTime;
  auto totalSeconds =
      std::chrono::duration_cast<std::chrono::seconds>(duration).count();

  int hours = static_cast<int>(totalSeconds / 3600);
  int minutes = static_cast<int>((totalSeconds % 3600) / 60);
  int seconds = static_cast<int>(totalSeconds % 60);

  std::stringstream time;
  time << std::setfill('0') << std::setw(2) << hours << ":" << std::setfill('0')
       << std::setw(2) << minutes << ":" << std::setfill('0') << std::setw(2)
       << seconds;

  Logger::Success("Completed in: " + time.str());
}

void Generator::Update() {
  if (!mRunning || !mCameraManager || !mViewMode) return;

  saveImage();

  int numViewModes = static_cast<int>(ViewMode::Count);
  mRenderSubId++;
  if (mRenderSubId == numViewModes) {
    saveTransforms();
    mRenderId++;
    mCameraManager->SwitchNext();
    mRenderSubId = 0;
  }

  int viewModeInt = static_cast<int>(*mViewMode);
  viewModeInt++;
  viewModeInt %= numViewModes;
  *mViewMode = static_cast<ViewMode>(viewModeInt);

  if (mRenderId >= mNumRenders) {
    Stop();
    return;
  }
}

std::string Generator::getFileName() const {
  std::ostringstream oss;
  oss << std::setfill('0') << std::setw(5) << mRenderId;
  return oss.str();
}
std::string Generator::getSubfolder() const {
  switch (mRenderSubId) {
  case 0: return SUBFOLDER_COLOR;
  case 1: return SUBFOLDER_SEGMENTATION;
  case 2: return SUBFOLDER_POSE_DATA;
  default: return SUBFOLDER_COLOR;
  }
}

void Generator::saveImage() {
  std::string path = mOutputFolder + getSubfolder() + getFileName() + ".png";
  FBO *fbo = mCameraManager->GetFBO();
  if (fbo && fbo->ColorTexture) {
    fbo->ColorTexture->Save(path);
  }
  Logger::Info("Image saved: " + path);
}
void Generator::saveTransforms() {
  std::string path =
      mOutputFolder + SUBFOLDER_POSE_DATA + getFileName() + ".json";
  std::ofstream outFile(path);
  json j;
  int id = 0;
  for (const auto &body : mPhysicsManager->GetBodies()) {
    const auto &transform = body->getTransform();
    const auto &pos = transform.getPosition();
    const auto &quat = transform.getOrientation();

    j["bodies"][id]["position"] = {pos.x, pos.y, pos.z};
    j["bodies"][id]["quaternion"] = {quat.x, quat.y, quat.z, quat.w};
    id++;
  }
  CameraParameters *params = mCameraManager->GetCamera()->GetParameters();
  Serialize::ToJson::Vec(j["camera"]["tvec"], params->Tvec);
  Serialize::ToJson::Vec(j["camera"]["rvec"], params->Rvec);
  Serialize::ToJson::Mat(j["camera"]["intrinsics"], params->Intrinsic);
  Serialize::ToJson::Vec(j["camera"]["distortion"], params->Distortion);
  Serialize::ToJson::Vec(j["camera"]["calibrated_size"],
                         params->ImageCalibratedSize);
  outFile << j.dump(4);
}
