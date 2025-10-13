# HUD Screenshot Reference

DEPRECATED: This file has been merged into `docs/HUD.md`.
See: docs/HUD.md for screenshot references and visual guidance.

---

This document shows what the HUD system looks like when running.

## Main View with HUD

```
┌───────────────────────────────────────────────────────────────────────────────┐
│ $25,000 (+$500/hr)  Population: 125  8:30 AM Day 5  1x                        │
└───────────────────────────────────────────────────────────────────────────────┘
┌─────────────┐  ┌──────────────────────────────────────────────────────────────┐
│BUILD MENU   │  │                                                              │
│─────────────│  │                   TowerForge - HUD Demo                      │
│             │  │                                                              │
│ [L] Lobby   │  │    ┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐ │
│    $1,000   │  │    │LOBBY│LOBBY│LOBBY│LOBBY│LOBBY│LOBBY│LOBBY│LOBBY│LOBBY│ │
│             │  │    └─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘ │
│ [O] Office  │  │    ┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐       │
│    $5,000   │  │    │     │     │OFFIC│OFFIC│OFFIC│OFFIC│OFFIC│OFFIC│       │
│             │  │    └─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘       │
│ [R] Rest.   │  │    ┌─────┬─────┬─────┬─────┬─────┬─────┐                   │
│    $8,000   │  │    │     │     │     │     │     │REST │REST │REST │       │
│             │  │    └─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘       │
│ [S] Shop    │  │    ┌─────┬─────┬─────┬─────┐                               │
│    $6,000   │  │    │     │SHOP │SHOP │SHOP │                               │
│             │  │    └─────┴─────┴─────┴─────┘                               │
│ [H] Hotel   │  │    ┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐       │
│    $12,000  │  │    │     │     │     │HOTEL│HOTEL│HOTEL│HOTEL│HOTEL│       │
│             │  │    └─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘       │
│ [E] Elevator│  │                                                              │
│    $15,000  │  │         ●          ●              ●                          │
│             │  │      (people)                                                │
└─────────────┘  └──────────────────────────────────────────────────────────────┘
                                                         ┌────────────────────────┐
┌────────────────────────────────┐                      │ OFFICE - Floor 5       │
│ ! Low satisfaction on Floor 3  │                      │────────────────────────│
│ + Milestone: 100 population    │                      │ Occupancy: 8/10        │
│ + Income milestone: $50K       │                      │ Revenue: $80/hr        │
└────────────────────────────────┘                      │ Satisfaction: :) 85%   │
                                                         │ Tenants: 8 workers     │
                                                         │                        │
                                  ┌────────────────────┐ │ [Demolish]  [Upgrade]  │
                                  │  ||  1x  2x  4x    │ └────────────────────────┘
                                  └────────────────────┘
```

## Component Breakdown

### 1. Top Bar (Full Width)
Shows critical game stats that are always visible:
- Current funds with income rate
- Total population
- Current time and day number
- Simulation speed multiplier

### 2. Build Menu (Left Side)
Vertical list of buildable facilities:
- Each entry shows an icon, name, and cost
- Click to select a facility type
- Selected facility is highlighted
- Can be toggled visible/hidden

### 3. Game View (Center/Main Area)
The tower grid with facilities and actors:
- Grid cells showing placed facilities (color-coded)
- People/actors as circles
- Elevators as vertical bars
- Click entities to view their info

### 4. Info Panel (Right Side)
Context-sensitive panel showing details:
- Changes based on what's clicked
- Facility info (shown above)
- Person info (when clicking people)
- Elevator info (when clicking elevators)

### 5. Notifications (Bottom Left)
Stack of recent messages:
- Color-coded by type (warning, success, info, error)
- Auto-expire after set duration
- Limited to 5 most recent

### 6. Speed Controls (Bottom Right)
Simulation speed buttons:
- Pause button
- 1x, 2x, 4x speed buttons
- Active button is highlighted

## Example: Person Info Panel

```
┌────────────────────────┐
│ PERSON #42             │
│────────────────────────│
│ State: WaitingElevator │
│ Current: Floor 1       │
│ Destination: Floor 8   │
│ Wait Time: 45s         │
│ Needs: Work            │
│ Satisfaction: :| 60%   │
└────────────────────────┘
```

## Example: Elevator Info Panel

```
┌────────────────────────┐
│ ELEVATOR #1            │
│────────────────────────│
│ Current Floor: 5 ↑     │
│ Occupancy: 6/8         │
│ Next Stop: Floor 8     │
│ Queue Length: 3        │
│ - Floor 1: 2 waiting   │
│ - Floor 3: 1 waiting   │
└────────────────────────┘
```

## Notification Types

### Warning (Orange Background)
```
┌────────────────────────────────┐
│ ! Low satisfaction on Floor 3  │
└────────────────────────────────┘
```

### Success (Green Background)
```
┌────────────────────────────────┐
│ + Milestone: 100 population    │
└────────────────────────────────┘
```

### Info (Sky Blue Background)
```
┌────────────────────────────────┐
│ i Click entities for details   │
└────────────────────────────────┘
```

### Error (Red Background)
```
┌────────────────────────────────┐
│ X Cannot build: Insufficient   │
└────────────────────────────────┘
```

## Speed Controls States

### Normal Speed (1x Active)
```
┌────────────────────┐
│ ||  [1x] 2x  4x    │
└────────────────────┘
```

### Paused
```
┌────────────────────┐
│ [||] 1x  2x  4x    │
│  P                 │
└────────────────────┘
```

### Fast Forward (4x Active)
```
┌────────────────────┐
│ ||  1x  2x  [4x]   │
└────────────────────┘
```

## Visual Hierarchy

1. **Primary Information** (Always Visible)
   - Top bar stats
   - Speed controls

2. **Secondary Information** (On Demand)
   - Info panels (when entity selected)
   - Build menu (can be toggled)

3. **Tertiary Information** (Temporary)
   - Notifications (auto-expire)

## Design Principles

- **Non-intrusive**: Semi-transparent backgrounds
- **Readable**: High contrast text, adequate font sizes
- **Responsive**: Updates in real-time
- **Informative**: Shows all critical game data
- **Organized**: Logical grouping of information
- **Accessible**: Clear visual hierarchy and icons

## Using the HUD

The HUD is integrated into the main `towerforge` application:

1. Run `./bin/towerforge`
2. Observe the top bar showing real-time game stats
3. Click on facilities in the game view to see detailed info panels
4. Click on people to see actor state and destination
5. Use the build menu on the left to select facility types
6. Use speed controls at the bottom right to control simulation
7. Watch for notifications appearing at the bottom left

## Technical Notes

- Rendered using Raylib's immediate-mode GUI functions
- All UI elements are drawn every frame
- No texture assets required (uses primitive shapes and text)
- Color scheme designed for clarity and accessibility
- Layout uses fixed positions but can be adapted for different screen sizes
