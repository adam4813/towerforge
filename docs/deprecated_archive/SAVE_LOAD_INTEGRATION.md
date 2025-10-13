# Save/Load System Integration Guide

## Overview

This guide shows how to integrate the save/load system into TowerForge's main game loop.

## Integration Steps

### 1. Add Headers to main.cpp

```cpp
#include "core/save_load_manager.hpp"
#include "ui/save_load_menu.h"
```

### 2. Create SaveLoadManager Instance

After initializing ECSWorld:

```cpp
// Create and initialize the ECS world
ECSWorld ecs_world;
ecs_world.Initialize();

// Create save/load manager
TowerForge::Core::SaveLoadManager save_load_manager;
save_load_manager.Initialize();
save_load_manager.SetAutosaveEnabled(true);
save_load_manager.SetAutosaveInterval(120.0f);  // 2 minutes
```

### 3. Create SaveLoadMenu UI

```cpp
// Create save/load menu
SaveLoadMenu save_load_menu;
save_load_menu.SetSaveLoadManager(&save_load_manager);
```

### 4. Wire Up Load Game from Main Menu

In the main menu handler:

```cpp
case MenuOption::LoadGame:
    save_load_menu.Open(false);  // false = load mode
    
    // Show save/load menu overlay
    while (save_load_menu.IsOpen() && !renderer.ShouldClose()) {
        float delta_time = GetFrameTime();
        
        // Update menu
        save_load_menu.Update(delta_time);
        save_load_menu.HandleKeyboard();
        save_load_menu.HandleMouse(GetMouseX(), GetMouseY(), 
                                   IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
        
        // Render with menu overlay
        renderer.BeginFrame();
        main_menu.Render();  // Background
        save_load_menu.Render();  // Overlay
        renderer.EndFrame();
        
        // Check if user selected a save to load
        if (save_load_menu.GetLastAction() == SaveLoadAction::Load) {
            std::string slot_name = save_load_menu.GetSelectedSlotName();
            auto result = save_load_manager.LoadGame(slot_name, ecs_world);
            
            if (result.success) {
                current_mode = GameMode::InGame;
                std::cout << "Game loaded successfully!" << std::endl;
            } else {
                save_load_menu.SetError(result.error_message);
            }
        }
    }
    break;
```

### 5. Wire Up Save Game from Pause Menu

In the pause menu handler (during game):

```cpp
// In pause menu, add Save option
if (pause_menu_selection == "Save Game") {
    save_load_menu.Open(true);  // true = save mode
    
    while (save_load_menu.IsOpen() && !renderer.ShouldClose()) {
        float delta_time = GetFrameTime();
        
        save_load_menu.Update(delta_time);
        save_load_menu.HandleKeyboard();
        save_load_menu.HandleMouse(GetMouseX(), GetMouseY(), 
                                   IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
        
        renderer.BeginFrame();
        // Render game in background
        renderer.RenderWorld(ecs_world, camera);
        pause_menu.Render();
        save_load_menu.Render();
        renderer.EndFrame();
        
        if (save_load_menu.GetLastAction() == SaveLoadAction::Save) {
            std::string slot_name = save_load_menu.GetSelectedSlotName();
            auto result = save_load_manager.SaveGame(slot_name, "My Tower", ecs_world);
            
            if (result.success) {
                std::cout << "Game saved successfully!" << std::endl;
                save_load_menu.Close();
            } else {
                save_load_menu.SetError(result.error_message);
            }
        }
    }
}
```

### 6. Add Autosave Updates

In the main game loop:

```cpp
while (!pause_menu.ShouldQuit() && !renderer.ShouldClose()) {
    float delta_time = GetFrameTime();
    
    // Update autosave
    save_load_manager.UpdateAutosave(delta_time, ecs_world);
    
    // ... rest of game loop
}
```

### 7. Save on Quit

Before exiting:

```cpp
if (save_load_manager.IsAutosaveEnabled()) {
    std::cout << "Performing final autosave..." << std::endl;
    save_load_manager.Autosave(ecs_world);
}
```

## Complete Example Flow

### Loading a Game

1. User selects "Load Game" from main menu
2. SaveLoadMenu opens in load mode
3. User selects a save slot
4. User clicks "Load" button
5. SaveLoadManager loads the JSON file
6. SaveLoadManager deserializes into ECSWorld
7. Game mode changes to InGame
8. Player resumes at saved state

### Saving a Game

1. User pauses game (ESC)
2. User selects "Save Game" from pause menu
3. SaveLoadMenu opens in save mode
4. User selects existing slot or creates new one
5. User clicks "Save" button
6. SaveLoadManager serializes ECSWorld to JSON
7. SaveLoadManager writes file to disk
8. Confirmation message shown
9. Game resumes

### Autosave

1. Game runs normally
2. Every 2 minutes (default), autosave triggers
3. SaveLoadManager saves to "_autosave" slot
4. Status bar updates "Last saved: 0m 0s ago"
5. Game continues without interruption

## Error Scenarios

### Disk Full

```cpp
auto result = save_load_manager.SaveGame("slot1", "Tower", ecs_world);
if (result.error == SaveLoadError::DiskFull) {
    std::cout << "Error: " << result.error_message << std::endl;
    // Show error dialog to user
}
```

### Corrupt File

```cpp
auto result = save_load_manager.LoadGame("slot1", ecs_world);
if (result.error == SaveLoadError::CorruptFile) {
    std::cout << "Error: " << result.error_message << std::endl;
    // Offer to delete corrupt save
}
```

### Version Mismatch

```cpp
auto result = save_load_manager.LoadGame("slot1", ecs_world);
if (result.error == SaveLoadError::VersionMismatch) {
    std::cout << "Error: " << result.error_message << std::endl;
    // Inform user to update game
}
```

## Testing Checklist

- [ ] New game starts correctly
- [ ] Save creates file in correct location
- [ ] Load restores exact game state
- [ ] Multiple saves don't interfere
- [ ] Autosave works periodically
- [ ] Delete removes file
- [ ] Rename changes filename
- [ ] UI shows correct metadata
- [ ] Status bar updates
- [ ] Errors display correctly
- [ ] Escape closes dialogs
- [ ] Cross-platform paths work

## Save File Locations

After integration, saves will be stored in:

- **Linux:** `~/.towerforge/saves/`
- **macOS:** `~/.towerforge/saves/`
- **Windows:** `%APPDATA%/TowerForge/saves/`
- **Fallback:** `./saves/` (if home directory not available)

## Debugging

Enable verbose output:

```cpp
// Before saving
std::cout << "Saving to: " << save_load_manager.GetSavePath("slot1") << std::endl;

// Check what was serialized
auto json = save_load_manager.SerializeGameState(ecs_world, "Tower");
std::cout << json.dump(2) << std::endl;
```

## Performance Considerations

- Save operations are synchronous (blocking)
- Large towers (20+ floors, 100+ entities) may take 100-500ms to save
- Autosave runs on main thread - may cause brief stutter
- Consider implementing async save in future for large saves

## Future Improvements

1. **Async Saves:** Use background thread for save operations
2. **Save Compression:** Reduce file size with gzip
3. **Incremental Saves:** Only save changed entities
4. **Save Validation:** Checksum to detect corruption
5. **Cloud Integration:** Sync saves to cloud storage
6. **Quick Save:** F5 hotkey for instant save
7. **Save Browser:** Enhanced UI with thumbnails

// DEPRECATED: Merged into `docs/SAVE_LOAD.md`.
// See docs/SAVE_LOAD.md for integration instructions.
