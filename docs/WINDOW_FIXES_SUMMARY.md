# Window Management and Input Improvements - Implementation Summary

## Overview
This PR fixes three critical usability issues in TowerForge:
1. **Window Resizing**: Game window can now be resized by the user
2. **Escape Key Behavior**: ESC key now opens the pause menu instead of toggling it (more intuitive)
3. **Camera/Mouse Coordinate Transformation**: Fixed misalignment between mouse cursor and build placement tool when camera is active

## Changes Made

### 1. Enable Window Resizing
**File**: `src/rendering/renderer.cpp`
**Change**: Added `SetConfigFlags(FLAG_WINDOW_RESIZABLE)` before `InitWindow()` call

```cpp
void Renderer::Initialize(int width, int height, const char* title) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);  // NEW: Enable window resizing
    InitWindow(width, height, title);
    SetTargetFPS(60);
}
```

**Impact**: Users can now resize the game window by dragging edges or corners, providing better flexibility for different screen sizes and user preferences.

### 2. Fix Escape Key Behavior
**File**: `src/core/game.cpp`
**Change**: Modified `UpdateInGame()` to only pause (not toggle) when ESC is pressed

**Before**:
```cpp
if (IsKeyPressed(KEY_ESCAPE)) {
    // ... research menu check ...
    else if (!in_settings_from_pause_ && !in_audio_settings_from_pause_) {
        is_paused_ = !is_paused_;  // TOGGLE behavior
        // ...
    }
}
```

**After**:
```cpp
if (IsKeyPressed(KEY_ESCAPE)) {
    // ... research menu check ...
    else if (!in_settings_from_pause_ && !in_audio_settings_from_pause_ && !is_paused_) {
        is_paused_ = true;  // ONLY PAUSE, don't unpause
        audio_manager_->PlaySFX(towerforge::audio::AudioCue::MenuOpen);
        game_state_.paused = true;
    }
}
```

**Impact**: 
- ESC now opens the pause/settings menu (standard game behavior)
- To close the pause menu, users must select "Resume" from the menu
- More intuitive and prevents accidental unpausing

### 3. Fix Camera/Mouse Coordinate Transformation
This was the most complex fix, involving multiple files.

#### Problem
The game world (grid, elevators) was rendered WITHOUT camera transformation, but the placement system preview was rendered WITH camera transformation. This caused a mismatch between where the mouse appeared to be pointing and where the placement tool thought it was.

#### Solution
Applied camera transformation consistently across all game world rendering and mouse input.

**File**: `src/core/game.cpp` - `RenderInGame()`
**Change**: Moved ALL game world rendering inside camera mode

**Before**:
```cpp
void Game::RenderInGame() {
    // Draw grid WITHOUT camera
    // Draw elevators WITHOUT camera
    
    // Begin camera mode
    camera_->BeginMode();
    placement_system_->Render(...);
    camera_->EndMode();
}
```

**After**:
```cpp
void Game::RenderInGame() {
    // Begin camera mode for all game world rendering
    camera_->BeginMode();
    
    // Draw grid WITH camera
    // Draw elevators WITH camera
    // Render placement system preview WITH camera
    
    // End camera mode
    camera_->EndMode();
}
```

**File**: `src/core/game.cpp` - `HandleInGameInput()`
**Change**: Convert screen coordinates to world coordinates for mouse clicks

**Before**:
```cpp
int mouse_x = GetMouseX();
int mouse_y = GetMouseY();
// Use raw screen coordinates
placement_system_->HandleClick(mouse_x, mouse_y, ...);
```

**After**:
```cpp
int mouse_x = GetMouseX();
int mouse_y = GetMouseY();
// Convert screen coordinates to world coordinates
float world_x, world_y;
camera_->ScreenToWorld(mouse_x, mouse_y, world_x, world_y);
placement_system_->HandleClick(static_cast<int>(world_x), static_cast<int>(world_y), ...);
```

**File**: `include/ui/placement_system.h`
**Changes**:
1. Added forward declaration for Camera class
2. Added `SetCamera()` method to accept camera reference
3. Added `camera_` member variable

**File**: `src/ui/placement_system.cpp`
**Changes**:
1. Added `#include "rendering/camera.h"`
2. Initialize `camera_` to `nullptr` in constructor
3. Modified `Render()` to convert mouse coordinates using camera if set

**Before**:
```cpp
void PlacementSystem::Render(...) {
    int mouse_x = GetMouseX();
    int mouse_y = GetMouseY();
    // Use raw coordinates
}
```

**After**:
```cpp
void PlacementSystem::Render(...) {
    int mouse_x = GetMouseX();
    int mouse_y = GetMouseY();
    
    if (camera_ != nullptr) {
        float world_x, world_y;
        camera_->ScreenToWorld(mouse_x, mouse_y, world_x, world_y);
        mouse_x = static_cast<int>(world_x);
        mouse_y = static_cast<int>(world_y);
    }
    // Use converted coordinates
}
```

**File**: `src/core/game.cpp` - `InitializeGameSystems()`
**Change**: Set camera reference in PlacementSystem

```cpp
placement_system_ = new PlacementSystem(grid, facility_mgr, *build_menu_);
placement_system_->SetCamera(camera_);  // NEW: Provide camera for coordinate conversion
```

**Impact**:
- Build placement preview now aligns correctly with mouse cursor
- Camera panning/zooming works correctly with placement system
- Clicking on facilities shows correct information
- No more offset between visual cursor and placement location

## Testing

The changes were tested by:
1. Building the project successfully with no compilation errors
2. Verifying window resize flag is set in renderer initialization
3. Confirming escape key logic only pauses (doesn't toggle)
4. Ensuring camera coordinate transformation is applied consistently

## Files Modified

1. `src/rendering/renderer.cpp` - Add window resize flag
2. `src/core/game.cpp` - Fix escape key, camera rendering, mouse coordinate conversion
3. `include/ui/placement_system.h` - Add camera support
4. `src/ui/placement_system.cpp` - Implement camera coordinate conversion

## Minimal Changes Philosophy

All changes follow the principle of minimal modification:
- Only added necessary code (no refactoring of unrelated functionality)
- Preserved existing behavior except for the specific fixes
- No changes to interfaces beyond what's required for camera integration
- No new dependencies or libraries added

## Backward Compatibility

- All existing game features continue to work
- Camera controls remain unchanged
- Build menu, HUD, and other UI elements unaffected
- Save/load functionality not impacted

## Future Enhancements (Not Implemented)

The issue mentioned optional Alt-Enter fullscreen support. This was not implemented to keep changes minimal, but could be added with:

```cpp
// In Game::UpdateInGame() or similar
if (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_ENTER)) {
    ToggleFullscreen();
}
```

This is left for a future PR to maintain minimal scope.
