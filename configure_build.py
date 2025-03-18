import os
import subprocess
import shutil

def get_build_type():
    print("\nSelect build type (default: Release):")
    print("1. Debug")
    print("2. Release")
    choice = input("Enter choice (1/2): ").strip()
    build_type = "Release"
    if choice == "1": build_type = "Debug"
    elif choice == "2": build_type = "Release"
    else:
        print("Incorrect build type selected. Defaulting to Release")
        build_type = "Release"
    print(f"Build type: {build_type}")
    return build_type

def get_opencv_path():
    opencv_path = input("\nEnter the full path to the OpenCV directory (default: C:/opencv/build): ").strip()
    if opencv_path == "":
         opencv_path = "C:/opencv/build"
    print(f"OpenCV path: {opencv_path}")
    return opencv_path

def get_compile_commands():
    print("\nSelect if generate compile_commands.json (default: Yes)")
    print("1. Yes")
    print("2. No")
    choice = input("Enter choice (1/2): ").strip()
    compile_commands = True
    if choice == "1": compile_commands = True
    elif choice == "2": compile_commands = False
    else:
        print("Incorrect compile commands selected. Defaulting to True")
        compile_commands = True
    print(f"Compile commands: {compile_commands}")
    return compile_commands

def run_cmake(build_type, opencv_path, compile_commands):
    print("\nRunning CMake to configure and build the project...")
    build_dir = "build"
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)
    os.chdir(build_dir)
    
    configure_cmd = ["cmake", "..", f'-DOpenCV_DIR={opencv_path}', f"-DCMAKE_BUILD_TYPE={build_type}"]
    if compile_commands:
        configure_cmd.append("-DCMAKE_EXPORT_COMPILE_COMMANDS=ON")
    configure_cmd.append("-Wno-dev")
    
    build_cmd = ["cmake", "--build", ".", "--config", build_type]
    
    print("CONFIGURE: ", " ".join(configure_cmd))
    print("BUILD: ", " ".join(build_cmd))
    print("\n")

    # Run CMake configure command and output in real-time
    configure_result = subprocess.run(configure_cmd)
    if configure_result.returncode != 0:
        print("Error: CMake configuration failed!")
        return False

    # Run CMake build command and output in real-time
    build_result = subprocess.run(build_cmd)
    if build_result.returncode != 0:
        print("Error: CMake build failed!")
        return False

    if compile_commands:
        shutil.copy("compile_commands.json", "../compile_commands.json")
    
    print("CMake configuration and build succeeded!")
    return True

def main():
    print("Build Configuration")

    # Example functions to get these values
    build_type = get_build_type()
    opencv_path = get_opencv_path()
    compile_commands = get_compile_commands()

    result = run_cmake(build_type, opencv_path, compile_commands)
    if result:
        print("Build process complete!")
    else:
        print("Build failed!")

if __name__ == "__main__":
    main()
