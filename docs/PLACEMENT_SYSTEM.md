# Interactive Building and Placement System

## Overview

The Interactive Building and Placement System provides a complete mouse-driven interface for placing and removing facilities within the tower. It's the primary player interaction mechanism that makes TowerForge playable as a simulation game.

This system includes:
- Real-time visual feedback for placement and demolition
- Construction progress tracking
- **Advanced undo/redo functionality with Command Pattern**
- **Visual history panel for reviewing and navigating actions**
- Funds validation for undo/redo operations
- Economy integration

---

## Features

### Mouse-Driven Placement
- **Hover Preview**: Ghost outline of facility appears when hovering over valid grid positions
- **Visual Feedback**: Green highlight for valid placements, red for invalid
- **Click to Place**: Single click places the selected facility
- **Cost Display**: Shows facility cost in preview

### Build Menu (`BuildMenu`)
- Facility types with icons, names, and costs:
  - Lobby ($1,000) - 10 tiles
  - Office ($5,000) - 8 tiles
  - Restaurant ($8,000) - 6 tiles
  - Shop ($6,000) - 4 tiles
  - Hotel ($12,000) - 10 tiles
  - Elevator ($15,000) - 2 tiles
- Tool buttons:
  - Demolish (D)
  - Undo (Ctrl+Z)
  - Redo (Ctrl+Y)
- Visual indicators for demolish mode
- Grayed-out buttons when undo/redo unavailable
- Context-sensitive hints at bottom

### Placement Validation
- Sufficient funds check
- Grid space unoccupied
- Facility fits within grid bounds
- Red highlight and message for invalid placement

### Construction System
- Construction begins immediately on placement
- Visual overlay and progress bar
- Build times vary by facility type (10–25 seconds)

### Demolition Tool
- Activate via button or D key
- Red outline for demolition target
- 50% cost refund on demolition
- Click to demolish

### Undo/Redo System
- **Command Pattern Implementation**: Each action is encapsulated as a command object
- **Full State Restoration**: Demolish undo properly restores facilities with all their properties
- **Funds Validation**: Checks if player has enough funds before allowing undo/redo
- **History Limit**: Stores up to 50 actions (configurable)
- **History Panel**: Visual interface showing action history with timestamps
- Tracks placements and demolitions with complete metadata
- Keyboard shortcuts and menu buttons
- **Multiple Undo/Redo**: Click on history entries to undo/redo multiple actions at once

### History Panel (`HistoryPanel`)
- **Toggle Visibility**: Press H key to show/hide
- **Action Timeline**: Displays recent actions in chronological order
- **Visual Indicators**: Different colors for undo stack (white) and redo stack (blue)
- **Cost Display**: Shows cost/refund for each action
- **Timestamps**: Shows when each action was performed
- **Interactive**: Click on any entry to undo/redo to that point
- **Positioned**: Top-right corner, semi-transparent background

### Keyboard Shortcuts
- 1–5: Select facility types (future use)
- D: Toggle demolish mode
- **H: Toggle history panel**
- Ctrl+Z: Undo
- Ctrl+Y: Redo

---

## Architecture

### Core Components

#### Command Pattern (`include/core/command.hpp`, `src/core/command.cpp`)
Implements the Command pattern for encapsulating actions with full state restoration.

**Key Classes:**
- `ICommand` - Interface for all commands with Execute/Undo/GetDescription methods
- `FacilityState` - Captures complete facility state for restoration
- `PlaceFacilityCommand` - Encapsulates facility placement with cost and validation
- `DemolishFacilityCommand` - Encapsulates demolition with state capture and refund calculation

**Key Features:**
- Full state capture before demolition (type, position, width, capacity, cost)
- Automatic cost calculation based on facility type
- Proper restoration of demolished facilities with all properties

#### Command History (`include/core/command_history.hpp`, `src/core/command_history.cpp`)
Manages undo/redo stacks with validation and limits.

**Key Classes:**
- `HistoryEntry` - Stores command with metadata (description, timestamp, cost change)
- `CommandHistory` - Manages undo/redo stacks with size limits

**Key Methods:**
- `ExecuteCommand(command, funds)` - Executes command and adds to history
- `Undo(funds)` - Undoes last command with funds validation
- `Redo(funds)` - Redoes last undone command with funds validation
- `GetUndoStack()` / `GetRedoStack()` - Returns stacks for UI display

