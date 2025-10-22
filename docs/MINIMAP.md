# Minimap System

## Overview

The minimap is an interactive widget that displays a scaled overview of the entire tower and allows players to navigate by clicking or dragging a viewport indicator. It provides at-a-glance awareness of the full tower layout and enables quick navigation to different areas.

## Features

### Display
- **Position**: Top-right corner of screen (similar to camera controls overlay)
- **Size**: 200x150 pixels (configurable)
- **Content**: Scaled representation of full tower (all floors, all columns)
- **Background**: Semi-transparent dark overlay (85% opacity) with gray border
- **Always visible**: Shows complete tower overview at fixed "zoomed out" perspective

### Viewport Indicator
- **Appearance**: Yellow rectangle outline showing current camera viewport
- **Transparency**: Semi-transparent fill (30% opacity) with solid border
- **Real-time updates**: Follows camera position and zoom changes instantly
- **Proportional sizing**: Rectangle size scales with zoom level
  - Larger viewport indicator = more zoomed out (seeing more area)
  - Smaller viewport indicator = more zoomed in (seeing less area)

### Interactivity

#### Mouse Controls
- **Drag viewport**: Click and drag the yellow viewport indicator to move the camera smoothly
- **Click to jump**: Click anywhere on the minimap (outside viewport) to instantly center camera on that location
- **Visual feedback**: Viewport indicator updates in real-time during drag

#### Keyboard Controls
- **[M] key**: Toggle minimap visibility on/off
- **Quick access**: Hint displayed in top-right of minimap

#### Input Priority
- Minimap input is processed before general camera input
- When minimap consumes input, camera panning is disabled
- Prevents conflicts between minimap dragging and camera panning

## Technical Implementation

### Architecture

The minimap system consists of three main components:

1. **Minimap Class** (`include/ui/minimap.h`, `src/ui/minimap.cpp`)
   - Handles rendering, input, and coordinate conversions
   - Manages viewport indicator calculations
   - Tracks dragging state

2. **HUD Integration** (`include/ui/hud.h`, `src/ui/hud.cpp`)
   - Owns minimap instance
   - Provides initialization and rendering methods
   - Exposes toggle and input handling to game loop

3. **Game Loop Integration** (`src/core/game.cpp`)
   - Initializes minimap with tower dimensions
   - Processes minimap input before camera input
   - Renders minimap after camera controls overlay

### Coordinate System

#### World to Minimap Conversion
```cpp
// Calculate scale factor (fit tower in minimap)
float scale_x = minimap_width / tower_width;
float scale_y = minimap_height / tower_height;
float scale = min(scale_x, scale_y);

// Convert world position to minimap position
minimap_x = tower_x + (world_x * scale);
minimap_y = tower_y + (world_y * scale);
```

#### Minimap to World Conversion
```cpp
// Convert minimap click to world coordinates
float relative_x = minimap_x - tower_x;
float relative_y = minimap_y - tower_y;

world_x = relative_x / scale;
world_y = relative_y / scale;
```

#### Viewport Indicator Calculation
```cpp
// Calculate visible area in world units
float visible_width = screen_width / camera_zoom;
float visible_height = screen_height / camera_zoom;

// Get camera position (center of view)
float camera_x, camera_y;
camera.GetPosition(camera_x, camera_y);

// Calculate top-left corner of viewport
float viewport_top_left_x = camera_x - visible_width / 2.0f;
float viewport_top_left_y = camera_y - visible_height / 2.0f;

// Convert to minimap coordinates
int minimap_viewport_x = tower_x + (viewport_top_left_x * scale);
int minimap_viewport_y = tower_y + (viewport_top_left_y * scale);
float minimap_viewport_width = visible_width * scale;
float minimap_viewport_height = visible_height * scale;
```

### Camera Integration

The minimap requires additional camera methods for proper integration:

#### New Camera Methods
```cpp
// Get camera position in world space
void GetPosition(float& x, float& y) const;

// Set camera target position (for minimap navigation)
void SetTargetPosition(float x, float y);

// Get screen dimensions
void GetScreenDimensions(int& width, int& height) const;
```

These methods enable:
- Minimap to know current camera position for viewport indicator
- Minimap to move camera when clicked or dragged
- Accurate coordinate conversions based on screen size

## Usage

### For Players

1. **View minimap**: Minimap is visible by default in top-right corner
2. **Navigate quickly**: Click anywhere on minimap to jump camera to that location
3. **Pan smoothly**: Click and drag the yellow viewport indicator to move camera
4. **Toggle visibility**: Press **[M]** key to show/hide minimap
5. **Understand viewport**: Yellow rectangle shows what's currently visible on screen

### For Developers

#### Initialize Minimap
```cpp
// In Game::InitializeGameSystems()
hud_->InitializeMinimap(screen_width, screen_height, tower_width, tower_height);
```

