# Save/Load (Consolidated)

This document consolidates the content previously split across:
- `docs/SAVE_LOAD_SYSTEM.md`
- `docs/SAVE_LOAD_INTEGRATION.md`
- `docs/README_SAVE_LOAD.md`

It serves as the canonical save/load reference, integration guide, implementation summary, and testing checklist.

---

## Overview

SaveLoadManager serializes game state to JSON using `nlohmann::json` and stores files with the `.tfsave` extension. The system supports save slots, autosave, create/delete/rename operations, error handling, and UI integration via `SaveLoadMenu`.

Key features:
- Full ECS state serialization (singletons, components, tower grid, entities)
- Save slots and metadata (tower name, timestamp, version)
- Autosave (default: every 120s to `_autosave` slot)
- Error detection and user-facing messages (DiskFull, CorruptFile, VersionMismatch, etc.)
- Cross-platform paths and fallback directory


## Quick Usage

```cpp
save_manager.SaveGame("slot1", "My Tower", ecs_world);
auto res = save_manager.LoadGame("slot1", ecs_world);
```


## File format

Save files use JSON with `.tfsave` extension. Example structure:

- metadata: version, date, tower_name, current_day/time, economy summary, population
- time: simulation time and speed
- economy: balance, revenue, expenses
- grid: floors, columns
- entities: serialized entity list (component data)

Example (excerpt):

```json
{
  "metadata": {
    "game_version": "0.1.0",
    "save_date": "2025-10-07T14:30:00Z",
    "tower_name": "My Tower",
    "current_day": 5,
    "current_time": 14.5,
    "total_balance": 25000.0,
    "population": 50
  },
  "time": {
    "current_hour": 14.5,
    "current_day": 5,
    "simulation_speed": 1.0
  },
  "economy": {
    "total_balance": 25000.0,
    "total_revenue": 5000.0,
    "total_expenses": 2000.0
  },
  "grid": {
    "floors": 10,
    "columns": 20
  },
  "entities": []
}
```


## Implementation summary

Files and high-level responsibilities:
- `include/core/save_load_manager.hpp` - Save/load manager interface
- `src/core/save_load_manager.cpp` - Implementation (serialization, file I/O, slot management)
- `include/ui/save_load_menu.h` - Save/load menu interface
- `src/ui/save_load_menu.cpp` - UI implementation

Serialized components (examples): TimeManager, TowerEconomy, Position, Velocity, Person, BuildingComponent, GridPosition, Satisfaction, FacilityEconomics, ElevatorShaft/Car, PersonElevatorRequest, DailySchedule. The saved `TowerGrid` dimensions are used to resize the world on load.

Slot management:
- Slot names: alphanumeric, spaces, underscores, hyphens (max 64 chars). Reserved: `_autosave`.
- Create/Save: validates name, checks disk space (>10MB recommended), serializes and writes JSON file.
- Load: validates file exists, checks version compatibility, deserializes into ECSWorld.
- Delete: removes file from disk.
- Rename: validates new name and renames file.

Autosave:
- Default interval: 120 seconds
- Default slot: `_autosave`
- Triggers: periodic; also recommended to autosave on quit (example provided below)


## Integration guide

Steps to integrate SaveLoadManager into `main.cpp` and the UI.

1) Include headers:

```cpp
#include "core/save_load_manager.hpp"
#include "ui/save_load_menu.h"
```

2) Initialize and configure SaveLoadManager after creating the ECS world:

```cpp
ECSWorld ecs_world;
ecs_world.Initialize();

TowerForge::Core::SaveLoadManager save_load_manager;
save_load_manager.Initialize();
save_load_manager.SetAutosaveEnabled(true);
save_load_manager.SetAutosaveInterval(120.0f); // seconds
```

3) Create `SaveLoadMenu` and wire a pointer to the manager:

```cpp
SaveLoadMenu save_load_menu;
save_load_menu.SetSaveLoadManager(&save_load_manager);
```

4) Open Save/Load menu from the main menu (load mode example):

- Open menu in load mode, poll its state while rendering main menu in the background.
- When the user confirms a load, call `save_load_manager.LoadGame(slot, ecs_world)` and handle the result.

(See the fuller example below for both save and load flows.)


## Example integration snippets

Load flow (from main menu):

