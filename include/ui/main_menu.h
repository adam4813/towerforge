#pragma once

#include <raylib.h>
#include <string>
#include <vector>
#include <memory>

namespace towerforge::ui {

    // Forward declarations
    class Panel;
    class Button;

    /**
 * @brief Menu options available in the main menu
 */
    enum class MenuOption {
        NewGame,
        Tutorial,
        LoadGame,
        Achievements,
        Settings,
        Credits,
        Quit
    };

    /**
 * @brief Main menu class for the title screen
 * 
 * Displays a professional title screen with navigation to various game options
 */
    class MainMenu {
    public:
        MainMenu();
        ~MainMenu();
    
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
     * @brief Handle keyboard input for menu navigation
     * @return Selected menu option, or -1 if none selected
     */
        int HandleKeyboard();
    
        /**
     * @brief Handle mouse input for menu interaction
     * @param mouse_x Mouse X position
     * @param mouse_y Mouse Y position
     * @param clicked Whether mouse was clicked
     * @return Selected menu option, or -1 if none selected
     */
        int HandleMouse(int mouse_x, int mouse_y, bool clicked);
    
        /**
     * @brief Get the currently selected menu option
     * @return Index of selected option
     */
        int GetSelectedOption() const { return selected_option_; }
    
        /**
     * @brief Get the version string
     * @return Version string
     */
        static std::string GetVersion() { return "v0.1.0"; }
    
    private:
        static void RenderTitle();
        void RenderMenuOptions() const;

        static void RenderVersion();
        void RenderBackground() const;
    
        int selected_option_;  // Currently highlighted menu option
        float animation_time_; // For animations
    
        // Menu layout constants
        static constexpr int MENU_ITEM_HEIGHT = 50;
        static constexpr int MENU_ITEM_SPACING = 10;
        static constexpr int MENU_START_Y = 250;
        static constexpr int MENU_WIDTH = 300;
        static constexpr int TITLE_Y = 80;
    
        // Menu options
        struct MenuItem {
            std::string label;
            MenuOption option;
        };
    
        std::vector<MenuItem> menu_items_;
        
        // Main menu is a Panel container with Button children for menu items
        std::unique_ptr<Panel> main_panel_;
        std::vector<Button*> menu_item_buttons_;  // Raw pointers to buttons (owned by main_panel_)
        int selected_menu_option_;  // Stores the menu option selected via click callback
    };

}
