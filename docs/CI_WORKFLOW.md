# GitHub Actions CI/CD Workflow Documentation

## Overview

This document describes the GitHub Actions workflow for building TowerForge on multiple platforms (Windows, Linux, macOS).

## Workflow File

Location: `.github/workflows/build.yml`

## Triggers

The workflow runs on:
- **Push** to `main` or `develop` branches
- **Pull requests** targeting `main` or `develop` branches
- **Manual trigger** via GitHub Actions UI (workflow_dispatch)

## Build Matrix

The workflow uses a matrix strategy to build on multiple platforms and compilers:

| Platform | OS | Compiler | Build Type | vcpkg Triplet |
|----------|-----|----------|------------|---------------|
| Windows | windows-latest | MSVC | Release | x64-windows |
| Linux | ubuntu-latest | GCC 10 | Release | x64-linux |
| Linux | ubuntu-latest | Clang 12 | Release | x64-linux |
| macOS | macos-latest | Clang | Release | x64-osx |

## Workflow Steps

### 1. Checkout Repository
- Uses `actions/checkout@v4`
- Includes recursive submodule checkout

### 2. Install System Dependencies (Linux Only)
Installs required packages:
- build-essential
- cmake
- pkg-config
- X11 development libraries (libx11-dev, libxrandr-dev, libxinerama-dev, libxcursor-dev, libxi-dev)
- OpenGL libraries (libgl1-mesa-dev, libglu1-mesa-dev)
- xvfb (for headless rendering)

### 3. Set up MSVC (Windows Only)
- Uses `ilammy/msvc-dev-cmd@v1` to configure Visual Studio build tools

### 4. Set Environment Variables (Linux/macOS Only)
- Sets `CC` and `CXX` environment variables for the appropriate compiler

### 5. Setup vcpkg
- Uses `lukka/run-vcpkg@v11`
- Automatically sets up vcpkg and uses `VCPKG_ROOT` environment variable
- Automatically installs dependencies from `vcpkg.json`:
  - flecs
  - raylib
  - nlohmann-json

### 6. Configure CMake
Runs CMake configuration with:
- Build type: Release
- vcpkg toolchain file (from `$VCPKG_ROOT`)
- Platform-specific vcpkg triplet

### 7. Build
- Runs `cmake --build` with 4 parallel jobs
- Builds both `towerforge` and `screenshot_app` executables

### 8. Prepare Artifacts

**Windows:**
- Copies executables from `build/bin/Release/` or `build/Release/`
- Includes necessary DLLs from vcpkg

**Linux/macOS:**
- Copies executables from `build/bin/`
- Sets executable permissions

### 9. Create Archives

**Windows:**
- Uses 7-Zip to create `.zip` archive

**Linux/macOS:**
- Uses tar to create `.tar.gz` archive

### 10. Upload Artifacts
- Uses `actions/upload-artifact@v4`
- Artifacts are retained for 30 days
- Naming convention: `TowerForge-{os}-{compiler}`

## Artifacts

After a successful build, the following artifacts are available:

- `TowerForge-windows-latest-msvc.zip`
- `TowerForge-ubuntu-latest-gcc.tar.gz`
- `TowerForge-ubuntu-latest-clang.tar.gz`
- `TowerForge-macos-latest-clang.tar.gz`

Each archive contains:
- `towerforge` executable (main game)
- `screenshot_app` executable (screenshot generator)
- Required runtime libraries (Windows only - DLLs)

## Downloading Artifacts

1. Navigate to the [Actions tab](https://github.com/adam4813/towerforge/actions/workflows/build.yml)
2. Click on a successful workflow run
3. Scroll to the **Artifacts** section at the bottom
4. Download the archive for your platform
5. Extract and run the executable

**Note:** GitHub requires login to download artifacts. Artifacts expire after 30 days.

## Dependencies

The workflow relies on:

### vcpkg Dependencies (from vcpkg.json)
- **flecs** - Entity Component System framework
- **raylib** - 2D graphics library
- **nlohmann-json** - JSON parsing library

### System Dependencies (Linux)
- X11 and related libraries for windowing
- OpenGL for rendering
- Standard build tools (GCC/Clang, CMake)

### GitHub Actions
- `actions/checkout@v4` - Repository checkout
- `ilammy/msvc-dev-cmd@v1` - MSVC setup (Windows)
- `lukka/run-vcpkg@v11` - vcpkg integration
- `actions/upload-artifact@v4` - Artifact publishing

## Troubleshooting

### Build Failures

**vcpkg dependency installation fails:**
- Check vcpkg commit ID is valid
- Verify `vcpkg.json` dependencies are available

**CMake configuration fails:**
- Verify CMakeLists.txt is valid
- Check toolchain file path is correct
- Ensure all required system dependencies are installed

**Build errors:**
- Check compiler compatibility (C++20 required)
- Review build logs for specific errors
- Verify all source files are present

**Artifact upload fails:**
- Check that executables were built successfully
- Verify artifact paths exist
- Ensure archive creation succeeded

### Platform-Specific Issues

**Windows:**
- MSVC may require specific Windows SDK versions
- DLL dependencies must be copied to artifacts

**Linux:**
- X11 libraries must be installed
- Compiler versions (GCC 10+, Clang 12+) must support C++20

**macOS:**
- Xcode command-line tools required
- May need additional framework linkage

## Maintenance

### Updating Dependencies

To update vcpkg dependencies:
1. Modify `vcpkg.json` in the repository
2. Optionally update `vcpkgGitCommitId` in the workflow for newer vcpkg version

### Updating Compilers

To change compiler versions:
1. Update the `cc` and `cxx` matrix values
2. Ensure the new compiler is available on the runner OS

### Adding Build Configurations

To add Debug builds or other configurations:
1. Add new matrix entries with different `build_type` values
2. Adjust artifact naming to distinguish between configurations

## Best Practices

1. **Pin vcpkg commits** - Ensures reproducible builds
2. **Use matrix strategy** - Build on all target platforms
3. **Retain artifacts** - Set appropriate retention period
4. **Fail-fast: false** - Continue building other platforms if one fails
5. **Clear artifact naming** - Include OS and compiler in names
6. **Document requirements** - Keep README updated with download instructions

## CI Badge

The workflow includes a CI badge in README.md:

```markdown
[![Build Status](https://github.com/adam4813/towerforge/workflows/Build%20TowerForge%20(Windows,%20Linux,%20macOS)/badge.svg)](https://github.com/adam4813/towerforge/actions/workflows/build.yml)
```

This shows the current build status and links to the workflow runs.

## Future Enhancements

Potential improvements:
- Add automated testing step
- Generate release notes automatically
- Deploy to GitHub Releases on tags
- Add code coverage reporting
- Implement caching for faster builds
- Add build time measurements
- Cross-compile for additional architectures (ARM, etc.)
