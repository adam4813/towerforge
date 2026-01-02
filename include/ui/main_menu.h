#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

import engine;

namespace towerforge::core {
    enum class GameState;
}

namespace towerforge::ui {
    // Forward declarations
    struct MouseEvent;

    /**
     * @brief Main menu class for the title screen
     * 
     * Displays a professional title screen with navigation to various game options.
     * Menu items trigger state changes via callback pattern.
     */
    class MainMenu {
    public:
        using StateChangeCallback = std::function<void(core::GameState)>;

        MainMenu();

        ~MainMenu();

        /**
         * @brief Set the callback for state changes
         * @param callback Function to call when a menu option triggers a state change
         */
        void SetStateChangeCallback(const StateChangeCallback &callback);

        /**
         * @brief Render the main menu
         */
        void Render() const;

        /**
         * @brief Update menu state (called every frame)
         * @param delta_time Time elapsed since last frame
         */
        void Update(float delta_time);

        /**
         * @brief Process mouse events (modern unified API)
         * @param event Mouse event data
         * @return true if event was consumed
         */
        bool ProcessMouseEvent(const MouseEvent &event) const;

        /**
         * @brief Handle keyboard input for menu navigation
         * @deprecated Use ProcessKeyboardEvent instead (to be added)
         */
        void HandleKeyboard() const;

        /**
         * @brief Get the currently selected menu option
         * @return Index of selected option
         */
        int GetSelectedOption() const { return selected_option_; }

        void Initialize();

        /**
         * @brief Shutdown and cleanup resources
         */
        void Shutdown();

        /**
         * @brief Get the version string
         * @return Version string
         */
        static std::string GetVersion() { return "v0.1.0"; }

    private:
        void RenderBackground() const;

        /**
         * @brief Recalculate layout when window size changes
         */
        void UpdateLayout();

        int selected_option_; // Currently highlighted menu option
        float animation_time_; // For animations
        int last_screen_width_; // For detecting window resize
        int last_screen_height_; // For detecting window resize

        // Menu layout constants
        static constexpr int MENU_ITEM_HEIGHT = 50;
        static constexpr int MENU_ITEM_SPACING = 10;
        static constexpr int MENU_WIDTH = 300;

        // Menu options
        struct MenuItem {
            std::string label;
            core::GameState target_state;
        };

        std::vector<MenuItem> menu_items_;
        StateChangeCallback state_change_callback_;

        // Main menu is a Panel container with Button children for menu items
        std::unique_ptr<engine::ui::elements::Panel> main_panel_{nullptr};
        std::vector<engine::ui::elements::Button *> menu_item_buttons_;
        // Raw pointers to buttons (owned by main_panel_)
    };
}
