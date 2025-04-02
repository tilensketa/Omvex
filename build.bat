@echo off
setlocal enabledelayedexpansion

REM Set absolute paths
set "SCRIPT_DIR=%~dp0"
cd /d "%SCRIPT_DIR%"

set "BUILD_DIR=%SCRIPT_DIR%build"
set "DEPS_DIR=%SCRIPT_DIR%dependencies"
set OPENCV_REPO=https://github.com/opencv/opencv.git
set "OPENCV_DIR=%DEPS_DIR%\opencv"
set "OPENCV_BUILD_DIR=%OPENCV_DIR%\build"

REM Create directories
mkdir "%DEPS_DIR%" 2>nul
mkdir "%BUILD_DIR%" 2>nul

if not exist "%OPENCV_DIR%" (
    echo Cloning OpenCV...
    git clone %OPENCV_REPO% "%OPENCV_DIR%" || (
        echo Failed to clone OpenCV!
        exit /b 1
    )

    echo Building OpenCV...
    cd "%OPENCV_DIR%"
    mkdir build
    cd build

    cmake .. -DBUILD_LIST=calib3d,core,features2d,flann,imgproc ^
        -DCMAKE_BUILD_TYPE=Release ^
        -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF ^
        -DBUILD_EXAMPLES=OFF -DBUILD_opencv_python=OFF || (
        echo CMake failed!
        exit /b 1
    )

    cmake --build . --config Release || (
        echo Build failed!
        exit /b 1
    )

    REM Copy binaries
    xcopy /e /i /h "%OPENCV_BUILD_DIR%\bin\Release\*" "%BUILD_DIR%\Release\"
    echo OpenCV binaries copied to %BUILD_DIR%\Release
    cd "%SCRIPT_DIR%"
) else (
    echo OpenCV already exists. Skipping build.
)

REM Build Omvex
echo Building Omvex...
cd %BUILD_DIR%
cmake .. -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 (
    echo Project CMake configuration failed!
    cd ..
    exit /b 1
)

cmake --build . --config Release
if errorlevel 1 (
    echo Project build failed!
    cd ..
    exit /b 1
)

echo Build completed successfully!
cd ..
