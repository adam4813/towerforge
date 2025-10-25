# Implementation Notes (Canonical)

This is the consolidated implementation and changelog document for TowerForge. It merges and replaces the following legacy implementation notes:

- `docs/IMPLEMENTATION_SUMMARY.md`
- `docs/IMPLEMENTATION_NOTES.md`
- `docs/HUD_IMPLEMENTATION_SUMMARY.md`

Purpose: provide a single developer-facing reference for major systems, recent changes, file pointers, and next steps.

---

## Contents

1. Executive summary
2. Major systems (high-level)
   - Facilities
   - HUD
   - Audio & Settings
   - Placement & Grid
   - Elevators & People
   - Save/Load
3. Advanced facilities & staffing (implementation notes)
4. Files changed / where to find implementation
5. Testing & acceptance
6. Recommended actions for legacy docs
7. Next steps

---

## 1. Executive summary

This document centralizes implementation notes, design rationale, and pointers to source files for core systems. It is intended for developers and maintainers who need a quick orientation to where functionality is implemented, what changed recently, and what to review for future work.


## 2. Major systems (high-level)

### Facilities

- Core components and data types live under `include/core/` (e.g. `components.hpp`) and `src/core/` (e.g. `facility_manager.cpp`).
- FacilityManager provides creation/removal APIs and default attributes (width, capacity, colors) per facility type.
- Facility types: Lobby, Office, RetailShop, Restaurant, Hotel, Elevator, Residential (and extended/advanced types in recent changes).
- Rendering color and default visuals are defined alongside the facility type utilities.

Pointers:
- `include/core/components.hpp`
- `src/core/facility_manager.cpp`
- `docs/FACILITIES.md`


### HUD

- The HUD provides the persistent UI: Top Bar, Build Menu, Info Panels, Notifications, Speed Controls.
- Implementation is in `include/ui/hud.h`, `src/ui/hud.cpp`; build menu is in `include/ui/build_menu.h`, `src/ui/build_menu.cpp`.
- Data flow: the game/simulation pushes a `GameState` (funds, income_rate, population, time, speed) and entity-specific info structs (FacilityInfo, PersonInfo, ElevatorInfo) to the HUD.
- Notification system supports Warning/Success/Info/Error with auto-expiry and stacking.

Key layout constants (examples): TOP_BAR_HEIGHT=40, BUILD_MENU_WIDTH=200, INFO_PANEL_WIDTH=250, NOTIFICATION_SIZE=300x30.

For visual guidance and full HUD reference, see `docs/HUD.md` (this file consolidated HUD docs). The original `docs/HUD_IMPLEMENTATION_SUMMARY.md` has been merged into this file and into `docs/HUD.md`.


### Audio & Settings

- General Settings and Audio menu implementation provides master/music/sfx sliders and keyboard/mouse navigation; accessible from Title and Pause menus.
- Audio manager lives under `include/audio/` and `src/audio/`.

Pointers:
- `include/audio/audio_manager.h`
- `src/audio/audio_manager.cpp`


### Placement & Grid

- Placement and grid logic handle facility placement, width constraints, and build validation.
- Placement UI integrates with the build menu and renderer.

Pointers:
- `src/ui/placement_system.cpp`
- `src/core/grid_manager.cpp` (if present)


### Elevators & People

- Elevator and person logic integrates with ECS; info panels display elevator state and people state.
- ElevatorManager and person movement logic are in `src/core/` and `src/systems/` (search for elevator/person modules).

Pointers:
- `docs/ELEVATOR.md`
- `docs/PERSONS.md`


### Save / Load

- Save/Load manager and serialization utilities are documented in `docs/SAVE_LOAD.md` and implemented in `include/core/save_load_manager.hpp` and `src/core/save_load_manager.cpp`.


## 3. Advanced facilities & staffing (implementation notes)

Summary from the advanced facilities change set:

