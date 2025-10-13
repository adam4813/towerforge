# HUD — Heads-Up Display / Information System

This document consolidates the HUD documentation into a single canonical reference. It merges the content from the following source files:

- `docs/HUD_SYSTEM.md`
- `docs/HUD_VISUAL_LAYOUT.md`
- `docs/HUD_SCREENSHOT_REFERENCE.md`
- `docs/HUD_README.md`
- `docs/HUD_IMPLEMENTATION_SUMMARY.md`

If you'd like the originals kept around, see the "Original files and recommended actions" section below.

---

## Quick summary

The HUD provides the game's persistent UI: top bar (funds, population, time, speed), build menu, context-sensitive info panels (facility/person/elevator), notification system, and speed controls. It is implemented in `include/ui` and `src/ui` and rendered every frame using Raylib functions.

Sources: consolidated from the HUD docs listed above.

---

## Contents

1. Overview
2. Quick start (build & run)
3. Components
   - Top bar
   - Build menu
   - Info panels (Facility / Person / Elevator)
   - Notifications
   - Speed controls
4. Data structures
5. Visual layout, palette & fonts
6. Interaction zones & keyboard shortcuts
7. Integration and usage examples
8. Implementation summary & files
9. Original files and recommended actions
10. Next steps

---

## 1. Overview

The HUD (Heads-Up Display) displays critical game information and provides user controls for building and managing the tower. It is non-intrusive (semi-transparent panels) and updates in real time from the simulation state.

Primary goals:

- Present funds, income/expense, population, and time at a glance.
- Let players inspect entities (facilities, people, elevators) via info panels.
- Show transient messages (notifications).
- Allow facility selection via a build menu.
- Control simulation speed via dedicated UI.


## 2. Quick start (build & run)

To build and run the main application (works on platforms with dependencies installed):

```bash
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build .
# then run the game binary
./bin/towerforge
```

(Adjust toolchain path as needed for your vcpkg installation.)


## 3. Components

Top-level components of the HUD:

- Top bar (always visible)
- Build menu (left side, toggleable)
- Context-sensitive info panels (right side)
- Notifications (bottom-left)
- Speed controls (bottom-right)


### Top bar

Always visible across the top of the screen. Shows:

- Current funds and income rate (e.g. "$25,000 (+$500/hr)")
- Population count
- Current time and day (12-hour format with AM/PM)
- Simulation speed indicator (Pause/1x/2x/4x)

Layout constants used by the HUD include values such as TOP_BAR_HEIGHT, PANEL_WIDTH, PANEL_PADDING; specific constants live in `include/ui/hud.h`.


### Build menu

Left-side vertical menu listing buildable facilities with icon/color, name, and cost. Default facility list (configurable in code): Lobby, Office, Restaurant, Shop, Hotel, Elevator.

Features:

- Click-to-select a facility for placement
- Visual highlight of the currently selected facility
- Shows facility properties (name, cost, width)


### Info panels

Appears on the right side when the player clicks on an entity. It is context-sensitive and there are three types:

- Facility Info Panel — shows occupancy, revenue, satisfaction, tenant count, action buttons (Demolish / Upgrade)
- Person Info Panel — shows id/state/current floor/destination/wait time/needs/satisfaction
- Elevator Info Panel — shows id/current floor/direction/occupancy/next stop/queue details

Examples are available in the integration snippets below.


### Notifications

- Four types: Warning, Success, Info, Error
- Auto-expire after a configurable duration (default 5s)
- Queue limited to a maximum (default 5 visible)
- Color-coded background per type and stacked from the bottom-left


### Speed controls

Bottom-right: Pause, 1x, 2x, 4x. Active mode is highlighted (green for running speeds, red for pause).


## 4. Data structures

Below are the main structures used to transfer HUD data (examples shown in code-style form).

GameState

- funds : float
- income_rate : float
- population : int
- current_day : int
- current_time : float  // hours (0-24)
- speed_multiplier : int
- paused : bool

FacilityInfo

- type : std::string
- floor : int
- occupancy : int
- max_occupancy : int
- revenue : float
- satisfaction : float
- tenant_count : int

PersonInfo

- id : int
- state : std::string
- current_floor : int
- destination_floor : int
- wait_time : float
- needs : std::string
- satisfaction : float

ElevatorInfo

- id : int
- current_floor : int
- direction : std::string
- occupancy : int
- max_occupancy : int
- next_stop : int
- queue : std::vector<std::pair<int,int>> // {floor, waiting_count}

Notification

- type : enum { Warning, Success, Info, Error }
- message : std::string
- time_remaining : float


