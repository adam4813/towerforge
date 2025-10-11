# Tooltip System

## Overview

The Tooltip System provides contextual, helpful information for all interactive UI elements in TowerForge. Tooltips appear on hover and are accessible via keyboard navigation, enhancing usability and accessibility.

## Architecture

### Components

1. **Tooltip Class** (`include/ui/tooltip.h`)
   - Represents a single tooltip with static or dynamic text
   - Supports text generation functions for state-based content

2. **TooltipManager Class** (`include/ui/tooltip.h`)
   - Manages tooltip display lifecycle
   - Handles hover delays and positioning
   - Supports keyboard navigation focus
   - Ensures tooltips stay on screen

### Key Features

- **Hover Tooltips**: Appear after 0.5 second hover delay
- **Dynamic Content**: Update based on game state (funds, locked items, etc.)
- **Word Wrapping**: Automatically wraps long text (max 300px width)
- **Smart Positioning**: Adjusts to stay within screen bounds
- **Keyboard Navigation**: Support for Tab key navigation (focus-based)
- **Context Awareness**: Different tooltip content based on state

## Integration

### Build Menu Tooltips

Each facility button shows:
- Facility name and cost
- Size (width in cells)
- Current affordability status
- Tutorial mode lock status
- Keyboard shortcut (if applicable)

Example:
```
Lobby - $1,000
Width: 10 cells
Click to select for placement
```

When insufficient funds:
```
Lobby - $1,000
Width: 10 cells
[INSUFFICIENT FUNDS]
```

### HUD Tooltips

**Top Bar Elements:**
- Funds display: Explains income and balance
- Population: How to increase population
- Time/Day: Simulation time explanation
- Speed indicator: Current speed and how to change

**Speed Controls:**
- Pause button: "Pause/Resume simulation, Hotkey: SPACE"
- 1x/2x/4x buttons: Speed explanations

**Star Rating Panel:**
- Explains rating criteria
- How to earn stars
- Next milestone requirements

### Placement System Tooltips

Grid interaction tooltips show:
- Floor and column coordinates
- Selected facility information
- Placement cost
- Space availability
- Fund availability
- Demolish refund amount (50%)

Example during placement:
```
Place Lobby
Cost: $1,000
Floor: 5, Column: 3
```

If invalid:
```
Place Lobby
Cost: $1,000
Floor: 5, Column: 3
[SPACE NOT AVAILABLE]
```

## Usage Example

### Basic Static Tooltip

```cpp
#include "ui/tooltip.h"

// Create a simple tooltip
Tooltip tooltip("This is a helpful tooltip");

// Show via tooltip manager
tooltip_manager->ShowTooltip(tooltip, x, y, width, height);
```

### Dynamic Tooltip

```cpp
// Create dynamic tooltip that updates based on state
Tooltip tooltip([&current_funds, cost]() {
    std::stringstream ss;
    ss << "Cost: $" << cost << "\n";
    if (current_funds < cost) {
        ss << "[INSUFFICIENT FUNDS]";
    } else {
        ss << "Click to purchase";
    }
    return ss.str();
});

tooltip_manager->ShowTooltip(tooltip, x, y, width, height);
```

### Tooltip Manager Integration

```cpp
// In UI component initialization
auto tooltip_manager = std::make_unique<TooltipManager>();

// In update loop
void Update() {
    int mouse_x = GetMouseX();
    int mouse_y = GetMouseY();
    
    tooltip_manager->Update(mouse_x, mouse_y);
    
    // Check if hovering over element
    if (tooltip_manager->IsHovering(mouse_x, mouse_y, element_x, element_y, 
                                    element_width, element_height)) {
        Tooltip tooltip("Element description");
        tooltip_manager->ShowTooltip(tooltip, element_x, element_y, 
                                     element_width, element_height);
    } else {
        tooltip_manager->HideTooltip();
    }
}

// In render loop
void Render() {
    // Render UI elements first
    RenderUIElements();
    
    // Render tooltips on top
    tooltip_manager->Render();
}
```

## Keyboard Navigation

### Tab Navigation Support

The tooltip system supports keyboard navigation through the `SetKeyboardFocus()` method:

