#pragma once

#include <raylib.h>
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace towerforge::ui {

    // Forward declarations
    class Panel;
    class Button;
    class Checkbox;
    class Slider;
    class UIElement;
    class PanelHeaderOverlay;
    class DimOverlay;
    struct MouseEvent;

    /**
     * @brief Accessibility settings menu for configuring accessibility features
     * 
     * Provides controls for high-contrast mode, font scaling, and keyboard navigation settings.
     * All changes apply immediately and persist.
     * Now uses Panel architecture with proper Button/Checkbox/Slider components.
     */
    class AccessibilitySettingsMenu {
    public:
        using BackCallback = std::function<void()>;

        AccessibilitySettingsMenu();
        ~AccessibilitySettingsMenu();
    
        /**
         * @brief Render the accessibility settings menu
         */
        void Render() const;
    
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
         * @brief Process mouse events (modern unified API)
         * @param event Mouse event data
         * @return true if event was consumed
         */
        bool ProcessMouseEvent(const MouseEvent& event);
    
        /**
         * @brief Handle keyboard input for menu navigation
         * @return True if ESC was pressed to go back
         * @deprecated Use ProcessKeyboardEvent instead (to be added)
         */
        bool HandleKeyboard();
    
        /**
         * @brief Handle mouse input for menu interaction
         * @param mouse_x Mouse X position
         * @param mouse_y Mouse Y position
         * @param clicked Whether mouse was clicked
         * @return True if back button was clicked
         * @deprecated Use ProcessMouseEvent instead
         */
        bool HandleMouse(int mouse_x, int mouse_y, bool clicked);

        /**
         * @brief Set callback for back button
         */
        void SetBackCallback(const BackCallback& callback) { back_callback_ = callback; }
    
    private:
        void UpdateLayout();
        void UpdateSelection(int new_selection);
        void ApplySettings() const;
    
        BackCallback back_callback_;
        std::unique_ptr<Panel> settings_panel_;
        std::unique_ptr<PanelHeaderOverlay> header_overlay_;
        std::unique_ptr<DimOverlay> dim_overlay_;
        std::vector<UIElement*> interactive_elements_;  // Pointers to focusable elements
        
        // UI Components (owned by panel)
        Checkbox* high_contrast_checkbox_;
        Slider* font_scale_slider_;
        Checkbox* keyboard_nav_checkbox_;
        Button* back_button_;

        int selected_option_;  // Currently highlighted menu option
        float animation_time_; // For animations
        int last_screen_width_;
        int last_screen_height_;
    
        // Settings
        bool high_contrast_enabled_;
        float font_scale_;
        bool keyboard_navigation_enabled_;
    
        // Menu layout constants
        static constexpr int MENU_WIDTH = 500;
        static constexpr int CONTENT_START_Y = 100;  // Start below header + underline
        static constexpr int ITEM_HEIGHT = 60;
        static constexpr int ITEM_SPACING = 15;
        static constexpr int SLIDER_WIDTH = 300;
    };

} // namespace towerforge::ui
