# TowerForge Build Instructions

This document provides detailed instructions for building TowerForge from source.

## Prerequisites

### Required Software

- **CMake** 3.20 or higher
- **C++20 compatible compiler**:
  - Linux: GCC 10+ or Clang 10+
  - Windows: Visual Studio 2019+ or MinGW-w64
  - macOS: Xcode 12+ or Clang 10+
- **vcpkg** (recommended for dependency management)

### Linux Dependencies

On Ubuntu/Debian systems, install the following packages:

```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    libx11-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev
```

For headless environments (CI/CD), also install:

```bash
sudo apt-get install -y xvfb
```

## Building the Project

### Step 1: Clone the Repository

```bash
git clone https://github.com/adam4813/towerforge.git
cd towerforge
```

### Step 2: Configure CMake

**With vcpkg (recommended):**

```bash
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=/usr/local/share/vcpkg/scripts/buildsystems/vcpkg.cmake
```

**Without vcpkg:**

Ensure you have flecs and raylib installed on your system, then:

```bash
mkdir build
cd build
cmake ..
```

### Step 3: Build

```bash
cmake --build . --parallel $(nproc)
```

On Windows, use:

```bash
cmake --build . --parallel %NUMBER_OF_PROCESSORS%
```

### Step 4: Verify Build

The built binaries will be located in `build/bin/`:

- `towerforge` - Main game executable
- `screenshot_app` - Screenshot utility

You can run the build verification script:

```bash
./scripts/verify_build.sh
```

## Automated Build Verification

For convenience, a build verification script is provided that performs all steps:

```bash
./scripts/verify_build.sh
```

This script will:
1. Check for required dependencies
2. Set up the build directory
3. Configure with CMake
4. Build the project
5. Verify the binaries were created successfully

## Dependencies

TowerForge uses the following libraries (managed by vcpkg):

- **flecs** 4.1.1 - Entity Component System framework
- **raylib** 5.5 - Rendering library
- **nlohmann-json** 3.12.0 - JSON parsing

The first build will take longer as vcpkg downloads and compiles dependencies.

## Common Build Issues

### Issue: X11 libraries not found

**Solution:** Install the required X11 development packages:

```bash
sudo apt-get install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```

### Issue: OpenGL libraries not found

**Solution:** Install Mesa development packages:

```bash
sudo apt-get install libgl1-mesa-dev libglu1-mesa-dev
```

### Issue: CMake version too old

**Solution:** Install a newer version of CMake:

```bash
# Ubuntu 20.04+
sudo apt-get install cmake

# Or download from https://cmake.org/download/
```

### Issue: C++20 compiler not available

**Solution:** Update your compiler:

```bash
# Ubuntu
sudo apt-get install g++-10
export CXX=g++-10
```

## Platform-Specific Notes

### Linux

The project has been tested on Ubuntu 20.04+ and Debian 11+.

### Windows

When building on Windows:
- Use Visual Studio 2019 or later
- Or use MinGW-w64 with GCC 10+
- vcpkg integration is highly recommended

### macOS

When building on macOS:
- Use Xcode 12 or later
- Ensure command-line tools are installed: `xcode-select --install`

## CI/CD Integration

For continuous integration, ensure the build verification script passes:

```bash
# In your CI pipeline
./scripts/verify_build.sh
```

The script will exit with code 0 on success, non-zero on failure.

## Troubleshooting

If you encounter build issues:

1. Clean the build directory: `rm -rf build`
2. Clear vcpkg cache: `rm -rf build/vcpkg_installed`
3. Reconfigure: Follow steps from "Building the Project"
4. Check that all dependencies are installed
5. Verify you have a C++20 compatible compiler

For additional help, please file an issue at: https://github.com/adam4813/towerforge/issues

## Performance Optimization

For release builds with optimizations:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=/usr/local/share/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

## Development Builds

For debug builds with symbols:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=/usr/local/share/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Debug
```
