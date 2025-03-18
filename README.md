# Omvex

![omvex_logo](resources/OMVEX_logo.png)

## Overview
Omvex is a C++ application designed to generate synthetic images using 3D models and custom backgrounds. It utilizes multiple libraries for graphics rendering, physics simulation, and file handling.

## Features
- Loads and renders 3D models
- Supports OpenGL for high-performance rendering
- Integrates physics-based simulations
- Uses OpenCV to extract camera matrix

## Dependencies
This project relies on the following libraries:

### Graphics & Rendering
- **[OpenGL](https://www.opengl.org/)** - Graphics rendering API
- **[GLFW](https://github.com/glfw/glfw)** - Window management and input handling
- **[GLM](https://github.com/g-truc/glm)** - Mathematics library for graphics programming
- **[GLAD](https://github.com/Dav1dde/glad)** - OpenGL function loader
- **[Assimp](https://github.com/assimp/assimp)** - Asset Import Library for handling 3D model files

### Physics
- **[ReactPhysics3D](https://github.com/DanielChappuis/reactphysics3d)** - Physics engine for simulating rigid body dynamics

### Image Processing
- **[OpenCV](https://opencv.org/)** - Computer vision and image processing library
- **[stb_image](https://github.com/nothings/stb)** - Single-file public domain image loader

### UI
- **[ImGui](https://github.com/ocornut/imgui)** - Immediate-mode GUI for C++

### JSON Handling
- **[nlohmann/json](https://github.com/nlohmann/json)** - JSON library for modern C++

### File Dialogs
- **[Portable File Dialogs](https://github.com/samhocevar/portable-file-dialogs)** - Cross-platform file dialog library

## Installation & Setup
### Prerequisites
Ensure you have the following installed:
- CMake (3.10 or higher)
- A C++ compiler with C++17 support
- OpenGL drivers
- OpenCV system wide
- Python

### OpenCV Installation

#### Linux (Ubuntu/Debian)
Users can install OpenCV using their package manager:  
```sh
sudo apt update && sudo apt install libopencv-dev
```

#### Windows
Download Precompiled Binaries
1. Download OpenCV for **[Windows](https://opencv.org/releases/)**.
2. Extract the files (e.g., to C:\opencv).
3. Set environment variables:
    - Add C:\opencv\build\x64\vc16\bin to your PATH.


## Building the Project
1. Clone the repository:
   ```sh
   git clone https://github.com/tilensketa/Omvex.git
   cd Omvex
   ```
2. Run `configure_build.py` to build and follow steps:
   ```sh
   python configure_build.py
   ```
3. Run the application:
    - Linux (Ubuntu/Debian)
   ```sh
   cd build
   ./Omvex
   ```
    - Windows
   ```sh
   cd build/Release
   Omvex.exe
   ```

## Usage
- Configure camera settings
- Load 3D models and textures
- Render synthetic images

## Screenshots

### Application Preview
Camera calibration (dark, light theme)
<p align="center">
  <img src="resources/screenshots/camera_calibration_dark_18032025.png" width="49%">
  <img src="resources/screenshots/camera_calibration_light_18032025.png" width="49%">
</p>
Viewport (shaded, segmented)
<p align="center">
  <img src="resources/screenshots/viewport_shaded_18032025.png" width="49%">
  <img src="resources/screenshots/viewport_segmented_18032025.png" width="49%">
</p>

## Author
Tilen Šketa
