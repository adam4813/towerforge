# HUD Screenshot Reference

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

## Demo Controls

To explore the HUD in the demo application:

1. Run `./bin/hud_demo`
2. Press **1** to show Facility Info
3. Press **2** to show Person Info
4. Press **3** to show Elevator Info
5. Press **4** to hide all panels
6. Press **N** to add notifications
7. Press **SPACE** to pause/resume
8. Click build menu items to select facilities
9. Click game area to interact

## Technical Notes

- Rendered using Raylib's immediate-mode GUI functions
- All UI elements are drawn every frame
- No texture assets required (uses primitive shapes and text)
- Color scheme designed for clarity and accessibility
- Layout uses fixed positions but can be adapted for different screen sizes
