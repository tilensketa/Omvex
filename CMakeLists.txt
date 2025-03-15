cmake_minimum_required(VERSION 3.10)

project(Omvex)

# Use FetchContent to download and include GLM
include(FetchContent)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0.9.9.8
    GIT_SHALLOW true
)

FetchContent_GetProperties(glm)
if(NOT glm_POPULATED)
    FetchContent_Populate(glm)
    add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})
endif()

# FetchContent to download nlohmann/json
include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2 # Replace with the latest version or desired version
)
FetchContent_MakeAvailable(nlohmann_json)

# Use FetchContent to download and include GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.4
)

FetchContent_GetProperties(glfw)
if(NOT glfw_POPULATED)
    FetchContent_Populate(glfw)
    add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})
endif()

# Use FetchContent to download and include GLAD
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v0.1.34
)

FetchContent_GetProperties(glad)
if(NOT glad_POPULATED)
    FetchContent_Populate(glad)
    add_subdirectory(${glad_SOURCE_DIR} ${glad_BINARY_DIR})
endif()

FetchContent_Declare(
    reactphysics3d
    GIT_REPOSITORY https://github.com/DanielChappuis/reactphysics3d.git
    GIT_TAG v0.10.2
)
FetchContent_MakeAvailable(reactphysics3d)

# Use FetchContent to download and include Assimp
FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG v5.0.1
)

FetchContent_GetProperties(assimp)
if(NOT assimp_POPULATED)
  FetchContent_Populate(assimp)
  add_subdirectory(${assimp_SOURCE_DIR} ${assimp_BINARY_DIR})
endif()


function(download_files base_url target_dir file_list)
  foreach(file IN LISTS ${file_list})  # Dereference the list variable
    # message(STATUS "Processing file: ${file}")
    if(NOT EXISTS "${target_dir}/${file}")
      # Print the source and destination paths
      message(STATUS "Downloading: ${base_url}/${file}")
      # message(STATUS "Saving to: ${target_dir}/${file}")
      
      # Attempt to download the file
      file(DOWNLOAD "${base_url}/${file}" "${target_dir}/${file}" STATUS status)
      list(GET status 0 result_code)
      if(NOT ${result_code} EQUAL 0)
        message(FATAL_ERROR "Failed to download ${file}")
      endif()
    else()
      message(STATUS "File already exists: ${target_dir}/${file}")
    endif()
  endforeach()
endfunction()

set(IMGUI_BASE_URL "https://raw.githubusercontent.com/ocornut/imgui/refs/heads/docking")
set(IMGUI_TARGET_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui")

set(IMGUI_FILES
  "imconfig.h"
  "imgui.cpp"
  "imgui.h"
  "imgui_draw.cpp"
  "imgui_internal.h"
  "imgui_tables.cpp"
  "imgui_widgets.cpp"
  "imstb_rectpack.h"
  "imstb_textedit.h"
  "imstb_truetype.h"
  "backends/imgui_impl_glfw.h"
  "backends/imgui_impl_glfw.cpp"
  "backends/imgui_impl_opengl3.h"
  "backends/imgui_impl_opengl3.cpp"
  "backends/imgui_impl_opengl3_loader.h"
  "imgui_demo.cpp"
)

download_files("${IMGUI_BASE_URL}" "${IMGUI_TARGET_DIR}" IMGUI_FILES)

set(IMGUI_SOURCE_FILES
    "${IMGUI_TARGET_DIR}/imgui.cpp"
    "${IMGUI_TARGET_DIR}/imgui_draw.cpp"
    "${IMGUI_TARGET_DIR}/imgui_tables.cpp"
    "${IMGUI_TARGET_DIR}/imgui_widgets.cpp"
    "${IMGUI_TARGET_DIR}/backends/imgui_impl_glfw.cpp"
    "${IMGUI_TARGET_DIR}/backends/imgui_impl_opengl3.cpp"
    "${IMGUI_TARGET_DIR}/imgui_demo.cpp"
)


set(STB_BASE_URL "https://raw.githubusercontent.com/nothings/stb/refs/heads/master")
set(STB_TARGET_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/stb")

set(STB_FILES
  "stb_image.h"
  "stb_image_write.h"
)

download_files("${STB_BASE_URL}" "${STB_TARGET_DIR}" STB_FILES)

set(PFD_BASE_URL "https://raw.githubusercontent.com/samhocevar/portable-file-dialogs/refs/heads/main")
set(PFD_TARGET_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/pfd")
set(PFD_FILES
  "portable-file-dialogs.h"
)
download_files("${PFD_BASE_URL}" "${PFD_TARGET_DIR}" PFD_FILES)

# Find necessary packages
find_package(OpenGL REQUIRED)
set(OpenGL_GL_PREFERENCE "GLVND")  # Specify GLVND preference

# Find OpenCV package
find_package(OpenCV REQUIRED)

# Set source files
set(SOURCES
    src/main.cpp
    src/Application.cpp
    src/Texture.cpp
    src/CameraCalibrator.cpp
    src/CameraParameters.cpp
    src/Viewport.cpp
    src/Camera.cpp
    src/VAO.cpp
    src/VBO.cpp
    src/EBO.cpp
    src/Shader.cpp
    src/Mesh.cpp
    src/FBO.cpp
    src/Quad.cpp
    src/CameraManager.cpp
    src/PhysicsManager.cpp
    src/RenderManager.cpp
    src/ModelManager.cpp
    src/Model.cpp
    # Add other source files here if any
)

# Add executable
add_executable(Omvex ${SOURCES} ${IMGUI_SOURCE_FILES})

# Include directories for OpenGL, GLFW, GLM, GLAD, Assimp, Imgui and stb
target_include_directories(Omvex PRIVATE
    ${OPENGL_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
    ${glm_SOURCE_DIR}
    ${glad_SOURCE_DIR}/include
    ${opencv_SOURCE_DIR}/include
    ${reactphysics3d_SOURCE_DIR}/include
    ${assimp_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/stb
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/pfd
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link against OpenGL, GLFW, GLM, GLAD ,Assimp, Imgui and stb libraries
target_link_libraries(Omvex PRIVATE
    ${OPENGL_LIBRARIES}
    glfw
    ${glm_LIBRARY}
    glad
    assimp::assimp
    ${OpenCV_LIBS}
    nlohmann_json::nlohmann_json
    reactphysics3d
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
