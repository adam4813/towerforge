# Mouse Event System Documentation

## Overview

The TowerForge mouse event system provides a unified approach to handling mouse interactions for both UI elements and game objects. It uses a bubble-down event propagation model where events are processed through parent-child hierarchies with the ability to consume events to stop further propagation.

## Components

### 1. IMouseInteractive Interface

The base interface that all interactive elements implement.

```cpp
class IMouseInteractive {
public:
    virtual bool OnHover(const MouseEvent& event);
    virtual bool OnClick(const MouseEvent& event);
    virtual bool Contains(float x, float y) const = 0;
    virtual bool IsHovered() const;
};
```

**Key Features:**
- `OnHover`: Called when the mouse is over the element
- `OnClick`: Called when a mouse button is clicked
- Return `true` to consume the event and stop propagation
- Return `false` to allow the event to continue

### 2. UIElement Class

All UI elements inherit from `UIElement`, which implements `IMouseInteractive` with parent/child support.

**Event Propagation:**
1. Events bubble down to children first (reverse order for layering)
2. If a child consumes the event, processing stops
3. Otherwise, the parent's event handlers are called

**Usage Example:**
```cpp
// Create a panel
auto panel = std::make_unique<Panel>(100, 100, 400, 300);

// Create a button with a click callback
auto button = std::make_unique<Button>(10, 10, 100, 40, "Click Me");
button->SetClickCallback([]() {
    std::cout << "Button clicked!" << std::endl;
});

// Add button as child to panel
panel->AddChild(std::move(button));

// Process mouse events (in your update loop)
MouseEvent event(mouse_x, mouse_y, false, false, IsMouseButtonPressed(MOUSE_LEFT_BUTTON), false);
panel->ProcessMouseEvent(event);
```

### 3. Button Class

Enhanced button with automatic click handling and visual feedback.

**Features:**
- Automatic hover state tracking
- Visual feedback when hovered
- Click callbacks for easy event handling
- Enabled/disabled state support

**Example:**
```cpp
auto button = std::make_unique<Button>(50, 50, 150, 40, "New Game");
button->SetClickCallback([this]() {
    StartNewGame();
});
button->SetBackgroundColor(ColorAlpha(BLUE, 0.3f));
button->SetTextColor(WHITE);
```

### 4. MouseEventManager

For game objects and non-UI elements that need mouse interaction.

**Features:**
- Region-based event handling
- Priority-based event ordering
- Spatial queries for hit testing
- Dynamic region updates

**Usage Example:**
```cpp
// Create event manager
MouseEventManager event_manager;

// Register a facility for click events
Rectangle facility_bounds = {100, 200, 80, 60};
int handle = event_manager.RegisterRegion(
    MouseEventRegion(
        facility_bounds,
        // Hover callback
        [](const MouseEvent& event) {
            // Show facility info on hover
            return false; // Don't consume
        },
        // Click callback
        [this](const MouseEvent& event) {
            ShowFacilityDetails();
            return true; // Consume event
        },
        10 // Priority (higher = processed first)
    )
);

// In your update loop
MouseEvent event(GetMouseX(), GetMouseY(), 
                 IsMouseButtonDown(MOUSE_LEFT_BUTTON),
                 IsMouseButtonDown(MOUSE_RIGHT_BUTTON),
                 IsMouseButtonPressed(MOUSE_LEFT_BUTTON),
                 IsMouseButtonPressed(MOUSE_RIGHT_BUTTON));
event_manager.ProcessMouseEvents(event);

// When facility moves or is deleted
event_manager.UnregisterRegion(handle);
```

## Event Flow

### UI Elements (Hierarchical)

```
User clicks at (150, 50)
↓
Root Panel processes event
  ↓ Bubble down to children (reverse order)
  Child Panel checks if contains point → Yes
    ↓ Bubble down to children
    Button checks if contains point → Yes
      ↓ No children
      Button::OnClick() is called → Returns true (consumed)
    ✓ Event consumed, stop processing
```

### Game Objects (Region-based)

```
User clicks at (300, 400)
↓
MouseEventManager finds all regions at point
↓
Sort regions by priority (highest first)
↓
For each region:
  Call on_click callback
  If callback returns true → Stop processing
```

## Migration Guide

### Updating Existing UI Code

**Before:**
```cpp
int HandleMouse(int mouse_x, int mouse_y, bool clicked) {
    for (size_t i = 0; i < buttons.size(); ++i) {
        if (mouse_x >= button_x && mouse_x <= button_x + button_width &&
            mouse_y >= button_y && mouse_y <= button_y + button_height) {
            if (clicked) {
                return i;
            }
        }
    }
    return -1;
}
```

**After:**
```cpp
// In constructor: create buttons and add as children
for (size_t i = 0; i < menu_items.size(); ++i) {
    auto button = std::make_unique<Button>(...);
    button->SetClickCallback([this, i]() {
        selected_option = i;
    });
    panel->AddChild(std::move(button));
}

// In HandleMouse:
int HandleMouse(int mouse_x, int mouse_y, bool clicked) {
    MouseEvent event(mouse_x, mouse_y, false, false, clicked, false);
    selected_option = -1;
    panel->ProcessMouseEvent(event);
    return selected_option;
}
```

## Best Practices

1. **Use parent-child relationships**: Always add interactive elements as children when possible
2. **Consume events appropriately**: Return `true` from event handlers only when you want to stop propagation
3. **Set priorities**: Use higher priorities for elements that should receive events first
4. **Clean up**: Always unregister regions when game objects are destroyed
5. **Avoid manual hit testing**: Let the system handle it through `Contains()` and `ProcessMouseEvent()`

## Examples in Codebase

- **MainMenu**: Uses Button children with click callbacks
- **PauseMenu**: Uses Button children with click callbacks
- **BuildMenu**: Can be migrated to use clickable regions or buttons

## Future Enhancements

- Mouse wheel events
- Drag and drop support
- Double-click detection
- Right-click context menus
- Touch support for mobile platforms