#### Handle Input
```cpp
// In Game::HandleInGameInput() - before camera input
bool minimap_handled_input = hud_->HandleMinimapInput(*camera_);
camera_->HandleInput(minimap_handled_input);
```

#### Render Minimap
```cpp
// In Game::RenderInGame() - after camera controls, before end frame
hud_->RenderMinimap(*camera_);
```

#### Toggle Minimap
```cpp
// In Game::UpdateInGame() - keyboard shortcuts
if (IsKeyPressed(KEY_M)) {
    hud_->ToggleMinimap();
}
```

## Performance Considerations

### Optimizations
- **Minimal rendering**: Only draws rectangle outlines and fills
- **Pre-calculated scale**: Scale factor computed once per frame
- **Simple geometry**: Viewport indicator is a single rectangle
- **No complex textures**: All rendering uses basic shapes and colors

### Performance Impact
- **Negligible CPU overhead**: Simple coordinate math and rectangle drawing
- **No GPU stress**: Basic 2D primitives
- **Scalable**: Performance independent of tower size or zoom level

## Future Enhancements

Potential features for future development:

### Visual Enhancements
- **Facility indicators**: Show small colored dots for different facility types
- **Floor labels**: Display floor numbers on minimap edge
- **Elevator visualization**: Show elevator shafts as vertical lines
- **Density heatmap**: Color-code areas by population or activity

### Navigation Features
- **Camera presets**: Save and recall favorite view positions
- **Floor shortcuts**: Click floor numbers to jump to specific floors
- **Search integration**: Highlight facilities when searched

### Customization
- **Resize handles**: Allow players to resize minimap
- **Position options**: Choose corner placement (top-left, top-right, etc.)
- **Opacity control**: Adjust transparency via settings
- **Color themes**: Match minimap colors to UI theme

## Troubleshooting

### Common Issues

**Minimap not visible**
- Check if toggled off with [M] key
- Verify `InitializeMinimap()` was called during game initialization
- Ensure `RenderMinimap()` is called after camera mode ends

**Viewport indicator in wrong position**
- Verify camera position getters are working correctly
- Check coordinate conversion scale calculations
- Ensure screen dimensions are updated if window resizes

**Clicking minimap doesn't move camera**
- Confirm `HandleMinimapInput()` is called before `camera.HandleInput()`
- Check that `SetTargetPosition()` is implemented in Camera class
- Verify minimap bounds detection is correct

**Dragging feels laggy or jumpy**
- Ensure drag delta is calculated relative to drag start position
- Verify scale factor is consistent during drag
- Check that `SetTargetPosition()` doesn't interfere with camera smoothing

## Code References

### Header Files
- `include/ui/minimap.h` - Minimap class declaration
- `include/ui/hud.h` - HUD integration methods
- `include/rendering/camera.h` - Camera position getters/setters

### Implementation Files
- `src/ui/minimap.cpp` - Minimap rendering and input logic
- `src/ui/hud.cpp` - HUD integration implementation
- `src/rendering/camera.cpp` - Camera position management
- `src/core/game.cpp` - Game loop integration

### Related Documentation
- [CAMERA_SYSTEM.md](CAMERA_SYSTEM.md) - Camera controls and coordinate systems
- [HUD.md](HUD.md) - HUD system overview
- [IMPLEMENTATION.md](IMPLEMENTATION.md) - General implementation guidelines

## Testing

### Manual Testing Checklist
- [ ] Minimap displays in top-right corner
- [ ] Viewport indicator shows current view
- [ ] Viewport indicator updates when camera moves (WASD/arrow keys)
- [ ] Viewport indicator updates when camera zooms (mouse wheel)
- [ ] Clicking minimap jumps camera to location
- [ ] Dragging viewport indicator moves camera smoothly
- [ ] [M] key toggles minimap visibility
- [ ] Minimap works correctly at min zoom (0.25x)
- [ ] Minimap works correctly at max zoom (2.0x)
- [ ] No input conflicts with camera panning
- [ ] Coordinate conversions are accurate

### Automated Testing
Currently, minimap functionality is tested through manual gameplay testing. Automated tests can be added to verify:
- Coordinate conversion accuracy
- Viewport indicator size calculations
- Input handling priority
- Boundary detection

## Accessibility

The minimap supports the following accessibility features:
- **Keyboard control**: Full functionality available via [M] key toggle
- **High contrast**: Yellow viewport indicator on dark background
- **Clear labeling**: "MINIMAP" label and "[M]" hint always visible
- **Optional visibility**: Can be toggled off if distracting

Future accessibility improvements:
- Configurable colors for viewport indicator
- Size scaling options
- Screen reader announcements for minimap interactions
