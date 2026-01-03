#pragma once

#include "core/lua_mod_manager.hpp"
#include <memory>
#include <vector>

import engine;

namespace towerforge::ui {
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
        void SetModManager(core::LuaModManager *mod_manager);

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
         * @brief Initialize the UI components
         */
        void Initialize();

        /**
         * @brief Update the menu state
         * @param delta_time Time since last frame
         */
        void Update(float delta_time);

        /**
         * @brief Render the mods menu
         * 
         * Should be called every frame when the menu is visible
         */
        void Render() const;

        /**
         * @brief Process mouse events
         * @param event The mouse event to process
         * @return true if the event was handled
         */
        bool ProcessMouseEvent(const engine::ui::MouseEvent &event) const;

        /**
         * @brief Handle keyboard input
         */
        void HandleKeyboard();

        /**
         * @brief Shutdown and cleanup UI resources
         */
        void Shutdown();

    private:
        void UpdateLayout();

        void RebuildModsList();

        static void RenderDimOverlay();

        static constexpr int MENU_WIDTH = 700;
        static constexpr int MENU_HEIGHT = 550;
        static constexpr int HEADER_HEIGHT = 60;
        static constexpr int MOD_ITEM_HEIGHT = 100;
        static constexpr int MOD_ITEM_SPACING = 8;

        bool visible_;
        core::LuaModManager *mod_manager_;
        int selected_mod_index_;
        std::uint32_t last_screen_width_;
        std::uint32_t last_screen_height_;
        std::size_t last_mods_count_;

        std::unique_ptr<engine::ui::elements::Panel> main_panel_;
        engine::ui::elements::Container *mods_list_container_;
        std::vector<engine::ui::elements::Button *> mod_buttons_;
    };
}
