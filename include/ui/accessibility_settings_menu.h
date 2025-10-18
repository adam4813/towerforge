#pragma once

#include <raylib.h>
#include <string>
#include <vector>

namespace towerforge::ui {

    /**
     * @brief Accessibility settings menu for configuring accessibility features
     * 
     * Provides controls for high-contrast mode, font scaling, and keyboard navigation settings.
     * All changes apply immediately and persist.
     */
    class AccessibilitySettingsMenu {
    public:
        AccessibilitySettingsMenu();
        ~AccessibilitySettingsMenu();
    
        /**
         * @brief Render the accessibility settings menu
         */
        void Render();
    
        /**
         * @brief Update menu state (called every frame)
         * @param delta_time Time elapsed since last frame
         */
        void Update(float delta_time);
    
        /**
         * @brief Sync local values with AccessibilitySettings
         */
        void SyncWithSettings();
    
        /**
         * @brief Handle keyboard input for menu navigation
         * @return True if ESC was pressed to go back
         */
        bool HandleKeyboard();
    
        /**
         * @brief Handle mouse input for menu interaction
         * @param mouse_x Mouse X position
         * @param mouse_y Mouse Y position
         * @param clicked Whether mouse was clicked
         * @return True if back button was clicked
         */
        bool HandleMouse(int mouse_x, int mouse_y, bool clicked);
    
    private:
        static void RenderBackground();
        static void RenderHeader();
        void RenderControls() const;
        void RenderBackButton() const;
        void RenderCheckbox(const char* label, bool checked, int y_pos, bool is_selected) const;
        void RenderFontScaleSlider() const;
    
        void ApplySettings();
    
        int selected_option_;  // Currently highlighted menu option (0=high_contrast, 1=font_scale, 2=keyboard_nav, 3=back)
        float animation_time_; // For animations
    
        // Settings
        bool high_contrast_enabled_;
        float font_scale_;
        bool keyboard_navigation_enabled_;
    
        // Menu layout constants
        static constexpr int MENU_WIDTH = 500;
        static constexpr int MENU_HEIGHT = 400;
        static constexpr int HEADER_HEIGHT = 80;
        static constexpr int ITEM_HEIGHT = 50;
        static constexpr int ITEM_SPACING = 15;
        static constexpr int ITEMS_START_Y = 150;
        static constexpr int BACK_BUTTON_Y = 480;
        static constexpr int BACK_BUTTON_WIDTH = 150;
        static constexpr int BACK_BUTTON_HEIGHT = 50;
    };

} // namespace towerforge::ui
