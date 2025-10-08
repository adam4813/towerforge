# Research and Upgrade Tree System

## Overview

The Research and Upgrade Tree system allows players to spend research points to unlock new facility types, improve elevator performance, and gain various bonuses that enhance gameplay.

## Features

### Research Tree Structure

The research tree is organized in a 3-row grid layout with progressively more powerful upgrades:

- **Row 0 (Basic Tier)**: Starter upgrades available immediately
- **Row 1 (Mid Tier)**: Intermediate upgrades requiring basic prerequisites
- **Row 2 (Advanced Tier)**: Powerful end-game upgrades

### Node Types

1. **Facility Unlocks**: Unlock new building types
   - Office Spaces
   - Retail Shops
   - (More can be added)

2. **Elevator Improvements**:
   - Fast Elevators: +50% speed
   - Express Elevators: +100% speed
   - Large Elevators: +4 capacity

3. **Economic Bonuses**:
   - Revenue Optimization: +25% income
   - Efficient Building: -20% costs

4. **Operational Improvements**:
   - Rapid Construction: +50% build speed
   - Quality Service: +10 satisfaction points

### Research Points

Research points are automatically awarded based on milestones:

- **Tenant Milestones**: 5 points per 10 tenants
- **Income Milestones**: 3 points per $1,000/hour income

Players can also receive starting research points when beginning a new game.

### Node States

Each research node can be in one of three states:

- **🔒 Locked**: Prerequisites not met or insufficient points
- **✨ Upgradable**: Can be unlocked (prerequisites met, enough points)
- **✅ Unlocked**: Already unlocked and providing benefits

## User Interface

### Opening the Research Tree

- Press **R** during gameplay to toggle the research tree menu
- Press **ESC** while in the research tree to close it

### Menu Layout

```
┌─────────────────────────────────────────────────────────────┐
│                 RESEARCH/UPGRADE TREE                       │
│ Research Points: 50            Total Earned: 100            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌────┐  ┌────┐  ┌────┐                                   │
│  │ 🚀 │  │ 🏢 │  │ 🏪 │     Row 0: Basic Tier             │
│  │ 10 │  │  5 │  │  5 │                                   │
│  └────┘  └────┘  └────┘                                   │
│     ↑       ↑       ↑                                      │
│  ┌────┐  ┌────┐  ┌────┐                                   │
│  │ ⚡ │  │ 📦 │  │ 💰 │     Row 1: Mid Tier              │
│  │ 20 │  │ 15 │  │ 15 │                                   │
│  └────┘  └────┘  └────┘                                   │
│                                                             │
│  ┌────┐  ┌────┐  ┌────┐                                   │
│  │ 🏗️ │  │ 📉 │  │ 😊 │     Row 2: Advanced Tier         │
│  │ 25 │  │ 30 │  │ 25 │                                   │
│  └────┘  └────┘  └────┘                                   │
│                                                             │
│ ┌─────────────────────────┐                                │
│ │ Node Details:           │  (Hover over node to see)      │
│ │                         │                                │
│ │ Fast Elevators          │                                │
│ │ Status: Unlocked ✅     │                                │
│ │ Cost: 10 points         │                                │
│ │                         │                                │
│ │ Description:            │                                │
│ │ Increases elevator      │                                │
│ │ speed by 50%            │                                │
│ │                         │                                │
│ │ Effect:                 │                                │
│ │ +50% elevator speed     │                                │
│ └─────────────────────────┘                                │
└─────────────────────────────────────────────────────────────┘
```

### Interacting with Nodes

1. **Hover** over a node to see detailed information in the details panel
2. **Click** on an upgradable node (✨) to unlock it
3. Unlocked nodes (✅) provide immediate benefits
4. Locked nodes (🔒) show prerequisites and costs

## Default Research Tree

### Row 0 - Basic Tier

