# TowerForge Technical Design Document

**Version**: 1.0  
**Last Updated**: 2025-10-25  
**Status**: Living Document

---

## Table of Contents

1. [Executive Summary](#1-executive-summary)
2. [Architecture Overview](#2-architecture-overview)
3. [Core Systems](#3-core-systems)
4. [Rendering System](#4-rendering-system)
5. [Data Management](#5-data-management)
6. [Modding System](#6-modding-system)
7. [Build System](#7-build-system)
8. [Testing Strategy](#8-testing-strategy)
9. [Performance & Optimization](#9-performance--optimization)
10. [Deployment](#10-deployment)
11. [Future Architecture](#11-future-architecture)

---

## 1. Executive Summary

### 1.1 Technical Vision

TowerForge is built on a **modern C++20 foundation** leveraging the **Entity Component System (ECS)** pattern for maximum performance, maintainability, and extensibility. The architecture prioritizes:

- **Modularity**: Clean separation between simulation, rendering, and UI
- **Performance**: ECS enables data-oriented design for cache efficiency
- **Extensibility**: Lua modding API allows content creation without C++ knowledge
- **Cross-Platform**: CMake + vcpkg ensures consistent builds across Windows, Linux, macOS
- **Testability**: Comprehensive test suite (integration, E2E, unit tests)

### 1.2 Technology Stack

| Layer | Technology | Justification |
|-------|-----------|---------------|
| **Language** | C++20 | Modern features (ranges, concepts), performance, industry standard |
| **ECS Framework** | [flecs](https://github.com/SanderMertens/flecs) | High-performance, mature, excellent API |
| **Rendering** | [Raylib](https://www.raylib.com/) | Simple 2D API, hardware-accelerated, cross-platform |
| **Scripting** | Lua 5.4 | Lightweight, fast, easy to embed, sandboxed |
| **Build System** | CMake 3.20+ | Industry standard, excellent IDE support |
| **Package Manager** | vcpkg | Reproducible dependencies, cross-platform |
| **Testing** | Google Test (gtest) | Comprehensive, well-documented |
| **Version Control** | Git + GitHub | Standard, GitHub Actions for CI/CD |

### 1.3 Architectural Principles

1. **Composition Over Inheritance**: Build complexity from simple, focused components
2. **Data-Oriented Design**: Leverage ECS for cache-friendly memory layouts
3. **Separation of Concerns**: Simulation logic decoupled from rendering and UI
4. **Declarative UI**: Build UI trees once, update reactively on events
5. **Gang of Four Patterns**: Use proven patterns (Composite, Observer, Strategy, etc.) where appropriate
6. **Modern C++ Best Practices**: RAII, smart pointers, const correctness, ranges

---

## 2. Architecture Overview

### 2.1 High-Level Architecture

```
┌─────────────────────────────────────────────────────────┐
│                     Application Layer                    │
│  (Main loop, input handling, state management)          │
└────────────────┬─────────────────────────────┬──────────┘
                 │                             │
       ┌─────────▼─────────┐         ┌────────▼──────────┐
       │   Rendering Layer  │         │    UI Layer       │
       │   (Raylib-based)   │         │ (Reactive widgets)│
       └─────────┬──────────┘         └────────┬──────────┘
                 │                             │
       ┌─────────▼─────────────────────────────▼──────────┐
       │              Core Simulation Layer                │
       │  (ECS World, Systems, Components)                 │
       │  - TowerGrid, FacilityManager                     │
       │  - Person AI, Elevator simulation                 │
       │  - Economic systems, Time management              │
       └─────────┬─────────────────────────────────────────┘
                 │
       ┌─────────▼──────────┐         ┌───────────────────┐
       │   Modding Layer     │         │   Data Layer      │
       │   (Lua scripting)   │         │ (JSON, Save/Load) │
       └─────────────────────┘         └───────────────────┘
```

### 2.2 Module Boundaries

**Core Module** (`libcore`):
- Headless simulation library
- ECS world, components, systems
- No rendering or UI dependencies
- Can run in server/batch mode

**Rendering Module** (`librendering`):
- Raylib abstraction layer
- Batch rendering primitives
- Camera system
- Asset management

**UI Module** (`libui`):
- Widget hierarchy (buttons, panels, sliders, etc.)
- Event system (mouse, keyboard)
- Layout managers
- Accessibility support

**Application** (`towerforge` executable):
- Main game loop
- Integrates core + rendering + UI
- Input handling
- State machine (main menu, gameplay, pause, etc.)

**Modding** (embedded in core):
- Lua interpreter and sandboxing
- Bindings for facility/visitor registration
- Hot-loading of mods

### 2.3 Design Patterns in Use

| Pattern | Usage | Implementation |
|---------|-------|----------------|
| **Composite** | UI element hierarchy | `UIElement` with children |
| **Observer** | Event callbacks | `std::function` callbacks on UI elements |
| **Strategy** | Layout algorithms | `ILayoutStrategy` interface |
| **Factory** | Entity/component creation | `ECSWorld::CreateEntity()`, `FacilityManager` |
| **Singleton** | Global managers | `TimeManager`, `TowerEconomy`, `AccessibilitySettings` |
| **Command** | Undo/redo | `ICommand` interface for reversible actions |
| **State Machine** | Entity behaviors | `PersonState`, `ElevatorState` enums with transition logic |
| **Template Method** | Rendering pipeline | `UIElement::Render()` with virtual hooks |

**Reference**: `UI_DEVELOPMENT_BIBLE.md` for detailed pattern usage

---

## 3. Core Systems

### 3.1 Entity Component System (ECS)

#### 3.1.1 Architecture

TowerForge uses **flecs** for ECS. flecs provides:
- Fast iteration (cache-friendly memory layout)
- Query system for component filtering
- Relationship support (parent/child entities)
- Excellent debugging tools

**Key Abstractions**:
- **Entity**: Unique ID (integer) representing a game object
- **Component**: Data-only struct attached to entities
- **System**: Logic that operates on entities with specific components

#### 3.1.2 Component Types

**Defined in** `include/core/components.hpp`:

**Spatial Components**:
```cpp
struct Position {
    float x, y;
};

struct Velocity {
    float vx, vy;
};

struct GridPosition {
    int floor, column, width;
};
```

**Simulation Components**:
```cpp
struct Person {
    std::string name;
    PersonState state;
    int current_floor;
    float current_column;
    int destination_floor;
    float destination_column;
    float move_speed;
    float wait_time;
    std::string current_need;
    
    // Methods...
};

struct BuildingComponent {
    enum class Type { Lobby, Office, Residential, ... };
    Type type;
    int floor, column, width, capacity;
    int current_occupancy, current_staff, job_openings;
    float operating_start_hour, operating_end_hour;
    
    // Methods...
};

struct ElevatorCar {
    int shaft_entity_id;
    float current_floor;
    int target_floor;
    ElevatorState state;
    int max_capacity, current_occupancy;
    std::vector<int> stop_queue;
    std::vector<int> passenger_destinations;
    float state_timer;
    // Timing constants...
    
    // Methods...
};
```

**Economic Components**:
```cpp
struct FacilityEconomics {
    float base_rent_per_tenant;
    float daily_operating_cost;
    int max_tenants;
    int current_tenant_count;
    float quality_multiplier;  // Based on satisfaction
};

struct TowerEconomy {  // Singleton
    float total_balance;
    float daily_revenue;
    float daily_expenses;
    float hourly_revenue;
};
```

**Temporal Components**:
```cpp
struct TimeManager {  // Singleton
    float simulation_speed;
    float current_hour;
    int current_day;
    int current_week;
    
    // Methods...
};

struct DailySchedule {
    std::vector<ScheduledAction> weekday_actions;
    std::vector<ScheduledAction> weekend_actions;
    
    // Methods...
};
```

**Satisfaction Components**:
```cpp
struct Satisfaction {
    float satisfaction_score;  // 0-100%
    
    // Computed properties
    std::string GetLevelString() const;
    Color GetColor() const;
};
```

#### 3.1.3 System Types

**Systems** (`src/core/ecs_world.cpp`):

**Per-Frame Systems** (`OnUpdate`):
- `TimeSimulationSystem`: Advances in-game time
- `ScheduleExecutionSystem`: Triggers scheduled actions at specific times
- `MovementSystem`: Updates positions based on velocity
- `PersonHorizontalMovementSystem`: Handles walking on same floor
- `PersonWaitingSystem`: Creates elevator requests for floor changes
- `PersonElevatorBoardingSystem`: Handles boarding/exiting elevators
- `ElevatorCarMovementSystem`: Moves elevator cars, manages states
- `ElevatorCallSystem`: Assigns people to elevators

**Interval Systems** (every N seconds):
- `SatisfactionUpdateSystem`: Recalculates satisfaction based on factors
- `SatisfactionReportingSystem`: Logs satisfaction levels (every 30s)
- `FacilityEconomicsSystem`: Adjusts tenant counts based on satisfaction (every 60s)
- `DailyEconomyProcessingSystem`: Processes revenue/expenses once per day
- `RevenueCollectionSystem`: Collects revenue from facilities (every hour)
- `EconomicStatusReportingSystem`: Logs economic status (every 60s)
- `PersonStateLoggingSystem`: Debug logging (every 10s)
- `ElevatorLoggingSystem`: Debug logging (every 10s)

**System Registration**:
```cpp
void ECSWorld::Initialize() {
    // Register per-frame systems
    world_.system<Person, Position>("PersonHorizontalMovementSystem")
        .each([](flecs::entity e, Person& p, Position& pos) {
            // Movement logic...
        });
    
    // Register interval systems
    world_.system<Satisfaction>("SatisfactionReportingSystem")
        .interval(30.0f)
        .each([](flecs::entity e, const Satisfaction& sat) {
            // Reporting logic...
        });
}
```

#### 3.1.4 ECS Best Practices

**Component Design**:
- Keep components as plain data (POD when possible)
- Methods on components are allowed for convenience (e.g., `IsOperational()`)
- No inter-component dependencies (systems handle relationships)

**System Design**:
- Single Responsibility: Each system does one thing well
- No global state (except singletons like `TimeManager`)
- Systems communicate via components, not direct function calls

**Query Optimization**:
- Use specific queries (filter by component types) to reduce iteration
- Avoid unnecessary component fetches in tight loops

**Reference**: `include/core/ecs_world.hpp`, `src/core/ecs_world.cpp`

---

### 3.2 Tower Grid System

**Purpose**: Spatial representation of the tower's grid.

**Implementation**: `include/core/tower_grid.hpp`, `src/core/tower_grid.cpp`

```cpp
class TowerGrid {
public:
    TowerGrid(int initial_columns = 30, int initial_floors = 10);
    
    // Dynamic expansion
    void AddFloors(int count);
    void AddBasementFloors(int count);
    void AddColumns(int count);
    
    // Building state
    void BuildFloorCell(int floor, int column);
    bool IsBuilt(int floor, int column) const;
    
    // Facility management
    bool PlaceFacility(int floor, int column, int width, int entity_id);
    void RemoveFacility(int entity_id);
    bool IsOccupied(int floor, int column) const;
    int GetFacilityAt(int floor, int column) const;
    
    // Spatial queries
    bool IsSpaceAvailable(int floor, int column, int width) const;
    std::vector<GridPosition> GetAdjacentPositions(int floor, int column) const;
    
    // Getters
    int GetWidth() const;
    int GetHeight() const;
    
private:
    struct Cell {
        int facility_id = -1;  // -1 = empty
        bool built = false;
    };
    
    std::vector<std::vector<Cell>> grid_;  // [floor][column]
    int width_;
    int height_;
    int basement_count_;
};
```

**Key Features**:
- Dynamic floor/column expansion (no fixed limits)
- Building state tracking (allocated vs. built cells)
- Collision detection for facility placement
- Adjacency queries for satisfaction calculations

**Cost Model**:
- Adding floor: $50 per cell × width
- Building cell: Automatic when placing facility

**Reference**: `docs/DYNAMIC_FLOOR_SYSTEM.md`

---

### 3.3 Facility Manager

**Purpose**: High-level API for creating and managing facilities.

**Implementation**: `include/core/facility_manager.hpp`, `src/core/facility_manager.cpp`

```cpp
class FacilityManager {
public:
    explicit FacilityManager(ECSWorld& ecs_world, TowerGrid& tower_grid);
    
    // Creation with defaults
    flecs::entity CreateFacility(
        BuildingComponent::Type type,
        int floor,
        int column,
        int width = 0,  // 0 = use default
        const std::string& name = ""
    );
    
    // Removal
    void RemoveFacility(flecs::entity facility);
    void RemoveFacilityAt(int floor, int column);
    
    // Queries
    std::vector<flecs::entity> GetFacilitiesOnFloor(int floor) const;
    std::vector<flecs::entity> GetFacilitiesByType(BuildingComponent::Type type) const;
    
    // Facility type information
    int GetDefaultWidth(BuildingComponent::Type type) const;
    int GetDefaultCapacity(BuildingComponent::Type type) const;
    Color GetFacilityColor(BuildingComponent::Type type) const;
    
private:
    ECSWorld& ecs_world_;
    TowerGrid& tower_grid_;
    
    // Default attribute tables
    std::unordered_map<BuildingComponent::Type, int> default_widths_;
    std::unordered_map<BuildingComponent::Type, int> default_capacities_;
    std::unordered_map<BuildingComponent::Type, Color> facility_colors_;
};
```

**Default Attributes** (examples):
```cpp
// Width
{BuildingComponent::Type::Lobby, 10},
{BuildingComponent::Type::Office, 8},
{BuildingComponent::Type::Residential, 6},
{BuildingComponent::Type::RetailShop, 4},

// Capacity
{BuildingComponent::Type::Lobby, 50},
{BuildingComponent::Type::Office, 20},
{BuildingComponent::Type::Residential, 4},

// Colors
{BuildingComponent::Type::Lobby, GOLD},
{BuildingComponent::Type::Office, SKYBLUE},
{BuildingComponent::Type::Residential, PURPLE},
{BuildingComponent::Type::RetailShop, GREEN},
```

**Reference**: `docs/FACILITIES.md`

---

### 3.4 Person AI System

**State Machine Implementation**:

```cpp
enum class PersonState {
    Idle,
    Walking,
    WaitingForElevator,
    InElevator,
    AtDestination
};

struct Person {
    // ... (fields shown in 3.1.2)
    
    void SetDestination(int floor, float column, const std::string& need) {
        destination_floor = floor;
        destination_column = column;
        current_need = need;
        
        // Determine initial state
        if (floor != current_floor) {
            state = PersonState::WaitingForElevator;
        } else if (std::abs(column - current_column) > 0.5f) {
            state = PersonState::Walking;
        } else {
            state = PersonState::AtDestination;
        }
    }
    
    // State queries
    bool IsMoving() const;
    bool NeedsElevator() const;
    std::string GetStateString() const;
};
```

**System Logic** (simplified):
```cpp
// PersonHorizontalMovementSystem
void UpdateWalking(Person& person, Position& pos, float dt) {
    if (person.state != PersonState::Walking) return;
    
    float dx = person.destination_column - person.current_column;
    if (std::abs(dx) < 0.1f) {
        person.current_column = person.destination_column;
        person.state = PersonState::AtDestination;
        return;
    }
    
    float move_dir = (dx > 0) ? 1.0f : -1.0f;
    float move_amount = person.move_speed * dt;
    person.current_column += move_dir * move_amount;
    pos.x = person.current_column * CELL_WIDTH;
}

// PersonWaitingSystem
void CreateElevatorRequest(flecs::entity person_entity, Person& person) {
    if (person.state != PersonState::WaitingForElevator) return;
    if (person_entity.has<PersonElevatorRequest>()) return;  // Already has request
    
    // Find suitable elevator shaft
    auto shafts = FindElevatorShafts(person.current_floor, person.destination_floor);
    if (shafts.empty()) {
        // Fallback: simulate elevator trip
        person.wait_time += dt;
        if (person.wait_time > 10.0f) {
            person.current_floor = person.destination_floor;
            person.state = PersonState::Walking;
        }
        return;
    }
    
    // Create request
    person_entity.set<PersonElevatorRequest>({
        shafts[0].id,
        -1,  // car not yet assigned
        person.current_floor,
        person.destination_floor,
        0.0f,  // wait time
        false  // not boarding
    });
}
```

**Reference**: `docs/PERSONS.md`

---

### 3.5 Elevator System

**Architecture**:

```cpp
struct ElevatorShaft {
    int column;
    int bottom_floor;
    int top_floor;
    int car_count;
    
    int GetFloorRange() const;
    bool ServesFloor(int floor) const;
};

struct ElevatorCar {
    int shaft_entity_id;
    float current_floor;  // Float for smooth interpolation
    int target_floor;
    ElevatorState state;
    
    int max_capacity;
    int current_occupancy;
    
    std::vector<int> stop_queue;
    std::vector<int> passenger_destinations;
    
    // State transition timing
    float state_timer;
    float door_open_duration = 2.0f;
    float door_transition_duration = 1.0f;
    float floors_per_second = 2.0f;
    
    // Methods
    bool IsAtFloor() const;
    int GetCurrentFloorInt() const;
    bool HasCapacity() const;
    void AddStop(int floor);
    int GetNextStop() const;
    void RemoveCurrentStop();
    std::string GetStateString() const;
};

enum class ElevatorState {
    Idle,
    MovingUp,
    MovingDown,
    DoorsOpening,
    DoorsOpen,
    DoorsClosing
};
```

**State Machine Logic** (ElevatorCarMovementSystem):
```cpp
void UpdateElevatorCar(ElevatorCar& car, float dt) {
    switch (car.state) {
        case ElevatorState::Idle:
            if (!car.stop_queue.empty()) {
                car.target_floor = car.GetNextStop();
                car.state = (car.target_floor > car.GetCurrentFloorInt())
                    ? ElevatorState::MovingUp
                    : ElevatorState::MovingDown;
            }
            break;
            
        case ElevatorState::MovingUp:
        case ElevatorState::MovingDown:
            // Interpolate position
            float direction = (car.state == ElevatorState::MovingUp) ? 1.0f : -1.0f;
            car.current_floor += direction * car.floors_per_second * dt;
            
            // Arrived at target?
            if (car.IsAtFloor()) {
                car.current_floor = static_cast<float>(car.target_floor);
                car.state = ElevatorState::DoorsOpening;
                car.state_timer = 0.0f;
            }
            break;
            
        case ElevatorState::DoorsOpening:
            car.state_timer += dt;
            if (car.state_timer >= car.door_transition_duration) {
                car.RemoveCurrentStop();
                car.state = ElevatorState::DoorsOpen;
                car.state_timer = 0.0f;
            }
            break;
            
        case ElevatorState::DoorsOpen:
            car.state_timer += dt;
            if (car.state_timer >= car.door_open_duration) {
                car.state = ElevatorState::DoorsClosing;
                car.state_timer = 0.0f;
            }
            break;
            
        case ElevatorState::DoorsClosing:
            car.state_timer += dt;
            if (car.state_timer >= car.door_transition_duration) {
                car.state = ElevatorState::Idle;
            }
            break;
    }
}
```

**Assignment Algorithm** (ElevatorCallSystem):
```cpp
// Current: First-available
void AssignPersonToElevator(flecs::entity person_entity, PersonElevatorRequest& request) {
    auto cars = GetCarsForShaft(request.shaft_entity_id);
    
    for (auto& car_entity : cars) {
        auto car = car_entity.get<ElevatorCar>();
        if (car->HasCapacity()) {
            request.car_entity_id = car_entity.id();
            car->AddStop(request.call_floor);
            car->AddStop(request.destination_floor);
            break;
        }
    }
}

// Future: Direction-aware (SCAN/LOOK)
// - Assign to car moving in correct direction
// - Minimize total wait time
```

**Reference**: `docs/ELEVATOR.md`

---

### 3.6 Economic System

**Components**:

```cpp
// Global singleton
struct TowerEconomy {
    float total_balance = 25000.0f;  // Starting funds
    float daily_revenue = 0.0f;
    float daily_expenses = 0.0f;
    float hourly_revenue = 0.0f;
    
    void AddRevenue(float amount) { daily_revenue += amount; }
    void AddExpense(float amount) { daily_expenses += amount; }
    void ProcessDailyFinances();
};

// Per-facility component
struct FacilityEconomics {
    float base_rent_per_tenant = 150.0f;
    float daily_operating_cost = 30.0f;
    int max_tenants = 20;
    int current_tenant_count = 0;
    float quality_multiplier = 1.0f;
    
    float GetDailyRevenue() const {
        return base_rent_per_tenant * current_tenant_count * quality_multiplier;
    }
    
    float GetDailyExpense() const {
        return daily_operating_cost;
    }
};
```

**Systems**:

```cpp
// FacilityEconomicsSystem (every 60 seconds)
// Adjusts tenant counts based on satisfaction
void UpdateTenantCounts(FacilityEconomics& econ, const Satisfaction& sat) {
    if (sat.satisfaction_score > 70.0f) {
        // Attract new tenants (up to max)
        if (econ.current_tenant_count < econ.max_tenants) {
            econ.current_tenant_count++;
        }
    } else if (sat.satisfaction_score < 30.0f) {
        // Lose tenants
        if (econ.current_tenant_count > 0) {
            econ.current_tenant_count--;
        }
    }
    
    // Update quality multiplier
    if (sat.satisfaction_score > 80.0f) {
        econ.quality_multiplier = 2.0f;
    } else if (sat.satisfaction_score > 70.0f) {
        econ.quality_multiplier = 1.2f;
    } else if (sat.satisfaction_score < 30.0f) {
        econ.quality_multiplier = 0.5f;
    } else {
        econ.quality_multiplier = 1.0f;
    }
}

// RevenueCollectionSystem (every in-game hour)
void CollectRevenue() {
    auto* tower_economy = ecs_world.get_mut<TowerEconomy>();
    
    float hourly_revenue = 0.0f;
    
    // Iterate all facilities with economics
    ecs_world.each<FacilityEconomics>([&](flecs::entity e, const FacilityEconomics& econ) {
        float daily_revenue = econ.GetDailyRevenue();
        hourly_revenue += daily_revenue / 24.0f;
    });
    
    tower_economy->hourly_revenue = hourly_revenue;
    tower_economy->AddRevenue(hourly_revenue);
}

// DailyEconomyProcessingSystem (once per in-game day)
void ProcessDailyFinances() {
    auto* tower_economy = ecs_world.get_mut<TowerEconomy>();
    
    // Collect expenses
    ecs_world.each<FacilityEconomics>([&](flecs::entity e, const FacilityEconomics& econ) {
        tower_economy->AddExpense(econ.GetDailyExpense());
    });
    
    // Update balance
    tower_economy->ProcessDailyFinances();
}
```

**Reference**: `docs/IMPLEMENTATION.md`

---

### 3.7 Save/Load System

**Architecture**:

```cpp
class SaveLoadManager {
public:
    static constexpr int MAX_SAVE_SLOTS = 20;
    
    // Save operations
    bool SaveGame(ECSWorld& world, const std::string& slot_name);
    bool QuickSave(ECSWorld& world);
    bool AutoSave(ECSWorld& world);
    
    // Load operations
    bool LoadGame(ECSWorld& world, const std::string& slot_name);
    bool QuickLoad(ECSWorld& world);
    
    // Slot management
    std::vector<SaveSlotInfo> GetSaveSlots() const;
    bool DeleteSave(const std::string& slot_name);
    
private:
    std::string GetSaveDirectory() const;
    std::string GetSaveFilePath(const std::string& slot_name) const;
    
    nlohmann::json SerializeWorld(const ECSWorld& world) const;
    void DeserializeWorld(ECSWorld& world, const nlohmann::json& data);
};

struct SaveSlotInfo {
    std::string slot_name;
    std::string timestamp;
    int tower_height;
    int population;
    float funds;
};
```

**JSON Format** (simplified):
```json
{
  "version": "1.0",
  "timestamp": "2025-10-25T12:34:56Z",
  "tower": {
    "width": 30,
    "height": 20,
    "basement_count": 2,
    "grid": [ /* 2D array of cell states */ ]
  },
  "economy": {
    "balance": 125000.50,
    "daily_revenue": 5000.0,
    "daily_expenses": 2000.0
  },
  "time": {
    "current_hour": 14.5,
    "current_day": 15,
    "simulation_speed": 60.0
  },
  "entities": [
    {
      "id": 1,
      "name": "Lobby",
      "components": {
        "BuildingComponent": { /* ... */ },
        "GridPosition": { /* ... */ },
        "FacilityEconomics": { /* ... */ }
      }
    },
    {
      "id": 2,
      "name": "Alice",
      "components": {
        "Person": { /* ... */ },
        "Position": { /* ... */ },
        "Satisfaction": { /* ... */ }
      }
    }
  ],
  "research": {
    "points": 1500,
    "unlocked_nodes": ["efficient_elevators", "quick_construction"]
  },
  "achievements": {
    "first_facility": { "unlocked": true, "date": "2025-10-20" },
    "reach_10_floors": { "unlocked": false }
  }
}
```

**Serialization Strategy**:
- Each entity serialized with its components
- Component data serialized to JSON via custom `to_json()` functions
- Entity IDs preserved across save/load for relationship integrity
- Validation on load (version check, schema validation)

**Reference**: `docs/SAVE_LOAD.md`

---

## 4. Rendering System

### 4.1 Architecture

**Purpose**: Provide 2D vector graphics rendering abstraction over Raylib.

**Structure**:
```
librendering/
├── include/rendering/
│   ├── renderer.h          # Core rendering interface
│   ├── camera.h            # 2D camera with pan/zoom
│   ├── batch_renderer.h    # Batch rendering for performance
│   └── color_palette.h     # Consistent color definitions
└── src/rendering/
    ├── renderer.cpp
    ├── camera.cpp
    └── batch_renderer.cpp
```

### 4.2 Renderer Interface

```cpp
class Renderer {
public:
    Renderer(int screen_width, int screen_height);
    ~Renderer();
    
    // Frame management
    void BeginFrame();
    void EndFrame();
    
    // Primitives
    void DrawRectangle(Rectangle rect, Color color);
    void DrawRectangleOutline(Rectangle rect, Color color, float thickness = 1.0f);
    void DrawCircle(Vector2 center, float radius, Color color);
    void DrawLine(Vector2 start, Vector2 end, Color color, float thickness = 1.0f);
    void DrawText(const std::string& text, Vector2 pos, int font_size, Color color);
    
    // Advanced
    void DrawTextCentered(const std::string& text, Rectangle bounds, int font_size, Color color);
    void DrawTextWrapped(const std::string& text, Rectangle bounds, int font_size, Color color);
    void DrawProgressBar(Rectangle bounds, float progress, Color fg_color, Color bg_color);
    
    // Camera
    void SetCamera(const Camera2D& camera);
    Camera2D& GetCamera();
    
    // Utilities
    Vector2 GetTextSize(const std::string& text, int font_size) const;
    bool IsKeyPressed(int key) const;
    bool IsMouseButtonPressed(int button) const;
    Vector2 GetMousePosition() const;
    
private:
    Camera2D camera_;
    int screen_width_;
    int screen_height_;
};
```

### 4.3 Batch Rendering

**Purpose**: Reduce draw calls by batching similar primitives.

```cpp
class BatchRenderer {
public:
    // Collect rectangles
    void AddRectangle(Rectangle rect, Color color);
    void AddRectangleOutline(Rectangle rect, Color color, float thickness);
    
    // Collect circles
    void AddCircle(Vector2 center, float radius, Color color);
    
    // Flush to GPU
    void Flush();
    
    // Auto-flush when batch full
    static constexpr int MAX_BATCH_SIZE = 1000;
    
private:
    struct RectData { Rectangle rect; Color color; };
    struct CircleData { Vector2 center; float radius; Color color; };
    
    std::vector<RectData> rectangles_;
    std::vector<CircleData> circles_;
};
```

**Usage**:
```cpp
void RenderTower() {
    BatchRenderer batch;
    
    // Collect all facility rectangles
    for (auto& facility : facilities) {
        batch.AddRectangle(facility.GetBounds(), facility.GetColor());
    }
    
    // Collect all person circles
    for (auto& person : people) {
        batch.AddCircle(person.GetPosition(), person.GetRadius(), person.GetColor());
    }
    
    // Draw everything in one GPU call
    batch.Flush();
}
```

**Performance**: ~60x improvement over per-entity draw calls for 500+ entities.

**Reference**: `docs/batch_renderer/`

---

### 4.4 Camera System

```cpp
class Camera2D {
public:
    Camera2D(int screen_width, int screen_height);
    
    // Pan
    void Pan(Vector2 delta);
    void SetPosition(Vector2 pos);
    
    // Zoom
    void Zoom(float factor);  // e.g., 1.1 = zoom in 10%
    void SetZoom(float zoom);  // 1.0 = default, 2.0 = 2x zoom
    
    // Bounds
    void SetBounds(Rectangle bounds);  // Constrain camera
    void ResetBounds();
    
    // Queries
    Vector2 ScreenToWorld(Vector2 screen_pos) const;
    Vector2 WorldToScreen(Vector2 world_pos) const;
    Rectangle GetVisibleArea() const;
    
    // Raylib integration
    Camera2D ToRaylibCamera() const;
    
private:
    Vector2 position_;
    float zoom_;
    Rectangle bounds_;
    int screen_width_;
    int screen_height_;
};
```

**Usage**:
```cpp
// Pan with middle mouse drag
if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
    Vector2 delta = GetMouseDelta();
    camera.Pan(delta);
}

// Zoom with scroll wheel
float scroll = GetMouseWheelMove();
if (scroll != 0.0f) {
    camera.Zoom(1.0f + scroll * 0.1f);
}

// Apply to renderer
renderer.SetCamera(camera.ToRaylibCamera());
```

**Reference**: `docs/CAMERA_SYSTEM.md`

---

## 5. Data Management

### 5.1 User Preferences

**Purpose**: Persist user settings across sessions.

**Implementation**: `include/core/user_preferences.hpp`

```cpp
class UserPreferences {
public:
    static UserPreferences& GetInstance();
    
    // Audio settings
    void SetMasterVolume(float volume);  // 0.0 - 1.0
    float GetMasterVolume() const;
    
    void SetMusicVolume(float volume);
    float GetMusicVolume() const;
    
    void SetMuteAll(bool muted);
    bool IsMuteAll() const;
    
    // UI settings
    void SetUIScale(float scale);  // 0.5 - 2.0
    float GetUIScale() const;
    
    void SetColorMode(ColorMode mode);  // Normal, HighContrast
    ColorMode GetColorMode() const;
    
    // Persistence
    void Save();
    void Load();
    
private:
    UserPreferences();  // Singleton
    
    std::string GetPreferencesPath() const;  // Platform-specific
    
    // Settings
    float master_volume_ = 1.0f;
    float music_volume_ = 0.8f;
    float sfx_volume_ = 1.0f;
    bool mute_all_ = false;
    float ui_scale_ = 1.0f;
    ColorMode color_mode_ = ColorMode::Normal;
};
```

**Storage Location**:
- Windows: `%APPDATA%/TowerForge/user_preferences.json`
- Linux: `~/.config/towerforge/user_preferences.json`
- macOS: `~/Library/Application Support/TowerForge/user_preferences.json`

**JSON Format**:
```json
{
  "audio": {
    "master_volume": 1.0,
    "music_volume": 0.8,
    "sfx_volume": 1.0,
    "mute_all": false
  },
  "ui": {
    "scale": 1.0,
    "color_mode": "normal"
  }
}
```

---

### 5.2 Accessibility Settings

**Purpose**: Manage accessibility features.

**Implementation**: `include/core/accessibility_settings.hpp`

```cpp
class AccessibilitySettings {
public:
    static AccessibilitySettings& GetInstance();
    
    // High-contrast mode
    void SetHighContrast(bool enabled);
    bool IsHighContrast() const;
    
    // Font scaling
    void SetFontScale(float scale);  // 0.5 - 3.0
    float GetFontScale() const;
    
    // Keyboard navigation
    void SetKeyboardNavEnabled(bool enabled);
    bool IsKeyboardNavEnabled() const;
    
    // Persistence
    void Save();
    void Load();
    
private:
    bool high_contrast_ = false;
    float font_scale_ = 1.0f;
    bool keyboard_nav_ = true;
};
```

**Integration**:
```cpp
// UI rendering with accessibility
void RenderButton() {
    const auto& settings = AccessibilitySettings::GetInstance();
    
    Color bg_color = settings.IsHighContrast() ? WHITE : GRAY;
    int font_size = static_cast<int>(20 * settings.GetFontScale());
    
    DrawRectangle(bounds, bg_color);
    DrawText(label, pos, font_size, BLACK);
}
```

**Reference**: `docs/ACCESSIBILITY.md`

---

## 6. Modding System

### 6.1 Lua Integration

**Purpose**: Allow players to add custom facilities and visitor types without C++ knowledge.

**Architecture**:

```cpp
class LuaModManager {
public:
    LuaModManager(ECSWorld& ecs_world);
    ~LuaModManager();
    
    // Mod loading
    void LoadModsFromDirectory(const std::string& mods_dir);
    void LoadMod(const std::string& mod_path);
    void UnloadMod(const std::string& mod_id);
    
    // Mod queries
    std::vector<ModInfo> GetLoadedMods() const;
    bool IsModLoaded(const std::string& mod_id) const;
    
private:
    lua_State* lua_;
    ECSWorld& ecs_world_;
    std::unordered_map<std::string, ModInfo> loaded_mods_;
    
    void SetupSandbox();
    void RegisterTowerForgeAPI();
};

struct ModInfo {
    std::string id;
    std::string name;
    std::string version;
    std::string author;
    std::string description;
};
```

### 6.2 Lua API

**Exposed Functions**:

```lua
-- TowerForge.RegisterFacilityType(table)
TowerForge.RegisterFacilityType({
    id = "gaming_lounge",
    name = "Gaming Lounge",
    default_width = 10,
    default_capacity = 25,
    required_staff = 3,
    operating_start_hour = 12.0,
    operating_end_hour = 24.0,
    base_rent = 180.0,
    operating_cost = 50.0,
    color = 0xFF8C00FF  -- RGBA
})

-- TowerForge.RegisterVisitorType(table)
TowerForge.RegisterVisitorType({
    id = "gamer",
    name = "Gamer",
    archetype = "entertainment_seeker",
    preferred_facilities = {"gaming_lounge", "arcade"},
    spawn_probability = 0.15
})

-- TowerForge.Log(...)
TowerForge.Log("Mod loaded:", ModInfo.name)
```

### 6.3 Sandboxing

**Security Measures**:
- Disable file I/O (`io` library)
- Disable system commands (`os.execute`, `os.remove`)
- Disable network access
- Whitelist safe standard library functions

```cpp
void LuaModManager::SetupSandbox() {
    // Remove dangerous functions
    lua_pushnil(lua_);
    lua_setglobal(lua_, "dofile");
    lua_pushnil(lua_);
    lua_setglobal(lua_, "loadfile");
    lua_pushnil(lua_);
    lua_setglobal(lua_, "require");
    
    // Remove io library
    lua_pushnil(lua_);
    lua_setglobal(lua_, "io");
    
    // Whitelist safe os functions
    lua_getglobal(lua_, "os");
    lua_pushnil(lua_);
    lua_setfield(lua_, -2, "execute");
    lua_pushnil(lua_);
    lua_setfield(lua_, -2, "remove");
    lua_pop(lua_, 1);
}
```

**Reference**: `docs/MODDING_API.md`

---

## 7. Build System

### 7.1 CMake Structure

**Project Layout**:
```
towerforge/
├── CMakeLists.txt              # Root CMake
├── CMakePresets.json           # Build presets
├── vcpkg.json                  # Dependency manifest
├── include/                    # Public headers
├── src/                        # Implementation
│   ├── core/
│   ├── rendering/
│   ├── ui/
│   └── main.cpp
├── tests/                      # Test suite
│   ├── integration/
│   ├── e2e/
│   └── unit/
└── build/                      # Build outputs (gitignored)
```

**Root CMakeLists.txt** (simplified):
```cmake
cmake_minimum_required(VERSION 3.20)
project(TowerForge VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# vcpkg dependencies
find_package(raylib CONFIG REQUIRED)
find_package(flecs CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Lua REQUIRED)

# Core library
add_library(libcore STATIC
    src/core/ecs_world.cpp
    src/core/tower_grid.cpp
    src/core/facility_manager.cpp
    # ...
)
target_include_directories(libcore PUBLIC include)
target_link_libraries(libcore PUBLIC flecs::flecs_static nlohmann_json::nlohmann_json ${LUA_LIBRARIES})

# Rendering library
add_library(librendering STATIC
    src/rendering/renderer.cpp
    src/rendering/camera.cpp
    # ...
)
target_link_libraries(librendering PUBLIC raylib libcore)

# UI library
add_library(libui STATIC
    src/ui/ui_element.cpp
    src/ui/button.cpp
    # ...
)
target_link_libraries(libui PUBLIC librendering libcore)

# Main executable
add_executable(towerforge src/main.cpp)
target_link_libraries(towerforge PRIVATE libcore librendering libui)

# Tests (if enabled)
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
```

### 7.2 Build Presets

**CMakePresets.json** (excerpt):
```json
{
  "version": 3,
  "configurePresets": [
    {
      "name": "cli-native",
      "description": "CLI build for agents/automation",
      "binaryDir": "${sourceDir}/build-cli/cli-native",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Debug",
        "CMAKE_TOOLCHAIN_FILE": "$env{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      }
    },
    {
      "name": "cli-test",
      "description": "CLI test build",
      "binaryDir": "${sourceDir}/build-cli/cli-test",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Debug",
        "BUILD_TESTING": "ON",
        "CMAKE_TOOLCHAIN_FILE": "$env{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      }
    }
  ],
  "buildPresets": [
    {
      "name": "cli-native-debug",
      "configurePreset": "cli-native",
      "configuration": "Debug"
    },
    {
      "name": "cli-test-debug",
      "configurePreset": "cli-test",
      "configuration": "Debug"
    }
  ],
  "testPresets": [
    {
      "name": "cli-test-debug",
      "configurePreset": "cli-test",
      "output": {"outputOnFailure": true}
    }
  ]
}
```

**Usage**:
```bash
# Configure
cmake --preset cli-native

# Build
cmake --build --preset cli-native-debug --parallel $(nproc)

# Run
./build-cli/cli-native/bin/Debug/towerforge
```

### 7.3 Dependency Management (vcpkg)

**vcpkg.json**:
```json
{
  "name": "towerforge",
  "version": "1.0.0",
  "dependencies": [
    "raylib",
    "flecs",
    "nlohmann-json",
    "lua",
    {
      "name": "gtest",
      "platform": "!android & !ios"
    }
  ]
}
```

**Installation**:
```bash
# Clone vcpkg
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg

# Bootstrap
./bootstrap-vcpkg.sh  # Linux/macOS
bootstrap-vcpkg.bat   # Windows

# Set environment
export VCPKG_ROOT=/path/to/vcpkg
```

**Benefits**:
- Reproducible builds (versions locked in manifest)
- Cross-platform (Windows, Linux, macOS)
- Binary caching (faster CI builds)

---

## 8. Testing Strategy

### 8.1 Test Structure

**Test Categories**:
1. **Integration Tests** (highest priority): Test interactions between major components
2. **End-to-End (E2E) Tests**: Test complete workflows (game init, placement, save/load)
3. **Unit Tests** (lowest priority): Test complex/unique logic only

**Directory Structure**:
```
tests/
├── integration/
│   ├── test_tower_grid_integration.cpp
│   ├── test_facility_manager_integration.cpp
│   ├── test_ecs_world_integration.cpp
│   ├── test_save_load_integration.cpp
│   ├── test_achievement_manager_integration.cpp
│   └── test_lua_mod_manager_integration.cpp
├── e2e/
│   ├── test_game_initialization_e2e.cpp
│   ├── test_facility_placement_workflow_e2e.cpp
│   └── test_save_load_workflow_e2e.cpp
└── unit/
    ├── test_user_preferences_unit.cpp
    ├── test_command_history_unit.cpp
    └── test_accessibility_settings_unit.cpp
```

### 8.2 Testing Best Practices

**Arrange-Act-Assert Pattern**:
```cpp
TEST(TowerGridIntegrationTest, PlaceFacilitySuccess) {
    // Arrange
    ECSWorld ecs_world;
    ecs_world.Initialize();
    TowerGrid grid(30, 10);
    
    // Act
    bool placed = grid.PlaceFacility(0, 5, 8, 1);
    
    // Assert
    EXPECT_TRUE(placed);
    EXPECT_EQ(grid.GetFacilityAt(0, 5), 1);
    EXPECT_TRUE(grid.IsOccupied(0, 5));
}
```

**Test Independence**:
- Each test should set up its own fixtures
- No shared mutable state between tests
- Tests can run in any order

**Naming Convention**:
```
Test[ComponentName][TestType], [TestName]
                      ^^^^^^^^
                      integration | e2e | unit

Examples:
- TowerGridIntegrationTest, PlaceFacilitySuccess
- GameInitializationE2ETest, CompleteStartupSequence
- UserPreferencesUnitTest, SaveAndLoadPreferences
```

### 8.3 Running Tests

**Command Line**:
```bash
# Configure tests
cmake --preset cli-test

# Build tests
cmake --build --preset cli-test-debug --parallel $(nproc)

# Run all tests
ctest --preset cli-test-debug

# Run specific category
cd build-cli/cli-test/tests
ctest -C Debug -R ".*_integration" --output-on-failure
ctest -C Debug -R ".*_e2e" --output-on-failure
ctest -C Debug -R ".*_unit" --output-on-failure

# Run individual test
./build-cli/cli-test/bin/Debug/test_tower_grid_integration
```

**Continuous Integration**:
- Tests run automatically on every PR
- Multi-platform (Windows, Linux, macOS)
- Builds must pass 100% before merge

**Reference**: `TESTING.md`

---

## 9. Performance & Optimization

### 9.1 Performance Targets

| Metric | Target | Measured |
|--------|--------|----------|
| **Frame Rate** | 60 FPS | 60 FPS (500 entities) |
| **Simulation Rate** | 60 Hz | 60 Hz |
| **Load Time** | <5s (large saves) | ~3s (1000 entities) |
| **Memory Usage** | <500 MB | ~250 MB (typical game) |
| **Entity Limit** | 2000+ entities | Tested to 2000 |

### 9.2 Optimization Techniques

**ECS Optimization**:
- **Data-Oriented Design**: Components stored contiguously in memory
- **Cache Efficiency**: Systems iterate over components in order
- **Query Optimization**: Filter entities by component masks (O(n) → O(matching))

**Rendering Optimization**:
- **Batch Rendering**: Group similar draw calls (60x faster for 500+ entities)
- **Culling**: Skip off-screen entities
- **Dirty Flags**: Only recalculate layouts when needed

**UI Optimization**:
- **Declarative UI**: Build once, render many (avoid per-frame construction)
- **Reactive Updates**: Update only on events, not every frame
- **Lazy Layout**: Recalculate only on resize or content change

**Memory Optimization**:
- **Smart Pointers**: `unique_ptr` for ownership, raw pointers for access (no overhead)
- **RAII**: Automatic resource cleanup (no leaks)
- **Move Semantics**: Avoid unnecessary copies (C++11+)

### 9.3 Profiling

**Tools**:
- **Tracy Profiler**: Real-time frame profiler (future integration)
- **Valgrind**: Memory leak detection (Linux)
- **Visual Studio Profiler**: CPU/memory profiling (Windows)
- **Instruments**: Performance profiling (macOS)

**Profiling Hotspots**:
```cpp
// Use scoped timers for profiling
{
    ScopedTimer timer("UpdatePersonMovement");
    UpdatePersonMovement();
}  // Automatically logs elapsed time
```

---

## 10. Deployment

### 10.1 Build Artifacts

**Per-Platform Artifacts**:
- **Windows (MSVC)**: `TowerForge-windows-latest-msvc.zip`
  - `towerforge.exe`
  - `*.dll` (Raylib, flecs, etc.)
  - `assets/` directory
- **Linux (GCC/Clang)**: `TowerForge-ubuntu-latest-gcc.tar.gz`
  - `towerforge` (executable)
  - `assets/` directory
- **macOS (Clang)**: `TowerForge-macos-latest-clang.tar.gz`
  - `TowerForge.app` bundle
  - Embedded assets

### 10.2 Continuous Integration (CI/CD)

**GitHub Actions Workflow**:
```yaml
name: Build TowerForge
on: [push, pull_request]
jobs:
  build:
    strategy:
      matrix:
        os: [windows-latest, ubuntu-latest, macos-latest]
        compiler: [msvc, gcc, clang]
    runs-on: ${{ matrix.os }}
    steps:
      - uses: actions/checkout@v3
      - name: Setup vcpkg
        run: |
          git clone https://github.com/microsoft/vcpkg.git
          ./vcpkg/bootstrap-vcpkg.sh
      - name: Configure
        run: cmake --preset cli-native
      - name: Build
        run: cmake --build --preset cli-native-debug --parallel
      - name: Test
        run: ctest --preset cli-test-debug
      - name: Package
        run: cpack
      - name: Upload Artifacts
        uses: actions/upload-artifact@v3
        with:
          name: TowerForge-${{ matrix.os }}-${{ matrix.compiler }}
          path: build/TowerForge-*.zip
```

**CI Features**:
- Multi-platform builds (Windows, Linux, macOS)
- Multi-compiler (MSVC, GCC, Clang)
- Automated testing (fail fast on test failures)
- Artifact upload (30-day retention)
- Badge in README showing build status

### 10.3 Release Process

**Versioning**: Semantic Versioning (MAJOR.MINOR.PATCH)
- MAJOR: Breaking changes (e.g., save format incompatibility)
- MINOR: New features (e.g., new facility types)
- PATCH: Bug fixes

**Release Checklist**:
1. Update version in `CMakeLists.txt`, `vcpkg.json`
2. Update `CHANGELOG.md` with changes
3. Tag commit: `git tag v1.0.0`
4. Push tag: `git push --tags`
5. GitHub Actions automatically builds and creates release
6. Write release notes highlighting key changes
7. Attach build artifacts to GitHub release

---

## 11. Future Architecture

### 11.1 Planned Enhancements

**Multiplayer Architecture**:
- **Client-Server Model**: Headless server runs core simulation
- **State Synchronization**: Clients receive entity snapshots
- **Prediction**: Client-side prediction for responsiveness
- **Authority**: Server is authoritative (prevent cheating)

**Audio System**:
- **FMOD Integration**: Professional audio middleware
- **Spatial Audio**: 3D positioning for ambient sounds
- **Music System**: Dynamic music based on tower state
- **Accessibility**: Closed captions for audio cues

**Advanced AI**:
- **Behavior Trees**: More complex person AI (shopping, dining, entertainment)
- **Needs System**: Hunger, entertainment, comfort, social
- **Emergent Behavior**: Persons interact with each other
- **Pathfinding**: A* or flow fields for movement

**Modding Enhancements**:
- **Visual Modding**: Custom sprites, textures
- **Scripted Events**: Lua-defined random events
- **Custom UI**: Lua-defined menus and overlays
- **Workshop Integration**: Steam Workshop for mod distribution

### 11.2 Scalability Considerations

**Entity Scaling**:
- Current: 2,000 entities (tested)
- Target: 10,000+ entities (future)
- Approach: Job system for multi-threading, spatial partitioning

**Save File Optimization**:
- Current: JSON (human-readable, ~1 MB for large tower)
- Future: Binary format (10x smaller, faster load)
- Approach: MessagePack or custom binary serialization

**Network Optimization** (multiplayer):
- Delta compression (send only changes)
- Interest management (send only visible entities)
- Lag compensation techniques

---

## Appendix A: Code Style Guide

### C++20 Style

**Naming Conventions**:
- Classes/Structs: `PascalCase` (e.g., `TowerGrid`, `ECSWorld`)
- Functions/Methods: `PascalCase` (e.g., `CreateFacility()`, `GetWidth()`)
- Variables: `snake_case` (e.g., `facility_id`, `current_floor`)
- Constants: `UPPER_SNAKE_CASE` (e.g., `MAX_CAPACITY`, `DEFAULT_WIDTH`)
- Private Members: `snake_case_` (trailing underscore, e.g., `width_`, `grid_`)

**File Organization**:
- Headers: `include/module/file.hpp`
- Implementation: `src/module/file.cpp`
- One class per file (exceptions for small nested classes)

**Documentation**:
- Use Doxygen-style comments for public APIs
- Inline comments for complex logic only
- Self-documenting code preferred

**Modern C++ Features**:
- Use `auto` for complex types, explicit types for clarity
- Use range-based for loops over iterator loops
- Use `std::optional` for nullable returns (not pointers)
- Use structured bindings for tuples/pairs

---

## Appendix B: Glossary

**ECS**: Entity Component System  
**flecs**: C++ ECS framework  
**Raylib**: 2D/3D game programming library  
**vcpkg**: C++ package manager  
**CMake**: Cross-platform build system generator  
**Lua**: Lightweight scripting language  
**RAII**: Resource Acquisition Is Initialization (C++ idiom)  
**AAA**: Industry term for high-budget, high-quality games  
**HUD**: Heads-Up Display  
**CI/CD**: Continuous Integration / Continuous Deployment  

---

## Appendix C: Change Log

**Version 1.0** (2025-10-25):
- Initial comprehensive TDD
- Consolidates all technical documentation
- Defines architecture, systems, and implementation details
- Establishes build, testing, and deployment processes

---

**TowerForge Technical Design Document**  
© 2025 TowerForge Contributors  
Licensed under MIT (or similar open source license)

*"Modern C++20 architecture for the next generation of vertical city sims."*
