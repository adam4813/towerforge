#pragma once

#include <raylib.h>
#include <string>

namespace towerforge::ui {

    /**
     * @brief Reusable overlay header for panels
     * 
     * Renders a title and decorative underline on top of panels.
     * Used by settings menus and other panel-based UI.
     */
    class PanelHeaderOverlay {
    public:
        PanelHeaderOverlay(const std::string& title, int font_size = 32);
        
        /**
         * @brief Render the header overlay
         * @param panel_x X position of the panel
         * @param panel_y Y position of the panel
         * @param panel_width Width of the panel
         */
        void Render(int panel_x, int panel_y, int panel_width) const;
        
        void SetTitle(const std::string& title) { title_ = title; }
        void SetFontSize(int font_size) { font_size_ = font_size; }
        void SetTitleColor(Color color) { title_color_ = color; }
        void SetYOffset(int offset) { y_offset_ = offset; }
        
    private:
        std::string title_;
        int font_size_;
        Color title_color_;
        int y_offset_;  // Y offset from panel top
        
        static constexpr int DEFAULT_Y_OFFSET = 30;
        static constexpr int UNDERLINE_SPACING = 40;
        static constexpr int UNDERLINE_THICKNESS = 2;
        static constexpr int UNDERLINE_PADDING = 40;  // Extra width on each side
    };

}
