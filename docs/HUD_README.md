# HUD and Information Display System - Quick Start

## Overview

The HUD (Heads-Up Display) and Information Display System provides a complete user interface for TowerForge, displaying game stats, entity information, notifications, and build controls.

## What's Included

### Core Components

1. **Top Bar HUD** - Always-visible game stats
   - Current funds and income rate
   - Population count
   - Time of day and day number
   - Simulation speed indicator

2. **Info Panels** - Detailed entity information
   - Facility Panel: occupancy, revenue, satisfaction
   - Person Panel: state, location, needs, wait time
   - Elevator Panel: floor, direction, queue details

3. **Build Menu** - Facility selection interface
   - Six facility types with costs
   - Visual icons and pricing
   - Click-to-select interface

4. **Notification System** - Event messages
   - Four types: Warning, Success, Info, Error
   - Color-coded display
   - Auto-expiry after 5 seconds

5. **Speed Controls** - Simulation speed UI
   - Pause, 1x, 2x, 4x buttons
   - Visual feedback for active state

## Quick Start

### Building

```bash
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build .
```

### Running the Application

```bash
./bin/towerforge
```

The main application includes integrated HUD features showing:
- Real-time game statistics in the top bar
- Interactive info panels when clicking entities
- Notification system for important events
- Build menu for facility selection
- Speed controls for simulation

## Using in Your Code

### Basic Setup

```cpp
#include "ui/hud.h"
#include "ui/build_menu.h"

// Create HUD and build menu
towerforge::ui::HUD hud;
towerforge::ui::BuildMenu build_menu;

// Set game state
towerforge::ui::GameState state;
state.funds = 25000.0f;
state.income_rate = 500.0f;
state.population = 125;
state.current_day = 5;
state.current_time = 8.5f;
state.speed_multiplier = 1;
state.paused = false;

hud.SetGameState(state);
```

### Game Loop Integration

```cpp
while (!renderer.ShouldClose()) {
    // Update simulation
    ecs_world.Update(delta_time);
    
    // Update HUD
    hud.SetGameState(state);
    hud.Update(delta_time);
    
    // Handle input
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        int mx = GetMouseX();
        int my = GetMouseY();
        
        if (build_menu.HandleClick(mx, my) >= 0) {
            // Facility selected
        } else if (!hud.HandleClick(mx, my)) {
            // Click in game area
        }
    }
    
    // Render
    renderer.BeginFrame();
    RenderGame();
    hud.Render();
    build_menu.Render();
    renderer.EndFrame();
}
```

### Showing Info Panels

```cpp
// Facility clicked
towerforge::ui::FacilityInfo info;
info.type = "OFFICE";
info.floor = 5;
info.occupancy = 8;
info.max_occupancy = 10;
info.revenue = 80.0f;
info.satisfaction = 85.0f;
info.tenant_count = 8;
hud.ShowFacilityInfo(info);

// Person clicked
towerforge::ui::PersonInfo person;
person.id = 42;
person.state = "WaitingElevator";
person.current_floor = 1;
person.destination_floor = 8;
person.wait_time = 45.0f;
person.needs = "Work";
person.satisfaction = 60.0f;
hud.ShowPersonInfo(person);
```

### Adding Notifications

```cpp
using namespace towerforge::ui;

hud.AddNotification(Notification::Type::Warning, 
                   "Low satisfaction on Floor 3");

hud.AddNotification(Notification::Type::Success, 
                   "Milestone: 100 population");

hud.AddNotification(Notification::Type::Info, 
                   "New facility constructed");

hud.AddNotification(Notification::Type::Error, 
                   "Insufficient funds");
```

## Documentation

- **[HUD_SYSTEM.md](HUD_SYSTEM.md)** - Complete API documentation
- **[HUD_VISUAL_LAYOUT.md](HUD_VISUAL_LAYOUT.md)** - Visual layout guide
- **[HUD_SCREENSHOT_REFERENCE.md](HUD_SCREENSHOT_REFERENCE.md)** - Visual reference
- **[HUD_IMPLEMENTATION_SUMMARY.md](HUD_IMPLEMENTATION_SUMMARY.md)** - Implementation details

## File Structure

```
include/ui/
  ├── hud.h              # Main HUD class
  └── build_menu.h       # Build menu class

src/ui/
  ├── hud.cpp            # HUD implementation
  └── build_menu.cpp     # Build menu implementation

src/
  └── main.cpp           # Main app with HUD

docs/
  ├── HUD_SYSTEM.md
  ├── HUD_VISUAL_LAYOUT.md
  ├── HUD_SCREENSHOT_REFERENCE.md
  ├── HUD_IMPLEMENTATION_SUMMARY.md
  └── DESIGN_DECISION_NO_DEMOS.md
```

## Data Structures

### GameState
```cpp
struct GameState {
    float funds;
    float income_rate;
    int population;
    int current_day;
    float current_time;
    int speed_multiplier;
    bool paused;
};
```

### FacilityInfo
```cpp
struct FacilityInfo {
    std::string type;
    int floor;
    int occupancy;
    int max_occupancy;
    float revenue;
    float satisfaction;
    int tenant_count;
};
```

### PersonInfo
```cpp
struct PersonInfo {
    int id;
    std::string state;
    int current_floor;
    int destination_floor;
    float wait_time;
    std::string needs;
    float satisfaction;
};
```

### ElevatorInfo
```cpp
struct ElevatorInfo {
    int id;
    int current_floor;
    std::string direction;
    int occupancy;
    int max_occupancy;
    int next_stop;
    std::vector<std::pair<int, int>> queue;
};
```

## Visual Preview

```
┌────────────────────────────────────────────────────────────┐
│ $25,000 (+$500/hr) | Population: 125 | 8:30 AM Day 5 | 1x │
└────────────────────────────────────────────────────────────┘
┌───────────┐  ┌────────────────────────────────────────────┐
│BUILD MENU │  │ GAME VIEW                                  │
│           │  │                                            │
│ Lobby     │  │  [Tower Grid with Facilities]             │
│ Office    │  │                                            │
│ Rest.     │  │                                            │
│ Shop      │  │                                            │
│ Hotel     │  │                                            │
│ Elevator  │  │                                            │
└───────────┘  └────────────────────────────────────────────┘
                                    ┌───────────────────────┐
┌─────────────────────┐             │ OFFICE - Floor 5      │
│ ! Warning message   │             │ Occupancy: 8/10       │
│ + Success message   │             │ Revenue: $80/hr       │
└─────────────────────┘             │ Satisfaction: :) 85%  │
                                    └───────────────────────┘
              ┌──────────────┐
              │ || 1x 2x 4x  │
              └──────────────┘
```

## Features

✅ Real-time game stat display
✅ Interactive entity information
✅ Notification system with auto-expiry
✅ Facility build menu
✅ Simulation speed controls
✅ Mouse input handling
✅ Comprehensive documentation
✅ Working demo application

## Dependencies

- **Raylib** - For rendering
- **C++20 Standard Library** - For containers and utilities
- **TowerForge Core** - For ECS integration (optional)

## Next Steps

1. Build and run the application: `./bin/towerforge`
2. Explore the documentation in `docs/`
3. Integrate into your game using the examples above
4. Customize colors and layout as needed

## Support

For questions or issues:
- See full documentation in `docs/HUD_SYSTEM.md`
- Review the integration in `src/main.cpp`
- Check the design decision document in `docs/DESIGN_DECISION_NO_DEMOS.md`

## License

Part of the TowerForge project. See main LICENSE file.
