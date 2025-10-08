# Audio System Documentation

## Overview

The TowerForge audio system provides centralized management of background music, sound effects (SFX), and ambient audio. It supports volume control, fade transitions, and graceful error handling for missing or corrupt audio files.

## Architecture

The audio system follows a singleton pattern with the `AudioManager` class as the central component:

```
AUDIO SYSTEM OVERVIEW
======================
┌─────────────────────────────┐
│  AudioManager (singleton)   │
├─────────────────────────────┤
│  - PlayMusic(track)         │
│  - PlaySFX(cue)             │
│  - PlayAmbient(track)       │
│  - SetVolume(type, value)   │
│  - FadeIn/Out(track)        │
└─────────────────────────────┘
    |         |         |
    V         V         V
[Music]   [SFX]     [Ambience]
```

## Key Components

### AudioManager

Location: `include/audio/audio_manager.h`, `src/audio/audio_manager.cpp`

The `AudioManager` is a singleton class that manages all audio playback. It must be initialized before use and will automatically clean up resources on shutdown.

#### Initialization

```cpp
#include "audio/audio_manager.h"

auto& audio_manager = towerforge::audio::AudioManager::GetInstance();
audio_manager.Initialize();

// Update in game loop
audio_manager.Update(delta_time);

// Shutdown (automatically called on destruction)
audio_manager.Shutdown();
```

#### Playing Music

```cpp
// Play music with loop and fade-in
audio_manager.PlayMusic(towerforge::audio::AudioCue::MainTheme, true, 1.0f);

// Stop music with fade-out
audio_manager.StopMusic(1.0f);
```

#### Playing Sound Effects

```cpp
// Play a sound effect at default volume
audio_manager.PlaySFX(towerforge::audio::AudioCue::MenuClick);

// Play a sound effect at custom volume (0.0-1.0)
audio_manager.PlaySFX(towerforge::audio::AudioCue::Achievement, 0.8f);
```

#### Playing Ambient Sounds

```cpp
// Play ambient sound with loop and fade-in
audio_manager.PlayAmbient(towerforge::audio::AudioCue::Construction, true, 2.0f);

// Stop ambient sound with fade-out
audio_manager.StopAmbient(1.5f);
```

#### Volume Control

```cpp
// Set master volume (affects all audio)
audio_manager.SetMasterVolume(0.7f);

// Set specific audio type volumes
audio_manager.SetVolume(towerforge::audio::AudioType::Music, 0.5f);
audio_manager.SetVolume(towerforge::audio::AudioType::SFX, 0.6f);
audio_manager.SetVolume(towerforge::audio::AudioType::Ambient, 0.4f);

// Get current volumes
float master_vol = audio_manager.GetMasterVolume();
float music_vol = audio_manager.GetVolume(towerforge::audio::AudioType::Music);
```

### Audio Cues

Audio cues are predefined identifiers for consistent audio playback across the game. They are defined in the `AudioCue` enum.

#### Menu Sounds
- `MenuClick` - Navigation sound
- `MenuConfirm` - Confirmation sound
- `MenuError` - Error feedback
- `MenuOpen` - Menu open sound
- `MenuClose` - Menu close sound

#### Gameplay Sounds
- `FacilityPlace` - Building placement
- `FacilityDemolish` - Building demolition
- `ElevatorDing` - Elevator arrival
- `ElevatorChime` - Elevator call
- `Construction` - Construction in progress
- `ConstructionComplete` - Construction finished

#### Event Sounds
- `Achievement` - Achievement unlocked
- `Milestone` - Milestone reached
- `Toast` - Notification toast
- `Alert` - Alert/warning

#### Error Sounds
- `InvalidPlacement` - Invalid placement attempt
- `ErrorBuzz` - General error

#### Music Tracks
- `MainTheme` - Title screen music
- `GameplayLoop` - In-game music
- `VictoryTheme` - Victory screen music
- `Fanfare` - Achievement fanfare

## Integration Points

### Audio Settings Menu

The `AudioSettingsMenu` is automatically integrated with the `AudioManager`. Volume changes in the settings menu are immediately applied to the audio system.

```cpp
#include "ui/audio_settings_menu.h"

AudioSettingsMenu audio_settings_menu;
audio_settings_menu.Update(delta_time);

// The menu automatically syncs with AudioManager
if (audio_settings_menu.HandleKeyboard()) {
    // User pressed back
}
```

### Event Mapping

The following table shows when audio cues are triggered:

