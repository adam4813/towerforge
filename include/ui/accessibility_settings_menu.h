#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

import engine;

namespace towerforge::ui {
    /**
     * @brief Accessibility settings menu for configuring accessibility features
     * 
     * Provides controls for high-contrast mode, font scaling, and keyboard navigation settings.
     * All changes apply immediately and persist.
     * Uses engine Panel with layout components.
     */
    class AccessibilitySettingsMenu {
    public:
        using BackCallback = std::function<void()>;

        AccessibilitySettingsMenu();

        ~AccessibilitySettingsMenu();

        /**
         * @brief Initialize UI components (must be called after graphics context is ready)
         */
        void Initialize();

        /**
         * @brief Clean up UI components
         */
        void Shutdown();

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
        bool ProcessMouseEvent(const engine::ui::MouseEvent &event) const;

        /**
         * @brief Handle keyboard input for menu navigation
         * @return True if ESC was pressed to go back
         * @deprecated Use ProcessKeyboardEvent instead (to be added)
         */
        void HandleKeyboard() const;

        /**
         * @brief Set callback for back button
         */
        void SetBackCallback(const BackCallback &callback) { back_callback_ = callback; }

    private:
        void UpdateLayout();

        void ApplySettings() const;

        static void RenderDimOverlay();

        BackCallback back_callback_;
        std::unique_ptr<engine::ui::elements::Panel> settings_panel_;

        // UI Components (owned by panel)
        engine::ui::elements::Checkbox *high_contrast_checkbox_;
        engine::ui::elements::Slider *font_scale_slider_;
        engine::ui::elements::Checkbox *keyboard_nav_checkbox_;
        engine::ui::elements::Button *back_button_;

        int selected_option_; // Currently highlighted menu option
        float animation_time_; // For animations
        int last_screen_width_;
        int last_screen_height_;

        // Settings
        bool high_contrast_enabled_;
        float font_scale_;
        bool keyboard_navigation_enabled_;

        // Menu layout constants
        static constexpr int MENU_WIDTH = 500;
        static constexpr int MENU_HEIGHT = 275;
        static constexpr int ITEM_HEIGHT = 30;
        static constexpr int ITEM_SPACING = 15;
    };
} // namespace towerforge::ui
