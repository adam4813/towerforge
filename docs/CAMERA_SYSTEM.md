# Camera Controls and Viewport System

## Overview

The Camera Controls and Viewport System provides smooth pan, zoom, and entity tracking capabilities for navigating the tower view in TowerForge. It's built on top of Raylib's Camera2D system and integrates seamlessly with the existing rendering pipeline.

## Features

### Pan Controls
- **Mouse Drag:** Click and drag anywhere in the game view to pan the camera
- **Arrow Keys:** Use arrow keys to pan in any direction
- **WASD Keys:** Alternative keyboard controls (W=up, A=left, S=down, D=right)
- **Auto Speed Scaling:** Pan speed automatically adjusts based on zoom level

### Zoom Controls
- **Mouse Wheel:** Scroll to zoom in/out
- **+/- Keys:** Use plus/minus keys for discrete zoom steps
- **Zoom Range:** 25% (0.25x) to 200% (2.0x)
- **Smooth Interpolation:** Zoom transitions are smoothly interpolated

### Follow Mode
- **Entity Tracking:** Camera can lock onto and follow entities (people, elevators)
- **Visual Indicator:** Shows which entity is being followed and its position
- **Toggle:** Press F key to stop following
- **Auto-Disable:** Panning or zooming automatically stops following

### Camera Bounds
- **Boundary Checking:** Camera cannot pan outside tower boundaries
- **Smart Centering:** When zoomed out beyond tower size, camera centers automatically
- **Responsive Bounds:** Bounds adjust based on current zoom level

### Smooth Movement
- **Pan Smoothing:** 8x smoothing factor for fluid camera movement
- **Zoom Smoothing:** 6x smoothing factor for smooth zoom transitions
- **Frame-Independent:** Movement is frame-rate independent using delta time

## Architecture

### Class Structure

```cpp
namespace towerforge {
namespace rendering {
    class Camera {
        // Public interface
        void Initialize(int screen_width, int screen_height, 
                       float tower_width, float tower_height);
        void Update(float delta_time);
        void HandleInput(bool hud_handled);
        void BeginMode() const;
        void EndMode() const;
        
        // Camera control
        void Reset();
        void FollowEntity(float x, float y, int id);
        void StopFollowing();
        
        // Coordinate conversion
        void ScreenToWorld(int screen_x, int screen_y, 
                          float& world_x, float& world_y) const;
        void WorldToScreen(float world_x, float world_y,
                          int& screen_x, int& screen_y) const;
        
        // UI rendering
        void RenderControlsOverlay() const;
        void RenderFollowIndicator() const;
    };
}
}
```

### Integration Pattern

The camera integrates into the game loop as follows:

```cpp
// Setup
towerforge::rendering::Camera camera;
camera.Initialize(screen_width, screen_height, tower_width, tower_height);

// Game loop
while (!renderer.ShouldClose()) {
    // Update camera
    camera.Update(delta_time);
    camera.HandleInput(hud_handled_input);
    
    // Render
    renderer.BeginFrame();
    renderer.Clear(BACKGROUND_COLOR);
    
    // World-space rendering (affected by camera)
    camera.BeginMode();
    DrawTower();
    DrawEntities();
    camera.EndMode();
    
    // Screen-space UI (not affected by camera)
    hud.Render();
    camera.RenderControlsOverlay();
    camera.RenderFollowIndicator();
    
    renderer.EndFrame();
}
```

## Usage Examples

### Basic Camera Setup

```cpp
#include "rendering/camera.h"

// Create camera
towerforge::rendering::Camera camera;

// Initialize with screen size and tower bounds
camera.Initialize(800, 600, 1200.0f, 800.0f);
```

### Follow an Entity

```cpp
// When user clicks on a person or elevator
if (entity_clicked) {
    float entity_x = entity.GetPosition().x;
    float entity_y = entity.GetPosition().y;
    int entity_id = entity.GetId();
    
    camera.FollowEntity(entity_x, entity_y, entity_id);
}

// Update entity position each frame if following
if (camera.IsFollowing()) {
    camera.FollowEntity(entity.GetPosition().x, 
                       entity.GetPosition().y,
                       camera.GetFollowedEntityId());
}
```

