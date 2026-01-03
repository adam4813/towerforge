# Help System Implementation

## Overview

The help system provides context-sensitive assistance to players through an in-game overlay accessible via the F1 key or
help icon buttons. It automatically detects the current game context and displays relevant help topics.

## Features

### 1. Context Detection

The help system automatically detects which screen or menu is active:

- **Main Game**: General gameplay mechanics and controls
- **Build Menu**: Facility placement and construction
- **Research Tree**: Research mechanics and tower points
- **Mods Menu**: Mod management and loading
- **Staff Management**: Hiring and managing staff
- **Settings**: Game configuration options
- **Tutorial**: Tutorial-specific guidance
- **Pause Menu**: Pause menu options
- **History Panel**: Undo/redo functionality
- **Notification Center**: Notification management

### 2. Accessibility

- **F1 Key**: Press F1 to open help for the current context
- **ESC Key**: Close help overlay
- **Help Icons**: Clickable "?" icons in menus (research tree, build menu)
- **Mouse Wheel**: Scroll through help content
- **Click Outside**: Click outside the overlay to close

### 3. Help Content Structure

Each context contains multiple help topics with:

- **Title**: Topic heading
- **Content**: Detailed explanation with word wrapping
- **Quick Tips**: Bulleted list of actionable tips (optional)

## Implementation Details

### Core Classes

#### `HelpSystem` Class

Location: `include/ui/help_system.h`, `src/ui/help_system.cpp`

**Key Methods:**

- `Initialize()`: Load help content for all contexts
- `Show(HelpContext)`: Display help for specific context
- `Hide()`: Close help overlay
- `Toggle()`: Toggle help visibility
- `Update(delta_time)`: Update animations
- `Render()`: Draw help overlay
- `HandleMouse(x, y, clicked)`: Process mouse input

**Key Members:**

- `help_content_`: Map of contexts to help topics
- `current_context_`: Active help context
- `scroll_offset_`: Current scroll position
- `visible_`: Whether help is shown

### Integration Points

#### Game Class Integration

Location: `src/core/game.cpp`

**Initialization:**

```cpp
// In InitializeGameSystems()
help_system_ = new HelpSystem();
help_system_->Initialize();
```

**Input Handling:**

```cpp
// F1 key toggles help
if (help_system_ != nullptr && IsKeyPressed(KEY_F1)) {
    // Determine context based on active UI
    HelpContext context = /* detect context */;
    help_system_->Show(context);
}

// ESC closes help if visible
if (IsKeyPressed(KEY_ESCAPE)) {
    if (help_system_ != nullptr && help_system_->IsVisible()) {
        help_system_->Hide();
    }
}
```

**Rendering:**

```cpp
// Render help last (on top of everything)
if (help_system_ != nullptr && help_system_->IsVisible()) {
    help_system_->Render();
}
```

### Visual Design

#### Help Overlay Layout

```
┌─────────────────────────────────────────────────┐
│ [Context Name]                           [X]    │ ← Header (50px)
├─────────────────────────────────────────────────┤
│                                                 │
│  ┌─ Topic Title                                │
│  │  Topic content with automatic word          │
│  │  wrapping for readability...                │
│  │                                             │
│  │  Quick Tips:                                │
│  │  • Tip one                                  │
│  │  • Tip two                                  │
│  └─                                             │
│                                                 │
│  ┌─ Another Topic                              │ ← Scrollable
│  │  More content...                            │   Content
│  └─                                             │
│                                                 │
│                                              [█]│ ← Scrollbar
│ Press F1 or ESC to close                       │
└─────────────────────────────────────────────────┘
   Dimensions: 700x550 pixels, centered on screen
```

#### Help Icon Design

```
┌───┐
│ ? │  ← Blue circle with "?" text
└───┘    Hover: Lighter blue with subtle glow
         Click: Opens help for menu context
```

### Help Content Examples

