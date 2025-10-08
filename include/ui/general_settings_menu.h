#pragma once

#include <raylib.h>
#include <string>
#include <vector>

namespace towerforge {
namespace ui {

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
 * Currently only Audio Settings is implemented; others are placeholders.
 */
class GeneralSettingsMenu {
public:
    GeneralSettingsMenu();
    ~GeneralSettingsMenu();
    
    /**
     * @brief Render the general settings menu
     */
    void Render();
    
    /**
     * @brief Update menu state (called every frame)
     * @param delta_time Time elapsed since last frame
     */
    void Update(float delta_time);
    
    /**
     * @brief Handle keyboard input for menu navigation
     * @return Selected option index, or -1 if none selected
     */
    int HandleKeyboard();
    
    /**
     * @brief Handle mouse input for menu interaction
     * @param mouse_x Mouse X position
     * @param mouse_y Mouse Y position
     * @param clicked Whether mouse was clicked
     * @return Selected option index, or -1 if none selected
     */
    int HandleMouse(int mouse_x, int mouse_y, bool clicked);
    
private:
    void RenderBackground();
    void RenderHeader();
    void RenderMenuOptions();
    
    int selected_option_;  // Currently highlighted menu option
    float animation_time_; // For animations
    
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
};

} // namespace ui
} // namespace towerforge
