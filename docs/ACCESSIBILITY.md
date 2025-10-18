# Accessibility Features

TowerForge includes comprehensive accessibility features to ensure the game is playable by everyone, regardless of their needs or preferences.

## Features Overview

### 1. High-Contrast Mode

High-contrast mode enhances the visual contrast of UI elements for better visibility.

**Benefits:**
- Improved visibility for users with low vision
- Easier to read text and distinguish UI elements
- Enhanced focus indicators

**How to enable:**
1. From the main menu, select "Settings"
2. Choose "Accessibility"
3. Toggle "High-Contrast Mode"

**Visual changes:**
- Brighter, more distinct colors (Yellow/White instead of Gold/Gray)
- Thicker borders on UI elements (3px vs 2px)
- Higher contrast between text and backgrounds
- More visible selection indicators

### 2. Font Scaling

Adjust text size throughout the game from 50% to 300% of the default size.

**Benefits:**
- Larger text for users with visual impairments
- Smaller text for those who prefer more information density
- Customizable to individual preferences

**How to adjust:**
1. From the main menu, select "Settings"
2. Choose "Accessibility"
3. Use LEFT/RIGHT arrow keys or drag the slider to adjust font scale
4. Font scale is displayed as a percentage (e.g., 150%)

**Supported range:**
- Minimum: 50% (half size)
- Default: 100% (normal size)
- Maximum: 300% (triple size)

### 3. Keyboard Navigation

Full keyboard support throughout the game interface.

**Benefits:**
- Complete game control without a mouse
- Consistent navigation patterns
- Visual focus indicators show current selection

**Keyboard Controls:**

#### General Navigation
- `UP/DOWN` or `W/S` - Navigate between menu items
- `ENTER` or `SPACE` - Select/activate item
- `ESC` - Go back/cancel

#### Settings Adjustment
- `LEFT/RIGHT` or `A/D` - Adjust sliders or toggle options
- `SPACE` - Toggle checkboxes

#### In-Game
- `ESC` - Open pause menu
- `R` - Toggle research menu
- `N` - Toggle notification center
- All menus support full keyboard navigation

**Focus Indicators:**
- Selected items are highlighted with a colored border
- Pulsing effect on the current selection
- Arrow indicator (">") shows current menu position

## Accessing Accessibility Settings

### From Main Menu
1. Launch TowerForge
2. Select "Settings" from the main menu
3. Choose "Accessibility"
4. Adjust your preferences

### From In-Game Pause Menu
1. Press `ESC` to pause the game
2. Select "Settings"
3. Choose "Accessibility"
4. Adjust your preferences

## Settings Persistence

All accessibility settings are automatically saved to `accessibility_settings.json` in the game directory. Your preferences will be retained between game sessions.

## Default Settings

When running TowerForge for the first time, the following defaults are used:
- High-Contrast Mode: **Disabled**
- Font Scale: **100%** (normal size)
- Keyboard Navigation: **Enabled**

## Tips for Best Experience

### For Low Vision Users
1. Enable High-Contrast Mode
2. Increase Font Scale to 150% or higher
3. Use keyboard navigation for precise control

### For Mobility Impairments
1. Use keyboard navigation exclusively
2. Adjust font scale as needed for comfortable reading
3. Take advantage of keyboard shortcuts

### For Customization
1. Try different font scales to find your preference
2. High-contrast mode works well on both light and dark displays
3. All settings can be changed at any time

## Troubleshooting

**Q: Settings aren't saving between sessions**
- Check that the game has write permissions to its directory
- Look for `accessibility_settings.json` in the game folder

**Q: Font scale makes text too large/small**
- Reset to 100% and adjust in smaller increments (10-20% at a time)
- Text is limited to 50%-300% to prevent display issues

**Q: Keyboard navigation not working**
- Ensure Keyboard Navigation is enabled in Accessibility Settings
- Some elements may require focus - use TAB to cycle focus

## Technical Details

### Settings File Location
`accessibility_settings.json` in the game's root directory

### Settings File Format
```json
{
  "high_contrast_enabled": false,
  "font_scale": 1.0,
  "keyboard_navigation_enabled": true
}
```

### Supported Platforms
- Windows
- Linux
- macOS

All accessibility features work on all supported platforms without requiring additional configuration.

## Future Enhancements

Planned accessibility features for future releases:
- Screen reader support
- Color blind modes
- Customizable keybindings
- Audio cues for UI interactions
- Motion reduction options
- UI element size scaling (in addition to font scaling)

## Feedback

We welcome feedback on accessibility features! If you encounter any issues or have suggestions for improvements, please:
1. Open an issue on our GitHub repository
2. Provide details about your accessibility needs
3. Suggest specific improvements

Your feedback helps make TowerForge accessible to everyone!
