#!/bin/bash
# Complete build script with dependency setup for Linux
set -e  # Exit immediately if a command exits with a non-zero status

# Configuration
BUILD_DIR="build"
SOURCE_DIR=".."
DEPS_DIR="dependencies"
OPENCV_REPO="https://github.com/opencv/opencv.git"
OPENCV_DIR="$DEPS_DIR/opencv"
OPENCV_BUILD_DIR="$OPENCV_DIR/build"

# Create directories structure
echo "Setting up directories..."
mkdir -p "$DEPS_DIR"
mkdir -p "$BUILD_DIR"

# Clone and build OpenCV
echo "Setting up OpenCV dependencies..."
if [ ! -d "$OPENCV_DIR" ]; then
    echo "Cloning OpenCV repository..."
    cd "$DEPS_DIR"
    git clone "$OPENCV_REPO"
    if [ $? -ne 0 ]; then
        echo "Failed to clone OpenCV!"
        exit 1
    fi
    
    echo "Building OpenCV..."
    cd "opencv"
    mkdir -p build
    cd build
    cmake .. -DBUILD_LIST=calib3d,core,features2d,flann,imgproc -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_opencv_python=OFF
    if [ $? -ne 0 ]; then
        echo "OpenCV CMake configuration failed!"
        exit 1
    fi
    
    cmake --build . --config Release -- -j$(nproc)
    if [ $? -ne 0 ]; then
        echo "OpenCV build failed!"
        exit 1
    fi
    
    cp -r "lib"/* "../../../$BUILD_DIR/"
    echo "OpenCV files copied from $OPENCV_BUILD_DIR/lib to $BUILD_DIR"

    cd ../../..
else
    echo "OpenCV already exists, skipping clone and build..."
fi

# Build Omvex
echo "Building Omvex..."
cd "$BUILD_DIR"
cmake "$SOURCE_DIR" -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
if [ $? -ne 0 ]; then
    echo "Project CMake configuration failed!"
    cd ..
    exit 1
fi

cmake --build . --config Release -- -j$(nproc)
if [ $? -ne 0 ]; then
    echo "Project build failed!"
    cd ..
    exit 1
fi

cp  compile_commands.json "$SOURCE_DIR"/
echo "Compile commands copied"

echo "Build completed successfully!"
cd ..
