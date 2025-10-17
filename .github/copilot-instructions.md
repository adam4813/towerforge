# Copilot Instructions for TowerForge

## Minimum Requirements (must be satisfied before any build steps)

You must have these installed and validated before configuring or building the project:

- CMake 3.20 or newer (required)
- A C++20-capable compiler:
  - Windows: Visual Studio 2019+ (MSVC) or MinGW-w64 with GCC 10+
  - Linux: GCC 10+ or Clang 10+
  - macOS: Xcode 12+ / Clang 10+
- vcpkg (recommended for dependency management) — clone and bootstrap vcpkg before the first build
- On Linux (Ubuntu/Debian): X11 development libraries and OpenGL/Mesa dev packages when building locally or running renderer-based tests

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

## Modern C++ Best Practices

### Resource Management with RAII and Smart Pointers

Always use smart pointers for ownership semantics and automatic resource management:

**❌ Avoid: Manual memory management**
```cpp
// Don't do this - prone to leaks and requires manual cleanup
class Tower {
    TowerGrid* grid_;
    FacilityManager* facility_mgr_;
public:
    Tower() {
        grid_ = new TowerGrid(10, 20, 0);
        facility_mgr_ = new FacilityManager();
    }
    ~Tower() {
        delete grid_;
        delete facility_mgr_;
    }
};
```

**✅ Prefer: Smart pointers with clear ownership**
```cpp
// Good - automatic cleanup, exception-safe, clear ownership
class Tower {
    std::unique_ptr<TowerGrid> grid_;
    std::unique_ptr<FacilityManager> facility_mgr_;
public:
    Tower() 
        : grid_(std::make_unique<TowerGrid>(10, 20, 0)),
          facility_mgr_(std::make_unique<FacilityManager>()) {}
    // No destructor needed - RAII handles cleanup
};
```

### Small, Composable Functions

Build small, focused functions that do one thing well. Compose them to create complex behavior:

**❌ Avoid: Monolithic functions**
```cpp
// Don't do this - hard to test, understand, and maintain
void ProcessTowerUpdate(ECSWorld& world, float delta_time) {
    // Update time
    auto& time_mgr = world.GetWorld().get_mut<TimeManager>();
    time_mgr->current_hour += delta_time * time_mgr->speed_multiplier / 3600.0f;
    if (time_mgr->current_hour >= 24.0f) {
        time_mgr->current_hour -= 24.0f;
        time_mgr->current_day = (time_mgr->current_day + 1) % 7;
        if (time_mgr->current_day == 0) {
            time_mgr->current_week++;
        }
    }
    
    // Update satisfaction for all facilities
    world.GetWorld().each<BuildingComponent, Satisfaction>([&](auto e, auto& building, auto& sat) {
        float crowding_penalty = 0.0f;
        if (building.current_occupancy > building.capacity * 0.9f) {
            crowding_penalty = 10.0f;
        }
        sat.satisfaction_score = std::max(0.0f, sat.satisfaction_score - crowding_penalty * delta_time);
    });
    
    // Update economics
    // ... 50+ more lines of mixed concerns
}
```

**✅ Prefer: Small, composable functions**
```cpp
// Good - each function has a single responsibility
void UpdateSimulationTime(TimeManager& time_mgr, float delta_time) {
    time_mgr.current_hour += delta_time * time_mgr.speed_multiplier / 3600.0f;
    
    if (time_mgr.current_hour >= 24.0f) {
        AdvanceToNextDay(time_mgr);
    }
}

void AdvanceToNextDay(TimeManager& time_mgr) {
    time_mgr.current_hour -= 24.0f;
    time_mgr.current_day = (time_mgr.current_day + 1) % 7;
    
    if (time_mgr.current_day == 0) {
        time_mgr.current_week++;
    }
}

float CalculateCrowdingPenalty(const BuildingComponent& building) {
    if (building.current_occupancy > building.capacity * 0.9f) {
        return 10.0f;
    }
    return 0.0f;
}

void UpdateFacilitySatisfaction(BuildingComponent& building, Satisfaction& sat, float delta_time) {
    const float penalty = CalculateCrowdingPenalty(building);
    sat.satisfaction_score = std::max(0.0f, sat.satisfaction_score - penalty * delta_time);
}

// Clear, testable, maintainable
void ProcessTowerUpdate(ECSWorld& world, float delta_time) {
    auto& time_mgr = world.GetWorld().get_mut<TimeManager>();
    UpdateSimulationTime(*time_mgr, delta_time);
    
    world.GetWorld().each<BuildingComponent, Satisfaction>(
        [delta_time](auto e, auto& building, auto& sat) {
            UpdateFacilitySatisfaction(building, sat, delta_time);
        });
}
```

