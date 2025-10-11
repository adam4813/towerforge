#pragma once

#include "core/lua_mod_manager.hpp"
#include <string>
#include <vector>

namespace towerforge {
namespace ui {

/**
 * @brief UI Menu for managing Lua mods
 * 
 * Allows players to:
 * - View loaded mods
 * - Enable/disable mods
 * - See mod metadata (name, version, author, description)
 * - View error messages for failed mods
 */
class ModsMenu {
public:
    ModsMenu();
    ~ModsMenu();
    
    /**
     * @brief Set the mod manager to interact with
     * 
     * @param mod_manager Pointer to the LuaModManager
     */
    void SetModManager(TowerForge::Core::LuaModManager* mod_manager);
    
    /**
     * @brief Show the mods menu
     */
    void Show();
    
    /**
     * @brief Hide the mods menu
     */
    void Hide();
    
    /**
     * @brief Check if the menu is currently visible
     */
    bool IsVisible() const { return visible_; }
    
    /**
     * @brief Render the mods menu
     * 
     * Should be called every frame when the menu is visible
     */
    void Render();
    
private:
    bool visible_;
    TowerForge::Core::LuaModManager* mod_manager_;
    int selected_mod_index_;
    float scroll_offset_;
};

} // namespace ui
} // namespace towerforge
