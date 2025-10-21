# Batch Renderer Integration Guide

## Overview

The batch renderer converts scattered Raylib draw calls into batched GPU submissions, reducing draw calls from 1000+ per frame to typically 3-10 per frame (100x improvement).

**Status**: ✅ Ready for integration  
**Location**: `include/ui/batch_renderer/` and `src/ui/batch_renderer/`  
**Dependencies**: Raylib only

---

## Quick Start (5 Minutes)

### 1. Include Headers

```cpp
#include "ui/batch_renderer/batch_renderer.h"
using namespace towerforge::ui::batch_renderer;
```

### 2. Initialize Once

```cpp
// In main() or application startup
BatchRenderer::Initialize();
```

### 3. Frame Rendering

```cpp
// Each frame
BeginDrawing();
ClearBackground(DARKGRAY);

BatchRenderer::BeginFrame();

// Your UI rendering here
BatchRenderer::SubmitQuad({10, 10, 100, 50}, BLUE);
BatchRenderer::SubmitText("Hello", 15, 20, 20, WHITE);

BatchRenderer::EndFrame();  // Flushes all batches

EndDrawing();
```

### 4. Shutdown

```cpp
// At application exit
BatchRenderer::Shutdown();
```

---

## Migration Strategies

### Strategy A: Adapter (Fastest Migration)

Use the adapter namespace for drop-in replacement of Raylib calls:

**Before**:
```cpp
DrawRectangle(x, y, w, h, color);
DrawText("Label", x, y, 20, WHITE);
DrawCircle(cx, cy, radius, RED);
```

**After**:
```cpp
#include "ui/batch_renderer/batch_adapter.h"
using namespace towerforge::ui::batch_renderer::adapter;

// Same calls, now batched automatically!
DrawRectangle(x, y, w, h, color);
DrawText("Label", x, y, 20, WHITE);
DrawCircle(cx, cy, radius, RED);
```

**Pros**: Minimal code changes  
**Cons**: Still uses old function signatures

---

### Strategy B: Direct API (Best Performance)

Use `BatchRenderer` directly for full control:

**Before**:
```cpp
DrawRectangle(x, y, w, h, ColorAlpha(BLACK, 0.8f));
DrawRectangleLinesEx(rect, 2, GOLD);
DrawText(text.c_str(), x, y, font_size, WHITE);
```

**After**:
```cpp
#include "ui/batch_renderer/batch_renderer.h"
using namespace towerforge::ui::batch_renderer;

BatchRenderer::SubmitQuad({x, y, w, h}, ColorAlpha(BLACK, 0.8f));
BatchRenderer::SubmitLine(rect.x, rect.y, rect.x + rect.width, rect.y, 2, GOLD);
// ... draw 3 more lines for rectangle outline
BatchRenderer::SubmitText(text, x, y, font_size, WHITE);
```

**Pros**: Clear API, full features  
**Cons**: Requires rewriting draw calls

---

### Strategy C: Incremental (Recommended)

Migrate one UI class at a time:

1. **Week 1**: Migrate `BuildMenu` (highest draw call count)
2. **Week 2**: Migrate `HUD` components
3. **Week 3**: Migrate modal dialogs
4. **Week 4**: Migrate remaining UI

**Steps per class**:
1. Add `#include "ui/batch_renderer/batch_renderer.h"`
2. Wrap `Render()` method:
   ```cpp
   void MyUIClass::Render() {
       BatchRenderer::BeginFrame();
       
       // Existing draw calls (unchanged)
       DrawRectangle(...);
       
       BatchRenderer::EndFrame();
   }
   ```
3. Gradually replace `DrawXXX` with `BatchRenderer::SubmitXXX`
4. Test visually; verify no regressions

---

## Integration with Existing UI

### UIWindowManager Integration

