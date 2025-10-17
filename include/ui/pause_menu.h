#pragma once

#include <raylib.h>
#include <string>
#include <vector>
#include <memory>

namespace towerforge::ui {

    // Forward declaration
    class Panel;
    /**
 * @brief Menu options available in the pause menu
 */
    enum class PauseMenuOption {
        Resume,
        SaveGame,
        LoadGame,
        Settings,
        Mods,
        QuitToTitle
    };

    /**
 * @brief Pause menu class for in-game menu overlay
 * 
 * Displays when ESC is pressed during gameplay, pausing the simulation
 * and providing options to save, load, change settings, or quit
 */
    class PauseMenu {
    public:
        PauseMenu();
        ~PauseMenu();
    
        /**
     * @brief Render the pause menu overlay
     */
        void Render() const;
    
        /**
     * @brief Update menu state (called every frame when active)
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
     * @brief Show/hide confirmation dialog for quitting
     * @param show Whether to show the confirmation dialog
     */
        void ShowQuitConfirmation(const bool show) { show_quit_confirmation_ = show; }
    
        /**
     * @brief Check if quit confirmation is showing
     * @return True if confirmation dialog is visible
     */
        bool IsQuitConfirmationShowing() const { return show_quit_confirmation_; }
    
        /**
     * @brief Handle input for quit confirmation dialog
     * @return 1 for confirm, 0 for cancel, -1 for no action
     */
        int HandleQuitConfirmation();
    
    private:
        void RenderOverlay() const;
        void RenderMenuOptions() const;
        void RenderQuitConfirmation() const;
    
        int selected_option_;  // Currently highlighted menu option
        float animation_time_; // For animations
        bool show_quit_confirmation_;  // Whether to show quit confirmation dialog
        int quit_confirmation_selection_;  // 0 = Cancel, 1 = Confirm
    
        // Menu layout constants
        static constexpr int MENU_ITEM_HEIGHT = 50;
        static constexpr int MENU_ITEM_SPACING = 10;
        static constexpr int MENU_START_Y = 200;
        static constexpr int MENU_WIDTH = 350;
        static constexpr int TITLE_Y = 100;
    
        // Menu options
        struct MenuItem {
            std::string label;
            PauseMenuOption option;
        };
    
        std::vector<MenuItem> menu_items_;
        
        // Optional Panel objects for unified UI system
        std::vector<std::unique_ptr<Panel>> menu_item_panels_;
    };

}
