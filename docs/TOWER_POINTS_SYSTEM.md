# Tower Points & Management Facilities System

## Overview

The Tower Points system is a meta-progression mechanic that powers research and building unlocks in TowerForge. Instead of earning research points through generic milestones, players generate **Tower Points** by hiring management staff and building management facilities.

## Key Concepts

### Tower Points

**Tower Points** are the currency used to unlock research nodes. Unlike the old milestone-based system, Tower Points are generated continuously based on your tower's management infrastructure.

**Generation Rate:**
- Base: 1 Tower Point per hour per management staff member
- Management staff work in ManagementOffice and SatelliteOffice facilities
- Points accumulate over game time and can be spent on research

### Management Facilities

Two new facility types generate Tower Points:

1. **Management Office** (Main Tower Management)
   - Requires: 5 staff (executives, managers, analysts)
   - Width: 8 tiles
   - Generates base tower points from staff
   - Required to unlock advanced management features

2. **Satellite Office** (Branch Management)
   - Requires: 3 staff (managers, analysts)
   - Width: 6 tiles
   - Smaller management office
   - Can be used as prerequisite for certain research nodes

### Management Staff

Staff working in management facilities generate Tower Points:
- **Executives**: Senior leadership making strategic decisions
- **Managers**: Mid-level managers overseeing operations
- **Analysts**: Data analysts supporting decision-making

Each staff member generates 1 Tower Point per hour (game time).

## Research System Enhancements

### Conditional Prerequisites

Research nodes can now have multiple types of prerequisites:

1. **Research Prerequisites**: Other research nodes that must be unlocked first
   ```
   prerequisites = {"office_unlock", "shop_unlock"}
   ```

2. **Star Rating Requirement**: Minimum tower star rating needed
   ```
   min_star_rating = 3  // Requires 3-star tower
   ```

3. **Population Requirement**: Minimum tower population needed
   ```
   min_population = 100  // Requires 100 people in tower
   ```

4. **Facility Requirements**: Specific facilities that must exist
   ```
   required_facilities = {"Restaurant", "Hotel", "ManagementOffice"}
   ```

### Node States

Research nodes can be in one of four states:

- **Hidden** (❓): Prerequisites for visibility not met
- **Locked** (🔒): Visible but cannot unlock yet (insufficient points or prerequisites)
- **Upgradable** (✨): Can be unlocked (all prerequisites met, enough tower points)
- **Unlocked** (✅): Already unlocked and providing benefits

## Lua API for Research Nodes

Mods can register custom research nodes via Lua:

```lua
TowerForge.RegisterResearchNode({
    -- Required fields
    id = "unique_node_id",
    name = "Display Name",
    type = "FacilityUnlock",  -- Or "IncomeBonus", "ElevatorSpeed", etc.
    
    -- Cost and positioning
    cost = 30,              -- Tower points required
    grid_row = 1,           -- Row in research tree (0-2)
    grid_column = 3,        -- Column in research tree
    
    -- Optional metadata
    description = "Detailed description of the upgrade",
    icon = "🏨",           -- Emoji or character to display
    
    -- Effect configuration
    effect_value = 0.25,   -- Numeric effect (meaning depends on type)
    effect_target = "Hotel", -- Target for effect (e.g., facility type)
    
    -- Prerequisites
    prerequisites = {"office_unlock"},  -- Other research nodes required
    
    -- Conditional prerequisites
    min_star_rating = 2,               -- Minimum tower star rating
    min_population = 50,                -- Minimum population
    required_facilities = {"Restaurant"} -- Facilities that must exist
})
```

### Research Node Types

Available types for research nodes:

- **FacilityUnlock**: Unlocks a new building type (set `effect_target` to facility name)
- **ElevatorSpeed**: Increases elevator speed (set `effect_value` to multiplier, e.g., 0.5 for +50%)
- **ElevatorCapacity**: Increases elevator capacity (set `effect_value` to bonus capacity)
- **IncomeBonus**: Increases all income (set `effect_value` to multiplier, e.g., 0.25 for +25%)
- **SatisfactionBonus**: Increases tenant satisfaction (set `effect_value` to points)
- **ConstructionSpeed**: Faster construction (set `effect_value` to multiplier)
- **CostReduction**: Reduces costs (set `effect_value` to reduction %, e.g., 0.2 for 20% off)

## Example: Hotel Unlock

Here's a complete example of unlocking hotels through research:

```lua
-- Hotel unlock requires:
-- - 30 Tower Points
-- - Office unlock research completed
-- - 2-star tower rating
-- - At least 50 people in tower

TowerForge.RegisterResearchNode({
    id = "hotel_unlock",
    name = "Luxury Lodging",
    description = "Unlock hotel facilities for temporary guests",
    icon = "🏨",
    type = "FacilityUnlock",
    cost = 30,
    grid_row = 1,
    grid_column = 3,
    effect_target = "Hotel",
    prerequisites = {"office_unlock"},
    min_star_rating = 2,
    min_population = 50,
    required_facilities = {}
})
```

