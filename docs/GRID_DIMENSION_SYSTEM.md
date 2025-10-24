# Grid Dimension System

## Overview

The Grid Dimension System provides a flexible, screen-aware tower grid with configurable dimension limits and upgrade mechanics. The system calculates initial grid dimensions based on screen size and allows players to expand vertically through research upgrades.

## Grid Constants

The system defines three absolute maximum constants that can never be exceeded:

```cpp
MAX_HORIZONTAL_CELLS = 1000      // Maximum columns (sideways)
MAX_BELOW_GROUND_FLOORS = 20     // Maximum basement floors (upgradeable)
MAX_ABOVE_GROUND_FLOORS = 200    // Maximum above-ground floors (upgradeable)
```

## Screen-Based Initialization

### Calculation Formula

The grid initializes with dimensions calculated at runtime based on screen dimensions:

- **Horizontal Cells (Columns):** `screen_width / cell_width`
  - Example: 800px / 40px = 20 columns
  - Capped at `MAX_HORIZONTAL_CELLS`

- **Below Ground Floors:** `screen_height / cell_height`
  - Example: 600px / 50px = 12 basement floors
  - Capped at `MAX_BELOW_GROUND_FLOORS`

- **Above Ground Floors:** `(screen_height * 3) / cell_height`
  - Example: (600px * 3) / 50px = 36 above-ground floors
  - Capped at `MAX_ABOVE_GROUND_FLOORS`

### Cell Dimensions

Current default cell dimensions:
- **Cell Width:** 40 pixels
- **Cell Height:** 50 pixels

These values are defined in `Game` class initialization and used for both rendering and grid calculations.

## Dimension Limits

### Initial vs Maximum Dimensions

The system distinguishes between:

1. **Initial Dimensions:** Calculated from screen size at game start
2. **Current Maximum Dimensions:** Upgradeable limits that start at initial values
3. **Absolute Maximum Dimensions:** Hard-coded constants that cannot be exceeded

Example progression:
```
Initial:    36 above-ground floors
Upgrade 1:  50 above-ground floors (Sky Reach)
Upgrade 2:  100 above-ground floors (Skyscraper Engineering)
Absolute:   200 above-ground floors (MAX_ABOVE_GROUND_FLOORS)
```

### Upgrading Dimensions

Players can increase vertical limits through the Research Tree:

#### Basement Expansion Upgrades

1. **Deep Foundation** (10 tower points)
   - Increases basement depth to 5 floors
   - Effect Target: `BelowGround`

2. **Underground Complex** (25 tower points)
   - Increases basement depth to 10 floors
   - Requires: Deep Foundation
   - Effect Target: `BelowGround`

#### Upward Expansion Upgrades

1. **Sky Reach** (15 tower points)
   - Increases height limit by 50 floors
   - Effect Target: `AboveGround`

2. **Skyscraper Engineering** (40 tower points)
   - Increases height limit by 100 floors
   - Requires: Sky Reach
   - Minimum 3-star tower rating
   - Effect Target: `AboveGround`

## API Reference

### TowerGrid Methods

#### Dimension Queries

```cpp
// Get current maximum limits
int GetMaxAboveGroundFloors() const;
int GetMaxBelowGroundFloors() const;
int GetMaxHorizontalCells() const;

// Get current counts
int GetAboveGroundFloorCount() const;
int GetBelowGroundFloorCount() const;
int GetColumnCount() const;
```

#### Dimension Updates

```cpp
// Set new maximum limits (capped at absolute maximums)
void SetMaxAboveGroundFloors(int max_floors);
void SetMaxBelowGroundFloors(int max_floors);

// Check if expansion is allowed
bool CanAddFloors(int count) const;
bool CanAddBasementFloors(int count) const;
bool CanAddColumns(int count) const;
```

### ECSWorld Methods

```cpp
// Apply research upgrades to tower grid
void ApplyVerticalExpansionUpgrades();

// Create ECSWorld with screen-based dimensions
ECSWorld(int screen_width, int screen_height, 
         int cell_width, int cell_height);
```

## Save/Load Persistence

The system persists dimension data in save files:

