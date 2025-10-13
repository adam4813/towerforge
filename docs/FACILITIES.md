# Facilities (Consolidated)

This document consolidates facility-related documentation (basic and advanced) into a single canonical reference. It merges the previous `docs/FACILITIES.md` and `docs/ADVANCED_FACILITIES.md` content.

---

## Overview

Facilities are the building blocks of TowerForge’s gameplay: they provide services, attract visitors and tenants, and generate economic effects. This document describes core facility types, advanced facilities with staffing and operating hours, technical implementation details, rendering conventions, and future enhancements.

Key concepts:
- Each facility is represented by a `BuildingComponent` attached to an ECS entity.
- Facilities have placement rules, capacity, optional staffing, operating hours, and visual identity.
- The `FacilityManager` provides high-level creation and removal APIs.

---

## Core Facility Types

### Office
- Purpose: Commercial office space for workers.
- Gameplay role: Generates rent income; attracts office workers during business hours.
- Default attributes: Width 8, Capacity 20.
- Placement rules: Can be placed on any floor except ground floor (prefer upper floors for offices); requires elevator access for floors above 1.

### Residential
- Purpose: Condominium units for residents.
- Gameplay role: Provides housing and generates steady rent income.
- Default attributes: Width 6, Capacity 4 per unit.
- Placement rules: Not placed on ground floor; residents require access to lobby and facilities.

### RetailShop
- Purpose: Small commercial retail or service outlets.
- Gameplay role: Generates variable income based on foot traffic.
- Default attributes: Width 4, Capacity 15.
- Placement rules: Best placed on lower floors for accessibility; benefits from proximity to residential and lobby.

### Lobby
- Purpose: Main entrance and traffic funnel.
- Gameplay role: Required infrastructure; central access point for visitors and residents.
- Default attributes: Width 10, Capacity 50 (transient).
- Placement rules: Must be placed on ground floor (floor 0); typically unique per tower.

---

## Advanced Facility Types (Staffing, Hours, Costs)

The advanced facilities extend core types with staffing, operating hours, build costs, and gameplay-specific behavior.

### Gym
- Size: 7 tiles wide
- Capacity: 25
- Staff Required: 3
- Operating Hours: 09:00 - 21:00
- Build Cost: $10,000
- Build Time: 18s
- Purpose: Improves tenant happiness; attracts visitors
- Color: Orange

### Arcade
- Size: 5 tiles wide
- Capacity: 20
- Staff Required: 2
- Operating Hours: 09:00 - 21:00
- Build Cost: $9,000
- Build Time: 16s
- Purpose: Entertainment venue
- Color: Magenta

### Theater
- Size: 8 tiles wide
- Capacity: 50
- Staff Required: 3
- Operating Hours: 09:00 - 21:00
- Build Cost: $15,000
- Build Time: 22s
- Purpose: Major entertainment venue
- Color: Violet

### Conference Hall
- Size: 9 tiles wide
- Capacity: 60
- Staff Required: 2
- Operating Hours: 09:00 - 17:00
- Build Cost: $13,000
- Build Time: 20s
- Purpose: Events and meetings
- Color: Light Blue

### Flagship Store
- Size: 12 tiles wide
- Capacity: 40
- Staff Required: 4
- Operating Hours: 09:00 - 21:00
- Build Cost: $18,000
- Build Time: 28s
- Purpose: High-revenue retail
- Color: Turquoise

### Restaurant
- Size: 6 tiles wide
- Capacity: 30
- Staff Required: 4
- Operating Hours: 09:00 - 21:00
- Build Cost: $8,000
- Build Time: 20s
- Purpose: Food service; boosts happiness
- Color: Red

### Hotel
- Size: 10 tiles wide
- Capacity: 40 rooms
- Staff Required: 5
- Operating Hours: 24/7
- Build Cost: $12,000
- Build Time: 25s
- Purpose: Temporary lodging; generates guest income
- Color: Dark Blue

---

## Staffing System

Facilities that require staff have the following mechanics:

- `current_staff`: number of staff currently assigned
- `job_openings`: number of unfilled positions
- `GetRequiredEmployees()` returns the minimum required staff for the facility type
- `IsOperational(current_hour)` returns true if the facility has enough staff and is within operating hours

Operational rules:
- A facility does not provide benefits or generate revenue unless `IsOperational()` is true.
- Understaffed facilities show an inactive state in the UI and may decrease nearby tenant satisfaction.
- Staff are hired via the employment system and assigned to facilities; the UI exposes job openings.

Staff roles example table:

