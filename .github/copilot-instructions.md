# Copilot Instructions for TowerForge

## Project Overview

TowerForge is a modern open-source SimTower-inspired skyscraper simulation game built with C++20. It uses an Entity Component System (ECS) architecture for game logic and is designed with modularity in mind to support future multiplayer features.

## Tech Stack

- **Language**: C++20 (strict requirement)
- **ECS Framework**: [flecs](https://github.com/SanderMertens/flecs)
- **Rendering**: [Raylib](https://www.raylib.com/)
- **Build System**: CMake 3.20+
- **Package Manager**: vcpkg

## Architecture

The project follows a modular architecture:

- **Core**: Headless simulation library (ECS-based) - future module
- **Renderer**: 2D vector graphics using Raylib - future module
- **Simulation**: Tower management, tenants, elevators, people AI - future module

Currently, the project is in early development with basic structure in place.

## Coding Standards

### C++ Guidelines

- Use **C++20 standard** features and idioms
- Follow modern C++ best practices
- Use RAII for resource management
- Prefer standard library containers and algorithms
- Use smart pointers (`std::unique_ptr`, `std::shared_ptr`) over raw pointers
- Use `auto` where it improves readability
- Prefer `constexpr` and `const` where applicable

### Code Style

- Use meaningful variable and function names
- Keep functions small and focused
- Organize code into logical modules
- Add comments only when necessary to explain complex logic

### File Organization

- Header files should use `.h` extension
- Implementation files should use `.cpp` extension
- Headers should include proper include guards or `#pragma once`
- Place headers in `include/` directory (when structure is expanded)
- Place source files in `src/` directory

## Build System

### Prerequisites and Dependencies Setup

Before building, ensure the following are installed:

**Linux (Ubuntu/Debian):**
```bash
# Install build essentials and X11 libraries
sudo apt-get update
sudo apt-get install -y build-essential cmake pkg-config \
    libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev \
    libgl1-mesa-dev libglu1-mesa-dev xvfb

# vcpkg will automatically install flecs and raylib during first build
```

**Note:** When running in CI/CD or headless environments, `xvfb` is required for rendering screenshots.

### CMake Configuration

- Minimum CMake version: 3.20
- C++ standard is set to C++20 (required, not optional)
- vcpkg toolchain file is used for dependency management
- Output directories:
  - Runtime: `${CMAKE_BINARY_DIR}/bin`
  - Libraries: `${CMAKE_BINARY_DIR}/lib`
  - Archives: `${CMAKE_BINARY_DIR}/lib`

### Building the Project

```bash
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=/usr/local/share/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build .
```

**Note:** If vcpkg is installed locally, adjust the toolchain path:
```bash
cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
```

The first build will take longer as vcpkg downloads and compiles dependencies (flecs, raylib, glfw3).

### Dependencies

Dependencies are managed via vcpkg and defined in `vcpkg.json`:
- flecs (ECS framework)
- raylib (rendering library)

## Cross-Platform Support

The project targets multiple platforms:
- **Windows**: Visual Studio 2019+ or MinGW-w64
- **Linux**: GCC 10+ or Clang 10+
- **macOS**: Xcode 12+ or Clang 10+

When writing code, ensure compatibility across all platforms.

## Testing

Testing infrastructure is not yet established. When adding tests:
- Place test files in appropriate directories (to be determined)
- Use a standard C++ testing framework (to be selected)
- Ensure tests cover critical functionality

## Current Development Status

🚧 **Early Development** - Basic project structure in place

The project is in its initial stages. Future development will add:
- Core simulation engine
- Rendering module
- Game mechanics (tower management, tenants, elevators, people AI)
- User interface
- Multiplayer support

## When Making Changes

1. Ensure compatibility with C++20 standard
2. Follow the existing CMake structure
3. Keep changes modular and maintainable
4. Consider cross-platform compatibility
5. Update documentation when adding new features
6. Respect the ECS architecture pattern when adding game logic

---

## Consolidated Build Instructions (from docs/BUILD_INSTRUCTIONS.md)

This project includes a more detailed build instruction set in `docs/BUILD_INSTRUCTIONS.md`; below is a consolidated, quick-reference version developers should follow.

Prerequisites (summary):
- CMake 3.20+
- C++20-capable compiler: GCC 10+/Clang 10+/MSVC 2019+
- vcpkg (recommended)
- On Linux: X11 development libs and optionally `xvfb` for headless runs

Quick build steps:
1. Clone repository:

```bash
git clone https://github.com/adam4813/towerforge.git
cd towerforge
```

2. With vcpkg (recommended):

```bash
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --parallel <num_jobs>
```

On Windows with cmd.exe you can use `%NUMBER_OF_PROCESSORS%` for `<num_jobs>`.

3. Without vcpkg: ensure system has flecs and raylib installed and run `cmake ..` then build.

4. Built binaries are in `build/bin/` (or `build/bin/Release/` for some generators/configs):
- `towerforge` - Main game executable
- `screenshot_app` - Screenshot utility used for headless screenshot generation

5. Verification: a convenience script `./scripts/verify_build.sh` exists to run an automated verify-build sequence (configure, build, check artifacts).

Additional notes:
- First build may take longer because vcpkg will fetch and build dependencies (e.g., flecs, raylib, nlohmann-json)
- Known dependency versions referenced in docs: flecs 4.1.1, raylib 5.5, nlohmann-json 3.12.0

Troubleshooting highlights:
- Missing X11 / OpenGL libs: install system dev packages (see docs)
- Old CMake: upgrade to >=3.20
- Missing C++20 compiler: install/update GCC/Clang or use MSVC 2019+

Platform-specific tips:
- Windows: prefer Visual Studio 2019+ or MinGW-w64, integrate vcpkg with your toolchain
- Linux: tested on Ubuntu 20.04+/Debian 11+
- macOS: Xcode 12+ and command-line tools

Performance / build-type examples:

Release optimized build:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

Debug build:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Debug
```

---

## CI Workflow Summary (from docs/CI_WORKFLOW.md)

A GitHub Actions workflow (`.github/workflows/build.yml`) builds the project across platforms. Key points:
- Triggers: push to `main`/`develop`, PRs targeting those branches, and manual `workflow_dispatch`
- Matrix builds: Windows (MSVC), Linux (GCC), optionally Clang and macOS (some matrices are temporarily disabled)
- Uses `lukka/run-vcpkg` to install vcpkg deps from `vcpkg.json`
- Steps: checkout, install system deps (Linux), setup MSVC (Windows), configure CMake (with vcpkg toolchain), build, collect artifacts, archive, upload
- Artifacts: platform archives named `TowerForge-{os}-{compiler}` containing `towerforge` and `screenshot_app`; Windows drops required DLLs alongside executables
- Common CI helpers used: `actions/checkout@v4`, `lukka/run-vcpkg@v11`, `actions/upload-artifact@v4`

Maintenance notes for CI:
- Pin vcpkg commit in workflow for reproducibility
- Adjust matrix to add/remove compilers or build types
- Consider adding automated tests, caching, and release steps in future

---

## Design Decision: No Standalone Demo Applications (from docs/DESIGN_DECISION_NO_DEMOS.md)

Decision summary:
- The project will not maintain multiple standalone demo applications; the main `towerforge` binary is the single source of demos and feature showcase.

Rationale:
- Avoid code duplication and extra maintenance burden
- Ensure features are demonstrated in integrated, realistic conditions
- Reduce build complexity and distribution size
- Prefer documentation + screenshots over separate demo executables

Exception:
- `screenshot_app` remains as a lightweight tool for generating documentation screenshots in headless CI environments; it is not a user-facing demo.

