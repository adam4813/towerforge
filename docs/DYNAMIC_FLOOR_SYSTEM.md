# Dynamic Floor and Column Management System

## Overview

TowerForge now features a dynamic floor and column management system that allows players to expand their tower vertically (both up and down) and horizontally. This system replaces the previous fixed grid approach with a more flexible, cost-based expansion mechanic.

## Key Features

### 1. Dynamic Floor Expansion

- **Upward Growth**: Add new floors above the current top floor
- **Basement Construction**: Add basement floors below ground level (negative indices)
- **Floor Building State**: Track which floor cells are constructed vs. just allocated
- **Automatic Floor Building**: Floors are automatically built when placing facilities
- **Cost-Based System**: Each floor cell has a construction cost (default: $50 per cell)

### 2. Starting Configuration

The tower now starts with minimal construction:
- **1 Ground Floor** (index 0) - fully built by default
- **20 Columns** - available but not built (configurable, default constructor uses 10)
- **No Pre-filled Facilities** - clean slate for player creativity

### 3. Floor Building Mechanics

#### Implicit Floor Building
When placing a facility on an unbuilt floor position:
- The floor cells under the facility are automatically marked as built
- The total cost includes: **facility cost + (floor build cost × width)**
- Example: Placing an 8-cell office on unbuilt floor costs: $5000 (office) + ($50 × 8) = $5400

#### Explicit Floor Expansion
Players can manually expand the tower using UI controls:
- **Add Floor (+)**: Adds one floor above, costs $50 × column_count
- **Add Basement (-)**: Adds one basement below ground, costs $50 × column_count

### 4. Visual Indicators

The grid rendering shows the building state:
- **Built Floor Cells**: Solid white outline (opacity 0.2)
- **Unbuilt Floor Cells**: Faded dark gray outline (opacity 0.1)
- **Occupied Cells**: Colored rectangles (facility type dependent)

## Technical Implementation

### Core Classes

#### `TowerGrid`
Main grid management class with basement support:

```cpp
// Constructor with ground floor index support
TowerGrid(int initial_floors = 1, int initial_columns = 10, int ground_floor_index = 0);
// Note: In-game, the tower is typically initialized with 20 columns:
// TowerGrid(1, 20, 0)

// Floor expansion
int AddFloor();                    // Add floor above
int AddBasementFloor();            // Add basement below
bool RemoveTopFloor();             // Remove if empty
bool RemoveBottomFloor();          // Remove basement if empty

// Floor building state
bool BuildFloor(int floor, int start_column = 0, int width = -1);
bool IsFloorBuilt(int floor, int column) const;
bool IsEntireFloorBuilt(int floor) const;
static int GetFloorBuildCost();    // Returns $50

// Index conversion (handles negative basement indices)
int FloorToGridIndex(int floor) const;
int GridIndexToFloor(int grid_index) const;
```

#### `GridCell`
Enhanced to track building state:

```cpp
struct GridCell {
    bool occupied = false;         // Contains a facility
    int facility_id = -1;          // Entity ID
    bool floor_built = false;      // Floor is constructed
};
```

#### `FacilityManager`
Updated to handle floor building:

```cpp
// Calculate cost to build floors for facility
int CalculateFloorBuildCost(int floor, int column, int width) const;

// Build floors for facility placement
bool BuildFloorsForFacility(int floor, int column, int width);
```

### Floor Indexing System

The system supports basement floors through a flexible indexing scheme:

- **Positive Indices**: Above-ground floors (0, 1, 2, ...)
- **Negative Indices**: Basement floors (-1, -2, -3, ...)
- **Ground Level**: Floor index 0 (configurable via `ground_floor_index_`)

Internal conversion:
```cpp
// Floor -1 (first basement) → grid index 0
// Floor 0 (ground) → grid index 1
// Floor 1 → grid index 2
grid_index = floor - (ground_floor_index_ - basement_floors_)
```

## UI Integration

### Build Menu

New section "EXPANSION" added with two buttons:
- **Add Floor (+)**: Costs $50 × column_count
- **Add Basement (-)**: Costs $50 × column_count