| Facility Type | Required Staff | Roles |
|--------------|----------------|-------|
| Restaurant | 4 | Cooks, Servers |
| Hotel | 5 | Receptionists, Cleaners |
| Gym | 3 | Trainers, Attendants |
| Arcade | 2 | Clerks |
| Theater | 3 | Ushers, Staff |
| Conference Hall | 2 | Event Coordinators |
| Flagship Store | 4 | Shop Staff |
| Retail Shop | 2 | Shop Staff |
| Office | capacity/5 | Office Workers |

---

## Operating Hours & Behavior

- Default business hours: 09:00 - 17:00
- Entertainment/retail: extended hours 09:00 - 21:00
- Hotels: always operational

Facilities outside operating hours do not operate, even if fully staffed.

---

## Technical Implementation

### BuildingComponent
All facilities are modeled by the `BuildingComponent` in `include/core/components.hpp`. Example fields:

```cpp
struct BuildingComponent {
    enum class Type { Lobby, Office, Residential, RetailShop, Restaurant, Hotel, Elevator, Gym, Arcade, Theater, ConferenceHall, FlagshipStore } type;
    int floor;              // 0-based floor index
    int column;             // grid column
    int width;              // width in grid cells (0 = use default)
    int capacity;           // max occupancy or room count
    int current_occupancy;  // current number of people
    int current_staff;      // staff assigned
    int job_openings;       // unfilled roles
    float operating_start_hour; // e.g., 9.0
    float operating_end_hour;   // e.g., 21.0
};
```

### FacilityManager API

Use `FacilityManager` to create and remove facilities. Example usage:

```cpp
auto& facility_mgr = ecs_world.GetFacilityManager();

// Create lobby with explicit width
auto lobby = facility_mgr.CreateFacility(
    BuildingComponent::Type::Lobby,
    0,      // floor
    0,      // column
    10,     // width (or 0 for default)
    "MainLobby"  // optional name
);

// Create advanced facility (Gym)
auto gym = facility_mgr.CreateFacility(
    BuildingComponent::Type::Gym,
    3,      // floor
    5       // column
);

// Remove facility
facility_mgr.RemoveFacility(gym);
facility_mgr.RemoveFacilityAt(3, 5);
```

### Operational checks

```cpp
bool operational = building.IsOperational(current_hour);
int required = building.GetRequiredEmployees();
```

---

## Visual Design & Colors

Each facility type maps to a color used by the renderer and HUD to help players identify types quickly.

| Facility Type | Color | Description |
|---------------|-------|-------------|
| Office | SKYBLUE | Professional, clean look |
| Residential | PURPLE | Warm, homey feel |
| RetailShop | GREEN | Commercial, vibrant |
| Lobby | GOLD | Prestigious, welcoming |
| Restaurant | RED | Energetic, food-related |
| Hotel | DARKBLUE | Luxurious, calm |
| Elevator | GRAY | Utilitarian, neutral |
| Gym | ORANGE | Energetic, fitness |
| Arcade | MAGENTA | Playful, bright |
| Theater | VIOLET | Rich, dramatic |
| Conference Hall | LIGHTBLUE | Formal, business |
| Flagship Store | TURQUOISE | Premium retail |

Rendering tips:
- Use consistent color constants from the renderer palette
- Show inactive/understaffed facilities with a desaturated overlay
- Display occupancy and staff counts in info panels

---

## Testing & Validation

Test checklist:
- Create each facility type via the build menu and verify default attributes
- Ensure staffing requirements cause facilities to be non-operational when understaffed
- Confirm operating hours are enforced
- Verify color mapping and rendering overlays
- Check placement rules (e.g., Lobby must be floor 0)

Automated tests (if present) should validate `FacilityManager` defaults and `BuildingComponent` helper methods.

---

## Future Enhancements

Planned additions:
- Economic simulation (detailed rent, income, operating costs)
- Facility upgrades and customization
- Adjacency bonuses and placement synergies
- Staff scheduling and shift management
- Staff skills/levels and training
- Inventory management for retail/restaurants
- Noise and environmental effects
- Placeholder CC0 audio/visual assets for CI/demo runs

---

## References

- `include/core/components.hpp` — `BuildingComponent` definition
- `src/core/facility_manager.cpp` — facility defaults and creation logic
- `src/ui/build_menu.cpp` — integration with the UI build menu

---

This file is the consolidated facilities reference. The older split files (`docs/FACILITIES.md` and `docs/ADVANCED_FACILITIES.md`) were merged into this document; if you prefer, archive the historical files to `docs/deprecated_archive/` to avoid duplication.
