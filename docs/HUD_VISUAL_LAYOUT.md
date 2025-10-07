# HUD Visual Layout

## Screen Layout Overview

```
┌────────────────────────────────────────────────────────────────────────┐
│ TOP BAR (40px height)                                                  │
│ $25,000 (+$500/hr) | Population: 125 | 8:30 AM Day 5 | 1x             │
└────────────────────────────────────────────────────────────────────────┘
┌──────────────────┐┌────────────────────────────────────────────────────┐
│  BUILD MENU      ││  GAME VIEW (Main Tower Display Area)              │
│  ────────────    ││                                                    │
│  [L] Lobby       ││                                                    │
│      $1,000      ││                                                    │
│                  ││                                                    │
│  [O] Office      ││                                                    │
│      $5,000      ││                                                    │
│                  ││                                                    │
│  [R] Restaurant  ││                                                    │
│      $8,000      ││                                                    │
│                  ││                                                    │
│  [S] Shop        ││                                                    │
│      $6,000      ││                                                    │
│                  ││                                                    │
│  [H] Hotel       ││                                                    │
│      $12,000     ││                                                    │
│                  ││                                                    │
│  [E] Elevator    ││                                                    │
│      $15,000     ││                                                    │
│                  ││                                                    │
│  Click to select ││                                                    │
└──────────────────┘└────────────────────────────────────────────────────┘
                                              ┌──────────────────────────┐
┌─────────────────────────────────┐          │ INFO PANEL (right side)  │
│ NOTIFICATIONS (bottom left)     │          │ ──────────────           │
│ ─────────────                   │          │ OFFICE - Floor 5         │
│ ! Low satisfaction on Floor 3   │          │ Occupancy: 8/10          │
│ + Milestone: 100 population     │          │ Revenue: $80/hr          │
│ + Income milestone: $50K        │          │ Satisfaction: :) 85%     │
└─────────────────────────────────┘          │ Tenants: 8 workers       │
                                              │                          │
                                              │ [Demolish]  [Upgrade]    │
                                              └──────────────────────────┘
                    ┌────────────────────────┐
                    │ SPEED CONTROLS         │
                    │ || ▶ ▶▶ ▶▶▶           │
                    │ P  1x 2x  4x           │
                    └────────────────────────┘
```

## Component Details

### Top Bar (Always Visible)
- **Left Section**: Funds with income/expense rate
  - Example: "$25,000 (+$500/hr)" in GREEN
  - Shows current balance and hourly rate
  
- **Center Section**: Population count
  - Example: "Population: 125" in WHITE
  - Total number of people in the tower

- **Right-Center**: Current time and day
  - Example: "8:30 AM Day 5" in SKY BLUE
  - 12-hour format with AM/PM

- **Far Right**: Speed indicator
  - Example: "1x" in YELLOW
  - Shows "PAUSED" in RED when simulation is paused

### Build Menu (Left Side, Toggleable)
- Semi-transparent black background
- 200px wide
- Shows all buildable facilities
- Each entry displays:
  - Icon/color box representing the facility
  - Facility name
  - Construction cost
- Selected facility is highlighted
- Located just below top bar

### Info Panels (Right Side, Context-Sensitive)
- Appears when clicking entities
- 250px wide
- Three types:

#### Facility Info Panel (Sky Blue Border)
```
┌────────────────────────┐
│ OFFICE - Floor 5       │ (Title in WHITE)
├────────────────────────┤
│ Occupancy: 8/10        │ (LIGHT GRAY)
│ Revenue: $80/hr        │ (GREEN)
│ Satisfaction: :) 85%   │ (LIGHT GRAY)
│ Tenants: 8 workers     │ (LIGHT GRAY)
│                        │
│ [Demolish] [Upgrade]   │ (Action buttons)
└────────────────────────┘
```

#### Person Info Panel (Yellow Border)
```
┌────────────────────────┐
│ PERSON #42             │ (Title in WHITE)
├────────────────────────┤
│ State: WaitingElevator │ (LIGHT GRAY)
│ Current: Floor 1       │ (LIGHT GRAY)
│ Destination: Floor 8   │ (LIGHT GRAY)
│ Wait Time: 45s         │ (RED if > 30s)
│ Needs: Work            │ (LIGHT GRAY)
│ Satisfaction: :| 60%   │ (LIGHT GRAY)
└────────────────────────┘
```

