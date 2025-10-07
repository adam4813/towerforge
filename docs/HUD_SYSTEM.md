# HUD and Information Display System

## Overview

The HUD (Heads-Up Display) and Information Display System provides a comprehensive user interface for TowerForge, displaying critical game information, entity details, notifications, and build controls.

## Components

### 1. HUD Class (`include/ui/hud.h`, `src/ui/hud.cpp`)

The main HUD class manages all on-screen UI elements including:

- **Top Bar**: Displays funds, population, time, and simulation speed
- **Info Panels**: Shows detailed information about selected entities
  - Facility Info Panel
  - Person Info Panel
  - Elevator Info Panel
- **Notification System**: Displays warnings, errors, successes, and info messages
- **Speed Controls**: UI for controlling simulation speed (pause, 1x, 2x, 4x)

#### Key Features

**Top Bar Display:**
- Current funds and income rate (e.g., "$25,000 (+$500/hr)")
- Population count
- Current time and day (e.g., "8:30 AM Day 5")
- Simulation speed indicator

**Info Panels:**
- Facility Panel: Occupancy, revenue, satisfaction, tenant count, action buttons
- Person Panel: State, current floor, destination, wait time, needs, satisfaction
- Elevator Panel: Current floor, direction, occupancy, next stop, queue details

**Notifications:**
- Four types: Warning, Success, Info, Error
- Auto-expire after specified duration (default 5 seconds)
- Queue limited to 5 most recent notifications
- Color-coded by type

**Speed Controls:**
- Visual buttons for Pause, 1x, 2x, 4x speed
- Highlights active speed setting

#### Usage Example

```cpp
#include "ui/hud.h"

// Create HUD
towerforge::ui::HUD hud;

// Set game state
towerforge::ui::GameState state;
state.funds = 25000.0f;
state.income_rate = 500.0f;
state.population = 125;
state.current_day = 5;
state.current_time = 8.5f;  // 8:30 AM
state.speed_multiplier = 1;
state.paused = false;

hud.SetGameState(state);

// Update and render every frame
hud.Update(delta_time);
hud.Render();

// Show facility info when user clicks a facility
towerforge::ui::FacilityInfo facility;
facility.type = "OFFICE";
facility.floor = 5;
facility.occupancy = 8;
facility.max_occupancy = 10;
facility.revenue = 80.0f;
facility.satisfaction = 85.0f;
facility.tenant_count = 8;
hud.ShowFacilityInfo(facility);

// Add notifications
hud.AddNotification(Notification::Type::Warning, "Low satisfaction on Floor 3");
hud.AddNotification(Notification::Type::Success, "Milestone: 100 population");
```

### 2. Build Menu (`include/ui/build_menu.h`, `src/ui/build_menu.cpp`)

The Build Menu displays available facility types with their costs and allows players to select what to build.

#### Features

- Lists all buildable facilities with icons, names, and costs
- Visual selection highlighting
- Click-to-select interface
- Displays facility properties (name, cost, width)

#### Facility Types

Default facility types included:
- **Lobby** ($1,000) - 10 cells wide - Gold color
- **Office** ($5,000) - 8 cells wide - Sky Blue
- **Restaurant** ($8,000) - 6 cells wide - Red
- **Shop** ($6,000) - 4 cells wide - Green
- **Hotel** ($12,000) - 10 cells wide - Purple
- **Elevator** ($15,000) - 2 cells wide - Gray

#### Usage Example

```cpp
#include "ui/build_menu.h"

// Create build menu
towerforge::ui::BuildMenu menu;

// Render
menu.Render();

// Handle clicks
if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    int selected = menu.HandleClick(GetMouseX(), GetMouseY());
    if (selected >= 0) {
        // User selected a facility type
        const auto& types = menu.GetFacilityTypes();
        std::cout << "Selected: " << types[selected].name << std::endl;
    }
}

// Get current selection
int current = menu.GetSelectedFacility();
if (current >= 0) {
    // A facility is selected for placement
}

// Clear selection
menu.ClearSelection();
```

## Data Structures

### GameState
Holds the current state of the game simulation:
```cpp
struct GameState {
    float funds;              // Current money
    float income_rate;        // Money per hour
    int population;           // Total people in tower
    int current_day;          // Day number
    float current_time;       // Time of day (0-24)
    int speed_multiplier;     // 1x, 2x, or 4x
    bool paused;              // Simulation paused?
};
```

### FacilityInfo
Information about a building facility:
```cpp
struct FacilityInfo {
    std::string type;         // "OFFICE", "RESTAURANT", etc.
    int floor;                // Floor number
    int occupancy;            // Current occupants
    int max_occupancy;        // Maximum capacity
    float revenue;            // Income per hour
    float satisfaction;       // 0-100 percentage
    int tenant_count;         // Number of tenants
};
```

### PersonInfo
Information about a person/actor:
```cpp
struct PersonInfo {
    int id;                   // Person ID
    std::string state;        // "WaitingElevator", "Working", etc.
    int current_floor;        // Current location
    int destination_floor;    // Where they're going
    float wait_time;          // Time waiting (seconds)
    std::string needs;        // Current need/goal
    float satisfaction;       // 0-100 percentage
};
```