| State/Action              | Music Cue       | SFX Cue              |
|---------------------------|-----------------|----------------------|
| Title Screen              | MainTheme       | MenuClick, MenuConfirm |
| Start Game                | GameplayLoop    | MenuConfirm          |
| Gameplay                  | GameplayLoop    | FacilityPlace, ElevatorDing |
| Pause Menu Open           | -               | MenuOpen             |
| Pause Menu Close          | -               | MenuClose            |
| Achievement Unlocked      | -               | Achievement          |
| Building Placed           | -               | FacilityPlace        |
| Building Demolished       | -               | FacilityDemolish     |
| Invalid Placement         | -               | InvalidPlacement     |

## Asset Organization

Audio files should be organized in the following structure:

```
assets/
└── audio/
    ├── music/
    │   ├── main_theme.ogg
    │   ├── gameplay_loop.ogg
    │   ├── victory_theme.ogg
    │   └── fanfare.ogg
    └── sfx/
        ├── menu_click.wav
        ├── menu_confirm.wav
        ├── menu_error.wav
        ├── menu_open.wav
        ├── menu_close.wav
        ├── facility_place.wav
        ├── facility_demolish.wav
        ├── elevator_ding.wav
        ├── elevator_chime.wav
        ├── construction.wav
        ├── construction_complete.wav
        ├── achievement.wav
        ├── milestone.wav
        ├── toast.wav
        ├── alert.wav
        ├── invalid_placement.wav
        └── error_buzz.wav
```

**Note:** Audio files are currently not included. The system will fail gracefully if files are missing, logging warnings but not crashing the game.

## Error Handling

The audio system is designed to be resilient:

1. **Missing Files**: If an audio file is not found, a warning is logged once per file, and playback is silently skipped
2. **Corrupt Files**: If a file fails to load, it is marked as failed to avoid repeated load attempts
3. **Uninitialized Audio**: All audio operations check if the system is initialized before proceeding
4. **Volume Clamping**: All volume values are automatically clamped to the valid range [0.0, 1.0]

## Features

### Fade Transitions

Both music and ambient audio support smooth fade-in and fade-out transitions:

```cpp
// Fade in over 2 seconds
audio_manager.PlayMusic(AudioCue::GameplayLoop, true, 2.0f);

// Fade out over 1.5 seconds
audio_manager.StopMusic(1.5f);
```

### Concurrent Sound Effects

Multiple sound effects can play simultaneously. The system does not limit the number of concurrent SFX.

### Volume Categories

The system supports independent volume control for:
- Master volume (affects all audio)
- Music volume
- SFX volume
- Ambient volume

### Real-time Updates

Volume changes are applied immediately to all playing audio (except during fade transitions).

## Future Enhancements

Potential improvements for the audio system:

1. **Audio Asset Pack**: Include placeholder/CC0 audio files for testing
2. **Spatial Audio**: 3D positional audio for in-game sounds
3. **Audio Mixer**: More advanced mixing and ducking
4. **Playlist Support**: Multiple tracks for varied gameplay music
5. **Adaptive Music**: Music that changes based on gameplay state
6. **Audio Filters**: EQ, reverb, and other effects
7. **Audio Preloading**: Preload commonly-used sounds on startup
8. **Memory Management**: LRU cache for audio resources

## Dependencies

- **Raylib**: For audio device management and playback
- **C++20 Standard Library**: For containers and utilities
- **TowerForge Audio Settings Menu**: For UI integration

## Example Usage

### Basic Setup (in main.cpp)

```cpp
#include "audio/audio_manager.h"

int main() {
    // Initialize renderer
    renderer.Initialize(800, 600, "TowerForge");
    
    // Initialize audio
    auto& audio_manager = towerforge::audio::AudioManager::GetInstance();
    audio_manager.Initialize();
    
    // Play main theme
    audio_manager.PlayMusic(towerforge::audio::AudioCue::MainTheme, true, 1.0f);
    
    // Game loop
    while (!renderer.ShouldClose()) {
        float delta_time = GetFrameTime();
        
        // Update audio
        audio_manager.Update(delta_time);
        
        // Game logic...
        
        // Play sound on event
        if (button_clicked) {
            audio_manager.PlaySFX(towerforge::audio::AudioCue::MenuClick);
        }
        
        renderer.BeginFrame();
        // Rendering...
        renderer.EndFrame();
    }
    
    // Cleanup happens automatically
    return 0;
}
```

## API Reference

See `include/audio/audio_manager.h` for complete API documentation with detailed method descriptions.