#### `PlacementSystem` (`include/ui/placement_system.h`, `src/ui/placement_system.cpp`)
Main controller for all placement interactions. **Updated to use CommandHistory**.

- `ConstructionState`: Tracks ongoing construction with build time and progress

**Key Methods:**
- `Update(delta_time)` - Updates construction progress
- `Render(...)` - Renders placement preview, construction overlays, and demolition highlights
- `HandleClick(...)` - Processes mouse clicks for placement/demolition with validation
- `HandleKeyboard()` - Processes keyboard shortcuts
- `MouseToGrid(...)` - Converts screen coordinates to grid coordinates
- `IsPlacementValid(...)` - Validates placement with funds, space, and bounds checks
- `PlaceFacility(...)` - Creates PlaceFacilityCommand and executes via CommandHistory
- `DemolishFacility(...)` - Creates DemolishFacilityCommand and executes via CommandHistory
- `Undo(funds)` / `Redo(funds)` - Delegates to CommandHistory with funds adjustment
- `GetCommandHistory()` - Returns command history for UI display

#### `HistoryPanel` (`include/ui/history_panel.h`, `src/ui/history_panel.cpp`)
Visual interface for reviewing and navigating action history.

**Key Classes:**
- `HistoryDisplayEntry` - UI representation of a history entry with formatting

**Key Methods:**
- `Render()` - Renders the history panel with scrollable list
- `HandleClick(mouse_x, mouse_y)` - Returns number of steps to undo/redo (positive=undo, negative=redo)
- `UpdateFromHistory(history)` - Refreshes display from CommandHistory
- `ToggleVisible()` - Shows/hides the panel
- `IsMouseOver(x, y)` - Checks if mouse is over the panel

#### `BuildMenu` (`include/ui/build_menu.h`, `src/ui/build_menu.cpp`)
Displays facility selection menu and tool buttons.

**Key Methods:**
- `Render(can_undo, can_redo, demolish_mode)` - Renders menu with state
- `HandleClick(mouse_x, mouse_y, ...)` - Returns selected item or tool
- Return codes: `>= 0` (facility), `-2` (demolish), `-3` (undo), `-4` (redo)

---

## Integration in Main Application

- PlacementSystem instance created with grid and facility manager
- Grid rendering with floor labels
- Placement preview rendering
- Mouse click handling for placement/demolition
- Keyboard shortcut handling
- Economy updates with cost changes
- Notifications for user feedback

**Grid Rendering:**
- 40px cell width × 50px cell height
- Floor labels on the left
- Grid cell outlines
- Colored facility rectangles
- Offset positioning (300, 100)

---

## Implementation Notes

- **Command Pattern**: Clean separation of action execution and state management
- **Full State Restoration**: Demolish operations capture complete facility state for accurate undo
- **Funds Validation**: All undo/redo operations validate available funds before execution
- Construction queue with progress tracking
- **Command history with 50-action limit** (configurable via CommandHistory constructor)
- Economy integration with proper cost/refund handling
- Build time estimation (10–28 seconds depending on facility type)
- Real-time hover preview with green/red validation
- User notifications for actions
- **Interactive history panel** with timeline view and click-to-undo/redo

### Command Pattern Benefits
- **Encapsulation**: Each action is self-contained with its own Execute/Undo logic
- **Extensibility**: Easy to add new action types (e.g., floor expansion, bulk operations)
- **State Management**: Automatic capture and restoration of facility state
- **Testability**: Commands can be tested independently
- **Separation of Concerns**: UI logic separated from action execution

---

## Documentation & References

- This document consolidates all placement system documentation and implementation notes as of October 18, 2025.
- **Major Update**: Refactored to use Command Pattern with full state restoration and visual history panel
- For further details, see code in:
  - Command Pattern: `include/core/command.hpp`, `src/core/command.cpp`
  - Command History: `include/core/command_history.hpp`, `src/core/command_history.cpp`
  - Placement System: `include/ui/placement_system.h`, `src/ui/placement_system.cpp`
  - Build Menu: `include/ui/build_menu.h`, `src/ui/build_menu.cpp`
  - History Panel: `include/ui/history_panel.h`, `src/ui/history_panel.cpp`
- Screenshot: `placement_system_demo.png` (to be updated)
