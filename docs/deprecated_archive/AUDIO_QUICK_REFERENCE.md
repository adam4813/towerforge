# Audio System Quick Reference

// DEPRECATED: Merged into `docs/AUDIO.md`.
// See docs/AUDIO.md for quick usage examples and reference.

## Quick Start

```cpp
#include "audio/audio_manager.h"

// Get singleton instance
auto& audio = towerforge::audio::AudioManager::GetInstance();

// Initialize (do this once at startup)
audio.Initialize();

// Update in game loop
audio.Update(delta_time);

// Play music
audio.PlayMusic(towerforge::audio::AudioCue::MainTheme, true, 1.0f);

// Play sound effect
audio.PlaySFX(towerforge::audio::AudioCue::MenuClick);

// Set volumes
audio.SetMasterVolume(0.7f);
audio.SetVolume(towerforge::audio::AudioType::Music, 0.5f);
audio.SetVolume(towerforge::audio::AudioType::SFX, 0.6f);
```

## Common Use Cases

### Menu Navigation
```cpp
// On button hover
audio.PlaySFX(towerforge::audio::AudioCue::MenuClick);

// On button click
audio.PlaySFX(towerforge::audio::AudioCue::MenuConfirm);

// On error
audio.PlaySFX(towerforge::audio::AudioCue::MenuError);
```

### Gameplay Events
```cpp
// Building placed
audio.PlaySFX(towerforge::audio::AudioCue::FacilityPlace);

// Building demolished
audio.PlaySFX(towerforge::audio::AudioCue::FacilityDemolish);

// Achievement unlocked
audio.PlaySFX(towerforge::audio::AudioCue::Achievement);
```

### Music Transitions
```cpp
// Start title screen
audio.PlayMusic(towerforge::audio::AudioCue::MainTheme, true, 1.0f);

// Transition to gameplay
audio.StopMusic(1.0f);  // Fade out over 1 second
audio.PlayMusic(towerforge::audio::AudioCue::GameplayLoop, true, 2.0f);  // Fade in over 2 seconds
```

### Volume Control
```cpp
// Get current volumes
float master = audio.GetMasterVolume();
float music = audio.GetVolume(towerforge::audio::AudioType::Music);
float sfx = audio.GetVolume(towerforge::audio::AudioType::SFX);

// Set volumes (0.0 to 1.0)
audio.SetMasterVolume(0.8f);
audio.SetVolume(towerforge::audio::AudioType::Music, 0.6f);
audio.SetVolume(towerforge::audio::AudioType::SFX, 0.7f);
audio.SetVolume(towerforge::audio::AudioType::Ambient, 0.4f);
```

## All Available Audio Cues

### Menu Sounds
- `AudioCue::MenuClick` - Navigation
- `AudioCue::MenuConfirm` - Confirmation
- `AudioCue::MenuError` - Error feedback
- `AudioCue::MenuOpen` - Menu open
- `AudioCue::MenuClose` - Menu close

### Gameplay Sounds
- `AudioCue::FacilityPlace` - Building placed
- `AudioCue::FacilityDemolish` - Building demolished
- `AudioCue::ElevatorDing` - Elevator arrival
- `AudioCue::ElevatorChime` - Elevator call
- `AudioCue::Construction` - Building in progress
- `AudioCue::ConstructionComplete` - Building finished

### Event Sounds
- `AudioCue::Achievement` - Achievement unlocked
- `AudioCue::Milestone` - Milestone reached
- `AudioCue::Toast` - Notification
- `AudioCue::Alert` - Alert/warning

### Error Sounds
- `AudioCue::InvalidPlacement` - Invalid placement
- `AudioCue::ErrorBuzz` - General error

### Music Tracks
- `AudioCue::MainTheme` - Title screen
- `AudioCue::GameplayLoop` - In-game music
- `AudioCue::VictoryTheme` - Victory screen
- `AudioCue::Fanfare` - Achievement fanfare

## Audio File Locations

Place audio files in the following structure:

```
assets/audio/
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

## Integration with Settings Menu

The AudioSettingsMenu automatically syncs with AudioManager:

```cpp
#include "ui/audio_settings_menu.h"

AudioSettingsMenu audio_menu;

// Update (syncs with AudioManager automatically)
audio_menu.Update(delta_time);

// Handle input
if (audio_menu.HandleKeyboard()) {
    // User pressed back
}
audio_menu.HandleMouse(mouse_x, mouse_y, clicked);

// Render
audio_menu.Render();
```

## Error Handling

The system handles errors gracefully:

```cpp
// Missing audio file - logs warning once, doesn't crash
audio.PlaySFX(towerforge::audio::AudioCue::MenuClick);
// Output: "Warning: Failed to load sound file: assets/audio/sfx/menu_click.wav"

// Subsequent calls are silently ignored (no spam)
audio.PlaySFX(towerforge::audio::AudioCue::MenuClick);  // No output

// Check if initialized
if (audio.IsInitialized()) {
    // Safe to use audio
}
```

## Best Practices

1. **Initialize Early**: Call `Initialize()` after creating the window
2. **Update Every Frame**: Call `Update(delta_time)` in your game loop
3. **Use Fade Transitions**: Fade music in/out for smooth transitions
4. **Test Without Assets**: System works without audio files (for CI/CD)
5. **Volume Range**: Always use 0.0-1.0 for volume (auto-clamped)
6. **Concurrent SFX**: Multiple sound effects can play simultaneously
7. **One Music Track**: Only one music track plays at a time (auto-stops previous)

## Complete Example

```cpp
#include "audio/audio_manager.h"
#include "ui/audio_settings_menu.h"

int main() {
    // Initialize renderer first
    renderer.Initialize(800, 600, "TowerForge");
    
    // Initialize audio
    auto& audio = towerforge::audio::AudioManager::GetInstance();
    audio.Initialize();
    
    // Start title music
    audio.PlayMusic(towerforge::audio::AudioCue::MainTheme, true, 1.0f);
    
    // Game loop
    while (!renderer.ShouldClose()) {
        float dt = GetFrameTime();
        
        // Update audio
        audio.Update(dt);
        
        // Handle events
        if (button_clicked) {
            audio.PlaySFX(towerforge::audio::AudioCue::MenuConfirm);
            
            // Start game
            audio.StopMusic(1.0f);
            audio.PlayMusic(towerforge::audio::AudioCue::GameplayLoop, true, 2.0f);
        }
        
        // Render
        renderer.BeginFrame();
        // ... rendering code ...
        renderer.EndFrame();
    }
    
    // Cleanup happens automatically when audio goes out of scope
    return 0;
}
```

## See Also

- Full Documentation: `docs/AUDIO_SYSTEM.md`
- Implementation Summary: `AUDIO_IMPLEMENTATION_SUMMARY.md`
- API Reference: `include/audio/audio_manager.h`
