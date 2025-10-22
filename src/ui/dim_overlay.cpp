#include "ui/dim_overlay.h"
#include "ui/batch_renderer/batch_adapter.h"

namespace towerforge::ui {

    DimOverlay::DimOverlay(const float opacity)
        : color_(BLACK)
          , opacity_(opacity) {
    }

    void DimOverlay::Render() const {
        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        
        batch_renderer::adapter::DrawRectangle(
            0, 0, 
            screen_width, 
            screen_height, 
            ColorAlpha(color_, opacity_)
        );
    }

}