Returns special codes from `HandleClick()`:
- `-5`: Add Floor action
- `-6`: Add Basement action

### In-Game Notifications

Players receive feedback on:
- Successful floor/basement addition with cost
- Insufficient funds warnings
- Facility placement with floor building costs

## Gameplay Implications

### Strategic Decisions

1. **Early Game**: Start with minimal floors, expand as needed
2. **Cost Management**: Balance facility construction with floor expansion
3. **Vertical Planning**: Choose between adding floors up or basements down
4. **Horizontal Growth**: Columns are cheap to add but require building before use

### Economic Balance

- **Floor Cell Cost**: $50 per cell
- **Typical Floor (20 columns as in-game)**: $1,000 to build completely
- **Large Facility (10 cells wide)**: Adds $500 if floor unbuilt
- **Starting Funds**: $10,000 (can afford 10 full floors or 20 smaller facilities)

Note: Column count is configurable. The default constructor uses 10 columns, but the game initializes with 20 columns.

## Future Enhancements

Potential improvements for future iterations:

1. **Column Management**: Add/remove columns dynamically
2. **Floor Removal**: Remove empty floors to reclaim some costs
3. **Building Requirements**: Enforce continuous construction (no gaps)
4. **Elevator Integration**: Require elevators to access higher floors
5. **Structural Limits**: Maximum height/depth based on foundation
6. **Basement Utilities**: Special basement-only facilities (parking, utilities)
7. **Multi-story Buildings**: Facilities that span multiple floors

## Migration Notes

### From Fixed Grid

The old system:
- Started with 10 pre-allocated floors
- All floors were "built" by default
- No expansion mechanism

The new system:
- Starts with 1 ground floor
- Floors must be built explicitly or implicitly
- Supports unlimited expansion (within game limits)

### Save Game Compatibility

When loading old saves:
- All existing floor cells are marked as `floor_built = true`
- No basement floors initially
- Ground floor index defaults to 0

## Examples

### Example 1: New Game Start

```cpp
// Initial state (in-game configuration)
TowerGrid grid(1, 20, 0);  // 1 floor, 20 columns, ground at 0
// Only ground floor (0) is built
// No facilities yet
```

### Example 2: Building First Facility

```cpp
// Player places Lobby (10 cells) on ground floor
CreateFacility(Lobby, floor=0, column=0, width=10);
// Cost: $1000 (lobby) + $0 (floor already built)
// Ground floor cells 0-9 remain built
```

### Example 3: Expanding Upward

```cpp
// Player adds floor above
grid.AddFloor();  
// Cost: $50 × 20 = $1000
// Creates floor 1 (unbuilt cells)

// Player places Office (8 cells) on new floor
CreateFacility(Office, floor=1, column=0, width=8);
// Cost: $5000 (office) + $400 (8 floor cells)
// Floor 1 cells 0-7 now built
```

### Example 4: Adding Basement

```cpp
// Player adds basement
grid.AddBasementFloor();
// Cost: $50 × columns (20 in this case) = $1000
// Creates floor -1 (unbuilt cells)

// Player places parking facility
CreateFacility(Parking, floor=-1, column=5, width=15);
// Cost: $8000 (parking) + $750 (15 floor cells)
```

## Performance Considerations

- **Memory**: Minimal overhead (1 bool per grid cell)
- **Rendering**: Visual distinction between built/unbuilt is lightweight
- **Grid Resizing**: O(n) where n = columns, but infrequent operation
- **Index Conversion**: O(1) constant time operation

## Testing Checklist

- [x] Tower starts with 1 floor built
- [x] Can add floors above
- [x] Can add basements below
- [x] Floor building costs calculated correctly
- [x] Placing facility auto-builds floors
- [x] Visual indicators show built vs unbuilt
- [x] UI buttons for manual expansion work
- [x] Insufficient funds handled gracefully
- [ ] Floor removal works correctly
- [ ] Save/load preserves floor state
- [ ] Multi-floor facilities work correctly

## License

Part of TowerForge - see main project LICENSE.
