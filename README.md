# TowerForge

A modular tower defense game engine built with C++ and CMake.

## Requirements

- CMake 3.20 or higher
- C++20 compatible compiler (GCC 10+, Clang 10+, MSVC 2019+)
- vcpkg (for dependency management)

## Building the Project

### 1. Install vcpkg

First, clone and bootstrap vcpkg in the project directory:

```bash
# Clone vcpkg
git clone https://github.com/microsoft/vcpkg.git

# Bootstrap vcpkg (Linux/macOS)
./vcpkg/bootstrap-vcpkg.sh

# Bootstrap vcpkg (Windows)
.\vcpkg\bootstrap-vcpkg.bat
```

### 2. Configure and Build

#### Linux/macOS

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake

# Build
cmake --build .

# Run the executable
./bin/towerforge
```

#### Windows

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_TOOLCHAIN_FILE=..\vcpkg\scripts\buildsystems\vcpkg.cmake

# Build
cmake --build .

# Run the executable
.\bin\Debug\towerforge.exe
```

## Project Structure

```
towerforge/
├── CMakeLists.txt          # Root CMake configuration
├── vcpkg.json              # vcpkg dependency manifest
├── README.md               # This file
├── .gitignore              # Git ignore rules
├── src/                    # Source files
│   └── main.cpp            # Application entry point
├── include/                # Public header files
│   └── towerforge/         # Project headers
└── third_party/            # Third-party dependencies
```

## Adding Dependencies

To add a new dependency via vcpkg, edit `vcpkg.json` and add it to the `dependencies` array:

```json
{
  "dependencies": [
    "sdl2",
    "glm"
  ]
}
```

Then reconfigure CMake and the dependencies will be automatically installed.

## Future Modules

The project is structured to support future expansion with these planned modules:

- **Core** - Game logic and engine foundation
- **Rendering** - Graphics and visualization
- **Simulation** - Physics and game mechanics
- **Audio** - Sound and music systems
- **UI** - User interface components

## Cross-Platform Support

This project is designed to build on:
- **Windows** (MSVC, MinGW)
- **Linux** (GCC, Clang)
- **macOS** (Clang)

## License

TBD