| Node | Name | Type | Cost | Effect |
|------|------|------|------|--------|
| 🚀 | Fast Elevators | Elevator Speed | 10 | +50% elevator speed |
| 🏢 | Office Spaces | Facility Unlock | 5 | Unlock office facilities |
| 🏪 | Retail Shops | Facility Unlock | 5 | Unlock retail shops |

### Row 1 - Mid Tier

| Node | Name | Type | Cost | Prerequisites | Effect |
|------|------|------|------|---------------|--------|
| ⚡ | Express Elevators | Elevator Speed | 20 | Fast Elevators | +100% elevator speed |
| 📦 | Large Elevators | Elevator Capacity | 15 | Fast Elevators | +4 capacity |
| 💰 | Revenue Optimization | Income Bonus | 15 | Office Spaces | +25% income |

### Row 2 - Advanced Tier

| Node | Name | Type | Cost | Prerequisites | Effect |
|------|------|------|------|---------------|--------|
| 🏗️ | Rapid Construction | Construction Speed | 25 | - | +50% build speed |
| 📉 | Efficient Building | Cost Reduction | 30 | - | -20% costs |
| 😊 | Quality Service | Satisfaction Bonus | 25 | Revenue Optimization | +10 satisfaction |

## Save/Load Support

Research progress is automatically saved and restored:

- Unlocked nodes and their states
- Research points (available and total earned)
- Global bonuses applied (multipliers, bonuses)

When loading a save:
1. The research tree is reinitialized with default nodes
2. Previously unlocked nodes are restored
3. Bonuses are reapplied automatically
4. Node states are updated based on current conditions

## Implementation Details

### Components

**ResearchNode**: Individual node in the tree
```cpp
struct ResearchNode {
    std::string id;
    std::string name;
    std::string description;
    std::string icon;
    ResearchNodeType type;
    ResearchNodeState state;
    int cost;
    std::vector<std::string> prerequisites;
    int grid_row;
    int grid_column;
    float effect_value;
    std::string effect_target;
};
```

**ResearchTree**: Singleton managing the entire tree
```cpp
struct ResearchTree {
    int research_points;
    int total_points_earned;
    std::vector<ResearchNode> nodes;
    
    // Global bonuses
    float income_multiplier;
    float satisfaction_bonus;
    float construction_speed_multiplier;
    float cost_reduction;
    float elevator_speed_multiplier;
    int elevator_capacity_bonus;
};
```

### Systems

**Research Points Award System**: Awards points based on milestones
- Runs every 5 seconds
- Checks tenant count and income milestones
- Awards points automatically when milestones are reached

### API

**Unlocking Nodes**:
```cpp
ResearchTree& research = world.get_mut<ResearchTree>();
bool success = research.UnlockNode("node_id");
```

**Checking Facility Unlocks**:
```cpp
bool unlocked = research.IsFacilityUnlocked("Office");
```

**Awarding Points**:
```cpp
research.AwardPoints(10);
```

## Future Enhancements

Potential improvements:

1. **Dynamic Trees**: Allow multiple tech tree configurations
2. **More Node Types**: Add new upgrade categories
3. **Visual Effects**: Add animations when unlocking nodes
4. **Achievements**: Special nodes for completing challenges
5. **Research Specializations**: Branch paths for different playstyles
6. **Research Speed**: Upgrades that increase point generation rate
7. **Reset/Respec**: Allow players to reset tree and refund points
8. **Tooltips**: Enhanced tooltips with statistics and recommendations

## Files

### Headers
- `include/core/components.hpp` - ResearchNode and ResearchTree structures
- `include/ui/research_tree_menu.h` - UI menu class

### Implementation
- `src/ui/research_tree_menu.cpp` - UI rendering and interaction
- `src/core/ecs_world.cpp` - Research points award system
- `src/core/save_load_manager.cpp` - Serialization/deserialization

### Demo
- `research_tree_demo.png` - Screenshot of the UI

## License

Part of TowerForge - see main project LICENSE.