### ElevatorInfo
Information about an elevator:
```cpp
struct ElevatorInfo {
    int id;                          // Elevator ID
    int current_floor;               // Current position
    std::string direction;           // "UP", "DOWN", or "IDLE"
    int occupancy;                   // Current passengers
    int max_occupancy;               // Maximum capacity
    int next_stop;                   // Next floor destination
    std::vector<std::pair<int, int>> queue;  // {floor, waiting count}
};
```

### Notification
Display message with auto-expiry:
```cpp
struct Notification {
    enum class Type { Warning, Success, Info, Error };
    
    Type type;
    std::string message;
    float time_remaining;    // Seconds until removed
};
```

## Layout Constants

The HUD uses these layout constants (defined in `hud.h`):

```cpp
static constexpr int TOP_BAR_HEIGHT = 40;
static constexpr int PANEL_WIDTH = 250;
static constexpr int PANEL_PADDING = 10;
static constexpr int NOTIFICATION_WIDTH = 300;
static constexpr int NOTIFICATION_HEIGHT = 30;
static constexpr int SPEED_CONTROL_WIDTH = 200;
static constexpr int SPEED_CONTROL_HEIGHT = 40;
```

## Integration with Game Loop

Typical integration in the main game loop:

```cpp
HUD hud;
BuildMenu build_menu;
GameState game_state = { /* initial values */ };

while (!renderer.ShouldClose()) {
    // Update simulation
    ecs_world.Update(delta_time);
    
    // Update game state based on simulation
    game_state.funds += income * delta_time;
    game_state.current_time += delta_time / 3600.0f;
    
    // Update HUD
    hud.SetGameState(game_state);
    hud.Update(delta_time);
    
    // Handle input
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        int mx = GetMouseX();
        int my = GetMouseY();
        
        // Check build menu
        if (build_menu.HandleClick(mx, my) >= 0) {
            // Handle facility selection
        }
        // Check HUD elements
        else if (!hud.HandleClick(mx, my)) {
            // Handle game area click
            // Show info panels based on what was clicked
        }
    }
    
    // Render
    renderer.BeginFrame();
    renderer.Clear(DARKGRAY);
    
    // Render game world
    RenderTower();
    RenderActors();
    
    // Render UI
    hud.Render();
    build_menu.Render();
    
    renderer.EndFrame();
}
```

## Demo Application

A standalone demo application is provided in `src/hud_demo.cpp` that showcases all HUD features:

### Building and Running

```bash
cd build
cmake --build .
./bin/hud_demo
```

### Demo Controls

- **1**: Show Facility Info Panel
- **2**: Show Person Info Panel
- **3**: Show Elevator Info Panel
- **4**: Hide all info panels
- **N**: Add a test notification
- **SPACE**: Toggle pause
- **Mouse Click**: Interact with build menu and other UI elements
- **ESC**: Exit demo

## Visual Design

### Color Scheme

- **Top Bar**: Semi-transparent black background with gold accent line
- **Panels**: Dark semi-transparent background with colored headers
  - Facility: Sky Blue
  - Person: Yellow
  - Elevator: Purple
- **Notifications**: Color-coded by type
  - Warning: Orange
  - Success: Green
  - Info: Sky Blue
  - Error: Red
- **Speed Controls**: Dark gray buttons, green when active, red when paused

### Text Formatting

- Top bar uses 20pt font
- Panel titles use 16pt font
- Panel content uses 14pt font
- Notifications use 14pt font
- UI hints use 10-12pt font

### Satisfaction Emoji

The HUD displays simple text-based satisfaction indicators:
- `:)` for 80%+ satisfaction (happy)
- `:|` for 60-79% satisfaction (neutral)
- `:/` for 40-59% satisfaction (concerned)
- `:(` for <40% satisfaction (unhappy)

## Future Enhancements

Potential improvements for the HUD system:

1. **Graphical Icons**: Replace text emoji with actual sprite/icon assets
2. **Animation**: Add fade-in/fade-out for notifications and panels
3. **Tooltips**: Hover tooltips for buttons and UI elements
4. **Customizable Layout**: Allow users to reposition panels
5. **Statistics Graphs**: Charts for population growth, income over time
6. **Mini-map**: Small overview of the entire tower
7. **Build Mode Preview**: Ghost preview of facility placement
8. **Keyboard Shortcuts**: Hotkeys for all build menu items
9. **Sound Effects**: Audio feedback for notifications and actions
10. **Localization**: Support for multiple languages

## Dependencies

The HUD system depends on:

- **Raylib**: For rendering (DrawRectangle, DrawText, DrawLine, etc.)
- **C++20 Standard Library**: For containers (vector, string, etc.)
- **TowerForge Core**: For ECS integration (optional, not required)

## Testing

The HUD system can be tested independently from the full game simulation. The `hud_demo.cpp` application provides a complete test environment with simulated data.

## License

Part of the TowerForge project. See main LICENSE file for details.