## 5. Visual layout, palette & fonts

Design principles:

- Semi-transparent backgrounds to avoid obscuring the game view
- High-contrast text and consistent font sizes
- Color-coding for types and statuses

Color accents (examples used in the renderer):

- Top bar: semi-transparent black with GOLD accent
- Facility panel border: SKYBLUE
- Person panel border: YELLOW
- Elevator panel border: PURPLE
- Notifications: ORANGE (warning), GREEN (success), SKYBLUE (info), RED (error)

Font sizes (suggested): Top bar 20pt, Panel titles 16pt, Panel content 14pt, Notifications 14pt.

Minimum recommended window size: 800x600. Optimal: 1200x800 or larger.


## 6. Interaction zones & keyboard shortcuts

Zones:

- Top bar: display-only
- Build menu: clickable items to select facility types
- Info panels: interactive action buttons (Demolish, Upgrade)
- Game view center: clickable entities to show info panels
- Speed controls: clickable

Suggested keyboard shortcuts (demo):

- 1: Show Facility Info Panel
- 2: Show Person Info Panel
- 3: Show Elevator Info Panel
- 4: Hide all info panels
- N: Add notification (debug)
- SPACE: Toggle pause
- ESC: Close window


## 7. Integration and usage examples

Typical usage in game loop (pseudocode):

```cpp
// create HUD & build menu
towerforge::ui::HUD hud;
towerforge::ui::BuildMenu menu;

// update and render
hud.SetGameState(game_state);
hud.Update(delta_time);
hud.Render();
menu.Render();

// handle clicks
if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
  int mx = GetMouseX(), my = GetMouseY();
  if (menu.HandleClick(mx,my) >= 0) { /* facility selected */ }
  else if (!hud.HandleClick(mx,my)) { /* game area click */ }
}

// show entity info
towerforge::ui::FacilityInfo f;
f.type = "OFFICE"; f.floor = 5; f.occupancy = 8; f.max_occupancy = 10;
hud.ShowFacilityInfo(f);

// add notification
hud.AddNotification(Notification::Type::Warning, "Low satisfaction on Floor 3");
```


## 8. Implementation summary & files

Primary implementation lives under `include/ui` and `src/ui`:

- include/ui/hud.h — main HUD class and data structures
- include/ui/build_menu.h — build menu definitions
- src/ui/hud.cpp — rendering and HUD logic
- src/ui/build_menu.cpp — build menu implementation
- src/main.cpp — application integration and demo usage

The HUD uses Raylib drawing calls and updates each frame. The HUD API provides methods like SetGameState, Update, Render, AddNotification, ShowFacilityInfo, ShowPersonInfo, HandleClick, etc.


## 9. Original files and recommended actions

Files that were consolidated into this `docs/HUD.md`:

- `docs/HUD_SYSTEM.md` — combined
- `docs/HUD_VISUAL_LAYOUT.md` — combined
- `docs/HUD_SCREENSHOT_REFERENCE.md` — combined
- `docs/HUD_README.md` — combined
- `docs/HUD_IMPLEMENTATION_SUMMARY.md` — combined

Recommended actions for each original file (so you can move or archive them):

- `docs/HUD_SYSTEM.md` — DEPRECATED, safe to archive or move to `docs/deprecated/`.
- `docs/HUD_VISUAL_LAYOUT.md` — DEPRECATED, archive or move; the visual/layout content is now in this file.
- `docs/HUD_SCREENSHOT_REFERENCE.md` — DEPRECATED; if you value the ASCII-art screenshot references for historical purposes, move to `docs/visual_refs/` or `docs/deprecated/`; otherwise archive.
- `docs/HUD_README.md` — DEPRECATED; shorter quick-start material is retained here.
- `docs/HUD_IMPLEMENTATION_SUMMARY.md` — DEPRECATED; I recommend keeping this one (or moving it to `docs/implementation/`) because it records implementation traces, file lists, and acceptance status useful for developers and audits.

If you'd like, I can automatically move the deprecated files to `docs/deprecated/` (or create that directory) and leave small placeholder files that link back to `docs/HUD.md`.


## 10. Next steps

- (Optional) Move the deprecated files to `docs/deprecated/` and leave an empty file containing a short redirect note. I can do this for you.
- Consider adding a small CHANGELOG entry noting the merge.
- If you want, I can split `HUD.md` into user-facing (Overview + Quick Start) and developer-facing (Implementation Summary + file lists) sections or files.


---

Consolidation performed: HUD docs merged into `docs/HUD.md`. Original files are left untouched but marked deprecated above; tell me if you want me to move them into an archive folder and leave redirect stubs.