### Convert Coordinates

```cpp
// Screen to world (e.g., for mouse clicks)
int mouse_x = GetMouseX();
int mouse_y = GetMouseY();
float world_x, world_y;
camera.ScreenToWorld(mouse_x, mouse_y, world_x, world_y);

// Now you can check if world coordinates hit any game objects
if (IsPointInBuilding(world_x, world_y)) {
    // Handle building click
}

// World to screen (e.g., for rendering UI at world position)
float entity_x = 100.0f;
float entity_y = 200.0f;
int screen_x, screen_y;
camera.WorldToScreen(entity_x, entity_y, screen_x, screen_y);
DrawText("Entity!", screen_x, screen_y, 14, WHITE);
```

### Reset Camera

```cpp
// Reset to default view (centered, 100% zoom)
camera.Reset();

// Or let user press Home key
// (handled automatically by camera.HandleInput())
```

## UI Overlays

### Camera Controls Overlay (Bottom Right)
Displays:
- Pan instructions (Click+Drag or Arrow Keys)
- Zoom instructions (Mouse Wheel or +/- keys)
- Reset instruction (Home key)
- Follow instruction (F key)
- Current zoom percentage

### Follow Mode Indicator (Top Center)
Shows when following an entity:
- Entity ID being followed
- Current position coordinates
- Stop following button hint

## Technical Details

### Constants

```cpp
static constexpr float MIN_ZOOM = 0.25f;           // 25% zoom
static constexpr float MAX_ZOOM = 2.0f;            // 200% zoom
static constexpr float ZOOM_INCREMENT = 0.1f;      // 10% per step
static constexpr float PAN_SMOOTHING = 8.0f;       // Pan interpolation
static constexpr float ZOOM_SMOOTHING = 6.0f;      // Zoom interpolation
static constexpr float KEYBOARD_PAN_SPEED = 300.0f; // Units per second
```

### Performance Considerations

1. **Culling Optimization:** With camera bounds, you can implement viewport culling:
   ```cpp
   // Only render entities visible in current viewport
   for (auto& entity : entities) {
       int screen_x, screen_y;
       camera.WorldToScreen(entity.x, entity.y, screen_x, screen_y);
       
       if (screen_x >= 0 && screen_x < screen_width &&
           screen_y >= 0 && screen_y < screen_height) {
           entity.Render();
       }
   }
   ```

2. **Frame-Independent:** All movement uses delta time for consistent behavior

3. **Minimal Overhead:** Camera transformations are handled by GPU

## Keyboard Shortcuts Summary

| Key | Action |
|-----|--------|
| Left Arrow / A | Pan left |
| Right Arrow / D | Pan right |
| Up Arrow / W | Pan up |
| Down Arrow / S | Pan down |
| Mouse Wheel | Zoom in/out |
| + / Numpad + | Zoom in |
| - / Numpad - | Zoom out |
| Home | Reset camera |
| F | Toggle follow mode |
| Click + Drag | Pan camera |

## Future Enhancements

Potential future additions:

1. **Minimap Widget:** Small overview map in corner showing full tower
2. **Camera State Save/Restore:** Save and restore camera position between sessions
3. **Zoom Levels with Detail:** Hide/show details based on zoom (as mentioned in original spec)
4. **Camera Shake:** For events like elevators arriving or buildings being built
5. **Smooth Zoom to Point:** Zoom centered on mouse cursor position
6. **Camera Presets:** Quick jump to specific floors or areas
7. **Cinematic Mode:** Automated camera movements for tours or cutscenes

## Dependencies

- Raylib 5.0+ (Camera2D system)
- TowerForge Renderer (for integration)
- C++20 (for implementation)

## Files

- `include/rendering/camera.h` - Camera class interface
- `src/rendering/camera.cpp` - Camera implementation
- `src/camera_screenshot.cpp` - Example/demo application

## Testing

A dedicated screenshot application (`camera_screenshot`) is provided that demonstrates:
- Camera controls overlay
- Follow mode indicator
- Multi-floor tower visualization
- Entity tracking
- All camera features working together

Run with:
```bash
cd build
./bin/camera_screenshot
```

## License

Same as TowerForge project (MIT License)
