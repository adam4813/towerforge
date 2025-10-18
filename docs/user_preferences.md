# User Preferences System

## Overview

The TowerForge user preferences system provides centralized management of all user settings, ensuring they persist between game sessions. Settings are stored in a JSON file in the user's configuration directory.

## Configuration File Location

The preferences are stored in a platform-specific location:

- **Windows**: `%APPDATA%/TowerForge/user_preferences.json`
- **Linux/macOS**: `~/.towerforge/user_preferences.json`

## Supported Settings

### Audio Settings
- **Master Volume** (0.0 - 1.0): Overall game volume
- **Music Volume** (0.0 - 1.0): Background music volume
- **SFX Volume** (0.0 - 1.0): Sound effects volume
- **Mute All**: Global mute toggle
- **Mute Music**: Music-only mute toggle
- **Mute SFX**: Sound effects-only mute toggle
- **Enable Ambient**: Ambient sounds toggle

### UI Settings
- **UI Scale** (0.5 - 2.0): User interface scaling factor
  - 1.0 = 100% (default)
  - 1.5 = 150%
  - 2.0 = 200%
- **Color Mode**: Visual theme selection
  - Light
  - Dark (default)
  - High Contrast

### Notification Preferences
- Filter settings for different notification types:
  - Info notifications
  - Warning notifications
  - Error notifications
  - Success notifications
  - Achievement notifications
  - Event notifications
- Filter by read/unread status
- Filter by pinned/unpinned status

### Accessibility Settings
- **High Contrast Mode**: Enhanced visual contrast for better visibility
- **Font Scale** (0.5 - 3.0): Text size scaling
  - 1.0 = 100% (default)
  - 1.5 = 150%
  - 2.0 = 200%
- **Keyboard Navigation**: Enable/disable keyboard navigation

## Usage

### For Developers

The preferences system uses a singleton pattern for easy access:

```cpp
#include "core/user_preferences.hpp"

// Get the preferences instance
auto& prefs = TowerForge::Core::UserPreferences::GetInstance();

// Read settings
float volume = prefs.GetMasterVolume();
float uiScale = prefs.GetUIScale();
bool highContrast = prefs.IsHighContrastEnabled();

// Modify settings (automatically saves)
prefs.SetMasterVolume(0.8f);
prefs.SetUIScale(1.5f);
prefs.SetHighContrastEnabled(true);
```

### Automatic Loading and Saving

- **Loading**: Preferences are automatically loaded when the `UserPreferences` singleton is first accessed (typically at game startup)
- **Saving**: Each setter method automatically saves changes to disk, ensuring no data loss
- **Error Handling**: If the preferences file doesn't exist, default values are used. Any errors during load/save are logged but don't crash the game.

### Integration with UI Menus

The preferences system is integrated with existing settings menus:

- **AudioSettingsMenu**: Reads from and writes to UserPreferences
- **AccessibilitySettingsMenu**: Reads from and writes to UserPreferences
- **NotificationCenter**: Loads notification filter from UserPreferences

## File Format

The preferences are stored in JSON format for easy debugging and manual editing if needed:

```json
{
  "audio": {
    "master_volume": 0.7,
    "music_volume": 0.5,
    "sfx_volume": 0.6,
    "mute_all": false,
    "mute_music": false,
    "mute_sfx": false,
    "enable_ambient": true
  },
  "ui": {
    "scale": 1.0,
    "color_mode": "dark"
  },
  "notifications": {
    "show_info": true,
    "show_warning": true,
    "show_error": true,
    "show_success": true,
    "show_achievement": true,
    "show_event": true,
    "show_read": true,
    "show_unread": true,
    "show_pinned": true,
    "show_unpinned": true
  },
  "accessibility": {
    "high_contrast_enabled": false,
    "font_scale": 1.0,
    "keyboard_navigation_enabled": true
  }
}
```

## Testing

A test program is provided to verify the preferences system:

```bash
# Build the test program
cmake --build build --target test_user_preferences

# Run the test
./build/bin/test_user_preferences  # Linux/macOS
.\build\bin\test_user_preferences.exe  # Windows
```

### Manual Testing

1. Launch the game
2. Go to Settings menu
3. Change various settings (audio, accessibility, etc.)
4. Close the game completely
5. Relaunch the game
6. Verify that all settings are preserved

## Migration from Legacy Systems

The new unified preferences system maintains backward compatibility with legacy settings files:

- **accessibility_settings.json**: Legacy file, settings now stored in user_preferences.json
- **audio_settings.cfg**: Legacy file (old text format), settings now stored in user_preferences.json

Both systems coexist temporarily to ensure smooth transition. The AccessibilitySettingsMenu syncs changes to both the new UserPreferences system and the legacy AccessibilitySettings for compatibility.

## Future Enhancements

Potential improvements for the preferences system:

1. **Validation**: Add more robust validation for setting values
2. **Versioning**: Add version field to handle format changes
3. **Import/Export**: Allow users to backup and restore settings
4. **Cloud Sync**: Optional cloud synchronization of preferences
5. **Per-Profile Settings**: Support multiple user profiles
6. **Reset to Defaults**: Provide UI to reset all settings to defaults