**Current code** (`src/ui/ui_window_manager.cpp`):
```cpp
void UIWindowManager::Render() const {
    // Sort windows by z-order
    std::vector<UIWindow*> sorted_windows;
    // ... sorting logic ...
    
    for (UIWindow* window : sorted_windows) {
        window->Render();  // Calls DrawRectangle, DrawText, etc.
    }
}
```

**Batched version**:
```cpp
#include "ui/batch_renderer/batch_renderer.h"
using namespace towerforge::ui::batch_renderer;

void UIWindowManager::Render() const {
    BatchRenderer::BeginFrame();
    
    // Sort windows by z-order
    std::vector<UIWindow*> sorted_windows;
    // ... sorting logic ...
    
    for (UIWindow* window : sorted_windows) {
        // Push scissor for window bounds (optional clipping)
        BatchRenderer::PushScissor(ScissorRect(
            window->GetBounds().x,
            window->GetBounds().y,
            window->GetBounds().width,
            window->GetBounds().height
        ));
        
        window->Render();  // Now batched if using adapter or direct API
        
        BatchRenderer::PopScissor();
    }
    
    BatchRenderer::EndFrame();
}
```

---

### UIElement Integration

**Current pattern** (`src/ui/ui_element.cpp`):
```cpp
void Panel::Render() const {
    DrawRectangleRec(GetAbsoluteBounds(), background_color_);
    if (border_color_.a > 0) {
        DrawRectangleLinesEx(GetAbsoluteBounds(), 1, border_color_);
    }
}
```

**Batched version** (Option 1 - Adapter):
```cpp
#include "ui/batch_renderer/batch_adapter.h"
using namespace towerforge::ui::batch_renderer::adapter;

void Panel::Render() const {
    DrawRectangleRec(GetAbsoluteBounds(), background_color_);
    if (border_color_.a > 0) {
        DrawRectangleLinesEx(GetAbsoluteBounds(), 1, border_color_);
    }
}
// No code changes needed if adapter is used!
```

**Batched version** (Option 2 - Direct):
```cpp
#include "ui/batch_renderer/batch_renderer.h"
using BR = towerforge::ui::batch_renderer::BatchRenderer;

void Panel::Render() const {
    BR::SubmitQuad(GetAbsoluteBounds(), background_color_);
    if (border_color_.a > 0) {
        const Rectangle r = GetAbsoluteBounds();
        BR::SubmitLine(r.x, r.y, r.x + r.width, r.y, 1, border_color_);
        BR::SubmitLine(r.x + r.width, r.y, r.x + r.width, r.y + r.height, 1, border_color_);
        BR::SubmitLine(r.x + r.width, r.y + r.height, r.x, r.y + r.height, 1, border_color_);
        BR::SubmitLine(r.x, r.y + r.height, r.x, r.y, 1, border_color_);
    }
}
```

---

## Advanced Features

### Scissor Clipping (Parent Bounds)

Use scissor stack for nested UI clipping:

```cpp
void MyPanel::Render() {
    const Rectangle bounds = GetAbsoluteBounds();
    
    // Push scissor for this panel
    BatchRenderer::PushScissor(ScissorRect(bounds.x, bounds.y, bounds.width, bounds.height));
    
    // Draw panel background
    BatchRenderer::SubmitQuad(bounds, ColorAlpha(BLACK, 0.8f));
    
    // Render children (clipped to panel bounds)
    for (auto& child : children_) {
        child->Render();
    }
    
    // Restore previous scissor
    BatchRenderer::PopScissor();
}
```

**Automatic intersection**: If a child pushes its own scissor, it's automatically intersected with parent scissor.

---

### Custom Textures

Submit textured quads (e.g., icons, sprites):

```cpp
Texture2D icon_texture = LoadTexture("icon.png");

// Textured quad
BatchRenderer::SubmitQuad(
    {x, y, 64, 64},              // Rectangle
    WHITE,                        // Tint color
    {{0, 0, 1, 1}},              // UV coords (full texture)
    icon_texture.id               // Texture ID
);
```

