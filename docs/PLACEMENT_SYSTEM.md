# Interactive Building and Placement System

## Overview

The Interactive Building and Placement System provides a complete mouse-driven interface for placing and removing facilities within the tower. It's the primary player interaction mechanism that makes TowerForge playable as a simulation game.

This system includes:
- Real-time visual feedback for placement and demolition
- Construction progress tracking
- Undo/redo functionality
- Economy integration
- Dedicated demo application

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
- Stores up to 20 actions
- Tracks placements and demolitions
- Keyboard shortcuts and menu buttons

### Keyboard Shortcuts
- 1–5: Select facility types (future use)
- D: Toggle demolish mode
- Ctrl+Z: Undo
- Ctrl+Y: Redo

---

## Architecture

### Core Components

#### `PlacementSystem` (`include/ui/placement_system.h`, `src/ui/placement_system.cpp`)
Main controller for all placement interactions.

- `ConstructionState`: Tracks ongoing construction with build time and progress
- `PlacementAction`: Records placement/demolition actions for undo/redo

**Key Methods:**
- `Update(delta_time)` - Updates construction progress
- `Render(...)` - Renders placement preview, construction overlays, and demolition highlights
- `HandleClick(...)` - Processes mouse clicks for placement/demolition with validation
- `HandleKeyboard()` - Processes keyboard shortcuts
- `MouseToGrid(...)` - Converts screen coordinates to grid coordinates
- `IsPlacementValid(...)` - Validates placement with funds, space, and bounds checks
- `PlaceFacility(...)` - Creates facility and deducts cost
- `DemolishFacility(...)` - Removes facility and adds refund
- `Undo()` / `Redo()` - Manages action history

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

- Construction queue with progress tracking
- Undo/redo stack (20 actions max)
- Economy integration
- Build time estimation (10–25 seconds)
- Real-time hover preview with green/red validation
- User notifications for actions

---

## Documentation & References

- This document consolidates all placement system documentation and implementation notes as of October 12, 2025.
- For further details, see code in `include/ui/placement_system.h`, `src/ui/placement_system.cpp`, `include/ui/build_menu.h`, and `src/ui/build_menu.cpp`.
- Screenshot: `placement_system_demo.png`