## Gameplay Flow

1. **Early Game:**
   - Build basic facilities (Lobby, Office, Residential)
   - Hire initial staff for offices
   - No Tower Points yet - basic facilities don't require research

2. **Mid Game:**
   - Build your first ManagementOffice
   - Hire 5 management staff (executives, managers, analysts)
   - Start generating Tower Points (5 points/hour with 5 staff)
   - Unlock basic research nodes (Office, Shop)
   - Reach 2-star rating and 50 population

3. **Late Game:**
   - Build SatelliteOffice for more management capacity
   - Hire more management staff to accelerate Tower Points generation
   - Unlock advanced buildings (Hotel, Gym, Theater)
   - Meet high star rating and population requirements
   - Unlock powerful bonuses (income, cost reduction, efficiency)

## Design Philosophy

### Why Tower Points?

The Tower Points system:

1. **Creates meaningful choices**: Players must decide when to invest in management vs. revenue-generating facilities
2. **Rewards infrastructure**: Building management capabilities pays off through faster progression
3. **Scales with tower size**: Larger towers can afford more management, generating points faster
4. **Ties to theme**: Research is driven by your tower's management expertise, not arbitrary milestones
5. **Extensible**: Mods can add new research nodes with complex prerequisites

### Balancing Progression

- **Early Unlock**: Basic facilities (Office, Shop, Residential) are unlocked by default
- **Management Investment**: Mid-tier facilities require investing in management infrastructure
- **Compound Requirements**: Advanced facilities need Tower Points + star rating + population + other facilities
- **Strategic Timing**: Players balance when to expand management vs. when to spend points on research

## Implementation Details

### Core Components

**ResearchTree** singleton:
```cpp
struct ResearchTree {
    int tower_points;                  // Available points to spend
    int management_staff_count;        // Total management staff
    float tower_points_per_hour;       // Generation rate
    // ... global bonuses from research
};
```

**ResearchNode** with conditional prerequisites:
```cpp
struct ResearchNode {
    std::string id;
    std::string name;
    ResearchNodeType type;
    int cost;  // Tower points required
    
    // Conditional prerequisites
    int min_star_rating;
    int min_population;
    std::vector<std::string> required_facilities;
    // ...
};
```

### Systems

**Tower Points Generation** (runs every second):
- Counts management staff in ManagementOffice and SatelliteOffice facilities
- Generates points based on: `staff_count * 1 point/hour * elapsed_time`
- Updates available Tower Points

**Node State Updates**:
- Checks all prerequisites (research, star rating, population, facilities)
- Updates node state: Hidden → Locked → Upgradable → Unlocked

## API Reference

### C++ API

```cpp
// Award tower points manually
ResearchTree& research = world.get_mut<ResearchTree>();
research.AwardPoints(10);

// Generate points from management staff (called automatically)
research.GenerateTowerPoints(delta_hours);

// Update management staff count (called automatically)
research.UpdateManagementStaffCount(staff_count);

// Check if node can be unlocked
bool can_unlock = research.CanUnlock(node, star_rating, population, facilities);

// Check if node is visible
bool visible = research.IsVisible(node, star_rating, population);

// Unlock a node
bool success = research.UnlockNode("node_id");
```

### Lua API

```lua
-- Register a research node
TowerForge.RegisterResearchNode({
    id = "node_id",
    name = "Node Name",
    type = "FacilityUnlock",
    cost = 30,
    -- ... other fields
})

-- Log messages for debugging
TowerForge.Log("Research node registered!")
```

## Future Enhancements

Potential expansions to the system:

1. **Management Upgrades**: Research nodes that increase Tower Points generation rate
2. **Specialization Paths**: Different types of management (Finance, Operations, HR) with unique bonuses
3. **Management Tiers**: Executive Suite vs. regular office with different generation rates
4. **Research Speed Modifiers**: Facilities or upgrades that boost point generation
5. **Dynamic Trees**: Multiple research trees for different playstyles
6. **Achievement Integration**: Special research nodes unlocked by achievements

## Files Modified

### Core Implementation
- `include/core/components.hpp`: ResearchTree, ResearchNode with conditional prerequisites
- `src/core/ecs_world.cpp`: Tower Points generation system
- `src/core/facility_manager.cpp`: ManagementOffice and SatelliteOffice support

### Lua Integration
- `include/core/lua_mod_manager.hpp`: Lua_RegisterResearchNode declaration
- `src/core/lua_mod_manager.cpp`: Lua_RegisterResearchNode implementation

### Example Mods
- `mods/example_hotel_research.lua`: Hotel unlock example
- `mods/example_advanced_research.lua`: Advanced research nodes with complex prerequisites

## License

Part of TowerForge - see main project LICENSE.