### Declarative Style with Standard Algorithms

Use standard algorithms and ranges to express intent clearly rather than manual loops:

**❌ Avoid: Manual loops with imperative logic**
```cpp
// Don't do this - verbose, error-prone, hard to parallelize
std::vector<flecs::entity> GetHighSatisfactionFacilities(
    const std::vector<flecs::entity>& facilities) {
    std::vector<flecs::entity> result;
    for (int i = 0; i < facilities.size(); i++) {
        const auto* sat = facilities[i].get<Satisfaction>();
        if (sat && sat->satisfaction_score > 70.0f) {
            result.push_back(facilities[i]);
        }
    }
    return result;
}

float CalculateTotalRevenue(const std::vector<flecs::entity>& facilities) {
    float total = 0.0f;
    for (size_t i = 0; i < facilities.size(); i++) {
        const auto* econ = facilities[i].get<FacilityEconomics>();
        if (econ) {
            total += econ->current_rent * econ->current_tenant_count;
        }
    }
    return total;
}
```

**✅ Prefer: Declarative style with standard algorithms**
```cpp
// Good - clear intent, less error-prone, easier to optimize
// Returns a lazy-evaluated view that filters entities with high satisfaction
auto GetHighSatisfactionFacilities(const std::vector<flecs::entity>& facilities) {
    return facilities 
        | std::views::filter([](const auto& e) {
            const auto* sat = e.get<Satisfaction>();
            return sat && sat->satisfaction_score > 70.0f;
          });
}

// Alternative: If you need a concrete vector, materialize the view
std::vector<flecs::entity> GetHighSatisfactionFacilitiesVector(
    const std::vector<flecs::entity>& facilities) {
    auto filtered = facilities 
        | std::views::filter([](const auto& e) {
            const auto* sat = e.get<Satisfaction>();
            return sat && sat->satisfaction_score > 70.0f;
          });
    return std::vector<flecs::entity>(filtered.begin(), filtered.end());
}

float CalculateTotalRevenue(const std::vector<flecs::entity>& facilities) {
    return std::ranges::fold_left(
        facilities 
        | std::views::filter([](const auto& e) { 
            return e.get<FacilityEconomics>() != nullptr; 
          })
        | std::views::transform([](const auto& e) {
            // Note: econ is guaranteed non-null due to filter above,
            // but defensive check shown for documentation purposes
            const auto* econ = e.get<FacilityEconomics>();
            return econ ? econ->current_rent * econ->current_tenant_count : 0.0f;
          }),
        0.0f,
        std::plus{}
    );
}

// Or using std::transform_reduce for cleaner parallel execution potential
float CalculateTotalRevenueParallel(const std::vector<flecs::entity>& facilities) {
    return std::transform_reduce(
        facilities.begin(), facilities.end(),
        0.0f,
        std::plus{},
        [](const auto& e) {
            if (const auto* econ = e.get<FacilityEconomics>()) {
                return econ->current_rent * econ->current_tenant_count;
            }
            return 0.0f;
        }
    );
}
```

### Additional Modern C++ Patterns

**Use `std::optional` for optional values instead of pointers or sentinel values:**
```cpp
// Good - clearly expresses that a value may not exist
std::optional<FacilityEconomics> FindFacilityEconomics(flecs::entity e) {
    if (const auto* econ = e.get<FacilityEconomics>()) {
        return *econ;
    }
    return std::nullopt;
}
```

