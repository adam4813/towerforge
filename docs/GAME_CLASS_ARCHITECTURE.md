# Game Class Architecture

## Overview

The Game class encapsulates all game logic and state management, providing a clean separation between the main entry point and game implementation.

## Game States

The game supports the following states:
- **TitleScreen**: Main menu with options to start new game, load game, view achievements, settings, credits, or quit
- **InGame**: Active gameplay simulation with tower management
- **Settings**: Configuration menu for audio, controls, display, accessibility, and gameplay
- **Credits**: Information about the game and technologies used
- **Achievements**: Display of unlocked and locked achievements
- **Quit**: Clean exit state

## Architecture Benefits

### Before Refactoring
- **1116 lines** in main.cpp with all logic in one function
- Difficult to maintain and extend
- State transitions scattered throughout the code
- Hard to test individual states

### After Refactoring
- **12 lines** in main.cpp (96% reduction)
- Clean separation of concerns with dedicated methods per state
- Easy to add new game states
- Each state has its own Update, Render, and HandleInput methods
- Game systems initialized on-demand for better resource management

## Class Structure

### Public Interface
```cpp
Game();              // Constructor
~Game();             // Destructor
bool Initialize();   // Initialize renderer, audio, and core systems
void Run();          // Main game loop
void Shutdown();     // Cleanup and exit
```

### Private State Methods
Each game state has three dedicated methods:
- `Update{State}(delta_time)` - Update state logic
- `Render{State}()` - Render state visuals
- `Handle{State}Input()` - Process user input

Example for TitleScreen:
```cpp
void UpdateTitleScreen(float delta_time);
void RenderTitleScreen();
void HandleTitleScreenInput();
```

### Resource Management
- **Core systems** (renderer, audio manager, main menus) are always active
- **Game systems** (ECS world, HUD, camera, etc.) are created on-demand when entering InGame state
- Systems are properly cleaned up when returning to title screen or exiting

## Usage

The simplified main.cpp:
```cpp
int main(int argc, char* argv[]) {
    towerforge::core::Game game;
    
    if (!game.Initialize()) {
        return 1;
    }
    
    game.Run();
    game.Shutdown();
    
    return 0;
}
```

## Future Enhancements

The new architecture makes it easy to:
- Add new game states (e.g., GameOver, Tutorial, Multiplayer)
- Implement state-specific save/load functionality
- Add state transition animations
- Test individual states in isolation
- Implement state history/stack for complex navigation
- Add state-specific debug overlays

## File Organization

- `include/core/game.h` - Game class declaration
- `src/core/game.cpp` - Game class implementation
- `src/main.cpp` - Minimal entry point