```cpp
// Set focus on element
tooltip_manager->SetKeyboardFocus(element_id);

// When element has focus, show its tooltip
if (tooltip_manager->GetKeyboardFocus() == element_id) {
    Tooltip tooltip("Element description");
    tooltip_manager->ShowTooltip(tooltip, element_x, element_y, 
                                 element_width, element_height);
}

// Clear focus
tooltip_manager->ClearKeyboardFocus();
```

### Keyboard Shortcuts Shown in Tooltips

Many tooltips include keyboard shortcut information:
- "Hotkey: D" for demolish mode
- "Hotkey: Ctrl+Z" for undo
- "Hotkey: Ctrl+Y" for redo
- "Hotkey: SPACE" for pause/resume

## Visual Design

### Appearance

- **Background**: Black with 95% opacity
- **Border**: Gold color matching UI theme
- **Text Color**: White for readability
- **Font Size**: 12pt (smaller than main UI text)
- **Padding**: 8px all around
- **Max Width**: 300px with automatic word wrapping

### Positioning

1. **Default**: Below element with 10px offset
2. **Screen Edge**: Adjusts to stay on screen
3. **Vertical Overflow**: Shows above element if needed
4. **Horizontal Overflow**: Shifts left to fit within screen

## Implementation Details

### Constants

```cpp
static constexpr float HOVER_DELAY = 0.5f;      // 0.5 seconds before showing
static constexpr int TOOLTIP_PADDING = 8;        // Internal padding
static constexpr int TOOLTIP_FONT_SIZE = 12;     // Font size
static constexpr int TOOLTIP_MAX_WIDTH = 300;    // Maximum width
static constexpr int TOOLTIP_OFFSET_Y = 10;      // Offset below element
```

### Word Wrapping Algorithm

The tooltip system automatically wraps text to fit within the maximum width:
1. Split text by spaces
2. Measure each word
3. Build lines that fit within max width
4. Break long words if necessary
5. Render each line with proper spacing

## Accessibility Features

1. **High Contrast**: Black background with white text and gold border
2. **Readable Font Size**: 12pt minimum
3. **Keyboard Support**: Full keyboard navigation capability
4. **Clear Hierarchy**: Tooltips render on top of all other UI
5. **Consistent Behavior**: Same hover delay across all elements
6. **Screen Reader Ready**: Text-based content structure

## Best Practices

### Writing Tooltip Content

1. **Be Concise**: Keep tooltips brief and to the point
2. **Use Context**: Include relevant state information
3. **Show Shortcuts**: Always mention keyboard shortcuts
4. **Explain Actions**: Make it clear what will happen
5. **Indicate State**: Show if action is unavailable and why

### Good Examples

```
"Lobby - $1,000
Width: 10 cells
Click to select for placement"
```

```
"Pause/Resume simulation
Hotkey: SPACE"
```

```
"Place Office
Cost: $5,000
Floor: 3, Column: 5
[INSUFFICIENT FUNDS]"
```

### Bad Examples

```
"Lobby"  // Too vague
```

```
"This is the lobby facility that costs $1,000 and is 10 cells wide and can be placed by clicking on it"  // Too long
```

## Performance Considerations

- Tooltips are only rendered when visible
- Hover delay prevents excessive tooltip creation
- Dynamic tooltips are evaluated only when visible
- Single tooltip instance at a time reduces memory usage
- Word wrapping is cached within render cycle

## Future Enhancements

Potential improvements:
1. Custom tooltip styles per UI section
2. Rich text formatting (bold, colors)
3. Icon support in tooltips
4. Multi-column tooltips for complex data
5. Tooltip animations (fade in/out)
6. Configurable hover delay
7. Tooltip history for keyboard users
8. Sound effects on tooltip appearance
9. Localization support
10. Custom positioning strategies

## Dependencies

- **Raylib**: For text rendering and measurement
- **C++20**: For std::function and modern features
- **UI Components**: Integrated with HUD, BuildMenu, PlacementSystem

## Testing

To test tooltips:

1. **Hover Test**: Hover over each UI element and verify tooltip appears
2. **Content Test**: Verify tooltip content is accurate and helpful
3. **State Test**: Test dynamic content with different game states
4. **Position Test**: Test tooltips near screen edges
5. **Keyboard Test**: Test keyboard navigation focus
6. **Performance Test**: Verify no lag when showing tooltips

## Known Limitations

- Single tooltip at a time (no simultaneous tooltips)
- Basic text-only format (no rich text yet)
- Fixed hover delay (not customizable per element)
- No tooltip history for keyboard navigation
