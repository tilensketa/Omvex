cmake_minimum_required(VERSION 3.11)

project(Omvex)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_EXTENSIONS OFF)

add_definitions(-DNOMINMAX)

include(FetchContent)

function(FetchAndAddSubdirectory name url tag)
    # Include the FetchContent module if not already included
    include(FetchContent)
    
    # Declare the content to be fetched
    FetchContent_Declare(
        ${name}
        GIT_REPOSITORY ${url}
        GIT_TAG ${tag}
    )

    # Check if the content has already been populated
    FetchContent_GetProperties(${name})
    if(NOT ${name}_POPULATED)
        # Fetch and populate the content if not already done
        FetchContent_Populate(${name})

        # Process optional arguments as key-value pairs
        list(LENGTH ARGN arg_length)

        # Ensure even number of arguments (key-value pairs)
        if(NOT arg_length EQUAL 0 AND NOT arg_length EQUAL 2)
            math(EXPR remainder "${arg_length} % 2")
            if(NOT remainder EQUAL 0)
                message(FATAL_ERROR "Error: FetchAndAddSubdirectory received an uneven number of arguments! Make sure all options have values.")
            endif()
        endif()

        # Loop through arguments as key-value pairs
        set(index 0)
        while(index LESS arg_length)
            list(GET ARGN ${index} option_name)
            math(EXPR index "${index} + 1")
            list(GET ARGN ${index} option_value)
            math(EXPR index "${index} + 1")

            message(STATUS "Setting ${option_name} to ${option_value}")
            set(${option_name} ${option_value} CACHE BOOL "" FORCE)
        endwhile()

        # Add the content's directory as a subdirectory for the build process
        add_subdirectory(${${name}_SOURCE_DIR} ${${name}_BINARY_DIR})
    endif()
endfunction()

# Find OpenCV package
set(OpenCV_DIR "dependencies/opencv/build")
find_package(OpenCV REQUIRED)

# WARN
if(WIN32)
    set(PLATFORM_LIBS ws2_32)
elseif(UNIX)
    find_package(X11 REQUIRED)
    set(PLATFORM_LIBS ${X11_LIBRARIES} pthread dl)
endif()

FetchAndAddSubdirectory(glfw https://github.com/glfw/glfw.git latest)
FetchAndAddSubdirectory(glad https://github.com/Dav1dde/glad.git v0.1.34)
FetchAndAddSubdirectory(glm https://github.com/g-truc/glm.git 1.0.1)
target_compile_definitions(glm INTERFACE GLM_ENABLE_EXPERIMENTAL)
FetchAndAddSubdirectory(reactphysics3d https://github.com/DanielChappuis/reactphysics3d.git v0.10.2)

FetchAndAddSubdirectory(assimp https://github.com/assimp/assimp.git v5.4.3
ASSIMP_BUILD_ASSIMP_TOOLS OFF
ASSIMP_BUILD_TESTS OFF
ASSIMP_INSTALL OFF
ASSIMP_NO_EXPORT ON
ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF
ASSIMP_BUILD_OBJ_IMPORTER ON
)
FetchAndAddSubdirectory(nlohmann_json https://github.com/nlohmann/json.git v3.11.2)

FetchContent_Declare(imgui GIT_REPOSITORY https://github.com/ocornut/imgui.git GIT_TAG docking)
FetchContent_MakeAvailable(imgui)
add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})
target_link_libraries(imgui PRIVATE glfw OpenGL::GL ${PLATFORM_LIBS})

FetchContent_Declare(stb GIT_REPOSITORY https://github.com/nothings/stb.git GIT_TAG master)
FetchContent_MakeAvailable(stb)
add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE ${stb_SOURCE_DIR})

FetchContent_Declare(pfd GIT_REPOSITORY https://github.com/samhocevar/portable-file-dialogs.git GIT_TAG main)
FetchContent_MakeAvailable(pfd)
add_library(pfd INTERFACE)
target_include_directories(pfd INTERFACE ${pfd_SOURCE_DIR})
target_compile_features(pfd INTERFACE cxx_std_17)

find_package(OpenGL REQUIRED)

set(SOURCES
    src/main.cpp

    src/Core/Logger.cpp
    src/Core/Application.cpp
    src/Core/CameraCalibrator.cpp
    src/Core/Viewport.cpp
    src/Core/Generator.cpp
    src/Core/Context.cpp

    src/Core/Loaders/TutorialLoader.cpp
    src/Core/Loaders/ExampleLoader.cpp

    src/Core/Camera/Camera.cpp
    src/Core/Camera/CameraMath.cpp
    src/Core/Camera/CameraParameters.cpp

    src/Managers/CameraParametersManager.cpp
    src/Managers/ModelManager.cpp
    src/Managers/CameraManager.cpp
    src/Managers/PhysicsManager.cpp

    src/Rendering/Textures/Texture.cpp
    src/Rendering/Shaders/Shader.cpp
    src/Rendering/Shaders/Renderer.cpp
    src/Rendering/Models/Model.cpp
    src/Rendering/Models/Mesh.cpp
    src/Rendering/Models/Quad.cpp
    src/Rendering/Buffers/FBO.cpp

    src/Utilities/FileSystem.cpp
    src/Utilities/GlmToString.cpp
    src/Utilities/GlmToCv.cpp
    src/Utilities/CvToGlm.cpp
    src/Utilities/ImGuiHelpers.cpp

    # Add other source files here if any
)

add_executable(Omvex ${SOURCES})

target_link_libraries(Omvex PRIVATE
    ${OpenCV_LIBS}
    glad
    glfw
    OpenGL::GL
    glm::glm
    nlohmann_json::nlohmann_json
    reactphysics3d
    assimp::assimp
    imgui
    stb_image
    pfd
    ${PLATFORM_LIBS}
)

target_include_directories(Omvex PRIVATE
    ${glad_SOURCE_DIR}/include
    ${glm_SOURCE_DIR}
    ${reactphysics3d_SOURCE_DIR}/include
    ${assimp_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/opencv/include
)
