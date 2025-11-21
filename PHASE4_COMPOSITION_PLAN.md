# Phase 4: Full Composition for Info Windows

## Status: **COMPLETED** ✅

## Overview
Info windows currently create StatItems and SectionHeaders as UIElements but **do not add them as children**. Instead, they store them in `unique_ptr` members and manually render them using a `renderAt()` lambda. This violates proper Composite pattern and prevents automatic event handling.

## Current Architecture (Incomplete Composition)

```cpp
class FacilityWindow : public UIWindow {
private:
    // Stored as unique_ptr members (NOT added as children)
    std::unique_ptr<StatItem> occupancy_stat_;
    std::unique_ptr<StatItem> revenue_stat_;
    // ... 10+ more stats
    
    void BuildComponents() {
        // Creates but DOESN'T add as children
        occupancy_stat_ = std::make_unique<StatItem>(0, y, "Occupancy:");
    }
    
    void RenderContent() const {
        // Manual positioning and rendering
        auto renderAt = [x, y_base](UIElement* elem) {
            elem->SetRelativePosition(x, y_base + elem->GetRelativeBounds().y);
            elem->Render();  // Manual call!
        };
        renderAt(occupancy_stat_.get());  // Repeated for each stat
    }
};
```

**Problems:**
1. ❌ Not using Composite pattern properly
2. ❌ Manual rendering required
3. ❌ No automatic event handling
4. ❌ Children not in UIElement tree
5. ❌ Inconsistent with button_panel_ (which IS added as child)

## Target Architecture (Proper Composition)

```cpp
class FacilityWindow : public UIWindow {
private:
    // Raw pointers (ownership transferred to UIWindow)
    StatItem* occupancy_stat_;
    StatItem* revenue_stat_;
    // ... 10+ more stats
    
    void BuildComponents() {
        // Create AND add as children
        auto occupancy = std::make_unique<StatItem>(padding, y, "Occupancy:");
        occupancy_stat_ = occupancy.get();  // Store raw pointer
        AddChild(std::move(occupancy));     // Transfer ownership
    }
    
    void RenderContent() const override {
        // Automatic rendering via Panel!
        Panel::Render();  // Renders all children automatically
    }
};
```

**Benefits:**
1. ✅ Proper Composite pattern
2. ✅ Automatic rendering (no manual loops)
3. ✅ Automatic event handling
4. ✅ Children in proper UIElement tree
5. ✅ Consistent with button_panel_ pattern

## Required Changes

### 1. Update Headers (3 files)

**include/ui/info_windows.h**:
- Change all `std::unique_ptr<StatItem>` → `StatItem*`
- Change all `std::unique_ptr<SectionHeader>` → `SectionHeader*`
- Change `std::unique_ptr<Panel> button_panel_` → `Panel* button_panel_`
- Change `std::vector<std::unique_ptr<StatItem>> adjacency_items_` → `std::vector<StatItem*>`

**Affected members:**
- FacilityWindow: 9 StatItems, 2 SectionHeaders, 1 Panel, dynamic adjacency list
- PersonWindow: 11 StatItems, 2 SectionHeaders
- ElevatorWindow: 4 StatItems, dynamic queue items list

**Total: ~35 member variables**

### 2. Update BuildComponents() (3 functions)

**Pattern for static components:**
```cpp
// OLD:
occupancy_stat_ = std::make_unique<StatItem>(0, y, "Occupancy:");

// NEW:
auto occupancy = std::make_unique<StatItem>(WindowChrome::GetPadding(), y, "Occupancy:");
occupancy_stat_ = occupancy.get();
AddChild(std::move(occupancy));
```

**Pattern for dynamic components (adjacency items, queue items):**
- Created in UpdateComponentValues()
- Use `RemoveChild()` to remove old ones
- Use `AddChild()` to add new ones
- Store raw pointers in vector

### 3. Update RenderContent() (3 functions)

**Simplify drastically:**
```cpp
void FacilityWindow::RenderContent() const override {
    // Render all children automatically
    Panel::Render();
    
    // Manual rendering only for non-UIElement components (AlertBar)
    const Rectangle bounds = GetAbsoluteBounds();
    const int x = static_cast<int>(bounds.x) + WindowChrome::GetPadding();
    const int y_base = static_cast<int>(bounds.y) + WindowChrome::GetTitleBarHeight() 
                     + WindowChrome::GetPadding();
    
    fire_alert_->Render(x, y_base);
    security_alert_->Render(x, y_base);
}
```

**Removes:**
- `renderAt` lambda (~10 lines)
- Manual positioning loops (~30-50 lines per window)
- Manual render calls (15-30 calls per window)

**Net result: ~100 lines removed across 3 windows**

### 4. Update UpdateComponentValues() (for dynamic items)

**Adjacency items pattern:**
```cpp
void FacilityWindow::UpdateComponentValues() {
    // ... other updates ...
    
    // Remove old dynamic children
    for (auto* item : adjacency_items_) {
        RemoveChild(item);
    }
    adjacency_items_.clear();
    
    // Add new dynamic children
    float adj_y_base = adjacency_header_->GetRelativeBounds().y + 25;
    float adj_y = 0;
    
    for (const auto& effect : info_.adjacency_effects) {
        auto item = std::make_unique<StatItem>(
            WindowChrome::GetPadding(), adj_y_base + adj_y, ""
        );
        item->SetValue(effect, color);
        
        adjacency_items_.push_back(item.get());  // Store raw pointer
        AddChild(std::move(item));               // Transfer ownership
        adj_y += 18;
    }
    
    adjacency_header_->SetVisible(!info_.adjacency_effects.empty());
}
```

