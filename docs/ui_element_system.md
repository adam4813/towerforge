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
- **Use for**: Containers, windows, groups of UI elements

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

### Button Class

The `Button` class extends `UIElement` to provide an interactive button with:

- Text label
- Background and border colors
- Text color and font size
- **Use for**: Clickable menu items, tool buttons, interactive controls

```cpp
// Create a button with label and optional colors
Button(float x, float y, float width, float height,
       const std::string& label = "",
       Color background = ColorAlpha(DARKGRAY, 0.3f),
       Color border = GRAY);

// Update appearance dynamically
void SetLabel(const std::string& label);
void SetBackgroundColor(Color color);
void SetBorderColor(Color color);
void SetTextColor(Color color);
void SetFontSize(int size);

// Render button with centered text
void Render() const override;
```

## Usage Examples

### Example 1: Simple Panel

```cpp
// Create a panel at position (10, 60) with size 200x500
auto panel = std::make_unique<Panel>(10, 60, 200, 500);
panel->Render();
```

### Example 2: Parent-Child Hierarchy with Buttons

```cpp
// Create parent panel (container)
auto parent = std::make_unique<Panel>(100, 100, 300, 400);

// Create button at position (10, 10) relative to parent
// Will appear at screen position (110, 110)
auto button = std::make_unique<Button>(10, 10, 100, 50, "Click Me");
parent->AddChild(std::move(button));

// Render parent (automatically renders all children)
parent->Render();
```

### Example 3: Menu with Buttons (MainMenu pattern)

```cpp
class MainMenu {
private:
    std::unique_ptr<Panel> main_panel_;          // Container
    std::vector<std::unique_ptr<Button>> menu_buttons_;  // Interactive items
    
public:
    MainMenu() {
        // Main menu is a Panel (container)
        main_panel_ = std::make_unique<Panel>(0, 0, 800, 600, BLANK, BLANK);
        
        // Create Button objects for each menu item
        std::vector<std::string> labels = {"New Game", "Load Game", "Settings", "Quit"};
        for (size_t i = 0; i < labels.size(); i++) {
            auto button = std::make_unique<Button>(
                0,  // x will be updated during render for centering
                250 + i * 60,  // y position
                300, 50,       // width, height
                labels[i]      // button label
            );
            menu_buttons_.push_back(std::move(button));
        }
    }
    
    void Render(int selected_index) {
        int screen_width = GetScreenWidth();
        
        for (size_t i = 0; i < menu_buttons_.size(); i++) {
            auto& button = menu_buttons_[i];
            
            // Update position for centering
            int x = (screen_width - 300) / 2;
            button->SetRelativePosition(x, 250 + i * 60);
            
            // Update appearance based on selection
            bool selected = (i == selected_index);
            button->SetBackgroundColor(selected ? ColorAlpha(GOLD, 0.3f) 
                                                : ColorAlpha(DARKGRAY, 0.2f));
            button->SetBorderColor(selected ? GOLD : GRAY);
            button->SetTextColor(selected ? WHITE : LIGHTGRAY);
            
            // Render button (includes text)
            button->Render();
        }
    }
};
```

## Updated Components

### BuildMenu

- BuildMenu is a `Panel` (container)
- Panel is constructed in the constructor with appropriate bounds
- The existing `RenderPanel` helper function has been overloaded to work with Panel objects
- Maintains backward compatibility with existing Rectangle-based code

### MainMenu

- MainMenu is a `Panel` (container)
- Each menu item is a `Button` object stored in `menu_item_buttons_`
- Buttons are dynamically updated during rendering for centering and selection effects
- Buttons handle their own text rendering
- Simplifies future layout changes

### PauseMenu

- PauseMenu is a `Panel` (container)
- Each menu item is a `Button` object stored in `menu_item_buttons_`
- Buttons handle background, border, and text rendering
- Selection state is reflected through Button color updates

## Element Type Guide

**Use Panel for:**
- Containers (BuildMenu, MainMenu, PauseMenu)
- Windows and dialog boxes
- Groups of related UI elements
- Areas that contain other UI elements

**Use Button for:**
- Clickable menu items
- Tool buttons (Undo, Redo, etc.)
- Interactive controls
- Any UI element the user can click to trigger an action

**Use UIElement directly for:**
- Custom UI element types
- Elements that need specific rendering behavior
- Base class for new specialized elements

## Benefits

1. **Consistent Hierarchy**: All UI elements follow the same parent/child pattern
2. **Relative Positioning**: Simplifies layout by working in relative coordinates
3. **Easier Refactoring**: Moving a parent automatically moves all children
4. **Cleaner Code**: Less manual coordinate calculation
5. **Proper Separation**: Containers (Panel) vs Interactive Elements (Button)
6. **Future-Ready**: Easy to add new UI element types by extending UIElement

## Future Enhancements

- Add layout managers (vertical/horizontal stacking, grid, etc.)
- Implement anchoring and docking
- Add margin and padding support
- Create more specialized UI element types (Label, TextBox, Slider, etc.)
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