#### Main Gameplay Help

- **Welcome to TowerForge**: Introduction to game mechanics
- **Basic Controls**: Keyboard shortcuts and mouse controls
- **Managing Your Tower**: Rating system and tenant satisfaction

#### Build Menu Help

- **Building Facilities**: How to place facilities
- **Floor Management**: Adding floors and basements
- **Undo and Redo**: Using history functionality

#### Research Tree Help

- **Research System**: Unlocking upgrades
- **Earning Research Points**: Milestone achievements

## User Experience Flow

### Opening Help

1. User presses F1 or clicks help icon
2. System detects current context
3. Help overlay appears with fade-in animation
4. Relevant help topics are displayed
5. User can scroll to read all content

### Navigating Help

1. Mouse wheel scrolls through content
2. ESC or click outside closes help
3. Visual feedback on hover/click

### Context Switching

1. User closes help
2. Opens different menu (e.g., research tree)
3. Presses F1 again
4. Help shows research tree content automatically

## Customization Guide

### Adding New Help Topics

1. **Define Context** (if new):

```cpp
// In help_system.h
enum class HelpContext {
    // ... existing contexts
    NewContext
};
```

2. **Create Initialization Method**:

```cpp
// In help_system.cpp
void HelpSystem::InitializeNewContextHelp() {
    std::vector<HelpTopic> topics;
    
    topics.emplace_back(
        "Topic Title",
        "Topic content explaining the feature...",
        std::vector<std::string>{
            "Quick tip 1",
            "Quick tip 2"
        }
    );
    
    help_content_[HelpContext::NewContext] = topics;
}
```

3. **Call in Initialize()**:

```cpp
void HelpSystem::Initialize() {
    // ... existing initializations
    InitializeNewContextHelp();
}
```

## Testing

### Manual Testing Checklist

- [ ] F1 opens help in main game
- [ ] F1 opens research help in research tree
- [ ] F1 opens build help when build menu is focused
- [ ] ESC closes help overlay
- [ ] Click outside closes help
- [ ] Mouse wheel scrolls content
- [ ] Help icons are visible in menus
- [ ] Clicking help icons opens correct context
- [ ] All help content displays correctly
- [ ] Text wrapping works properly
- [ ] Scrollbar appears for long content

### Known Limitations

- Help content is currently hardcoded (could be externalized to JSON/XML)
- No search functionality within help
- No hyperlinks between help topics
- Single language support (English only)

## Future Enhancements

### Potential Improvements

1. **Interactive Tutorials**: Link help topics to interactive tutorials
2. **Video/GIF Support**: Embed demonstrations
3. **Search Functionality**: Search across all help content
4. **Bookmarks**: Save frequently accessed topics
5. **Localization**: Multi-language support
6. **Context History**: Navigate back through help contexts
7. **External Content**: Load help from external files for easier updates
8. **Tooltips Integration**: Show abbreviated help as tooltips

### Performance Considerations

- Help system only updates when visible
- Content is pre-loaded during initialization
- Rendering is optimized with scissor mode for scrolling
- No dynamic allocations during rendering

## Keyboard Shortcuts Reference

| Key           | Action                                  |
|---------------|-----------------------------------------|
| F1            | Toggle help overlay (context-sensitive) |
| ESC           | Close help overlay                      |
| Mouse Wheel   | Scroll help content                     |
| Click Outside | Close help overlay                      |

## Related Documentation

- [Tutorial System](TUTORIAL_IMPLEMENTATION.md)
- [Tooltip System](TOOLTIP_SYSTEM.md)
- [UI Element System](ui_element_system.md)
- [Notification Center](NOTIFICATION_CENTER.md)

## Change Log

### Version 1.0 (Initial Implementation)

- Context-sensitive help system
- F1 key support
- Help icons in menus
- Scrollable help content
- 10 help contexts with comprehensive topics
- Integration with game loop and UI