## Implementation Checklist

### FacilityWindow
- [x] Update header: 9 StatItems, 2 SectionHeaders, 1 Panel → raw pointers
- [x] Update BuildComponents(): Add 12 components as children
- [x] Update UpdateComponentValues(): Handle dynamic adjacency items
- [x] Update RenderContent(): Replace with Panel::Render()
- [x] Test: Verify window renders correctly
- [x] Test: Verify button clicks work
- [x] Test: Verify adjacency items update dynamically

### PersonWindow
- [x] Update header: 11 StatItems, 2 SectionHeaders → raw pointers
- [x] Update BuildComponents(): Add 13 components as children
- [x] Update RenderContent(): Replace with Panel::Render()
- [x] Test: Verify conditional sections (staff, needs) work

### ElevatorWindow
- [x] Update header: 4 StatItems, dynamic queue → raw pointers
- [x] Update BuildComponents(): Add 4 components as children
- [x] Update UpdateComponentValues(): Handle dynamic queue items
- [x] Update RenderContent(): Replace with Panel::Render()
- [x] Test: Verify queue items update dynamically

### Final Verification
- [x] Build successful
- [x] No memory leaks (children owned by UIWindow)
- [x] All windows render correctly
- [x] Mouse events work (buttons, stats if implemented)
- [x] Dynamic content updates properly
- [x] Window resizing works (adjacency, queue changes)

## Estimated Effort

- **Files to modify:** 6 (3 headers + 3 implementations)
- **Functions to update:** 9 (3x BuildComponents, 3x RenderContent, 3x Update)
- **Member variables to change:** ~35
- **Lines of code removed:** ~150 (manual rendering)
- **Lines of code added:** ~80 (proper AddChild pattern)
- **Net reduction:** ~70 lines
- **Time estimate:** 1-2 hours

## Benefits After Completion

### Immediate
1. **Automatic rendering** - No more manual `renderAt()` loops
2. **Automatic events** - Stats can have click handlers, tooltips
3. **Consistent architecture** - All windows use same pattern
4. **Less code** - ~70 lines removed
5. **Proper Composite pattern** - Children in tree

### Future Enablement
1. **Click on stat for details** - Drill-down to detailed view
2. **Hover tooltips** - Explain what each stat means
3. **Animated transitions** - Stats can fade/slide when updated
4. **Keyboard navigation** - Tab through stats
5. **Accessibility** - Screen readers can traverse tree

## Notes

- **AlertBar is not a UIElement** - Must stay manually rendered until refactored
- **Dynamic items require RemoveChild** - Can't just clear vector
- **Ownership is key** - UIWindow owns children, we store raw pointers
- **Pattern already exists** - button_panel_ shows the correct approach
- **This is optional** - Current manual rendering works, just not ideal

## Related Work

- **Phase 1**: WindowChrome extraction ✅
- **Phase 2**: UIWindow inherits from Panel ✅
- **Phase 3**: StatItem/SectionHeader modernization ✅
- **Phase 4**: Full composition (this document) ✅
- **Phase 5** (future): SpeedControlPanel refactor (uses ButtonBounds, not Buttons)
- **Phase 6** (future): AlertBar becomes UIElement

## Implementation Notes (Completed)

### What Was Implemented

1. **Header Changes**: Converted all `std::unique_ptr<StatItem>`, `std::unique_ptr<SectionHeader>`, and `std::unique_ptr<Panel>` members to raw pointers across all three info windows (35 member variables total).

2. **BuildComponents() Updates**: All static components now use the proper pattern:
   ```cpp
   auto component = std::make_unique<ComponentType>(padding, y, ...);
   member_ptr_ = component.get();  // Store raw pointer
   AddChild(std::move(component)); // Transfer ownership
   ```

3. **Dynamic Component Management**: Adjacency items (FacilityWindow) and queue items (ElevatorWindow) properly remove old children via `RemoveChild()` before adding new ones.

4. **RenderContent() Simplification**:
   - **FacilityWindow**: Reduced from ~50 lines to 11 lines (78% reduction)
   - **PersonWindow**: Reduced from ~45 lines to 3 lines (93% reduction)
   - **ElevatorWindow**: Reduced from ~23 lines to 3 lines (87% reduction)
   - All now use `Panel::Render()` for automatic child rendering
   - Only AlertBar (not a UIElement) still requires manual rendering

5. **Conditional Rendering**: Kept existing conditional logic in UpdateComponentValues() rather than using visibility API (StatItem/SectionHeader don't expose visibility control yet - future enhancement).

### Results

- **Build**: ✅ Successful (only pre-existing warnings about missing `override` keywords)
- **Tests**: ✅ All 12 tests passing
- **Code Reduction**: ~110 lines removed, ~80 lines added = **net -30 lines**
- **Architectural Improvement**: Proper Composite pattern, automatic rendering, proper ownership model

### Future Enhancements

- Add visibility API to UIElement base class for conditional rendering without updating values
- Convert AlertBar to UIElement for full automatic rendering
- Consider dynamic layout instead of fixed Y positions

## References

- See `TabBar` for excellent example of proper child management
- See `GridPanel` for dynamic item handling
- See `ActionBar` for button children pattern
- Current button_panel_ in FacilityWindow shows correct ownership model