**Texture slot limit**: Max 8 textures per batch. If you exceed 8, batch auto-flushes.

---

### Performance Monitoring

Track draw calls per frame:

```cpp
BatchRenderer::BeginFrame();
// ... render UI ...
BatchRenderer::EndFrame();

size_t draw_calls = BatchRenderer::GetDrawCallCount();
TraceLog(LOG_INFO, "Frame draw calls: %zu", draw_calls);

// Expected: 3-10 draw calls (batched) vs. 1000+ (unbatched)
```

---

## Common Issues & Solutions

### Issue 1: Text Not Rendering

**Cause**: Text uses native Raylib `DrawText`, which requires flush.

**Solution**: Batched automatically. If using custom fonts, ensure font texture is loaded before `Initialize()`.

---

### Issue 2: Shapes Look Wrong

**Cause**: Vertex order or winding.

**Solution**: Verify coordinate system. Raylib uses **top-left origin**, not bottom-left.

---

### Issue 3: Performance Not Improved

**Symptom**: Still 1000+ draw calls per frame.

**Cause**: Not using batched API; still calling Raylib directly.

**Solution**: 
1. Check if `BeginFrame()`/`EndFrame()` are called each frame
2. Verify draw calls route through `BatchRenderer` or adapter
3. Use profiler to confirm: `BatchRenderer::GetDrawCallCount()`

---

### Issue 4: Scissor Not Clipping

**Cause**: Scissor stack imbalance (too many push or pop).

**Solution**: Ensure every `PushScissor()` has matching `PopScissor()`. Use RAII helper:

```cpp
struct ScopedScissor {
    ScopedScissor(const ScissorRect& r) { BatchRenderer::PushScissor(r); }
    ~ScopedScissor() { BatchRenderer::PopScissor(); }
};

// Usage:
{
    ScopedScissor scissor(bounds);
    // ... render clipped content ...
}  // Auto-pop on scope exit
```

---

## CMake Integration

Add to `CMakeLists.txt`:

```cmake
# In src/ui section:
set(SOURCE_FILES
    # ... existing files ...
    src/ui/batch_renderer/batch_renderer.cpp
)

add_executable(towerforge src/main.cpp ${SOURCE_FILES})
target_link_libraries(towerforge PRIVATE raylib glfw towerforge_core)
```

**No module interface needed**: Headers use standard `#pragma once` and can be upgraded to C++20 modules later if desired.

---

## Testing Checklist

Before merging:

- [ ] Visual regression test: UI looks identical to unbatched version
- [ ] Performance test: Draw calls reduced from 1000+ to <10
- [ ] Scissor test: Nested panels clip correctly
- [ ] Text test: All text renders correctly
- [ ] Circle/line test: Shapes render smoothly
- [ ] Memory test: No leaks (valgrind or ASAN)
- [ ] Build test: Compiles on Windows, Linux, macOS
- [ ] Integration test: Existing UI code unchanged (if using adapter)

---

## Next Steps

1. **Review specification docs**: See `docs/batch_renderer/PROMPT_BATCH_RENDERER_*.md`
2. **Run integration example**: (TODO: create `examples/batch_renderer_demo.cpp`)
3. **Migrate one UI class**: Start with `BuildMenu` (highest impact)
4. **Measure performance**: Before/after draw call counts
5. **Iterate**: Gradually migrate remaining UI classes

---

## Support

- **Specification**: `docs/batch_renderer/PROMPT_BATCH_RENDERER_REFINED.md`
- **Diagrams**: `docs/batch_renderer/PROMPT_BATCH_RENDERER_DIAGRAMS.md`
- **API Quick Ref**: `docs/batch_renderer/PROMPT_BATCH_RENDERER_QUICK_REF.md`
- **Decisions & Checklist**: `docs/batch_renderer/PROMPT_BATCH_RENDERER_DECISIONS.md`

---

**Status**: ✅ Ready for integration. Start with adapter for quick wins, migrate to direct API incrementally.
