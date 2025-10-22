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

    /**
     * @brief Settings options available in the general settings menu
     */
    enum class SettingsOption {
        Audio,
        Controls,
        Display,
        Accessibility,
        Gameplay,
        Back
    };

    /**
     * @brief General settings menu providing access to all game settings
     * 
     * Displays a menu with links to various settings categories.
     * Uses callback pattern for option selection.
     */
    class GeneralSettingsMenu {
    public:
        using OptionCallback = std::function<void(SettingsOption)>;
        
        GeneralSettingsMenu();
        ~GeneralSettingsMenu();
        
        /**
         * @brief Set the callback for option selection
         * @param callback Function to call when an option is selected
         */
        void SetOptionCallback(OptionCallback callback);

        /**
         * @brief Render the general settings menu
         */
        void Render() const;

        /**
         * @brief Update menu state (called every frame)
         * @param delta_time Time elapsed since last frame
         */
        void Update(float delta_time);

        /**
         * @brief Handle keyboard input for menu navigation
         */
        void HandleKeyboard();

        /**
         * @brief Handle mouse input for menu interaction
         * @param mouse_x Mouse X position
         * @param mouse_y Mouse Y position
         * @param clicked Whether mouse was clicked
         */
        void HandleMouse(int mouse_x, int mouse_y, bool clicked);

    private:
        void RenderBackground() const;
        static void RenderHeader();
        void RenderMenuOptions() const;
        void UpdateButtonSelection(int new_selection);
        void UpdateLayout();

        int selected_option_;  // Currently highlighted menu option
        float animation_time_; // For animations
        int last_screen_width_;
        int last_screen_height_;

        // Menu layout constants
        static constexpr int MENU_ITEM_HEIGHT = 60;
        static constexpr int MENU_ITEM_SPACING = 15;
        static constexpr int MENU_START_Y = 180;
        static constexpr int MENU_WIDTH = 400;
        static constexpr int HEADER_Y = 100;

        // Menu items
        struct MenuItem {
            std::string label;
            SettingsOption option;
        };

        std::vector<MenuItem> menu_items_;
        OptionCallback option_callback_;
        
        // Settings menu panel with Button children
        std::unique_ptr<Panel> settings_panel_;
        std::vector<Button*> menu_item_buttons_;  // Raw pointers to buttons (owned by settings_panel_)
    };
}
