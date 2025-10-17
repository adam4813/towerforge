# Unified UI Element System

## Overview

This document describes the unified UI element concept implemented in TowerForge to provide consistent parent/child relationships and relative positioning throughout the UI system.

## Architecture

### UIElement Base Class

The `UIElement` class (`include/ui/ui_element.h`) is the foundation of the unified UI system. It provides:

- **Relative positioning**: Each element's position is relative to its parent (or screen if no parent)
- **Parent/child relationships**: Elements can have parents and children, forming a hierarchy
- **Automatic absolute positioning**: Absolute screen coordinates are calculated by walking up the parent chain
- **Containment testing**: Check if a point is inside the element

#### Key Methods

```cpp
// Get absolute screen-space bounds (includes parent offsets)
Rectangle GetAbsoluteBounds() const;

// Get bounds relative to parent
Rectangle GetRelativeBounds() const;

// Set parent element (nullptr for root elements)
void SetParent(UIElement* parent);

// Add a child element
void AddChild(std::unique_ptr<UIElement> child);

// Update position relative to parent
void SetRelativePosition(float x, float y);

// Check if point is inside element
bool Contains(float x, float y) const;
```

### Panel Class

The `Panel` class extends `UIElement` to provide a basic rectangular container with:

- Background color
- Optional border
- Automatic rendering of itself and all children

```cpp
// Create a panel with optional colors
Panel(float x, float y, float width, float height,
      Color background = ColorAlpha(BLACK, 0.8f),
      Color border = BLANK);

// Update appearance dynamically
void SetBackgroundColor(Color color);
void SetBorderColor(Color color);

// Render panel and all children
void Render() const override;
```

## Usage Examples

### Example 1: Simple Panel

```cpp
// Create a panel at position (10, 60) with size 200x500
auto panel = std::make_unique<Panel>(10, 60, 200, 500);
panel->Render();
```

### Example 2: Parent-Child Hierarchy

```cpp
// Create parent panel
auto parent = std::make_unique<Panel>(100, 100, 300, 400);

// Create child panel at position (10, 10) relative to parent
// Will appear at screen position (110, 110)
auto child = std::make_unique<Panel>(10, 10, 100, 50);
parent->AddChild(std::move(child));

// Render parent (automatically renders all children)
parent->Render();
```

### Example 3: Dynamic Menu Items (MainMenu pattern)

```cpp
class MainMenu {
private:
    std::vector<std::unique_ptr<Panel>> menu_item_panels_;
    
public:
    MainMenu() {
        // Create panels for each menu item
        for (size_t i = 0; i < 7; i++) {
            auto panel = std::make_unique<Panel>(
                0,  // x will be updated during render for centering
                250 + i * 60,  // y position
                300, 50        // width, height
            );
            menu_item_panels_.push_back(std::move(panel));
        }
    }
    
    void Render(int selected_index) {
        int screen_width = GetScreenWidth();
        
        for (size_t i = 0; i < menu_item_panels_.size(); i++) {
            auto& panel = menu_item_panels_[i];
            
            // Update position for centering
            int x = (screen_width - 300) / 2;
            panel->SetRelativePosition(x, 250 + i * 60);
            
            // Update appearance based on selection
            bool selected = (i == selected_index);
            panel->SetBackgroundColor(selected ? ColorAlpha(GOLD, 0.3f) 
                                                : ColorAlpha(DARKGRAY, 0.2f));
            panel->SetBorderColor(selected ? GOLD : GRAY);
            
            // Render panel
            panel->Render();
            
            // Draw text on top of panel
            // ... text rendering code ...
        }
    }
};
```

## Updated Components

### BuildMenu

- Now includes a `panel_` member of type `std::unique_ptr<Panel>`
- Panel is constructed in the constructor with appropriate bounds
- The existing `RenderPanel` helper function has been overloaded to work with Panel objects
- Maintains backward compatibility with existing Rectangle-based code

### MainMenu

- Each menu item now has an associated `Panel` object stored in `menu_item_panels_`
- Panels are dynamically updated during rendering for centering and selection effects
- Simplifies future layout changes

### PauseMenu

- Similar to MainMenu, each menu item has a Panel object
- Panels handle background and border rendering
- Selection state is reflected through Panel color updates

## Benefits

1. **Consistent Hierarchy**: All UI elements follow the same parent/child pattern
2. **Relative Positioning**: Simplifies layout by working in relative coordinates
3. **Easier Refactoring**: Moving a parent automatically moves all children
4. **Cleaner Code**: Less manual coordinate calculation
5. **Future-Ready**: Easy to add new UI element types by extending UIElement

## Future Enhancements

- Add layout managers (vertical/horizontal stacking, grid, etc.)
- Implement anchoring and docking
- Add margin and padding support
- Create specialized UI element types (Button, Label, TextBox, etc.)
- Add event handling through the hierarchy
- Implement z-ordering for overlapping elements

## Integration Guide

To update existing UI code to use the new system:

1. Include `ui/ui_element.h` in your UI class header
2. Add forward declaration: `class Panel;`
3. Add Panel member(s) to your class
4. Initialize Panel(s) in constructor with relative positions
5. Use `panel->Render()` to draw the panel
6. Use `panel->SetBackgroundColor()` / `SetBorderColor()` for dynamic styling
7. For positioning relative to screen dimensions, update positions in your render method

### Example Migration

**Before:**
```cpp
void MyMenu::Render() {
    Rectangle bounds = {10, 60, 200, 500};
    DrawRectangleRec(bounds, ColorAlpha(BLACK, 0.8f));
    // ... more drawing ...
}
```

**After:**
```cpp
class MyMenu {
    std::unique_ptr<Panel> panel_;
public:
    MyMenu() {
        panel_ = std::make_unique<Panel>(10, 60, 200, 500);
    }
    
    void Render() {
        panel_->Render();
        // ... additional drawing on top ...
    }
};
```

## Notes

- The system is designed to coexist with existing UI code
- Migration to the new system can be done incrementally
- UIWindow class remains separate for now and may be integrated in the future
- Performance impact is minimal due to efficient absolute position caching
