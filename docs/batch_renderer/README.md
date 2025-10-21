# Batch Renderer - API Documentation

**Purpose**: Batched UI rendering for TowerForge  
**Performance**: Reduces draw calls from 100+ to single digits per frame  
**Location**: `include/ui/batch_renderer/`, `src/ui/batch_renderer/`

---

## Quick Start

```cpp
#include "ui/batch_renderer/batch_adapter.h"
using namespace towerforge::ui::batch_renderer::adapter;

// All existing Raylib draw calls now automatically batch!
DrawRectangle(x, y, w, h, color);
DrawText("Hello", x, y, 20, WHITE);
```

Or use the direct API:

```cpp
#include "ui/batch_renderer/batch_renderer.h"
using namespace towerforge::ui::batch_renderer;

BatchRenderer::BeginFrame();
BatchRenderer::SubmitQuad({x, y, w, h}, color);
BatchRenderer::SubmitText("Hello", x, y, 20, WHITE);
BatchRenderer::EndFrame();
```

---

## API Files

- **`batch_types.h`** — Core types (Vertex, ScissorRect, color helpers)
- **`batch_renderer.h`** — Main API (BeginFrame, SubmitQuad, SubmitLine, etc.)
- **`batch_adapter.h`** — Drop-in Raylib replacements (DrawRectangle, DrawCircle, etc.)

---

## Core API

### Lifecycle
```cpp
BatchRenderer::Initialize();   // Once at startup (called in game.cpp)
BatchRenderer::BeginFrame();   // Each frame start
BatchRenderer::EndFrame();     // Each frame end (auto-flushes)
BatchRenderer::Shutdown();     // Once at exit (called in game.cpp)
```

### Drawing (Direct API)
```cpp
// Rectangles
BatchRenderer::SubmitQuad({x, y, w, h}, color);

// Lines
BatchRenderer::SubmitLine(x0, y0, x1, y1, thickness, color);

// Circles
BatchRenderer::SubmitCircle(center_x, center_y, radius, color);

// Text (flushes batch, uses native Raylib)
BatchRenderer::SubmitText(text, x, y, font_size, color);
```

### Adapter API (Drop-in Replacements)
```cpp
using namespace towerforge::ui::batch_renderer::adapter;

DrawRectangle(x, y, w, h, color);
DrawCircle(cx, cy, radius, color);
DrawText("Hello", x, y, 20, WHITE);
// All standard Raylib draw calls automatically batch
```

### Clipping
```cpp
BatchRenderer::PushScissor({x, y, w, h});
// ... clipped rendering ...
BatchRenderer::PopScissor();
```

### Debug
```cpp
size_t calls = BatchRenderer::GetDrawCallCount();
BatchRenderer::ResetDrawCallCount();
```

---

## Integration

Already integrated into TowerForge:
- `CMakeLists.txt` includes batch_renderer.cpp
- `game.cpp` calls Initialize/Shutdown
- `main_menu.cpp` uses adapter for automatic batching

To use in other UI classes, add to any `Render()` method:
```cpp
#include "ui/batch_renderer/batch_adapter.h"
using namespace towerforge::ui::batch_renderer::adapter;
// Existing DrawXXX calls now batch automatically
```

---

## Performance

**Expected**: 100+ draw calls → single digits per frame

Check with: `BatchRenderer::GetDrawCallCount()` after `EndFrame()`

---

## Documentation

- **Integration Guide**: `INTEGRATION_GUIDE.md` - Migration strategies
- **API Reference**: `PROMPT_BATCH_RENDERER_QUICK_REF.md` - Complete API listing
- **Examples**: `EXAMPLE_USAGE.cpp` - Code samples