```cpp
case MenuOption::LoadGame:
    save_load_menu.Open(false); // load mode
    while (save_load_menu.IsOpen() && !renderer.ShouldClose()) {
        float delta_time = GetFrameTime();
        save_load_menu.Update(delta_time);
        save_load_menu.HandleKeyboard();
        save_load_menu.HandleMouse(GetMouseX(), GetMouseY(), IsMouseButtonPressed(MOUSE_LEFT_BUTTON));

        renderer.BeginFrame();
        main_menu.Render();
        save_load_menu.Render();
        renderer.EndFrame();

        if (save_load_menu.GetLastAction() == SaveLoadAction::Load) {
            std::string slot_name = save_load_menu.GetSelectedSlotName();
            auto result = save_load_manager.LoadGame(slot_name, ecs_world);
            if (result.success) {
                current_mode = GameMode::InGame;
            } else {
                save_load_menu.SetError(result.error_message);
            }
        }
    }
    break;
```

Save flow (from pause menu):

```cpp
if (pause_menu_selection == "Save Game") {
    save_load_menu.Open(true); // save mode
    while (save_load_menu.IsOpen() && !renderer.ShouldClose()) {
        float delta_time = GetFrameTime();
        save_load_menu.Update(delta_time);
        save_load_menu.HandleKeyboard();
        save_load_menu.HandleMouse(GetMouseX(), GetMouseY(), IsMouseButtonPressed(MOUSE_LEFT_BUTTON));

        renderer.BeginFrame();
        renderer.RenderWorld(ecs_world, camera);
        pause_menu.Render();
        save_load_menu.Render();
        renderer.EndFrame();

        if (save_load_menu.GetLastAction() == SaveLoadAction::Save) {
            std::string slot_name = save_load_menu.GetSelectedSlotName();
            auto result = save_load_manager.SaveGame(slot_name, "My Tower", ecs_world);
            if (result.success) {
                save_load_menu.Close();
            } else {
                save_load_menu.SetError(result.error_message);
            }
        }
    }
}
```

Autosave update (call from game loop):

```cpp
while (!pause_menu.ShouldQuit() && !renderer.ShouldClose()) {
    float delta_time = GetFrameTime();
    save_load_manager.UpdateAutosave(delta_time, ecs_world);
    // rest of game loop...
}
```

Save-on-quit example:

```cpp
if (save_load_manager.IsAutosaveEnabled()) {
    save_load_manager.Autosave(ecs_world);
}
```


## Error handling

Errors are surfaced via `SaveLoadResult` (example) and include:
- DiskFull
- CorruptFile
- VersionMismatch
- FileNotFound
- PermissionDenied
- InvalidSlotName
- SlotAlreadyExists
- UnknownError

Errors should be displayed in a modal dialog and in the status bar. Example handling when loading:

```cpp
auto result = save_load_manager.LoadGame("slot1", ecs_world);
if (!result.success) {
    if (result.error == SaveLoadError::VersionMismatch) {
        save_load_menu.SetError("Save file version is incompatible. Please update the game.");
    } else {
        save_load_menu.SetError(result.error_message);
    }
}
```


## UI design notes

- Slot list with metadata (tower name, timestamp, balance, population)
- Action buttons: `Save/Load` `Delete` `Rename` `Cancel`
- Controls: arrow keys to navigate, Enter to select, Delete to remove, Escape to close
- Status bar: autosave status, time since last save, error messages


## Testing checklist

- [ ] New game starts correctly
- [ ] Save creates file in correct location
- [ ] Load restores exact game state
- [ ] Multiple saves don't interfere
- [ ] Autosave works periodically and on quit
- [ ] Delete removes file
- [ ] Rename updates filename and metadata
- [ ] UI shows correct metadata
- [ ] Status bar updates
- [ ] Errors display correctly
- [ ] Cross-platform paths work


## Locations

- Windows: `%APPDATA%/TowerForge/saves/`
- Linux/macOS: `~/.towerforge/saves/`
- Fallback: `./saves/`

Use `std::filesystem` to build paths and create directories if missing.


## Performance and limitations

- Current implementation is synchronous; saving large towers (20+ floors, many entities) may take 100-500ms.
- Future improvements: async saves, compression, incremental saves, backup rotations.
- Known limitations: entity IDs are not preserved; some in-flight actions or UI state may not be fully restored.


## Future improvements

1. Async/background save thread
2. Compression (gzip)
3. Incremental/delta saves
4. Cloud sync/backup
5. Quick save (hotkey)
6. Save thumbnails and browse UI
7. Migration and checksum validation


## References and implementation pointers

- Interface: `include/core/save_load_manager.hpp`
- Implementation: `src/core/save_load_manager.cpp`
- UI: `include/ui/save_load_menu.h`, `src/ui/save_load_menu.cpp`
- Tests: `src/test_save_load.cpp` (if present)


---

This consolidated file replaces the previous split documents. The original split files are preserved in the repository history but can be removed from the working tree to avoid duplication.
