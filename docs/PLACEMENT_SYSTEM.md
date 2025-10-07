# Interactive Building and Placement System

## Overview

The Interactive Building and Placement System provides a complete mouse-driven interface for placing and removing facilities within the tower. It's the primary player interaction mechanism that makes TowerForge playable as a simulation game.

## Features

### 1. Mouse-Driven Placement
- **Hover Preview**: Ghost outline of facility appears when hovering over valid grid positions
- **Visual Feedback**: Green highlight for valid placements, red for invalid
- **Click to Place**: Single click places the selected facility
- **Cost Display**: Shows facility cost in preview

### 2. Build Menu (`BuildMenu`)
Located on the left sidebar, the build menu displays:
- **Facility Types** with icons, names, and costs:
  - Lobby ($1,000) - 10 tiles wide
  - Office ($5,000) - 8 tiles wide
  - Restaurant ($8,000) - 6 tiles wide
  - Shop ($6,000) - 4 tiles wide
  - Hotel ($12,000) - 10 tiles wide
  - Elevator ($15,000) - 2 tiles wide

- **Tool Buttons**:
  - Demolish (D) - Toggle demolition mode
  - Undo (Ctrl+Z) - Undo last action
  - Redo (Ctrl+Y) - Redo last undone action

### 3. Placement Validation
The system checks:
- ✅ Sufficient funds available
- ✅ Grid space is unoccupied
- ✅ Facility fits within grid bounds
- ❌ Insufficient funds → Red highlight + "INVALID" message
- ❌ Occupied space → Red highlight
- ❌ Out of bounds → No preview shown

### 4. Construction System
When a facility is placed:
- Construction begins immediately
- Visual overlay shows construction progress
- Progress bar displays percentage complete
- Build times vary by facility type:
  - Lobby: 10 seconds
  - Office: 15 seconds
  - Restaurant: 20 seconds
  - Shop: 15 seconds
  - Hotel: 25 seconds
  - Elevator: 12 seconds

### 5. Demolition Tool
- **Activate**: Click "Demolish (D)" button or press D key
- **Visual Feedback**: Red outline highlights facility under cursor
- **Recovery**: Receive 50% of original construction cost
- **Click to Demolish**: Single click removes facility and refunds funds

### 6. Undo/Redo System
- **Capacity**: Stores up to 20 actions
- **Actions Tracked**: Placements and demolitions
- **Keyboard Shortcuts**:
  - Ctrl+Z: Undo last action
  - Ctrl+Y: Redo last undone action
- **Menu Buttons**: Grayed out when unavailable

### 7. Keyboard Shortcuts
- **1-5**: Select facility types 1-5 (currently unused in menu)
- **D**: Toggle demolish mode
- **Ctrl+Z**: Undo
- **Ctrl+Y**: Redo

## Architecture

### Core Components

#### `PlacementSystem` (`include/ui/placement_system.h`)
Main controller for all placement interactions.

**Key Methods:**
- `Update(delta_time)` - Updates construction progress
- `Render(...)` - Renders placement preview and construction visuals
- `HandleClick(...)` - Processes placement/demolition clicks
- `HandleKeyboard()` - Processes keyboard shortcuts
- `Undo()` / `Redo()` - Manages action history

**Key Data Structures:**
- `ConstructionState` - Tracks ongoing construction with progress
- `PlacementAction` - Records actions for undo/redo

#### `BuildMenu` (`include/ui/build_menu.h`)
Displays facility selection menu and tool buttons.

**Key Methods:**
- `Render(can_undo, can_redo, demolish_mode)` - Renders menu with state
- `HandleClick(mouse_x, mouse_y, ...)` - Returns selected item or tool

**Return Values:**
- `>= 0`: Facility type index selected
- `-2`: Demolish button clicked
- `-3`: Undo button clicked
- `-4`: Redo button clicked
- `-1`: No selection

## Usage Example

### Basic Integration

```cpp
#include "ui/placement_system.h"
#include "ui/build_menu.h"
#include "core/tower_grid.hpp"
#include "core/facility_manager.hpp"

// Initialize components
TowerForge::Core::TowerGrid grid(10, 30);
TowerForge::Core::FacilityManager facility_mgr(world, grid);
towerforge::ui::BuildMenu build_menu;
towerforge::ui::PlacementSystem placement_system(grid, facility_mgr, build_menu);

// Game loop
const int grid_offset_x = 300;
const int grid_offset_y = 100;
const int cell_width = 40;
const int cell_height = 50;
float player_funds = 50000.0f;

while (running) {
    float delta_time = GetFrameTime();
    
    // Update
    placement_system.Update(delta_time);
    placement_system.HandleKeyboard();
    
    // Handle mouse clicks
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        int mouse_x = GetMouseX();
        int mouse_y = GetMouseY();
        
        // Check build menu
        int menu_result = build_menu.HandleClick(mouse_x, mouse_y, 
                                                 placement_system.CanUndo(), 
                                                 placement_system.CanRedo());
        
        if (menu_result >= 0) {
            // Facility selected
        } else if (menu_result == -2) {
            // Toggle demolish mode
            placement_system.SetDemolishMode(!placement_system.IsDemolishMode());
        } else if (menu_result == -3) {
            placement_system.Undo();
        } else if (menu_result == -4) {
            placement_system.Redo();
        } else {
            // Try placement/demolition
            int cost_change = placement_system.HandleClick(
                mouse_x, mouse_y, grid_offset_x, grid_offset_y, 
                cell_width, cell_height, player_funds);
            
            player_funds += cost_change;
        }
    }
    
    // Render
    BeginDrawing();
    
    // Draw grid and facilities...
    
    // Render placement system (preview and construction)
    placement_system.Render(grid_offset_x, grid_offset_y, cell_width, cell_height);
    
    // Render UI
    build_menu.Render(placement_system.CanUndo(), 
                     placement_system.CanRedo(), 
                     placement_system.IsDemolishMode());
    
    EndDrawing();
}
```