#### Elevator Info Panel (Purple Border)
```
┌────────────────────────┐
│ ELEVATOR #1            │ (Title in WHITE)
├────────────────────────┤
│ Current Floor: 5 ↑     │ (LIGHT GRAY)
│ Occupancy: 6/8         │ (LIGHT GRAY)
│ Next Stop: Floor 8     │ (LIGHT GRAY)
│ Queue Length: 3        │ (LIGHT GRAY)
│ - Floor 1: 2 waiting   │ (GRAY, indented)
│ - Floor 3: 1 waiting   │ (GRAY, indented)
└────────────────────────┘
```

### Notifications (Bottom Left)
- Stack vertically from bottom
- 300px wide, 30px height each
- Maximum 5 notifications shown
- Auto-fade after duration
- Color-coded by type:
  - **Warning** (ORANGE): Important alerts
  - **Success** (GREEN): Achievements, milestones
  - **Info** (SKY BLUE): General information
  - **Error** (RED): Critical problems

Example:
```
┌─────────────────────────────────┐
│ ! Low satisfaction on Floor 3   │ (ORANGE)
│ + Milestone: 100 population     │ (GREEN)
│ + Income milestone: $50K        │ (GREEN)
└─────────────────────────────────┘
```

### Speed Controls (Bottom Right)
- 200px wide, 40px height
- Four buttons:
  - **||** (Pause) - Stops simulation, turns RED when active
  - **1x** - Normal speed, turns GREEN when active
  - **2x** - Double speed, turns GREEN when active
  - **4x** - Quadruple speed, turns GREEN when active
- Inactive buttons are DARK GRAY
- Located at bottom right corner

## Color Palette

### Background Colors
- **Top Bar**: `ColorAlpha(BLACK, 0.7f)` - Semi-transparent black
- **Panels**: `ColorAlpha(BLACK, 0.8f)` - Darker semi-transparent
- **Build Menu**: `ColorAlpha(BLACK, 0.8f)`

### Accent Colors
- **Top Bar Border**: `GOLD`
- **Facility Panel Border**: `SKYBLUE`
- **Person Panel Border**: `YELLOW`
- **Elevator Panel Border**: `PURPLE`

### Text Colors
- **Primary Text**: `WHITE` (titles, main information)
- **Secondary Text**: `LIGHTGRAY` (details, descriptions)
- **Positive Values**: `GREEN` (funds, revenue, success)
- **Negative/Warning**: `RED` (low satisfaction, errors, pause)
- **Neutral Info**: `SKYBLUE` (time, info messages)
- **Special Values**: `YELLOW` (speed indicator, person selection)

### Facility Colors (in Build Menu)
- **Lobby**: `GOLD`
- **Office**: `SKYBLUE`
- **Restaurant**: `RED`
- **Shop**: `GREEN`
- **Hotel**: `PURPLE`
- **Elevator**: `GRAY`

## Font Sizes

- **Top Bar**: 20pt
- **Panel Titles**: 16pt
- **Panel Content**: 14pt
- **Notifications**: 14pt (icon: 20pt)
- **Build Menu Items**: 14pt (name), 12pt (cost)
- **Hints/Instructions**: 10-12pt

## Interaction Zones

1. **Top Bar**: Non-interactive display only
2. **Build Menu**: Click on facility to select for placement
3. **Info Panels**: Click on action buttons (Demolish, Upgrade)
4. **Game View**: Click on entities to show info panels
5. **Speed Controls**: Click buttons to change simulation speed
6. **Notifications**: Display only, no interaction

## Responsive Behavior

- Build menu can be toggled visible/hidden
- Info panels appear only when entity selected
- Notifications stack and auto-remove
- Speed controls always visible
- Layout adjusts for screen size:
  - Minimum recommended: 800x600
  - Optimal: 1200x800 or larger

## Keyboard Shortcuts (Demo)

- **1**: Show Facility Info Panel
- **2**: Show Person Info Panel  
- **3**: Show Elevator Info Panel
- **4**: Hide all info panels
- **N**: Add notification
- **SPACE**: Toggle pause
- **ESC**: Close window

## Accessibility Considerations

- Color-blind friendly: Uses both color and icons/text
- High contrast between text and backgrounds
- Clear visual hierarchy
- Adequate font sizes (14pt minimum for content)
- Semi-transparent backgrounds don't obscure game view
- Consistent layout and positioning
