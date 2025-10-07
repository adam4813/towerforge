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
