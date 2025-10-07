# TowerForge

A modern open-source SimTower-inspired skyscraper simulation game built with C++20.

## Tech Stack

- **Language**: C++20
- **ECS**: [flecs](https://github.com/SanderMertens/flecs)
- **Rendering**: [Raylib](https://www.raylib.com/)
- **Build System**: CMake 3.20+
- **Package Manager**: vcpkg

## Building

### Prerequisites

- CMake 3.20 or higher
- C++20 compatible compiler (GCC 10+, Clang 10+, MSVC 2019+)
- vcpkg (clone next to this repo or set VCPKG_ROOT)

**Linux Additional Requirements:**
- X11 development libraries: `libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev`
  ```bash
  # On Ubuntu/Debian:
  sudo apt-get install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev
  ```

### Build Steps

```bash
# Clone the repository
git clone https://github.com/adam4813/towerforge.git
cd towerforge

# Clone vcpkg if you haven't already
git clone https://github.com/microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.sh  # or bootstrap-vcpkg.bat on Windows

# Build the project
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build .

# Run the application
./bin/towerforge
```

**Note:** The first build will take longer as vcpkg downloads and builds dependencies (Raylib, GLFW3, flecs, etc.).

### Cross-Platform Support

- **Windows**: Visual Studio 2019+ or MinGW-w64
- **Linux**: GCC 10+ or Clang 10+
- **macOS**: Xcode 12+ or Clang 10+

## Architecture

TowerForge is designed with a modular architecture to support future multiplayer features:

- **Core**: Headless simulation library (ECS-based)
  - Built on [flecs ECS](https://github.com/SanderMertens/flecs) for high-performance entity management
  - Modular component-based architecture for easy extension
  - Example components: Actor (people), BuildingComponent (facilities)
  - Example systems: Movement, Actor logging, Building occupancy monitoring
- **Renderer**: 2D vector graphics using Raylib
  - Modular rendering system designed for easy ECS integration
  - Supports GUI, actors, building visuals, and more
  - Hardware-accelerated OpenGL rendering
- **Simulation**: Tower management, tenants, elevators, people AI

### Rendering System

The rendering system is built on [Raylib](https://www.raylib.com/), a simple and easy-to-use library for 2D graphics:

- **Modular Design**: The `Renderer` class in `include/rendering/renderer.h` provides a clean interface
- **ECS Ready**: Designed to integrate seamlessly with the flecs ECS for entity rendering
- **Extensible**: Easy to add new rendering features (sprites, text, particles, etc.)

Current demo renders:
- A blue rectangle (representing a building floor)
- A red circle (representing a person or elevator)

### ECS Structure

The Entity Component System (ECS) is the foundation of TowerForge's simulation. It provides:

**Components** (`include/core/components.hpp`):
- `Position`: 2D position in space
- `Velocity`: Movement velocity
- `Actor`: Represents people in the building (name, destination floor, speed)
- `BuildingComponent`: Represents building facilities (offices, restaurants, shops, etc.)

**Systems** (`src/core/ecs_world.cpp`):
- **Movement System**: Updates entity positions based on velocity
- **Actor Logging System**: Monitors and logs actor activity
- **Building Occupancy Monitor**: Tracks occupancy of building components

**ECS World** (`include/core/ecs_world.hpp`):
- Wrapper around flecs world for clean API
- Manages component and system registration
- Provides entity creation and simulation update methods

### Using the ECS

```cpp
#include "core/ecs_world.hpp"
#include "core/components.hpp"

// Create and initialize ECS world
TowerForge::Core::ECSWorld world;
world.Initialize();

// Create an actor entity
auto actor = world.CreateEntity("John");
actor.set<Position>({10.0f, 0.0f});
actor.set<Velocity>({0.5f, 0.0f});
actor.set<Actor>({"John", 5, 1.0f});

// Create a building component
auto office = world.CreateEntity("Office");
office.set<BuildingComponent>({
    BuildingComponent::Type::Office,
    5,   // floor
    8,   // width
    20   // capacity
});

// Run simulation
float delta_time = 1.0f / 60.0f;
world.Update(delta_time);
```

## Current Status

🚧 **Early Development** - Basic ECS integration complete

### What's Working
- ✅ Flecs ECS integrated and operational
- ✅ Core module with example components (Actor, BuildingComponent, Position, Velocity)
- ✅ Example systems (Movement, Actor logging, Building occupancy)
- ✅ Demo application showing ECS in action
- ✅ Basic project structure
- ✅ Raylib integration with 2D vector rendering
- ✅ Modular renderer design for ECS integration
- ✅ Working demo window with test shapes

### Running the Demo

After building, run the demo to see the ECS simulation:

```bash
cd build
./bin/towerforge
```

The demo creates example actors and building components, then runs a 30-second simulation showing:
- Actors moving with velocity
- Periodic logging of actor positions
- Building occupancy monitoring

See [Issues](https://github.com/adam4813/towerforge/issues) for the development roadmap.

## License

TBD
