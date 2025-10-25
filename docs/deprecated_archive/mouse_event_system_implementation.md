# Mouse Event System - Implementation Notes

## Overview
This document describes the implementation of the unified mouse event system for TowerForge, including design decisions, current limitations, and future improvements.

## Implementation Status

### Completed
- ✅ Core mouse event interface (`IMouseInteractive`)
- ✅ Mouse event data structure (`MouseEvent`)
- ✅ Mouse event manager for game objects (`MouseEventManager`)
- ✅ Bubble-down event propagation in UIElement
- ✅ Button class with click callbacks and hover states
- ✅ MainMenu integration
- ✅ PauseMenu integration
- ✅ GeneralSettingsMenu integration
- ✅ Comprehensive documentation and examples

### Not Yet Migrated
- BuildMenu (has custom rendering and more complex interaction patterns)
- AudioSettingsMenu
- ResearchTreeMenu
- SaveLoadMenu
- AchievementsMenu

These can be migrated on an as-needed basis using the same patterns demonstrated in the completed menus.

## Design Decisions

### 1. Bubble-Down Event Propagation
**Decision**: Events propagate from parent to children (bubble-down) rather than child to parent (bubble-up).

**Rationale**: 
- Matches typical UI framework behavior
- Allows children to consume events before parents see them
- Simpler to reason about for layered UIs

### 2. Separate Systems for UI vs Game Objects
**Decision**: UI elements use hierarchical propagation through `ProcessMouseEvent()`, while game objects use region-based handling via `MouseEventManager`.

**Rationale**:
- UI elements naturally form parent-child hierarchies
- Game objects exist in world space and need spatial queries
- Allows optimization of each system independently
- Clearer separation of concerns

### 3. Raw Pointers for Button Access
**Decision**: Menus store raw pointers to buttons while parent panels own them via `unique_ptr`.

**Rationale**:
- Parent panel clearly owns the buttons (single ownership)
- Raw pointers are safe as they only exist while parent exists
- Avoids shared ownership complexity
- Allows direct access for rendering updates

### 4. Dynamic Button Positioning
**Decision**: Button positions and colors are updated during render loop.

**Rationale**:
- Supports dynamic screen resizing
- Allows easy centering calculations
- Enables selection-based styling changes
- Simplifies implementation at cost of some performance

**Trade-off**: This is less efficient than caching positions, but provides flexibility. Could be optimized in the future by:
- Calculating positions only on screen resize events
- Caching colors and updating only on selection change
- Using dirty flags to minimize updates

### 5. Click Callbacks vs Virtual Methods
**Decision**: Buttons use callbacks (`std::function`) rather than requiring subclassing.

**Rationale**:
- More flexible and easier to use
- Allows inline lambda definitions
- Reduces boilerplate code
- Enables capturing context easily

## Current Limitations

### 1. No Touch/Multi-touch Support
Currently only supports mouse input. Future enhancement needed for mobile platforms.

### 2. No Drag and Drop
The system doesn't currently support drag and drop operations. This could be added by:
- Adding `OnDragStart()`, `OnDrag()`, `OnDragEnd()` to interface
- Tracking drag state in `ProcessMouseEvent()`

### 3. No Double-Click Detection
Single clicks only. Double-click could be added by tracking click timing.

### 4. No Right-Click Context Menus
Right-click is passed through but there's no built-in context menu support.

### 5. Performance Considerations
- Button positions calculated every frame (could cache)
- Event processing happens even when mouse hasn't moved (could optimize)
- No spatial indexing for MouseEventManager (fine for small counts)

## Testing Recommendations

Once build environment is available:

1. **Basic Interaction Tests**
   - Verify buttons respond to clicks
   - Verify hover states update correctly
   - Test keyboard + mouse selection sync

2. **Event Propagation Tests**
   - Nested buttons (child consumes, parent doesn't receive)
   - Multiple overlapping regions (priority ordering)
   - Event consumption (verify propagation stops)

3. **Edge Cases**
   - Very fast mouse movement
   - Click and drag outside element
   - Multiple buttons pressed simultaneously
   - Screen resize during interaction

4. **Performance Tests**
   - Many interactive elements (100+)
   - Rapid mouse movement
   - Frame rate impact

## Future Enhancements

### High Priority
1. **Optimization**: Cache button positions, update only on screen resize
2. **Optimization**: Cache button colors, update only on selection change
3. **Testing**: Add automated tests once test infrastructure exists

### Medium Priority
1. **Drag and Drop**: Add drag event support to interface
2. **Tooltips**: Integrate with existing tooltip system via hover events
3. **Keyboard Navigation**: Better integration with keyboard focus

### Low Priority
1. **Touch Support**: Multi-touch and gesture support
2. **Double-Click**: Add double-click detection
3. **Context Menus**: Right-click menu framework
4. **Spatial Indexing**: For MouseEventManager with many regions
5. **Animation**: Built-in hover/click animation support

## Migration Guide for Other Menus

To migrate a menu to the new system:

1. Add member variables:
   ```cpp
   std::unique_ptr<Panel> menu_panel_;
   std::vector<Button*> button_ptrs_;
   int selected_menu_option_;
   ```

2. In constructor, create buttons and add to panel:
   ```cpp
   auto button = std::make_unique<Button>(...);
   button->SetClickCallback([this, index]() {
       selected_menu_option_ = index;
   });
   Button* ptr = button.get();
   button_ptrs_.push_back(ptr);
   menu_panel_->AddChild(std::move(button));
   ```

3. Update HandleMouse:
   ```cpp
   MouseEvent event(...);
   selected_menu_option_ = -1;
   menu_panel_->ProcessMouseEvent(event);
   
   // Sync keyboard selection
   for (size_t i = 0; i < button_ptrs_.size(); ++i) {
       if (button_ptrs_[i]->IsHovered()) {
           selected_option_ = i;
           break;
       }
   }
   
   return selected_menu_option_;
   ```

4. Update rendering to use button positions and render methods

## Related Files
- `include/ui/mouse_interface.h` - Interface definition
- `include/ui/mouse_event_manager.h` - Game object event manager
- `include/ui/ui_element.h` - UI element with event support
- `src/ui/mouse_event_manager.cpp` - Event manager implementation
- `src/ui/ui_element.cpp` - Event propagation implementation
- `docs/mouse_event_system.md` - User documentation
- `docs/mouse_event_manager_example.cpp` - Example code