### Handling Cost Changes

```cpp
int cost_change = placement_system.HandleClick(..., player_funds);

if (cost_change < 0) {
    // Facility placed
    player_funds += cost_change;  // Deduct cost
    ShowNotification("Facility placed! Cost: $" + std::to_string(-cost_change));
} else if (cost_change > 0) {
    // Facility demolished
    player_funds += cost_change;  // Add refund
    ShowNotification("Facility demolished! Refund: $" + std::to_string(cost_change));
}
```

## Visual Design

### Color Scheme
- **Valid Placement**: Green (`ColorAlpha(GREEN, 0.3f)`) with green outline
- **Invalid Placement**: Red (`ColorAlpha(RED, 0.3f)`) with red outline
- **Construction**: Orange overlay (`ColorAlpha(ORANGE, 0.4f)`) with yellow progress bar
- **Demolish Hover**: Red overlay (`ColorAlpha(RED, 0.3f)`) with red outline

### Build Menu Layout
```
┌─────────────────┐
│ FACILITIES      │ ← Header
├─────────────────┤
│ [🟨 L] Lobby    │ ← Icon + Name
│      $1,000     │ ← Cost
├─────────────────┤
│ [🔵 O] Office   │
│      $5,000     │
├─────────────────┤
│ ... (4 more)    │
├─────────────────┤
│ TOOLS           │ ← Section divider
├─────────────────┤
│ Demolish (D)    │ ← Tool buttons
│ Undo (Ctrl+Z)   │
│ Redo (Ctrl+Y)   │
└─────────────────┘
```

### Placement Preview
```
Grid View:
┌────┬────┬────┬────┬────┐
│    │    │░░░░│░░░░│    │ ← Green: Valid placement preview
│    │    │░░░░│░░░░│    │   Shows facility outline + cost
├────┼────┼────┼────┼────┤
│    │▓▓▓▓│▓▓▓▓│▓▓▓▓│    │ ← Existing facility
├────┼────┼────┼────┼────┤
│    │░░░░│    │    │    │ ← Red: Invalid (insufficient width)
└────┴────┴────┴────┴────┘
```

### Construction State
```
┌────────────────────┐
│ 🏗️ Building... 45%  │ ← Orange overlay
│ ████████░░░░░░░░░░ │ ← Yellow progress bar
└────────────────────┘
```

## Performance Considerations

- **Update Frequency**: O(n) where n = number of constructions in progress
- **Render Complexity**: O(1) for preview, O(n) for construction overlays
- **Memory**: Minimal - stores up to 20 undo actions
- **Grid Queries**: Leverages TowerGrid's O(1) occupancy checks

## Future Enhancements

Potential improvements for the placement system:

1. **Drag-and-Drop**: Allow dragging to place multiple facilities
2. **Multi-Select**: Select and place multiple facility types
3. **Templates**: Save and load facility placement templates
4. **Rotation**: Rotate facilities for different orientations
5. **Adjacency Rules**: Require specific facilities near others (e.g., elevators near offices)
6. **Cost Estimation**: Show ongoing costs during construction
7. **Construction Queue**: Queue multiple construction projects
8. **Fast-Forward Construction**: Pay to complete construction instantly
9. **Facility Upgrades**: Upgrade placed facilities
10. **Copy/Paste**: Copy facility configurations

## Dependencies

- **raylib**: For rendering and input handling
- **TowerGrid**: Grid spatial system
- **FacilityManager**: Facility creation and removal
- **BuildMenu**: UI component for facility selection
- **ECS World**: Optional integration for entity management

## Testing

A demo application is provided to test the placement system:

```bash
# Build
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=/usr/local/share/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build .

# Run demo
./bin/placement_demo_app

# Generate screenshot (headless)
xvfb-run -a ./bin/placement_demo_app
```

The demo showcases:
- Facility placement with preview
- Demolition mode
- Construction progress
- Undo/redo functionality
- Build menu interaction

## License

Part of TowerForge - see main project LICENSE.