**Use structured bindings for clearer code:**
```cpp
// Good - readable and concise
for (const auto& [entity, position, velocity] : query) {
    UpdatePosition(position, velocity);
}
```

**Use `const` and `constexpr` aggressively:**
```cpp
// Good - compile-time constants and immutable values
constexpr float MAX_SATISFACTION = 100.0f;
constexpr int DEFAULT_CAPACITY = 20;

void ProcessFacility(const BuildingComponent& building) {
    const float occupancy_ratio = static_cast<float>(building.current_occupancy) / building.capacity;
    // building cannot be modified here
}
```

### Summary: Key Principles

1. **RAII and Smart Pointers**: Use `std::unique_ptr` and `std::shared_ptr` for automatic resource management
2. **Small Functions**: Write focused, single-responsibility functions (typically 5-20 lines)
3. **Declarative Over Imperative**: Use standard algorithms and ranges to express "what" not "how"
4. **Composability**: Build complex behavior from simple, reusable building blocks
5. **Type Safety**: Use `std::optional`, `std::variant`, and strong types instead of raw pointers or magic values
6. **Const Correctness**: Mark everything `const` that doesn't mutate, use `constexpr` for compile-time values
7. **Meaningful Names**: Function and variable names should clearly express intent and purpose

### File Organization

- Header files should use `.h` extension
- Implementation files should use `.cpp` extension
- Headers should include proper include guards or `#pragma once`
- Place headers in `include/` directory (when structure is expanded)
- Place source files in `src/` directory

## Preflight checklist (always follow these steps before building)

1. Confirm CMake version (must be >= 3.20):

```bash
cmake --version
```

2. Confirm your C++ compiler supports C++20:

- MSVC (developer command prompt):

```cmd
cl /?    # should show MSVC tools; use Visual Studio 2019 or later
```

- GCC/Clang (Linux/macOS):

```bash
g++ --version
clang++ --version
```

3. Clone and bootstrap vcpkg:

```bash
git clone https://github.com/microsoft/vcpkg.git
# On Windows (cmd.exe)
vcpkg\bootstrap-vcpkg.bat
# On Linux/macOS
./vcpkg/bootstrap-vcpkg.sh
```

4. On Ubuntu/Debian, install system packages:

```bash
sudo apt-get update && sudo apt-get install -y build-essential cmake pkg-config \
  libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev \
  libgl1-mesa-dev libglu1-mesa-dev xvfb
```

5. Configure CMake with the vcpkg toolchain:

```bash
mkdir -p build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
```

6. Build (use parallel jobs appropriate to your machine):

```bash
cmake --build . --parallel %NUMBER_OF_PROCESSORS%   # Windows (cmd.exe)
# or
cmake --build . --parallel $(nproc)                # Linux/macOS
```

7. (Optional) Run the verification script:

```bash
scripts\verify_build.sh   # Or run in bash on Windows Subsystem for Linux
```

---

## Build System

### CMake Configuration

- Always use CMake 3.20 or newer.
- Always use C++20.
- Always use the vcpkg toolchain file for dependency management.

### Building the Project

```bash
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=/usr/local/share/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build .
```

If vcpkg is installed locally, adjust the toolchain path:
```bash
cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
```

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
7. When adding new features, create and add relevant screenshots to the `screenshots/` folder whenever possible.
8. Documentation should be written or updated in the form of a user manual, focusing on usage and gameplay, not as an implementation summary.

---

## Consolidated Build Instructions

- Use CMake 3.20+ and a C++20-capable compiler.
- Use vcpkg for dependencies.
- On Linux, install all required system libraries before building.
- Always follow the preflight checklist above before building.

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

5. (Optional) Run `./scripts/verify_build.sh` to verify the build.

---

## No Standalone Demo Applications

- The main `towerforge` binary is the only demo and feature showcase.
- Temporary applications may be created during development but must not be committed.
- `screenshot_app` is the only exception, used for documentation screenshots in CI.
