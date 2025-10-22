#include "ui/panel_header_overlay.h"
#include "ui/ui_theme.h"
#include "ui/batch_renderer/batch_adapter.h"

namespace towerforge::ui {

    PanelHeaderOverlay::PanelHeaderOverlay(const std::string& title, const int font_size)
        : title_(title)
          , font_size_(font_size)
          , title_color_(UITheme::PRIMARY)
          , y_offset_(DEFAULT_Y_OFFSET) {
    }

    void PanelHeaderOverlay::Render(const int panel_x, const int panel_y, const int panel_width) const {
        const int title_width = MeasureText(title_.c_str(), font_size_);
        const int title_x = panel_x + (panel_width - title_width) / 2;
        const int title_y = panel_y + y_offset_;
        
        // Render title text
        batch_renderer::adapter::DrawText(
            title_.c_str(),
            title_x,
            title_y,
            font_size_,
            title_color_
        );
        
        // Render decorative underline
        const int line_width = title_width + UNDERLINE_PADDING;
        const int line_x = panel_x + (panel_width - line_width) / 2;
        const int line_y = title_y + UNDERLINE_SPACING;
        
        batch_renderer::adapter::DrawRectangle(
            line_x,
            line_y,
            line_width,
            UNDERLINE_THICKNESS,
            title_color_
        );
    }

}
