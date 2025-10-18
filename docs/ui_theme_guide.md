# TowerForge UI Theme System Guide

## Overview

The TowerForge UI theme system provides a centralized approach to managing colors, spacing, animations, and responsive layouts across the entire user interface. This ensures visual consistency and makes it easy to maintain and update the UI.

## Using the Theme System

### Include the Theme Header

```cpp
#include "ui/ui_theme.h"
```

### Color Palette

The theme provides a comprehensive color palette:

#### Primary Colors
- `UITheme::PRIMARY` - Gold accent color for highlighted elements
- `UITheme::PRIMARY_DARK` - Darker gold for hover/pressed states
- `UITheme::PRIMARY_LIGHT` - Lighter gold for special highlights

#### Background Colors
- `UITheme::BACKGROUND_DARK` - Main dark background
- `UITheme::BACKGROUND_PANEL` - Panel backgrounds
- `UITheme::BACKGROUND_MODAL` - Modal dialog backgrounds

#### Text Colors
- `UITheme::TEXT_PRIMARY` - Primary text (white)
- `UITheme::TEXT_SECONDARY` - Secondary text (light gray)
- `UITheme::TEXT_DISABLED` - Disabled text
- `UITheme::TEXT_ACCENT` - Accent text (gold)

#### State Colors
- `UITheme::SUCCESS` - Green for successful actions
- `UITheme::WARNING` - Yellow/orange for warnings
- `UITheme::ERROR` - Red for errors
- `UITheme::INFO` - Blue for informational messages

#### Example Usage
```cpp
// Draw a button background
DrawRectangle(x, y, width, height, UITheme::BUTTON_BACKGROUND);

// Draw border with primary color
DrawRectangleLinesEx(bounds, UITheme::BORDER_NORMAL, UITheme::PRIMARY);

// Draw text
DrawText(label.c_str(), x, y, UITheme::FONT_SIZE_NORMAL, UITheme::TEXT_PRIMARY);
```

### Spacing and Sizing

Use standardized spacing constants instead of magic numbers:

```cpp
// Padding
UITheme::PADDING_TINY      // 5px
UITheme::PADDING_SMALL     // 10px
UITheme::PADDING_MEDIUM    // 15px
UITheme::PADDING_LARGE     // 20px
UITheme::PADDING_XLARGE    // 30px

// Margins
UITheme::MARGIN_SMALL      // 10px
UITheme::MARGIN_MEDIUM     // 20px
UITheme::MARGIN_LARGE      // 40px

// Font Sizes
UITheme::FONT_SIZE_SMALL   // 14
UITheme::FONT_SIZE_NORMAL  // 16
UITheme::FONT_SIZE_MEDIUM  // 20
UITheme::FONT_SIZE_LARGE   // 24
UITheme::FONT_SIZE_TITLE   // 60

// Button Sizes
UITheme::BUTTON_HEIGHT_SMALL   // 30px
UITheme::BUTTON_HEIGHT_MEDIUM  // 40px
UITheme::BUTTON_HEIGHT_LARGE   // 50px
```

### Responsive Layouts

The theme provides helper functions for responsive design:

#### Center Position
```cpp
// Center an element within a container
int x = UITheme::CenterPosition(screen_width, element_width);
```

#### Responsive Width/Height
```cpp
// Get width as percentage of screen width
int width = UITheme::ResponsiveWidth(0.25f);  // 25% of screen width

// Get height as percentage of screen height  
int height = UITheme::ResponsiveHeight(0.5f);  // 50% of screen height
```

#### Clamp Size
```cpp
// Clamp a size between min and max values
int clamped = UITheme::ClampSize(calculated_size, min_size, max_size);

// Example: Responsive menu width (25% of screen, between 250-400px)
int menu_width = UITheme::ClampSize(
    UITheme::ResponsiveWidth(0.25f),
    250,  // Minimum
    400   // Maximum
);
```

#### Responsive Font Size
```cpp
// Scale font size based on screen dimensions
int font_size = UITheme::ResponsiveFontSize(UITheme::FONT_SIZE_NORMAL);
```

### Animations

The theme defines standard animation parameters:

```cpp
// Animation speeds (for use with delta_time)
UITheme::ANIMATION_SPEED_SLOW      // 2.0
UITheme::ANIMATION_SPEED_NORMAL    // 3.0
UITheme::ANIMATION_SPEED_FAST      // 5.0

// Animation durations (in seconds)
UITheme::ANIMATION_DURATION_QUICK  // 0.15s
UITheme::ANIMATION_DURATION_NORMAL // 0.3s
UITheme::ANIMATION_DURATION_SLOW   // 0.5s

// Notification durations
UITheme::NOTIFICATION_DURATION_SHORT   // 3.0s
UITheme::NOTIFICATION_DURATION_NORMAL  // 5.0s
UITheme::NOTIFICATION_DURATION_LONG    // 8.0s
```

#### Example Animation
```cpp
void MyPanel::Update(float delta_time) {
    if (is_animating_) {
        animation_progress_ += delta_time * UITheme::ANIMATION_SPEED_NORMAL;
        if (animation_progress_ >= 1.0f) {
            animation_progress_ = 1.0f;
            is_animating_ = false;
        }
    }
}

void MyPanel::Render() {
    // Apply animation progress to alpha
    Color bg_color = ColorAlpha(UITheme::BACKGROUND_PANEL, animation_progress_);
    DrawRectangleRec(bounds, bg_color);
}
```

## Best Practices

### 1. Always Use Theme Constants
❌ **Don't:**
```cpp
DrawRectangle(x, y, width, height, Color{30, 30, 40, 255});
DrawText(label, x + 10, y + 5, 20, WHITE);
```

✅ **Do:**
```cpp
DrawRectangle(x, y, width, height, UITheme::BACKGROUND_PANEL);
DrawText(label, x + UITheme::PADDING_SMALL, y + UITheme::PADDING_TINY, 
         UITheme::FONT_SIZE_MEDIUM, UITheme::TEXT_PRIMARY);
```

### 2. Make Layouts Responsive
❌ **Don't:**
```cpp
const int menu_width = 300;  // Fixed size
const int menu_x = (screen_width - menu_width) / 2;
```

✅ **Do:**
```cpp
const int menu_width = UITheme::ClampSize(
    UITheme::ResponsiveWidth(0.25f), 250, 400
);
const int menu_x = UITheme::CenterPosition(screen_width, menu_width);
```

### 3. Use Animation Constants
❌ **Don't:**
```cpp
animation_progress_ += delta_time * 3.0f;  // Magic number
```

✅ **Do:**
```cpp
animation_progress_ += delta_time * UITheme::ANIMATION_SPEED_NORMAL;
```

### 4. Test at Multiple Resolutions
Always test your UI at different window sizes:
- 800x600 (SVGA)
- 1024x768 (XGA)
- 1280x720 (HD)
- 1920x1080 (Full HD)

## Updating the Theme

To change the overall look of the UI, simply update the constants in `include/ui/ui_theme.h`. All UI elements using the theme will automatically reflect the changes.

### Example: Changing the Primary Color
```cpp
// In ui_theme.h
static constexpr Color PRIMARY = Color{100, 150, 255, 255};  // Change to blue
```

All buttons, borders, and accent colors will now use blue instead of gold.

## Animation Examples

### Fade-in Panel
```cpp
class MyPanel : public Panel {
    float animation_progress_ = 0.0f;
    
    void Update(float delta_time) {
        animation_progress_ += delta_time * UITheme::ANIMATION_SPEED_FAST;
        animation_progress_ = std::min(animation_progress_, 1.0f);
    }
    
    void Render() const override {
        Color bg = ColorAlpha(UITheme::BACKGROUND_PANEL, animation_progress_);
        DrawRectangleRec(GetAbsoluteBounds(), bg);
    }
};
```

### Pulsing Button
```cpp
void MyButton::Render() const {
    if (is_selected_) {
        float pulse = sin(GetTime() * UITheme::ANIMATION_SPEED_NORMAL) * 0.1f + 0.9f;
        Color text_color = ColorAlpha(UITheme::TEXT_PRIMARY, pulse);
        DrawText(label_, x, y, font_size_, text_color);
    }
}
```

## Screenshots

The following screenshots demonstrate the theme system in action:

- `screenshots/main_menu_themed.png` - Main menu with consistent theme colors
- `screenshots/main_menu_responsive_*.png` - Responsive layouts at different resolutions

## Related Files

- `include/ui/ui_theme.h` - Theme system definition
- `include/ui/ui_element.h` - Base UI element classes with animation support
- `src/ui/ui_element.cpp` - UI element implementations
- `src/ui/main_menu.cpp` - Example of theme usage
- `src/ui/notification_center.cpp` - Example of animations and theme colors