- Five new advanced facility types were added: Gym, Arcade, Theater, Conference Hall, Flagship Store. Existing Restaurant and Hotel were updated.
- Staffing fields were added to building components: `current_staff`, `operating_start_hour`, `operating_end_hour`, and helper functions such as `GetRequiredEmployees()` and `IsOperational()`.
- Build times, default widths, capacity defaults, and colors were updated for the new types.

Operational logic (concept):
- Facilities require minimum staff to be operational during their operating hours; otherwise they do not generate revenue or provide services.

Example API surface (conceptual):
- `bool BuildingComponent::IsOperational(float current_hour) const`
- `int BuildingComponent::GetRequiredEmployees() const`

Files touched (representative):
- `include/core/components.hpp` (added staffing fields and methods)
- `src/core/facility_manager.cpp` (updated defaults)
- `src/ui/build_menu.cpp` (added menu entries)
- `src/ui/placement_system.cpp` (mapping and build times)
- `src/screenshot_app.cpp` (rendering tweaks)

Documentation: `docs/ADVANCED_FACILITIES.md` created with full gameplay details and staffing mechanics.


## 4. Files changed / where to look

Primary implementation files:

- include/
  - `include/core/components.hpp` — core component definitions
  - `include/ui/hud.h` — HUD API and data structs
  - `include/ui/build_menu.h` — build menu API
  - `include/audio/audio_manager.h`

- src/
  - `src/core/facility_manager.cpp`
  - `src/ui/hud.cpp`
  - `src/ui/build_menu.cpp`
  - `src/ui/placement_system.cpp`
  - `src/core/save_load_manager.cpp`
  - `src/audio/audio_manager.cpp`
  - `src/main.cpp` (integration / demo usage)

- docs/
  - `docs/HUD.md` — canonical HUD doc (merged)
  - `docs/FACILITIES.md`
  - `docs/ADVANCED_FACILITIES.md`
  - `docs/IMPLEMENTATION.md` (this file)


## 5. Testing & acceptance

- The HUD and facility systems are exercised in the main demo application and screenshot utilities.
- Unit tests (if present) validate staffing rules, operational checks, and basic placement logic.
- Acceptance criteria are documented in prior summaries and have been consolidated here.


## 6. Recommended actions for legacy docs

Legacy files that were merged into this canonical `docs/IMPLEMENTATION.md` or into `docs/HUD.md`:

- `docs/IMPLEMENTATION_SUMMARY.md` — MERGED
- `docs/IMPLEMENTATION_NOTES.md` — MERGED
- `docs/HUD_IMPLEMENTATION_SUMMARY.md` — MERGED
- `docs/HUD_SYSTEM.md`, `docs/HUD_VISUAL_LAYOUT.md`, `docs/HUD_SCREENSHOT_REFERENCE.md`, `docs/HUD_README.md`, `docs/HUD_IMPLEMENTATION_SUMMARY.md` — all merged into `docs/HUD.md`

Recommended cleanup options (I can perform these if you want):
- Move merged/deprecated docs into `docs/deprecated/` and leave tiny redirect stubs at the original paths pointing to the merged files.
- Alternatively keep them for historical commit-level reference; they are now redundant in the tree.


## 7. Next steps

- (Optional) Run a docs-only CI job to ensure there are no broken links between docs.
- Consider adding a short CHANGELOG entry describing these merges.
- If you want, I can move the deprecated files to `docs/deprecated/` and create small redirect stubs here in the repo. Tell me if you prefer that and whether to keep or remove the ASCII-art screenshot references.

---

This file consolidates the implementation notes and should be considered the canonical developer reference for implementation pointers and recent changes.

# DEPRECATED: The following files were merged into this document and can be archived:
- docs/IMPLEMENTATION_SUMMARY.md
- docs/IMPLEMENTATION_NOTES.md
- docs/HUD_IMPLEMENTATION_SUMMARY.md


---

If you'd like me to move the deprecated docs into `docs/deprecated/` and create redirect stubs, say "Please archive merged implementation docs" and I will perform the moves and create the stubs.