```json
{
  "grid": {
    "floors": 48,
    "columns": 20,
    "ground_floor_index": 12,
    "basement_floors": 12,
    "max_above_ground_floors": 100,
    "max_below_ground_floors": 10
  }
}
```

### Fields

- `floors`: Total number of floors (above + below ground)
- `columns`: Number of horizontal cells
- `ground_floor_index`: Index offset for ground level
- `basement_floors`: Count of basement floors
- `max_above_ground_floors`: Current upgrade limit for above-ground expansion
- `max_below_ground_floors`: Current upgrade limit for basement expansion

### Loading Behavior

1. Dimension limits are restored first
2. Grid is resized to match saved floor/column counts
3. Limits ensure saved dimensions don't exceed maximums
4. Backward compatible with saves missing limit fields

## Integration Example

### Adding a New Floor

```cpp
TowerGrid& grid = ecs_world.GetTowerGrid();

// Check if we can add floors
if (grid.CanAddFloors(1)) {
    int new_floor_index = grid.AddFloor();
    if (new_floor_index >= 0) {
        // Floor added successfully
        std::cout << "Added floor at index: " << new_floor_index << std::endl;
    }
} else {
    // At maximum limit, need to upgrade
    std::cout << "Maximum floor limit reached!" << std::endl;
    std::cout << "Current max: " << grid.GetMaxAboveGroundFloors() << std::endl;
}
```

### Unlocking Vertical Expansion

```cpp
// In research tree menu or game update
ResearchTree& research_tree = ecs_world.GetWorld().get_mut<ResearchTree>();

// Unlock expansion node
if (research_tree.UnlockNode("upward_expansion_1")) {
    // Apply upgrades to grid
    ecs_world.ApplyVerticalExpansionUpgrades();
    
    std::cout << "New height limit: " 
              << ecs_world.GetTowerGrid().GetMaxAboveGroundFloors() 
              << " floors" << std::endl;
}
```

## Design Rationale

### Screen-Based Calculation

Calculating initial dimensions from screen size ensures:
- Grid is always visible and usable on the current display
- Consistent player experience across different resolutions
- Automatic adaptation to window resizing (future enhancement)

### Separate Maximums

The three-tier system (initial/current/absolute) provides:
- **Initial:** Balanced starting point based on display
- **Current:** Upgradeable player progression
- **Absolute:** Technical/gameplay constraints

### Horizontal vs Vertical

Different treatment of dimensions:
- **Horizontal:** Fixed at initialization (no upgrades currently)
  - Reasoning: Screen width already provides adequate space
  - Simpler gameplay (focus on vertical progression)
  
- **Vertical:** Upgradeable through research
  - Reasoning: Core gameplay loop is building upward
  - Provides progression and unlock goals

## Future Enhancements

Potential improvements to consider:

1. **Dynamic Screen Adaptation**
   - Recalculate grid on window resize
   - Preserve existing buildings

2. **Horizontal Expansion Upgrades**
   - Add research nodes for column expansion
   - Enable wider tower designs

3. **Custom Starting Dimensions**
   - Allow player-selected grid size at new game
   - Presets for different difficulty levels

4. **Dimension Costs**
   - Charge money/resources for adding floors
   - Make expansion a strategic decision

5. **Structural Limits**
   - Engineering constraints based on tower width
   - Require foundation upgrades for tall buildings

## Testing Checklist

When modifying the dimension system, verify:

- [ ] Grid initializes with correct screen-based dimensions
- [ ] Initial dimensions never exceed absolute maximums
- [ ] AddFloor/AddBasementFloor respect current limits
- [ ] SetMax methods properly clamp to absolute maximums
- [ ] SetMax methods never reduce below current count
- [ ] Research upgrades apply correctly
- [ ] ApplyVerticalExpansionUpgrades handles multiple unlocks
- [ ] Save/load preserves dimension limits
- [ ] Loading old saves without limit fields works
- [ ] UI shows correct available expansion when at limits

## Related Documentation

- [Research Tree System](RESEARCH_TREE_SYSTEM.md) - Upgrade mechanics
- [Save/Load System](SAVE_LOAD.md) - Persistence
- [Dynamic Floor System](DYNAMIC_FLOOR_SYSTEM.md) - Floor management
